/** 
 *  @file       crossplatform.c
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#include "crossplatform.h"

void vinil_uuid_generate(vinil_uuid* uuid) {
#ifdef _WIN32
  CoCreateGuid(uuid);
#else
  uuid_generate(*uuid);
#endif
}

int vinil_fseek(FILE *fd, int64_t offset, int origin) {
#ifdef _WIN32
  return _fseeki64(fd, offset, origin);
#else
  return fseeko(fd, offset, origin);
#endif
}
