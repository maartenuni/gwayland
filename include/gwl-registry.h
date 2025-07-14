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

#pragma once

#include "gwl-import.h"
#include "gwl-object.h"
#include "gwl-shm.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GWL_TYPE_REGISTRY gwl_registry_get_type()
GWL_PUBLIC
G_DECLARE_DERIVABLE_TYPE(GwlRegistry, gwl_registry, GWL, REGISTRY, GwlObject)

//#define GWL_REGISTRY_ERROR gwl_registry_error_quark()

// enum GwlRegistryError {
//     GWL_REGISTRY_ERROR_NO_CONNECTION /** no connection */
// };

struct _GwlRegistryClass {
    GwlObjectClass parent_class;

    void (*event_global)(GwlRegistry *self,
                         GwlObject   *object,
                         guint        name,
                         const gchar *interface_name,
                         guint        version);

    void (*event_global_remove)(GwlRegistry *self, guint name);

    gpointer padding[16];
};

GWL_PUBLIC GwlShm *
gwl_registry_get_shm(GwlRegistry *self);

G_END_DECLS
