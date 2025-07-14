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

#include <gwl-compositor-private.h>
#include <gwl-object.h>
#include <gwl-surface-private.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <gwl-enums.h>

/* ********* implementation of the gwl-compositor object ************/

G_DEFINE_TYPE(GwlCompositor, gwl_compositor, GWL_TYPE_OBJECT)

// Currently the compositor doesn't have any properties
//
// typedef enum { FIRST_PROPERTY, N_PROPERTIES } CompositorProperty;
//
// static GParamSpec *compositor_properties[N_PROPERTIES] = {NULL};
//
//  static void
//  gwl_compositor_set_property(GObject      *object,
//                              guint         property_id,
//                              const GValue *value,
//                              GParamSpec   *pspec)
//  {
//      // GwlCompositor *self = GWL_COMPOSITOR(object);
//      //  GwlCompositorPrivate *priv =
//      gwl_compositor_get_instance_private(self); (void) value;
//
//      switch ((CompositorProperty) property_id) {
//      default:
//          G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
//          break;
//      }
//  }
//
//  static void
//  gwl_compositor_get_property(GObject    *object,
//                              guint       property_id,
//                              GValue     *value,
//                              GParamSpec *pspec)
//  {
//      GwlCompositor *self = GWL_COMPOSITOR(object);
//      // GwlCompositorPrivate* priv =
//      gwl_compositor_get_instance_private(self); (void) value; (void) self;
//
//      switch (property_id) {
//      default:
//          G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
//          break;
//      }
//  }

static void
gwl_compositor_init(GwlCompositor *self)
{
    (void) self;
    // everything is set to zero
}

static void
gwl_compositor_constructed(GObject *self)
{
    // There is no compositor listener
    // wl_compositor_add_listener(
    //     gwl_object_get(GWL_OBJECT(self)), &compositor_listener, self);
    //
    G_OBJECT_CLASS(gwl_compositor_parent_class)->constructed(self);
}

static void
gwl_compositor_dispose(GObject *object)
{
    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_compositor_parent_class)->dispose(object);
}

static void
gwl_compositor_finalize(GObject *object)
{
    G_OBJECT_CLASS(gwl_compositor_parent_class)->finalize(object);
}

static GDestroyNotify
compositor_get_cleanup_func(void)
{
    return (GDestroyNotify) wl_compositor_destroy;
}

static void
gwl_compositor_class_init(GwlCompositorClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose     = gwl_compositor_dispose;
    object_class->finalize    = gwl_compositor_finalize;
    object_class->constructed = gwl_compositor_constructed;

    //    object_class->set_property = gwl_compositor_set_property;
    //    object_class->get_property = gwl_compositor_get_property;

    GwlObjectClass *gwl_obj_cls   = GWL_OBJECT_CLASS(klass);
    gwl_obj_cls->get_cleanup_func = compositor_get_cleanup_func;

    // obj_properties);

    // obj signals
}

/* * library private api * */

/**
 * @private
 *
 * Called by GwlDisplay in order to obtain the wayland server globals
 *
 */
GwlCompositor *
gwl_compositor_new(struct wl_compositor *compositor)
{
    g_return_val_if_fail(compositor != NULL, NULL);

    return g_object_new(GWL_TYPE_COMPOSITOR, "object", compositor, NULL);
}

/** public API **/

/**
 * gwl_compositor_get_surface:
 *
 * Get a new instance of [class@Surface]
 *
 * Returns:(transfer=full):A new surface
 */
GwlSurface *
gwl_compositor_get_surface(GwlCompositor *self)
{
    GwlSurface           *ret;
    struct wl_surface    *surface;
    struct wl_compositor *compositor = gwl_object_get(GWL_OBJECT(self));

    surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        g_critical("unable to instantiate instance of struct wl_surface");
        return 0;
    }

    ret = gwl_surface_new(surface);
    return ret;
}
