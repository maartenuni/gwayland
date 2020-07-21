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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gwl-display.h>
#include <gwl-registry.h>
#include <glib.h>

typedef struct DisplayFixture {
    GwlDisplay* display;
    GError*     setup_error;
} DisplayFixture;

static void
display_fixture_setup(DisplayFixture* fixture, gconstpointer null)
{
    (void) null;
    fixture->display = NULL;
    fixture->setup_error = NULL;

    fixture->display = gwl_display_new(&fixture->setup_error);
}

static void
display_fixture_teardown(DisplayFixture* fixture, gconstpointer null)
{
    (void) null;
    if (fixture->display)
        g_object_unref(G_OBJECT(fixture->display));
    if (fixture->setup_error)
        g_error_free(fixture->setup_error);
}

static void
registry_from_display(DisplayFixture* fixture, gconstpointer null)
{
    (void) null;
    GwlRegistry* registry = gwl_display_get_registry(fixture->display);
    g_assert_nonnull(registry);
    gwl_display_round_trip(fixture->display);
}

int registry_test()
{
    g_test_add("/GwlRegistry/registry_from_display",
            DisplayFixture,
            NULL,
            display_fixture_setup,
            registry_from_display,
            display_fixture_teardown
            );

    return 0;    
}
