#pragma once
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_GWAYLAND
    #define GWAYLAND_PUBLIC __declspec(dllexport)
  #else
    #define GWAYLAND_PUBLIC __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_GWAYLAND
      #define GWAYLAND_PUBLIC __attribute__ ((visibility ("default")))
  #else
      #define GWAYLAND_PUBLIC
  #endif
#endif

int GWAYLAND_PUBLIC gwa_func();

