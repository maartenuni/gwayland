project(
    'GWayland', 'c',
    version : '0.1',
    default_options : ['warning_level=3']
)

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_GWAYLAND']

subdir('src')
subdir('include')


# Make this library usable as a Meson subproject.
gwayland_dep = declare_dependency(
  include_directories: include_directories('./include'),
  link_with : gwayland_shared_lib
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'GWayland',
  filebase : 'gwayland',
  description : 'Meson sample project.',
  subdirs : 'gwayland',
  libraries : gwayland_shared_lib,
  version : '0.1',
)
