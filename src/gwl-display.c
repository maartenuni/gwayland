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

#include "gwl-display.h"
#include "glibconfig.h"
#include <wayland-client-core.h>
#include <wayland-client.h>
#include <errno.h>

typedef struct {
    struct wl_display* display;
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
    (void)object;
    // here we should drop references on other gobjects.
}

static void
gwl_display_finalize(GObject* object)
{
    GwlDisplayPrivate* priv = gwl_display_get_instance_private(
            GWL_DISPLAY(object)
            );
    if (priv->display)
        wl_display_disconnect(priv->display);
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
gwl_display_new(GError** error)
{
    GwlDisplay* ret = g_object_new(GWL_TYPE_DISPLAY, NULL);

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
    return ret;
}

GwlDisplay*
gwl_display_new_address(const gchar* server_address, GError** error)
{
    GwlDisplay* ret = g_object_new(
            GWL_TYPE_DISPLAY,
            "server_address", server_address,
            NULL);
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
    return ret;
}

