
test_sources = [
    'gwayland-test.c',
    'tests.h',
    'display-test.c',
]

test_deps = [glib_dep, wayland_dep]

test_exe = executable(
    'gwayland_test',
    test_sources,
    dependencies: gwayland_dep,
    link_with : gwayland_shared_lib
)

test('gwayland', test_exe)

