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

#include <gwl-display.h>
#include <gwl-registry.h>
#include <gwl-registry-private.h>
#include "glibconfig.h"
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <errno.h>

typedef struct {
    struct wl_display*  display;
    GwlRegistry*        registry;
    GMainLoop*          loop;
    GIOChannel*         io_channel;
    guint               source_id;
} GwlDisplayPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GwlDisplay, gwl_display, G_TYPE_OBJECT)
G_DEFINE_QUARK(gwl-display-error-quark, gwl_display_error)

enum {
    GWL_DISPLAY_SERVER_ADRESS = 1,
    //GWL_DISPLAY_REGISTRY,
    GWL_DISPLAY_FD,
    N_PROPERIES
};

static GParamSpec * obj_properties[N_PROPERIES] = {NULL};

static gboolean
gwl_display_process_events(
        GIOChannel* channel,
        GIOCondition event,
        gpointer data
        )
{
    (void) channel;
    (void) event;
    g_assert(event == G_IO_IN);
    GwlDisplay* display = data;
    GwlDisplayPrivate* priv;

    g_return_val_if_fail(GWL_IS_DISPLAY(display), FALSE);

    priv = gwl_display_get_instance_private(display);

    wl_display_dispatch(priv->display);

    return TRUE;
}

static gboolean
gwl_display_attach_to_main_loop(GwlDisplay* display, GMainLoop* loop, int connection_fd)
{
    GwlDisplayPrivate* priv = gwl_display_get_instance_private(display);
    priv->io_channel = g_io_channel_unix_new(connection_fd);
    int source_id;
    if (!priv->io_channel)
        return FALSE;
    
    GSource* source = g_io_create_watch(
            priv->io_channel,
            G_IO_IN
            );
    g_source_set_callback(
            source,
            (GSourceFunc)(&gwl_display_process_events),
            display,
            NULL
            );
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
gwl_display_set_property(GObject        *object,
                         guint           property_id,
                         const GValue   *value,
                         GParamSpec     *pspec
                         )
{
    GwlDisplay* self = GWL_DISPLAY(object);
    GwlDisplayPrivate* priv = gwl_display_get_instance_private(self);

    switch(property_id) {
        case GWL_DISPLAY_SERVER_ADRESS:
            {
                const gchar* adress = g_value_get_string(value);
                priv->display = wl_display_connect(adress);
                if (!priv->display)
                    return;
                struct wl_registry* registry = wl_display_get_registry(
                        priv->display
                        );
                priv->registry = gwl_registry_new(registry);
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
gwl_display_get_property(GObject       *object,
                         guint          property_id,
                         GValue        *value,
                         GParamSpec    *pspec
                         )
{
    GwlDisplay* self = GWL_DISPLAY(object);
    GwlDisplayPrivate* priv = gwl_display_get_instance_private(self);
    switch(property_id) {
        case GWL_DISPLAY_FD:
            {
                int fd = -1;
                if (priv->display)
                    fd = wl_display_get_fd(priv->display);
                g_value_set_int(value, fd);
            }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
gwl_display_init(GwlDisplay* self)
{
    (void) self;
    //GwlDisplayPrivate* priv = gwl_display_get_instance_private(self);
}

static void
gwl_display_dispose(GObject* object)
{
    g_assert(GWL_IS_DISPLAY(object));
    GwlDisplayPrivate* priv = gwl_display_get_instance_private(
            GWL_DISPLAY(object)
            );
    if (priv->registry)
        g_object_unref(priv->registry);
    if (priv->io_channel)
        g_io_channel_unref(priv->io_channel);
}

static void
gwl_display_finalize(GObject* object)
{
    GwlDisplayPrivate* priv = gwl_display_get_instance_private(
            GWL_DISPLAY(object)
            );
    if (priv->display)
        wl_display_disconnect(priv->display);
    if (priv->source_id) {
        g_source_remove(priv->source_id);
        priv->source_id = 0;
    }
}

static void
gwl_display_class_init(GwlDisplayClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->dispose       = gwl_display_dispose;
    object_class->finalize      = gwl_display_finalize;
    
    object_class->set_property  = gwl_display_set_property;
    object_class->get_property  = gwl_display_get_property;

    obj_properties[GWL_DISPLAY_FD] = 
        g_param_spec_int(
                "fd",
                "Fd",
                "The fd of the filedescriptor of the connection.",
                G_MININT,
                G_MAXINT,
                -1,
                G_PARAM_READABLE
                );

    obj_properties[GWL_DISPLAY_SERVER_ADRESS] = 
        g_param_spec_string(
                "server-address",
                "Server-Address",
                "The adress of the server e.g. NULL or wayland-0",
                NULL,
                G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY
                );

//    obj_properties[GWL_DISPLAY_REGISTRY] = 
//        g_param_spec_object(
//                "registry",
//                "Registry",
//                "The global registry object to obtain other wayland proxies.",
//                GWL_TYPE_REGISTRY,
//                G_PARAM_READ
//                );

    g_object_class_install_properties(object_class, N_PROPERIES, obj_properties);

}


/* ************** public functions ***************** */

GwlDisplay*
gwl_display_new_address(GMainLoop* loop, const gchar* server_address, GError** error)
{
    GwlDisplay* ret;

    g_return_val_if_fail(*error == NULL, NULL);
    
    ret = g_object_new(
            GWL_TYPE_DISPLAY,
            "server_address", server_address,
            NULL
            );
    int fd;
    g_object_get(G_OBJECT(ret),
            "fd", &fd,
            NULL
            );

    if (fd < 0) {
        g_set_error(
                error,
                GWL_DISPLAY_ERROR,
                GWL_DISPLAY_ERROR_NO_CONNECTION,
                "Unable to connect to server: %s\n",
                g_strerror(errno)
                );
        g_object_unref(G_OBJECT(ret));
        return NULL;
    }

    gwl_display_attach_to_main_loop(ret, loop, fd);
    return ret;
}

GwlDisplay*
gwl_display_new(GMainLoop* loop, GError** error)
{
    return gwl_display_new_address(loop, NULL, error);
}

void
gwl_display_round_trip (GwlDisplay* self)
{
    g_return_if_fail(GWL_IS_DISPLAY(self));

    GwlDisplayPrivate* priv = gwl_display_get_instance_private(self);
    wl_display_roundtrip(priv->display);
}

GwlRegistry*
gwl_display_get_registry(
        GwlDisplay* display
        )
{
    GwlDisplayPrivate* priv;
    g_return_val_if_fail(GWL_IS_DISPLAY(display), NULL);

    priv = gwl_display_get_instance_private(display);

    return priv->registry;
}
