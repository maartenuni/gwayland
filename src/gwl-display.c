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

#include <errno.h>
#include <gwl-display.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

// The private headers should include the public.
#include "gwl-registry-private.h"

static void
gwl_display_set_address(GwlDisplay *self, const gchar *address);

static void
gwl_display_set_fd(GwlDisplay *self, int fd);

static void
event_error(void              *data,
            struct wl_display *display,
            gpointer           object_id,
            guint32            code,
            const gchar       *message);

struct wl_display_listener listener = {
    .error = event_error,
};

typedef struct {
    GwlRegistry *registry;
    GMainLoop   *loop;
    GIOChannel  *io_channel;
    guint        source_id;
} GwlDisplayPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlDisplay, gwl_display, GWL_TYPE_OBJECT)

// clang-format off
G_DEFINE_QUARK(gwl-display-error-quark, gwl_display_error)

// clang-format on

typedef enum {
    DISPLAY_SERVER_ADRESS = 1,
    DISPLAY_REGISTRY,
    DISPLAY_FD,
    N_PROPERIES
} DisplayProperty;

static GParamSpec *obj_properties[N_PROPERIES] = {NULL};

enum DisplaySignals { SIGNAL_ERROR, NUM_SIGNALS };

guint32 display_signals[NUM_SIGNALS] = {0};

static gboolean
gwl_display_process_events(GIOChannel  *channel,
                           GIOCondition event,
                           gpointer     data)
{
    (void) channel;
    (void) event;
    g_assert(event & G_IO_IN || event & G_IO_OUT);
    GwlDisplay        *display = data;
    GwlObject         *gwlobj  = GWL_OBJECT(display);
    struct wl_display *wl_disp = gwl_object_get(gwlobj);

    g_return_val_if_fail(GWL_IS_DISPLAY(display), FALSE);

    g_debug(
        "Event & IN = %d\tEvent & OUT = %d", event & G_IO_IN, event & G_IO_OUT);

    if (event & G_IO_IN)
        wl_display_dispatch(wl_disp);
    if (event & G_IO_OUT)
        wl_display_flush(wl_disp);

    return TRUE;
}

static gboolean
gwl_display_attach_to_main_loop(GwlDisplay *display,
                                GMainLoop  *loop,
                                int         connection_fd)
{
    GwlDisplayPrivate *priv = gwl_display_get_instance_private(display);
    priv->io_channel        = g_io_channel_unix_new(connection_fd);
    int source_id;
    if (!priv->io_channel)
        return FALSE;

    GSource *source = g_io_create_watch(priv->io_channel, G_IO_IN | G_IO_OUT);
    g_source_set_callback(
        source, G_SOURCE_FUNC(gwl_display_process_events), display, NULL);

    if (loop)
        source_id = g_source_attach(source, g_main_loop_get_context(loop));
    else
        source_id = g_source_attach(source, NULL);

    if (source_id)
        priv->source_id = source_id;
    else
        goto fail;

    return TRUE;

fail:

    g_source_destroy(source);
    g_io_channel_unref(priv->io_channel);
    priv->io_channel = NULL;

    return FALSE;
}

static void
gwl_display_set_property(GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
    GwlDisplay *self = GWL_DISPLAY(object);

    switch ((DisplayProperty) property_id) {
    case DISPLAY_SERVER_ADRESS:
        gwl_display_set_address(self, g_value_get_string(value));
        break;
    case DISPLAY_FD:
        gwl_display_set_fd(self, g_value_get_int(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_display_get_property(GObject    *object,
                         guint       property_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
    GwlDisplay *self = GWL_DISPLAY(object);

    switch (property_id) {
    case DISPLAY_FD:
        g_value_set_int(value, gwl_display_get_fd(self));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gwl_display_init(GwlDisplay *self)
{
    (void) self;
    // GwlDisplayPrivate* priv = gwl_display_get_instance_private(self);
}

static void
gwl_display_dispose(GObject *object)
{
    g_assert(GWL_IS_DISPLAY(object));

    GwlDisplay        *self = GWL_DISPLAY(object);
    GwlDisplayPrivate *priv
        = gwl_display_get_instance_private(GWL_DISPLAY(object));

    g_clear_object(&priv->registry);

    if (gwl_display_get_connected(self)) {
        gwl_display_disconnect(self);
    }

    g_clear_pointer(&priv->io_channel, g_io_channel_unref);

    G_OBJECT_CLASS(gwl_display_parent_class)->dispose(object);
}

static void
gwl_display_finalize(GObject *object)
{
    GwlDisplayPrivate *priv
        = gwl_display_get_instance_private(GWL_DISPLAY(object));

    if (priv->source_id) {
        g_source_remove(priv->source_id);
        priv->source_id = 0;
    }

    G_OBJECT_CLASS(gwl_display_parent_class)->finalize(object);
}

static void
display_on_error(GwlDisplay  *self,
                 gpointer     object_id,
                 guint        code,
                 const gchar *message)
{
    g_warning("Error on object %p wl object:%p, code %u: %s\n",
              (gpointer) self,
              (gpointer) object_id,
              code,
              message);
}

static GDestroyNotify
display_get_cleanup_func(void)
{
    return (GDestroyNotify) wl_display_disconnect;
}

static void
gwl_display_class_init(GwlDisplayClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->dispose  = gwl_display_dispose;
    object_class->finalize = gwl_display_finalize;

    object_class->set_property = gwl_display_set_property;
    object_class->get_property = gwl_display_get_property;

    GwlObjectClass *gwl_object_class   = GWL_OBJECT_CLASS(klass);
    gwl_object_class->get_cleanup_func = display_get_cleanup_func;

    klass->on_error = display_on_error;

    obj_properties[DISPLAY_FD]
        = g_param_spec_int("fd",
                           "Fd",
                           "The fd of the filedescriptor of the connection.",
                           G_MININT,
                           G_MAXINT,
                           -1,
                           G_PARAM_READWRITE);

    obj_properties[DISPLAY_SERVER_ADRESS] = g_param_spec_string(
        "server-address",
        "Server-Address",
        "The address of the server e.g. NULL or wayland-0",
        NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    obj_properties[DISPLAY_REGISTRY] = g_param_spec_object(
        "registry",
        "Registry",
        "Get a copy to the registry to retrieve global wayland objects",
        GWL_TYPE_REGISTRY,
        G_PARAM_READABLE);

    g_object_class_install_properties(
        object_class, N_PROPERIES, obj_properties);

    display_signals[SIGNAL_ERROR]
        = g_signal_new("error",
                       GWL_TYPE_DISPLAY,
                       G_SIGNAL_RUN_LAST,
                       G_STRUCT_OFFSET(GwlDisplayClass, on_error),
                       NULL,
                       NULL,
                       NULL,
                       G_TYPE_NONE,
                       3,
                       G_TYPE_POINTER,
                       G_TYPE_UINT,
                       G_TYPE_STRING);
}

/* ************** public functions ***************** */

GwlDisplay *
gwl_display_new(const gchar *server_address)
{
    GwlDisplay *ret;

    ret = g_object_new(
        GWL_TYPE_DISPLAY, "server_address", server_address, NULL);

    return ret;
}

GwlDisplay *
gwl_display_new_fd(gint fd)
{
    GwlDisplay *ret;
    g_return_val_if_fail(fd >= 0, NULL);

    ret = g_object_new(GWL_TYPE_DISPLAY, "fd", fd, NULL);

    return ret;
}

void
gwl_display_disconnect(GwlDisplay *self)
{
    g_return_if_fail(GWL_IS_DISPLAY(self));

    // The GwlObject will call wl_display_disconnect on the wrapped instance
    gwl_object_cleanup(GWL_OBJECT(self));
}

gint
gwl_display_get_fd(GwlDisplay *self)
{
    g_return_val_if_fail(GWL_IS_DISPLAY(self), -1);
    if (!gwl_display_get_connected(self))
        return -1;

    return wl_display_get_fd(gwl_object_get(GWL_OBJECT(self)));
}

void
gwl_display_set_fd(GwlDisplay *self, gint fd)
{
    g_return_if_fail(GWL_IS_DISPLAY(self));

    struct wl_display *disp = wl_display_connect_to_fd(fd);

    gwl_object_set(GWL_OBJECT(self), disp);
}

void
gwl_display_set_address(GwlDisplay *self, const gchar *address)
{
    g_return_if_fail(GWL_IS_DISPLAY(self));

    struct wl_display *disp = wl_display_connect(address);

    gwl_object_set(GWL_OBJECT(self), disp);
}

gboolean
gwl_display_get_connected(GwlDisplay *self)
{
    g_return_val_if_fail(GWL_IS_DISPLAY(self), FALSE);

    return gwl_object_get(GWL_OBJECT(self)) != NULL;
}

gint
gwl_display_roundtrip(GwlDisplay *self)
{
    g_return_val_if_fail(GWL_IS_DISPLAY(self), -1);
    g_return_val_if_fail(gwl_display_get_connected(self), -1);

    return wl_display_roundtrip(gwl_object_get(GWL_OBJECT(self)));
}

/**
 * gwl_display_get_registry:
 *
 * Construct a registry, if none was constructed. Otherwise
 * obtain a reference to the existing instance. Wayland doesn't like
 * multiple registries to be created, hence you'll get a reference to the same
 * one each time.
 * This is the way to get a registry. Which mimics libwaylands way of
 * doing this.
 *
 * Returns:(transfer full): A reference to this displays registry.
 */
GwlRegistry *
gwl_display_get_registry(GwlDisplay *self)
{
    GwlDisplayPrivate *priv;
    g_return_val_if_fail(GWL_IS_DISPLAY(self), NULL);

    priv = gwl_display_get_instance_private(self);

    if (!priv->registry) {
        struct wl_registry *registry
            = wl_display_get_registry(gwl_object_get(GWL_OBJECT(self)));
        if (!registry)
            return NULL;
        priv->registry = gwl_registry_new(registry);
    }

    return g_object_ref(priv->registry);
}

/* ************ events ******************/

static void
event_error(void              *data,
            struct wl_display *display,
            gpointer           object_id,
            guint32            code,
            const gchar       *message)
{
    (void) display;
    GwlDisplay *self = GWL_DISPLAY(data);
    g_signal_emit(
        self, display_signals[SIGNAL_ERROR], 0, object_id, code, message);
}
