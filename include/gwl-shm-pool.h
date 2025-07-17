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

#include "gwl-import.h"
#include "gwl-object.h"
#include "gwl-shm-pool.h"

#include <glib-object.h>
#include <gwl-enums.h>

G_BEGIN_DECLS

#define GWL_TYPE_SHM_POOL gwl_shm_pool_get_type()
GWL_PUBLIC
G_DECLARE_DERIVABLE_TYPE(GwlShmPool, gwl_shm_pool, GWL, SHM_POOL, GwlObject)

struct _GwlShmPoolClass {
    GwlObjectClass parent_class;

    gpointer padding[16];
};

void
gwl_shm_memory_set_size(GwlShmPool *self, gint size);

// Temporary until GwlBuffer is implemented
typedef struct GwlBuffer GwlBuffer;

GwlBuffer *
gwl_shm_pool_get_buffer(GwlShmPool  *self,
                        gint         offset,
                        gint         width,
                        gint         height,
                        gint         stride,
                        GwlShmFormat format);

void
gwl_shm_pool_resize(GwlShmPool *self, gint new_size, GError **error);

G_END_DECLS
