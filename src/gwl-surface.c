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

#include <gwl-enum-types.h>
#include <gwl-object.h>
#include <gwl-surface-private.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

/* ********* implementation of the gwl-surface object ************/

static void
gwl_surface_bind_listener(GwlSurface *self);

static struct wl_surface_listener surface_listener;

typedef struct {
    gpointer output; // We might want to store the relevant output
} GwlSurfacePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlSurface, gwl_surface, GWL_TYPE_OBJECT)

enum { SIG_ENTER, SIG_LEAVE, LAST_SIGNAL };

typedef enum { FIRST_PROPERTY, N_PROPERTIES } SurfaceProperty;

// static GParamSpec *surface_properties[N_PROPERTIES] = {NULL};
static guint surface_signals[LAST_SIGNAL] = {0};

// static void
// gwl_surface_set_property(GObject      *object,
//                          guint         property_id,
//                          const GValue *value,
//                          GParamSpec   *pspec)
// {
//     // GwlSurface *self = GWL_SURFACE(object);
//     //  GwlSurfacePrivate *priv = gwl_surface_get_instance_private(self);
//     (void) value;
//
//     switch ((SurfaceProperty) property_id) {
//     default:
//         G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
//         break;
//     }
// }
//
// static void
// gwl_surface_get_property(GObject    *object,
//                          guint       property_id,
//                          GValue     *value,
//                          GParamSpec *pspec)
// {
//     GwlSurface *self = GWL_SURFACE(object);
//     // GwlSurfacePrivate* priv = gwl_surface_get_instance_private(self);
//     (void) value;
//     (void) self;
//
//     switch (property_id) {
//     default:
//         G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
//         break;
//     }
// }

static void
gwl_surface_init(GwlSurface *self)
{
    (void) self;
    // everything is set to zero
}

static void
gwl_surface_constructed(GObject *self)
{
    gwl_surface_bind_listener(GWL_SURFACE(self));
}

static void
gwl_surface_dispose(GObject *object)
{
    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_surface_parent_class)->dispose(object);
}

static void
gwl_surface_finalize(GObject *object)
{
    G_OBJECT_CLASS(gwl_surface_parent_class)->finalize(object);
}

static GDestroyNotify
surface_get_cleanup_func(void)
{
    return (GDestroyNotify) wl_surface_destroy;
}

static void
gwl_surface_class_init(GwlSurfaceClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose     = gwl_surface_dispose;
    object_class->finalize    = gwl_surface_finalize;
    object_class->constructed = gwl_surface_constructed;

    //    object_class->set_property = gwl_surface_set_property;
    //    object_class->get_property = gwl_surface_get_property;

    GwlObjectClass *gwl_obj_cls   = GWL_OBJECT_CLASS(klass);
    gwl_obj_cls->get_cleanup_func = surface_get_cleanup_func;

    // obj_properties);

    //    g_object_class_install_properties(
    //        object_class, N_PROPERTIES, surface_properties);

    //// obj signals Require implementation of GWL_TYPE_OUTPUT
    // surface_signals[SIG_ENTER]
    //     = g_signal_new("enter",
    //                    GWL_TYPE_SURFACE,
    //                    G_SIGNAL_RUN_LAST,
    //                    G_STRUCT_OFFSET(struct _GwlSurfaceClass, enter),
    //                    NULL,
    //                    NULL,
    //                    NULL,
    //                    G_TYPE_NONE,
    //                    1,
    //                    GWL_TYPE_OUTPUT);

    // surface_signals[SIG_LEAVE]
    //     = g_signal_new("leave",
    //                    GWL_TYPE_SURFACE,
    //                    G_SIGNAL_RUN_LAST,
    //                    G_STRUCT_OFFSET(struct _GwlSurfaceClass, leave),
    //                    NULL,
    //                    NULL,
    //                    NULL,
    //                    G_TYPE_NONE,
    //                    1,
    //                    GWL_TYPE_OUTPUT);
}

/* * library private api * */

/*
 * Install listener and callback's to obtain the globals.
 */
static void
gwl_surface_bind_listener(GwlSurface *self)
{
    struct wl_surface *surface = gwl_object_get(GWL_OBJECT(self));
    wl_surface_add_listener(surface, &surface_listener, self);
}

/**
 * @private
 */
GwlSurface *
gwl_surface_new(struct wl_surface *surface)
{
    g_return_val_if_fail(surface != NULL, NULL);

    return g_object_new(GWL_TYPE_SURFACE, "object", surface, NULL);
}

/** public API **/

/* * Use the surface to bind the globals. * */

static void
event_surface_enter(void              *data,
                    struct wl_surface *surface,
                    struct wl_output  *output)
{
    GwlSurface *self = GWL_SURFACE(data);
    g_return_if_fail(GWL_IS_SURFACE(self));
    g_warning("No implementation for: %s", __func__);
    (void) surface;
    (void) output;
}

static void
event_surface_leave(void              *data,
                    struct wl_surface *surface,
                    struct wl_output  *output)
{
    GwlSurface *self = GWL_SURFACE(data);
    g_return_if_fail(GWL_IS_SURFACE(self));
    g_warning("No implementation for: %s", __func__);
    (void) surface;
    (void) output;
}

static struct wl_surface_listener surface_listener
    = {.enter = event_surface_enter, .leave = event_surface_leave};
