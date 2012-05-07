/** 
 *  @file       crossplatform.h
 *  @brief      Defines some cross-platiform functions
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

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
  #define _FILE_OFFSET_BITS 64
  #include <uuid/uuid.h>
#endif

#include <stdio.h>
#include <stdint.h>

#include "util.h"

typedef uuid_t vinil_uuid;

VINILAPI void vinil_uuid_generate(vinil_uuid* uuid);
VINILAPI int vinil_fseek(FILE *fd, uint64_t offset, int origin);

#endif