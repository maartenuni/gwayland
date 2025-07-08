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

#ifndef GWL_DISPLAY_H
#define GWL_DISPLAY_H

#include "gwl-import.h"
#include <glib-object.h>
#include <gwl-registry.h>

G_BEGIN_DECLS

#define GWL_TYPE_DISPLAY gwl_display_get_type()
GWL_PUBLIC
G_DECLARE_DERIVABLE_TYPE(GwlDisplay, gwl_display, GWL, DISPLAY, GObject)

#define GWL_DISPLAY_ERROR gwl_display_error_quark()

enum GwlDisplayError {
    GWL_DISPLAY_ERROR_NO_CONNECTION, /** no connection */
    GWL_ERROR_FAILED,
};

struct _GwlDisplayClass {
    GObjectClass parent_class;

    void (*on_error)(GwlDisplay  *self,
                     gpointer     object_id,
                     guint        code,
                     const gchar *message);

    gpointer padding[16];
};

/*
 * Method definitions
 */

GWL_PUBLIC GwlDisplay *
gwl_display_new(const gchar *adress);

GWL_PUBLIC gint
gwl_display_get_fd(GwlDisplay *self);

GWL_PUBLIC gboolean
gwl_display_get_connected(GwlDisplay *self);

GWL_PUBLIC void
gwl_display_disconnect(GwlDisplay *self);

GWL_PUBLIC gint
gwl_display_roundtrip(GwlDisplay *self);

GWL_PUBLIC GwlRegistry *
gwl_display_get_registry(GwlDisplay *self);

G_END_DECLS

#endif
