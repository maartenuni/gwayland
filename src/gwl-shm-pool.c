/*
 * GWayland library gobject wrappers around waylandclient library.
 * Copyright (C) 2025 Maarten Duijndam
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

// for ftruncate
#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <gio/gio.h>
#include <gwl-enums.h>
#include <gwl-object.h>
#include <gwl-shm-pool-private.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

/* ********* implementation of the gwl-shm_pool object ************/

typedef struct {
    gint fd;
    gint allocated_size; // Should not decrease according to protocol
} GwlShmPoolPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlShmPool, gwl_shm_pool, GWL_TYPE_OBJECT)

enum { SIG_FORMAT, LAST_SIGNAL };

typedef enum { FIRST_PROPERTY, PROP_FD, PROP_SIZE, N_PROPERTIES } ShmProperty;

static GParamSpec *shm_pool_properties[N_PROPERTIES] = {NULL};
static guint       shm_pool_signals[LAST_SIGNAL]     = {0};

static void
gwl_shm_pool_set_property(GObject      *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    GwlShmPool        *self = GWL_SHM_POOL(object);
    GwlShmPoolPrivate *priv = gwl_shm_pool_get_instance_private(self);
    (void) value;
    GError *error = NULL;

    switch ((ShmProperty) property_id) {
    case PROP_FD:
        priv->fd = g_value_get_int(value);
        break;
    case PROP_SIZE:
        gwl_shm_pool_resize(self, g_value_get_int(value), &error);
        if (error) {
            g_critical("Unable to (re)size shm pool: %s", error->message);
            g_clear_error(&error);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_shm_pool_get_property(GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    GwlShmPool        *self = GWL_SHM_POOL(object);
    GwlShmPoolPrivate *priv = gwl_shm_pool_get_instance_private(self);
    (void) value;
    (void) self;

    switch (property_id) {
    case PROP_SIZE:
        g_value_set_int(value, priv->allocated_size);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_shm_pool_init(GwlShmPool *self)
{
    GwlShmPoolPrivate *priv = gwl_shm_pool_get_instance_private(self);
    (void) priv;
}

static void
gwl_shm_pool_dispose(GObject *object)
{
    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_shm_pool_parent_class)->dispose(object);
}

static void
gwl_shm_pool_finalize(GObject *object)
{
    G_OBJECT_CLASS(gwl_shm_pool_parent_class)->finalize(object);
}

static GDestroyNotify
shm_pool_get_cleanup_func(void)
{
    return (GDestroyNotify) wl_shm_pool_destroy;
}

static void
gwl_shm_pool_class_init(GwlShmPoolClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose  = gwl_shm_pool_dispose;
    object_class->finalize = gwl_shm_pool_finalize;

    object_class->set_property = gwl_shm_pool_set_property;
    object_class->get_property = gwl_shm_pool_get_property;

    GwlObjectClass *gwl_obj_cls   = GWL_OBJECT_CLASS(klass);
    gwl_obj_cls->get_cleanup_func = shm_pool_get_cleanup_func;

    // obj_properties);

    /**
     * ShmPool:fd
     *
     * The file descriptor used to manage the shared memory
     */
    shm_pool_properties[PROP_FD] = g_param_spec_int(
        "size",
        "Size",
        "The file descriptor belonging to this section of shared memory",
        0,
        G_MAXINT,
        0,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    /**
     * ShmPool:size
     *
     * The allocated amount of shared memory, this can only increase according
     * the wayland protocol.
     */
    shm_pool_properties[PROP_SIZE]
        = g_param_spec_int("size",
                           "Size",
                           "The allocated amount of shared memory.",
                           1,
                           G_MAXINT,
                           1,
                           G_PARAM_READWRITE);

    g_object_class_install_properties(
        object_class, N_PROPERTIES, shm_pool_properties);

    // obj signals
}

/* * library private api * */

static int
create_shm_fd(guint num_tries)
{
    char  random_name[128];
    guint nth_try = 0;
    int   fd;
    do {
        gint random = g_random_int();
        g_snprintf(random_name, sizeof(random_name), "/wl-shm-%d", random);
        fd = shm_open(random_name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(random_name);
            break;
        }
    } while (nth_try++ < num_tries && errno == EEXIST);
    return fd;
}

/**
 * gwl_shm_pool_new:
 *
 * Stability:private
 */
GwlShmPool *
gwl_shm_pool_new(struct wl_shm_pool *shm_pool, gint size)
{
    g_return_val_if_fail(shm_pool != NULL, NULL);
    g_return_val_if_fail(size > 0, NULL);

    int fd = create_shm_fd(100);

    // clang-format off
    return g_object_new(
        GWL_TYPE_SHM_POOL,
        "object", shm_pool,
        "fd", fd, // fd has to be set before size
        "size", size,
        NULL);
    // clang-format on
}

/** public API **/

void
gwl_shm_pool_resize(GwlShmPool *self, gint new_larger_size, GError **error)
{
    g_return_if_fail(GWL_IS_SHM_POOL(self));
    g_return_if_fail(error == NULL || *error != NULL);
    GwlShmPoolPrivate *priv = gwl_shm_pool_get_instance_private(self);

    g_warn_if_fail(new_larger_size >= priv->allocated_size);
    if (new_larger_size < priv->allocated_size)
        return;

    int ret = ftruncate(priv->fd, new_larger_size);
    if (ret != 0) {
        int save_errno = errno;
        g_set_error(error,
                    G_TYPE_IO_ERROR_ENUM,
                    g_io_error_from_errno(save_errno),
                    "Unable to resize pool: %s",
                    g_strerror(save_errno));
    }
    else {
        priv->allocated_size = new_larger_size;
    }
}
