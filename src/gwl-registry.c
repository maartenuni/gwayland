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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gwl-registry.h>
#include <gwl-registry-private.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

GwlRegistry* global_registry = NULL;

enum {
    WL_COMPOSITOR,
    WL_SHM,
};

static void registry_handle_global(
        void                   *data,
        struct wl_registry     *registry,
        uint32_t                name,
        const char             *interface,
        uint32_t                version
        );

static void
registry_handle_global_remove(
        void                   *data,
        struct wl_registry     *registry,
        uint32_t                name
        );


static const struct wl_registry_listener
registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

/* ********* implemenation of the gwl-registry object ************/

typedef struct {
    struct wl_registry* registry;
} GwlRegistryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlRegistry, gwl_registry, G_TYPE_OBJECT)
//G_DEFINE_QUARK(gwl-registry-error-quark, gwl_registry_error)

enum {
    GLOBAL,
    GLOBAL_REMOVE,
    LAST_SIGNAL
};

enum {
    FIRST_PROPERTY,
    N_PROPERTIES
};

static GParamSpec * obj_properties[N_PROPERTIES] = {NULL};
static guint registry_signals[LAST_SIGNAL] = {0};

static void
gwl_registry_set_property(GObject        *object,
                          guint           property_id,
                          const GValue   *value,
                          GParamSpec     *pspec
                          )
{
    GwlRegistry* self = GWL_REGISTRY(object);
    (void) self;
    //GwlRegistryPrivate* priv = gwl_registry_get_instance_private(self);
    (void) value;

    switch(property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
gwl_registry_get_property(GObject       *object,
                          guint          property_id,
                          GValue        *value,
                          GParamSpec    *pspec
                          )
{
    GwlRegistry* self = GWL_REGISTRY(object);
    //GwlRegistryPrivate* priv = gwl_registry_get_instance_private(self);
    (void) value;
    (void) self;

    switch(property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
gwl_registry_init(GwlRegistry* self)
{
    (void) self;
    //everything is set to zero
}

static void
gwl_registry_dispose(GObject* object)
{
    (void)object;
    // here we should drop references on other gobjects.
    G_OBJECT_CLASS(gwl_registry_parent_class)->dispose(object);
}

static void
gwl_registry_finalize(GObject* object)
{
    GwlRegistryPrivate* priv = gwl_registry_get_instance_private(
            GWL_REGISTRY(object)
            );
    if (priv->registry)
        wl_registry_destroy(priv->registry);

    G_OBJECT_CLASS(gwl_registry_parent_class)->finalize(object);
}

static void
gwl_registry_event_global(
        GwlRegistry*        self,
        struct wl_registry *registry,
        guint32             name,
        const char         *interface,
        uint32_t            version
        )
{
    g_debug("%s:%s:%d", __FILE__, __func__, __LINE__);
}
        
static void
gwl_registry_event_global_removed(
        GwlRegistry*        self,
        struct wl_registry *registry,
        guint32             name
        )
{
}


static void
gwl_registry_class_init(GwlRegistryClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->dispose       = gwl_registry_dispose;
    object_class->finalize      = gwl_registry_finalize;
    
//    object_class->set_property  = gwl_registry_set_property;
//    object_class->get_property  = gwl_registry_get_property;

    klass->event_global         = G_CALLBACK(gwl_registry_event_global);
    klass->event_global_remove  = G_CALLBACK(gwl_registry_event_global_removed);

    // g_object_class_install_properties(object_class, N_PROPERIES, obj_properties);
    registry_signals[GLOBAL] = g_signal_new(
            "global-added",
            G_TYPE_FROM_CLASS(klass),
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(GwlRegistryClass, event_global),
            NULL,
            NULL,
            NULL,
            G_TYPE_NONE,
            4,
            G_TYPE_POINTER,
            G_TYPE_UINT,
            G_TYPE_STRING,
            G_TYPE_UINT
            );
    g_assert(registry_signals[GLOBAL] != 0);
    //g_assert(registry_signals[GLOBAL_REMOVE] != 0);

}

/*
 * Install listener and callback's to obtain the globals.
 */
static void
gwl_registry_bind_listener(GwlRegistry* self)
{
    GwlRegistryPrivate* priv = gwl_registry_get_instance_private(self);
    wl_registry_add_listener(priv->registry, &registry_listener, self);
}

/* * library private api * */

/**
 * @private
 *
 * Called by GwlDisplay in order to obtain the wayland server globals
 *
 */
GwlRegistry*
gwl_registry_new(struct wl_registry* registry)
{
    GwlRegistry* ret = g_object_new(GWL_TYPE_REGISTRY, NULL);
    if (!ret)
        return ret;
    GwlRegistryPrivate* priv = gwl_registry_get_instance_private(ret);
    priv->registry = registry;
    gwl_registry_bind_listener(ret);
    return ret;
}

/** public API **/

/* * Use the registry to bind the globals. * */

static void
registry_handle_global(
        void                   *data,
        struct wl_registry     *registry,
        uint32_t                name,
        const char             *interface,
        uint32_t                version
        )
{
    GwlRegistry* reg = data;
    GwlRegistryPrivate* reg_priv;

    g_return_if_fail(GWL_IS_REGISTRY(reg) || G_IS_OBJECT(data));

    reg_priv = gwl_registry_get_instance_private(reg);
    g_debug("emitting signal: %s interface", interface);
    g_signal_emit(
            reg, // instance
            registry_signals[GLOBAL], // registered signal.
            0, // GQuark
            registry,   // the libwaylandclient registry instance
            name,       // the name of the instance
            interface,  // the name of the global.
            version,    // the supported version
            NULL
            );

    if (strcmp(interface, "wl_compositor") == 0) {
        // GwlCompositor compositor = gwl_compositor_new(registry, name);
        // emit signal that the compositor is added.
    }
    else if (strcmp(interface, "wl_shm") == 0) {
        // GwlShm shared_mem = gwl_shm_new(registry, name);
        // emit signal that the shm is added.
    }
}

static void
registry_handle_global_remove(
        void                   *data,
        struct wl_registry     *registry,
        uint32_t                name
        )
{
    // This space deliberately left blank
    (void) data;
    (void) registry;
    (void) name;
}
