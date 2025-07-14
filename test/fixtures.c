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

#include "fixtures.h"

/*
 * Fixture that sets up a wayland connection and lets the registry
 * to obtain its globals.
 */

void
display_registry_fixture_setup(DisplayRegistryFixture *fixture,
                               gconstpointer           null)
{
    (void) null;

    fixture->display = gwl_display_new(NULL);
    g_assert(gwl_display_get_connected(fixture->display));

    fixture->registry = gwl_display_get_registry(fixture->display);

    // Populate the registry.
    gwl_display_roundtrip(fixture->display);
}

void
display_registry_fixture_teardown(DisplayRegistryFixture *fixture,
                                  gconstpointer           null)
{
    (void) null;
    g_object_unref(fixture->registry);
    g_object_unref(fixture->display);
}
