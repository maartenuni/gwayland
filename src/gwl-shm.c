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

#include <fcntl.h>
#include <gwl-enum-types.h>
#include <gwl-enums.h>
#include <gwl-object.h>
#include <gwl-shm-pool-private.h>
#include <gwl-shm-private.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

/* ********* implementation of the gwl-shm object ************/

static struct wl_shm_listener shm_listener;

typedef struct {
    GArray *formats;
} GwlShmPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlShm, gwl_shm, GWL_TYPE_OBJECT)

enum { SIG_FORMAT, LAST_SIGNAL };

typedef enum { FIRST_PROPERTY, N_PROPERTIES } ShmProperty;

static guint shm_signals[LAST_SIGNAL] = {0};

static void
gwl_shm_init(GwlShm *self)
{
    GwlShmPrivate *priv = gwl_shm_get_instance_private(self);

    priv->formats = g_array_new(FALSE, FALSE, sizeof(gint64));
}

static void
gwl_shm_constructed(GObject *self)
{
    wl_shm_add_listener(gwl_object_get(GWL_OBJECT(self)), &shm_listener, self);
}

static void
gwl_shm_dispose(GObject *object)
{
    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_shm_parent_class)->dispose(object);
}

static void
gwl_shm_finalize(GObject *object)
{
    GwlShmPrivate *priv = gwl_shm_get_instance_private(GWL_SHM(object));
    g_array_unref(priv->formats);

    G_OBJECT_CLASS(gwl_shm_parent_class)->finalize(object);
}

static GDestroyNotify
shm_get_cleanup_func(void)
{
    return (GDestroyNotify) wl_shm_destroy;
}

static void
shm_format(GwlShm *self, GwlShmFormat format)
{
    GwlShmPrivate *priv    = gwl_shm_get_instance_private(self);
    guint64        iformat = format;

    if (!g_array_binary_search(priv->formats, &iformat, g_int64_equal, NULL)) {
        g_array_append_val(priv->formats, iformat);
        g_array_sort(priv->formats, g_int64_equal);
    }
}

static void
gwl_shm_class_init(GwlShmClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose     = gwl_shm_dispose;
    object_class->finalize    = gwl_shm_finalize;
    object_class->constructed = gwl_shm_constructed;

    //    object_class->set_property = gwl_shm_set_property;
    //    object_class->get_property = gwl_shm_get_property;

    GwlObjectClass *gwl_obj_cls   = GWL_OBJECT_CLASS(klass);
    gwl_obj_cls->get_cleanup_func = shm_get_cleanup_func;

    klass->format = shm_format;

    // obj_properties);

    //    g_object_class_install_properties(
    //        object_class, N_PROPERTIES, shm_properties);
    shm_signals[SIG_FORMAT]
        = g_signal_new("format",
                       GWL_TYPE_SHM,
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(struct _GwlShmClass, format),
                       NULL,
                       NULL,
                       NULL,
                       G_TYPE_NONE,
                       1,
                       GWL_TYPE_SHM_FORMAT);

    // obj signals
}

/* * library private api * */

/*
 * Install listener and callback's to obtain the globals.
 */
static void
gwl_shm_bind_listener(GwlShm *self)
{
    struct wl_shm *shm = gwl_object_get(GWL_OBJECT(self));
    wl_shm_add_listener(shm, &shm_listener, self);
}

/**
 * gwl_shm_new:
 *
 * Stability:private
 */
GwlShm *
gwl_shm_new(struct wl_shm *shm)
{
    g_return_val_if_fail(shm != NULL, NULL);

    return g_object_new(GWL_TYPE_SHM, "object", shm, NULL);
}

/*
 * Create shm memory file descriptor for sharing memory with the compostitor.
 */
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

#ifndef NDEBUG
    if (fd >= 0)
        g_info(
            "Opened posix shared memory object %d - at %s\n", fd, random_name);
#endif
    if (fd < 0)
        g_warning("Unable to open shared memory fd");

    return fd;
}

/** public API **/

GwlShmPool *
gwl_shm_create_pool(GwlShm *self, gint size)
{
    g_return_val_if_fail(GWL_IS_SHM(self), NULL);
    g_return_val_if_fail(size > 0, NULL);

    int fd = create_shm_fd(100);

    struct wl_shm_pool *pool = wl_shm_create_pool(
        (struct wl_shm *) gwl_object_get(GWL_OBJECT(self)), fd, size);

    return gwl_shm_pool_new(pool, size);
}

/** raw event handling from the compositor **/

static void
event_shm_format(void *data, struct wl_shm *shm, uint32_t format)
{
    GwlShm *self = data;
    g_return_if_fail(GWL_IS_SHM(self));
    (void) shm;

    g_signal_emit(self, shm_signals[SIG_FORMAT], 0, format, NULL);
}

static struct wl_shm_listener shm_listener = {.format = event_shm_format};
