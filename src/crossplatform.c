#include "crossplatform.h"

void vinil_cp_uuid_generate(vhd_uuid* uuid) {
#ifdef _WIN32
  CoCreateGuid(uuid);
#else
  uuid_generate(*uuid);
#endif
}