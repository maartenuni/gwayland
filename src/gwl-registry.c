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

#include <gwl-registry-private.h>
#include <gwl-registry.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

GwlRegistry *global_registry = NULL;

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

static const struct wl_registry_listener registry_listener = {
    .global        = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

/* ********* implemenation of the gwl-registry object ************/

typedef struct {
    struct wl_registry *registry;
} GwlRegistryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlRegistry, gwl_registry, G_TYPE_OBJECT)

enum { GLOBAL, GLOBAL_REMOVE, LAST_SIGNAL };

typedef enum { FIRST_PROPERTY, PROP_WL_PRIVATE, N_PROPERTIES } RegistryProperty;

static GParamSpec *registry_properties[N_PROPERTIES] = {NULL};
static guint       registry_signals[LAST_SIGNAL]     = {0};

static void
gwl_registry_set_property(GObject      *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    GwlRegistry        *self = GWL_REGISTRY(object);
    GwlRegistryPrivate *priv = gwl_registry_get_instance_private(self);

    switch ((RegistryProperty) property_id) {
    case PROP_WL_PRIVATE:
        priv->registry = g_value_get_pointer(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

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
gwl_registry_dispose(GObject *object)
{
    (void) object;
    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_registry_parent_class)->dispose(object);
}

static void
gwl_registry_finalize(GObject *object)
{
    GwlRegistryPrivate *priv
        = gwl_registry_get_instance_private(GWL_REGISTRY(object));
    g_clear_pointer(&priv->registry, wl_registry_destroy);

    G_OBJECT_CLASS(gwl_registry_parent_class)->finalize(object);
}

static void
gwl_registry_event_global(GwlRegistry        *self,
                          struct wl_registry *registry,
                          guint32             name,
                          const char         *interface,
                          uint32_t            version)
{
    g_debug("%s:%s:%d - global interface registered: %s version %d",
            __FILE__,
            __func__,
            __LINE__,
            interface,
            version);
    g_assert(GWL_IS_REGISTRY(self));
    (void) registry;
    if (g_strcmp0(wl_shm_interface.name, interface) == 0) {
        g_warning("Implement shared memory global");
    }
}

static void
gwl_registry_event_global_removed(GwlRegistry        *self,
                                  struct wl_registry *registry,
                                  guint32             name)
{
    (void) self;
    (void) registry;
    (void) name;
}

static void
gwl_registry_class_init(GwlRegistryClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose  = gwl_registry_dispose;
    object_class->finalize = gwl_registry_finalize;

    object_class->set_property = gwl_registry_set_property;
    object_class->get_property = gwl_registry_get_property;

    klass->event_global        = G_CALLBACK(gwl_registry_event_global);
    klass->event_global_remove = G_CALLBACK(gwl_registry_event_global_removed);

    /**
     * GwlRegistry:wl-private:
     *
     * A pointer to the underlying libwayland client. This property must be
     * set on construction.
     */
    registry_properties[PROP_WL_PRIVATE]
        = g_param_spec_pointer("wl-private",
                               "wl-private",
                               "The private libwayland struct wl_registry "
                               "pointer, must be set on construction",
                               G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(
        object_class, N_PROPERTIES, registry_properties);

    // obj_properties);
    registry_signals[GLOBAL]
        = g_signal_new("global-added",
                       G_TYPE_FROM_CLASS(klass),
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GwlRegistryClass, event_global),
                       NULL,
                       NULL,
                       NULL,
                       G_TYPE_NONE,
                       4,
                       G_TYPE_OBJECT, // The object belonging to this class
                       G_TYPE_UINT,   // The numeric name of the wayland object
                       G_TYPE_STRING, // The name of the interface
                       G_TYPE_UINT);  // version of the interface
}

/* * library private api * */

/*
 * Install listener and callback's to obtain the globals.
 */
static void
gwl_registry_bind_listener(GwlRegistry *self)
{
    GwlRegistryPrivate *priv = gwl_registry_get_instance_private(self);
    wl_registry_add_listener(priv->registry, &registry_listener, self);
}

/**
 * @private
 *
 * Called by GwlDisplay in order to obtain the wayland server globals
 *
 */
GwlRegistry *
gwl_registry_new(struct wl_registry *registry)
{
    g_return_val_if_fail(registry != NULL, NULL);

    return g_object_new(GWL_TYPE_REGISTRY, "wl-private", registry, NULL);
}

/** public API **/

/* * Use the registry to bind the globals. * */

static void
registry_handle_global(void               *data,
                       struct wl_registry *registry,
                       uint32_t            name,
                       const char         *interface,
                       uint32_t            version)
{
    GwlRegistry        *reg = data;
    GwlRegistryPrivate *reg_priv;

    g_return_if_fail(GWL_IS_REGISTRY(reg) || G_IS_OBJECT(data));

    reg_priv = gwl_registry_get_instance_private(reg);
    g_signal_emit(reg,                      // instance
                  registry_signals[GLOBAL], // registered signal.
                  0,                        // GQuark
                  registry,  // the libwaylandclient registry instance
                  name,      // the name of the instance
                  interface, // the name of the global.
                  version,   // the supported version
                  NULL);

    if (g_strcmp0(interface, "wl_compositor") == 0) {
        // GwlCompositor compositor = gwl_compositor_new(registry, name);
        // emit signal that the compositor is added.
    }
    else if (g_strcmp0(interface, "wl_shm") == 0) {
        // GwlShm shared_mem = gwl_shm_new(registry, name);
        // emit signal that the shm is added.
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
