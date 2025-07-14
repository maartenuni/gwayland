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

test_sources = files([
    'gwayland-test.c',
    'tests.h',
    'fixtures.c',
    'display-test.c',
    'registry-test.c',
    'shm-test.c',
])

test_deps = [glib_dep, wayland_dep]

test_exe = executable(
    'gwayland_test',
    test_sources,
    dependencies: gwayland_dep,
    include_directories: include_directories('../src/'),
)

test(
    'gwayland',
    test_exe,
    env : [
        'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
        'G_TEST_BUILDDIR=@0@'.format(meson.current_source_dir())
    ],
    protocol: 'tap'
)

