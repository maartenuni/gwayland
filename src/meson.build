#
# GWayland library gobject wrappers around waylandclient library.
# Copyright (C) 2020 Maarten Duijndam
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

enum_files = gnome.mkenums_simple (
    'gwl-enum-types',
    header_prefix: '#include "gwl-import.h"',
    decorator: 'GWL_PUBLIC',
    sources: public_headers,
)

gwayland_srcs = files(
    'gwl-display.c',
    'gwl-compositor.c',
    'gwl-object.c',
    'gwl-registry.c',
    'gwl-shm.c',
    'gwl-shm-pool.c',
    'gwl-surface.c',
)

gwayland_shared_lib = shared_library(
    'gwayland',
    gwayland_srcs + enum_files,
    install : true,
    c_args : lib_args,
    gnu_symbol_visibility : 'hidden',
    include_directories : ['../include', '.', '../src'],
    dependencies : [glib_dep, gobject_dep, gio_dep, wayland_dep],
)

