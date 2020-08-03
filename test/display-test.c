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

#include <glib.h>
#include <glib/gi18n.h>

#include <gwl-display.h>

static void 
display_new()
{
    GError* error = NULL;
    int fd = 0;
    GwlDisplay* display = gwl_display_new(NULL, &error);
    g_assert_nonnull(display);
    g_assert_null(error);

    g_object_get(G_OBJECT(display),
            "fd", &fd,
            NULL
            );
    g_assert_true(fd >= 0);

    g_object_unref(G_OBJECT(display));
}

static void 
display_new_address()
{
    GError* error = NULL;
    const char* address = "wayland-0";
    int fd = 0;
    GwlDisplay* display = gwl_display_new_address(NULL, address, &error);
    g_assert_nonnull(display);
    g_assert_null(error);

    g_object_get(G_OBJECT(display),
            "fd", &fd,
            NULL
            );
    g_assert_true(fd >= 0);

    g_object_unref(G_OBJECT(display));
}

static void 
display_wrong_address()
{
    GError* error = NULL;
    char address[1024];
    int random = g_test_rand_int();

    g_snprintf(address, sizeof(address), "woopsie%d", random);

    GwlDisplay* display = gwl_display_new_address(NULL, address, &error);
    g_assert_null(display);
    g_assert_nonnull(error);

    g_error_free(error);
}


int display_test()
{
    g_test_add_func("/GwlDisplay/display_new", display_new);
    g_test_add_func("/GwlDisplay/display_new_address", display_new_address);
    g_test_add_func("/GwlDisplay/display_wrong_address", display_wrong_address);

    return 0;
}
