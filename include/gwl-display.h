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

#ifndef GWL_DISPLAY_H
#define GWL_DISPLAY_H

#include <glib-object.h>
#include <gwl-registry.h>
#include "gwl-import.h"

G_BEGIN_DECLS

#define GWL_TYPE_DISPLAY gwl_display_get_type()
GWL_PUBLIC
G_DECLARE_DERIVABLE_TYPE(GwlDisplay, gwl_display, GWL, DISPLAY, GObject)

#define GWL_DISPLAY_ERROR gwl_display_error_quark()

enum GwlDisplayError {
    GWL_DISPLAY_ERROR_NO_CONNECTION /** no connection */
};

struct _GwlDisplayClass {
    GObjectClass parent_class;

    gpointer padding[16];
};

/*
 * Method definitions
 */

GWL_PUBLIC GwlDisplay*
gwl_display_new(GMainLoop* loop, GError** error);

GWL_PUBLIC GwlDisplay*
gwl_display_new_address(GMainLoop* loop, const gchar* server, GError** error);

GWL_PUBLIC void
gwl_display_round_trip(GwlDisplay* display);

GWL_PUBLIC GwlRegistry*
gwl_display_get_registry(GwlDisplay* display);

G_END_DECLS

#endif
