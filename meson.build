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

project(
    'GWayland', 'c',
    version : '0.1',
    default_options : ['warning_level=3']
)

glib_dep = dependency('glib-2.0')
gobject_dep = dependency('gobject-2.0')
wayland_dep = dependency('wayland-client')

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_GWL']

subdir('src')
subdir('include')

# Make this library usable as a Meson subproject.
gwayland_dep = declare_dependency(
    include_directories: include_directories('./include'),
    dependencies: [glib_dep, gobject_dep, wayland_dep],
    link_with : gwayland_shared_lib
)

subdir('test')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'GWayland',
  filebase : 'gwayland',
  description : 'Meson sample project.',
  subdirs : 'gwayland',
  libraries : gwayland_shared_lib,
  version : '0.1',
)
