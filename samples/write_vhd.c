#include "vhd.h"

#include <stdio.h>
#include <string.h>

// usage:
// ./write_vhd test.vhd 0 a
int main(int argc, char* argv[]) {
  char* vhd_path = argv[1];
  uint64_t vhd_sector = atol(argv[2]);
  char c = argv[3][0];
  char buffer[512];

  int i;
  for (i = 0; i < 512; i++)
    buffer[i] = c;

  VHD* vhd = vinil_vhd_open(vhd_path);

  if (!vhd) {
    printf("ERROR: Can't open %s\n", vhd_path);
    return -1;
  }

  if (!vinil_vhd_seek(vhd, vhd_sector, SEEK_SET)) {
    printf("seek error...\n");
    return -1;
  }

  if (!vinil_vhd_write(vhd, buffer, 1)) {
    printf("write error...\n");
    return -1;
  }

  vinil_vhd_close(vhd);

  return 0;
}
