#include "crossplatform.h"

void vinil_uuid_generate(vinil_uuid* uuid) {
#ifdef _WIN32
  CoCreateGuid(uuid);
#else
  uuid_generate(*uuid);
#endif
}