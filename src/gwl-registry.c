/*
 * GWayland library gobject wrappers around waylandclient library.
 * Copyright (C) 2020 Maarten Duijndam
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

#include "gwl-registry.h"
#include "gwl-object.h"
#include "gwl-shm-private.h"

#include <wayland-client-protocol.h>
#include <wayland-client.h>

enum {
    WL_COMPOSITOR,
    WL_SHM,
};

static void
registry_handle_global(void               *data,
                       struct wl_registry *registry,
                       uint32_t            name,
                       const char         *interface,
                       uint32_t            version);

static void
registry_handle_global_remove(void               *data,
                              struct wl_registry *registry,
                              uint32_t            name);

static const struct wl_registry_listener registry_listener;

static void
gwl_registry_bind_listener(GwlRegistry *self);

/* ********* implementation of the gwl-registry object ************/

typedef struct {
    GwlShm *shm;
} GwlRegistryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlRegistry, gwl_registry, GWL_TYPE_OBJECT)

enum { SIG_GLOBAL, SIG_GLOBAL_REMOVE, LAST_SIGNAL };

typedef enum { FIRST_PROPERTY, PROP_SHM, N_PROPERTIES } RegistryProperty;

static GParamSpec *registry_properties[N_PROPERTIES] = {NULL};
static guint       registry_signals[LAST_SIGNAL]     = {0};

static void
gwl_registry_get_property(GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    GwlRegistry *self = GWL_REGISTRY(object);
    // GwlRegistryPrivate* priv = gwl_registry_get_instance_private(self);
    (void) value;
    (void) self;

    switch (property_id) {
    case PROP_SHM:
        g_value_take_object(value, gwl_registry_get_shm(self));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_registry_init(GwlRegistry *self)
{
    (void) self;
    // everything is set to zero
}

static void
gwl_registry_constructed(GObject *self)
{
    gwl_registry_bind_listener(GWL_REGISTRY(self));
}

static void
gwl_registry_dispose(GObject *object)
{
    GwlRegistryPrivate *priv
        = gwl_registry_get_instance_private(GWL_REGISTRY(object));

    g_clear_object(&priv->shm);

    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_registry_parent_class)->dispose(object);
}

static void
gwl_registry_finalize(GObject *object)
{
    G_OBJECT_CLASS(gwl_registry_parent_class)->finalize(object);
}

static void
gwl_registry_event_global(GwlRegistry *self,
                          GwlObject   *object,
                          guint32      name,
                          const char  *interface,
                          uint32_t     version)
{
    (void) name;
    g_assert(GWL_IS_REGISTRY(self));
    g_assert(GWL_IS_OBJECT(object));

    GwlRegistryPrivate *priv = gwl_registry_get_instance_private(self);

    // Store the reference in GwlRegistryPrivate
    if (g_strcmp0(wl_shm_interface.name, interface) == 0) {
        priv->shm = GWL_SHM(object);
    }
}

static void
gwl_registry_event_global_removed(GwlRegistry *self, guint32 name)
{
    (void) self;
    (void) name;
}

static GDestroyNotify
registry_get_cleanup_func(void)
{
    return (GDestroyNotify) wl_registry_destroy;
}

static void
gwl_registry_class_init(GwlRegistryClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose      = gwl_registry_dispose;
    object_class->finalize     = gwl_registry_finalize;
    object_class->constructed  = gwl_registry_constructed;
    object_class->get_property = gwl_registry_get_property;

    GwlObjectClass *gwl_obj_cls   = GWL_OBJECT_CLASS(klass);
    gwl_obj_cls->get_cleanup_func = registry_get_cleanup_func;

    klass->event_global        = gwl_registry_event_global;
    klass->event_global_remove = gwl_registry_event_global_removed;

    /**
     * GwlRegistry:shm
     *
     * Get the binding to the Wayland Compositors Shared memory interface
     * global.
     *
     * When you get this property, you'll get it until it is removed and
     * make sure to release your reference to it.
     *
     * When a wayland session is started it's globals are announced through
     * the event global
     */
    registry_properties[PROP_SHM]
        = g_param_spec_object("shm",
                              "Shm",
                              "The global shared memory object",
                              GWL_TYPE_SHM,
                              G_PARAM_READABLE);

    g_object_class_install_properties(
        object_class, N_PROPERTIES, registry_properties);

    registry_signals[SIG_GLOBAL]
        = g_signal_new("global",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GwlRegistryClass, event_global),
                       NULL,
                       NULL,
                       NULL,
                       G_TYPE_NONE,
                       4,
                       GWL_TYPE_OBJECT, // The object belonging to this global
                       G_TYPE_UINT,   // The numeric name of the wayland object
                       G_TYPE_STRING, // The name of the interface
                       G_TYPE_UINT);  // version of the interface

    registry_signals[SIG_GLOBAL_REMOVE]
        = g_signal_new("global-remove",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GwlRegistryClass, event_global),
                       NULL,
                       NULL,
                       NULL,
                       G_TYPE_NONE,
                       1,
                       G_TYPE_UINT); // The numeric name of the wayland object
}

/* * library private api * */

/*
 * Install listener and callback's to obtain the globals.
 */
static void
gwl_registry_bind_listener(GwlRegistry *self)
{
    struct wl_registry *registry = gwl_object_get(GWL_OBJECT(self));
    wl_registry_add_listener(registry, &registry_listener, self);
}

/**
 * gwl_registry_new:(constructor)
 *
 * Called by GwlDisplay in order to construct the registry. This
 * method should be considered private. When you want the registry, use
 * [func@Display.get_registry].
 *
 * Stability:(private)
 * Returns:(transfer full): a reference to the displays registry instance.
 */
GwlRegistry *
gwl_registry_new(struct wl_registry *registry)
{
    g_return_val_if_fail(registry != NULL, NULL);

    return g_object_new(GWL_TYPE_REGISTRY, "object", registry, NULL);
}

/** public API **/

/**
 * gwl_registry_get_shm:
 *
 * Get a binding to the Wayland compositors SHared Memory global.
 * You get a reference stored by the GwlRegistry instance.
 *
 * Returns:(transfer full)(nullable): a reference to the registries Shm global
 * instance/binding
 */
GwlShm *
gwl_registry_get_shm(GwlRegistry *self)
{
    g_return_val_if_fail(GWL_IS_REGISTRY(self), NULL);

    GwlRegistryPrivate *priv = gwl_registry_get_instance_private(self);

    if (!priv->shm) {
        g_warning("The wayland global event for Shm hasn't fired yet");
        return NULL;
    }
    return g_object_ref(priv->shm);
}

/* * Use the registry to bind the globals. * */
static const struct wl_registry_listener registry_listener = {
    .global        = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

static void
registry_handle_global(void               *data,
                       struct wl_registry *registry,
                       uint32_t            name,
                       const char         *interface,
                       uint32_t            version)
{
    GwlRegistry *reg = data;

    g_return_if_fail(GWL_IS_REGISTRY(reg));

    // GwlRegistryPrivate *reg_priv;
    // reg_priv = gwl_registry_get_instance_private(reg);
    GwlObject *ret = NULL;

    if (g_strcmp0(interface, "wl_compositor") == 0) {
        // GwlCompositor compositor = gwl_compositor_new(registry, name);
        // emit signal that the compositor is added.
    }
    else if (g_strcmp0(interface, "wl_shm") == 0) {
        struct wl_shm *shm
            = wl_registry_bind(registry, name, &wl_shm_interface, version);

        ret = GWL_OBJECT(gwl_shm_new(shm));
    }

    if (ret) {
        g_signal_emit(reg,                          // instance
                      registry_signals[SIG_GLOBAL], // registered signal.
                      0,                            // GQuark
                      ret,                          // The global.
                      name,                         // the name of the instance
                      interface,                    // the name of the global.
                      version,                      // the supported version
                      NULL);
    }
}

static void
registry_handle_global_remove(void               *data,
                              struct wl_registry *registry,
                              uint32_t            name)
{
    // This space deliberately left blank
    (void) data;
    (void) registry;
    (void) name;
}
