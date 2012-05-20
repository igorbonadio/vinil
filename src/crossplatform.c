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

VINILAPI int64_t vinil_ftell(FILE *fd) {
#ifdef _WIN32
  return _ftelli64(fd);
#else
  return ftello(fd);
#endif
}

VINILAPI int vinil_truncate(FILE *fd, int64_t new_length) {
#ifdef _WIN32
  return _chsize(fileno(fd), new_length) == 0 ? TRUE : FALSE;
#else
  return ftruncate(fileno(fd), new_length) == 0 ? TRUE : FALSE;
#endif
}