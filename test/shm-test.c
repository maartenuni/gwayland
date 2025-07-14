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

#include "fixtures.h"

#include <glib.h>
#include <gwl-display.h>
#include <gwl-registry.h>
#include <gwl-shm.h>

#include <wayland-client-protocol.h>

/* ************* Creating a registry via the display connection ****** */

static void
format_discovered(GwlShm *shm, GwlShmFormat format, gpointer data)
{
    gint *counter = data;
    GWL_IS_SHM(shm);
    *counter += 1;

    // // to check the names
    // gchar *name = g_enum_to_string(GWL_TYPE_SHM_FORMAT, format);
    // g_print("Discovered format: %s\n", name);
    // g_free(name);
}

static void
registry_from_display(DisplayRegistryFixture *fixture, gconstpointer null)
{
    (void) null;
    gint count = 0;

    GwlShm *shm = gwl_registry_get_shm(fixture->registry);
    g_assert_true(GWL_IS_SHM(shm));

    g_signal_connect(shm, "format", G_CALLBACK(&format_discovered), &count);

    gwl_display_roundtrip(fixture->display);

    g_assert_cmpint(count, >, 0);

    g_object_unref(shm);
}

int
shm_test(void)
{
    g_test_add("/GwlShm/SignalEmission",
               DisplayRegistryFixture,
               NULL,
               display_registry_fixture_setup,
               registry_from_display,
               display_registry_fixture_teardown);

    return 0;
}
