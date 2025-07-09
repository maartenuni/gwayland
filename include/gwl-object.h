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

#include "gwl-import.h"

#include <glib-object.h>

#pragma once

G_BEGIN_DECLS

#define GWL_TYPE_OBJECT gwl_object_get_type()

G_DECLARE_DERIVABLE_TYPE(GwlObject, gwl_object, GWL, OBJECT, GObject)

struct _GwlObjectClass {
    GObjectClass parent_class;

    GDestroyNotify (*get_cleanup_func)(void);

    gpointer padding[16];
};

void
gwl_object_set(GwlObject *self, gpointer obj);

gpointer
gwl_object_get(GwlObject *self);

void
gwl_object_cleanup(GwlObject *self);

G_END_DECLS
