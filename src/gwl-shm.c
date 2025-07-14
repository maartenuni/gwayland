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

#include <gwl-object.h>
#include <gwl-shm-private.h>
#include <gwl-shm.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <gwl-enum-types.h>
#include <gwl-enums.h>

/* ********* implementation of the gwl-shm object ************/

static struct wl_shm_listener shm_listener;

typedef struct {
    GArray *formats;
} GwlShmPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlShm, gwl_shm, GWL_TYPE_OBJECT)

enum { SIG_FORMAT, LAST_SIGNAL };

typedef enum { FIRST_PROPERTY, N_PROPERTIES } ShmProperty;

static GParamSpec *shm_properties[N_PROPERTIES] = {NULL};
static guint       shm_signals[LAST_SIGNAL]     = {0};

static void
gwl_shm_set_property(GObject      *object,
                     guint         property_id,
                     const GValue *value,
                     GParamSpec   *pspec)
{
    // GwlShm *self = GWL_SHM(object);
    //  GwlShmPrivate *priv = gwl_shm_get_instance_private(self);
    (void) value;

    switch ((ShmProperty) property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_shm_get_property(GObject    *object,
                     guint       property_id,
                     GValue     *value,
                     GParamSpec *pspec)
{
    GwlShm *self = GWL_SHM(object);
    // GwlShmPrivate* priv = gwl_shm_get_instance_private(self);
    (void) value;
    (void) self;

    switch (property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

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

/** public API **/

/* * Use the shm to bind the globals. * */

static void
event_shm_format(GwlShm *self, struct wl_shm *shm, uint32_t format)
{
    g_return_if_fail(GWL_IS_SHM(self));
    (void) shm;

    g_signal_emit(self, shm_signals[SIG_FORMAT], 0, format, NULL);
}

static struct wl_shm_listener shm_listener = {.format = event_shm_format};
