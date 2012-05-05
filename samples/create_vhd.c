/** 
 *  @file       create_vhd.c
 *  @brief      This application creates a new Virtual Hard Disk.
 *              In fact, this application is limited by the atol function,
 *              so you can create VHDs up to 2GB.
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#include "vhd.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

// usage:
// ./create_vhd test.vhd 1024
int main(int argc, char* argv[]) {
  char* vhd_path = argv[1];      // filename
  uint64_t vhd_size = atol(argv[2]);  // virtual hard disk size

  VinilVHD* vhd = vinil_vhd_open(vhd_path);

  if (!vhd) {
    printf("ERROR: Can't open %s\n", vhd_path);
    return -1;
  }

  memcpy(vhd->footer->cookie, "conectix", 9);
  vhd->footer->features = 0;
  vhd->footer->file_format_version = 0x00010000;
  vhd->footer->data_offset = 0xFFFFFFFF;
  vhd->footer->timestamp = (uint32_t)time(NULL);
  memcpy(vhd->footer->creator_application, "vnil", 4);
  vhd->footer->creator_version = 0x00000001;
  vhd->footer->creator_host_os = 0x4D616320;                    // Mac OS X
  vhd->footer->original_size = vhd_size;
  vhd->footer->current_size = vhd_size;
  vhd->footer->disk_geometry = vinil_compute_chs(vhd_size);
  vhd->footer->disk_type = 2;                                   // Fixed
  vinil_uuid_generate(&vhd->footer->uuid);
  vhd->footer->saved_state = 0;
  vhd->footer->checksum = vinil_checksum_vhd_footer(vhd->footer);

  if (!vinil_vhd_commit_structural_changes(vhd)) {
    printf("ERROR: Can't commit structural changes\n");
    return -1;
  }

  vinil_vhd_close(vhd);

  return 0;
}

