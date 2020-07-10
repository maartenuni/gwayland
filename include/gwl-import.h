#pragma once
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_GWL
    #define GWL_PUBLIC __declspec(dllexport)
  #else
    #define GWL_PUBLIC __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_GWL
      #define GWL_PUBLIC __attribute__ ((visibility ("default")))
  #else
      #define GWL_PUBLIC
  #endif
#endif


