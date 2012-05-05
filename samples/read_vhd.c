/** 
 *  @file       read_vhd.c
 *  @brief      This application reads sectors from a Virtual Hard Disk.
 *              In fact, this application is limited by the atol function,
 *              so you can create VHDs up to 2GB.
 *              But it is important to mention that Vinil doens't have this
 *              limitation.
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */
 
#include "vhd.h"

#include <stdio.h>
#include <string.h>

// usage:
// ./read_vhd test.vhd 0
int main(int argc, char* argv[]) {
  char* vhd_path = argv[1];
  uint64_t vhd_sector = atol(argv[2]);
  char buffer[512];

  VinilVHD* vhd = vinil_vhd_open(vhd_path);

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

