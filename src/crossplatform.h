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

typedef uuid_t vinil_uuid;

VINILAPI void vinil_uuid_generate(vinil_uuid* uuid);

#endif