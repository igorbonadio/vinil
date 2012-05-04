#include "vhd.h"

#include <stdio.h>
#include <string.h>

// usage:
// ./read_vhd test.vhd 0
int main(int argc, char* argv[]) {
  char* vhd_path = argv[1];
  uint64_t vhd_sector = atol(argv[2]);
  char buffer[512];

  VHD* vhd = vinil_vhd_open(vhd_path);

  if (!vhd) {
    printf("ERROR: Can't open %s\n", vhd_path);
    return -1;
  }

  if (!vinil_vhd_seek(vhd, vhd_sector, SEEK_SET)) {
    printf("seek error...\n");
    return -1;
  }

  if (!vinil_vhd_read(vhd, buffer, 1)) {
    printf("read error...\n");
    return -1;
  }

  int i;
  for (i = 0; i < 512; i++)
    printf("%c", buffer[i]);
  printf("\n");

  vinil_vhd_close(vhd);

  return 0;
}

