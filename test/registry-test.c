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

#include <glib.h>
#include <gwl-display.h>
#include <gwl-registry.h>
#include <gwl-shm.h>

#include <wayland-client-protocol.h>

/* ************* Creating a registry via the display connection ****** */

typedef struct DisplayFixture {
    GwlDisplay *display;
    GError     *setup_error;
} DisplayFixture;

static void
display_fixture_setup(DisplayFixture *fixture, gconstpointer null)
{
    (void) null;

    fixture->display = gwl_display_new(NULL);
    g_assert(gwl_display_get_connected(fixture->display));
}

static void
display_fixture_teardown(DisplayFixture *fixture, gconstpointer null)
{
    (void) null;
    if (fixture->display)
        g_object_unref(G_OBJECT(fixture->display));
    if (fixture->setup_error)
        g_error_free(fixture->setup_error);
}

static void
registry_from_display(DisplayFixture *fixture, gconstpointer null)
{
    (void) null;
    GwlRegistry *registry = gwl_display_get_registry(fixture->display);
    g_assert_true(GWL_IS_REGISTRY(registry));
    gwl_display_roundtrip(fixture->display);

    g_object_unref(registry);
}

static void
on_get_globals_global(GwlRegistry *registry,
                      GwlObject   *obj,
                      guint        name,
                      const gchar *interface,
                      guint        version,
                      gpointer     data)
{
    (void) name;
    (void) version;
    int *num_globals = data;

    g_assert_true(GWL_IS_REGISTRY(registry));

    if (g_strcmp0(interface, wl_shm_interface.name) == 0) {
        g_assert_true(GWL_IS_SHM(obj));
        *num_globals += 1;
    }
}

static void
registry_get_globals(DisplayFixture *fixture, gconstpointer null)
{
    (void) null;
    int          num_globals = 0;
    GwlRegistry *registry    = gwl_display_get_registry(fixture->display);

    g_signal_connect(
        registry, "global", G_CALLBACK(on_get_globals_global), &num_globals);

    gwl_display_roundtrip(fixture->display); // Dispatch events in queue

    g_object_unref(registry);

    g_assert_cmpint(num_globals, >, 0);
}

static void
registry_get_global_instance(DisplayFixture *fixture, gconstpointer null)
{
    (void) null;
    GwlShm      *shm      = NULL;
    GwlRegistry *registry = gwl_display_get_registry(fixture->display);

    gwl_display_roundtrip(fixture->display); // Dispatch events in queue

    g_object_get(registry, "shm", &shm, NULL);

    g_assert_nonnull(shm);
    g_assert_true(GWL_IS_SHM(shm));

    // Both we and the registry has a reference.
    g_assert_cmpint(G_OBJECT(shm)->ref_count, ==, 2);

    g_object_unref(shm);
    g_object_unref(registry);
}

/* ****** Checking whether the mainloop works and signals are emitted ****** */

// Perhaps is better to do this in a separate test.
//
// typedef struct DisplayLoopFixture {
//     GwlDisplay   *display;
//     GError       *setup_error;
//     GMainContext *context;
//     GMainLoop    *loop;
//     gboolean      killed;
// } DisplayLoopFixture;
//
// static void
// display_loop_fixture_setup(DisplayLoopFixture *fixture, gconstpointer null)
// {
//     (void) null;
//     fixture->display     = NULL;
//     fixture->setup_error = NULL;
//     fixture->context     = g_main_context_new();
//     fixture->loop        = g_main_loop_new(fixture->context, FALSE);
//     fixture->killed      = 0;
//
//     fixture->display = gwl_display_new(fixture->loop, &fixture->setup_error);
// }
//
// static void
// display_loop_fixture_teardown(DisplayLoopFixture *fixture, gconstpointer
// null)
// {
//     (void) null;
//     if (fixture->loop)
//         g_main_loop_unref(fixture->loop);
//     if (fixture->context)
//         g_main_context_unref(fixture->context);
//     if (fixture->display)
//         g_object_unref(G_OBJECT(fixture->display));
//     if (fixture->setup_error)
//         g_error_free(fixture->setup_error);
// }
//
// static gboolean
// timeout_terminate_loop(gpointer data)
// {
//     DisplayLoopFixture *fixture = data;
//     // Mark the fixture to be killed via the time out.
//     fixture->killed             = TRUE;
//     g_main_loop_quit(fixture->loop);
//     return FALSE;
// }
//
// static void
// signal_terminate_loop(gpointer f)
// {
//     DisplayLoopFixture *fixture = f;
//     g_print("Terminating loop, %p!\n", fixture->loop);
//     g_main_loop_quit(fixture->loop);
// }
//
// static void
// registry_global_signal(DisplayLoopFixture *fixture, gconstpointer null)
// {
//     (void) null;
//     GwlRegistry *registry = gwl_display_get_registry(fixture->display);
//     g_assert_true(GWL_IS_REGISTRY(registry));
//
//     gulong handler = g_signal_connect(
//         registry, "global-added", (GCallback) signal_terminate_loop,
//         fixture);
//     g_debug("Handler is %lu", handler);
//     g_assert_cmpint(handler, >, 0);
//
//     GSource *source = g_timeout_source_new_seconds(1);
//     g_source_set_callback(source, timeout_terminate_loop, fixture, NULL);
//     g_source_attach(source, fixture->context);
//
//     // This should occur from the mainloop.
//     // gwl_display_roundtrip(fixture->display);
//
//     g_main_loop_run(fixture->loop);
//
//     g_assert_false(fixture->killed);
// }

int
registry_test(void)
{
    g_test_add("/GwlRegistry/registry_from_display",
               DisplayFixture,
               NULL,
               display_fixture_setup,
               registry_from_display,
               display_fixture_teardown);

    g_test_add("/GwlRegistry/get_globals",
               DisplayFixture,
               NULL,
               display_fixture_setup,
               registry_get_globals,
               display_fixture_teardown);

    g_test_add("/GwlRegistry/get_global_instance",
               DisplayFixture,
               NULL,
               display_fixture_setup,
               registry_get_global_instance,
               display_fixture_teardown);

    //     g_test_add("/GwlRegistry/signals_from_loop",
    //                DisplayLoopFixture,
    //                NULL,
    //                display_loop_fixture_setup,
    //                registry_global_signal,
    //                display_loop_fixture_teardown);

    return 0;
}
