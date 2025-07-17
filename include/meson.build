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

public_headers = files(
    'gwl.h',
    'gwl-compositor.h',
    'gwl-enums.h',
    'gwl-display.h',
    'gwl-import.h',
    'gwl-registry.h',
    'gwl-shm.h',
    'gwl-shm-pool.h',
    'gwl-surface.h',
)

private_headers = files(
    'gwl-registry-private.h',
    'gwl-compositor-private.h',
    'gwl-shm-private.h',
    'gwl-shm-pool-private.h',
    'gwl-surface-private.h',
)

install_headers(public_headers, subdir : 'gwayland')
