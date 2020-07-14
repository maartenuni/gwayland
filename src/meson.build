


gwayland_srcs = [
    'gwl-display.c',
]

gwayland_shared_lib = shared_library(
    'gwayland',
    gwayland_srcs,
    install : true,
    c_args : lib_args,
    gnu_symbol_visibility : 'hidden',
    include_directories : '../include',
    dependencies : [glib_dep, gobject_dep, wayland_dep],
)

