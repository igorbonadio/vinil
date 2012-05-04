#ifndef VINIL_CROSSPLATFORM_H_
#define VINIL_CROSSPLATFORM_H_

#ifndef _WIN32
  #define VINILAPI
#else
  #ifdef __cplusplus
    #define VINILAPI extern "C" __declspec(dllexport)
  #else
    #define VINILAPI __declspec(dllexport)
  #endif
#endif

#ifdef _WIN32
  #include <windows.h>
#else
  #include <uuid/uuid.h>
#endif

#include "util.h"

typedef uuid_t vhd_uuid;

VINILAPI void vinil_cp_uuid_generate(vhd_uuid* uuid);

#endif