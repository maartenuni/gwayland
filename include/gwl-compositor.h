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

#pragma once

#include "gwl-enums.h"
#include "gwl-import.h"
#include "gwl-object.h"
#include "gwl-surface.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GWL_TYPE_COMPOSITOR gwl_compositor_get_type()
GWL_PUBLIC
G_DECLARE_DERIVABLE_TYPE(
    GwlCompositor, gwl_compositor, GWL, COMPOSITOR, GwlObject)

struct _GwlCompositorClass {
    GwlObjectClass parent_class;

    gpointer padding[16];
};

GwlSurface *
gwl_compositor_get_surface(GwlCompositor *self);

G_END_DECLS
