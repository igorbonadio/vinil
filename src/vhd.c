/** 
 *  @file     vhd.c
 *  @author   Igor Bonadio
 */

#include "vhd.h"

#define htonll(value) ((u64)(htonl((u32)(value & 0xFFFFFFFFLL))) << 32) | htonl((u32)(value >> 32))

int vinil_checksum_vhd_footer(VHDFooter* vhd_footer) {
  unsigned char* buffer;
  buffer = (unsigned char*)vhd_footer;
  
  u32 temp_checksum = vhd_footer->checksum;
  vhd_footer->checksum = 0;
  
  u32 checksum = 0;
  int i;
  for (i = 0; i < sizeof(VHDFooter); i++)
    checksum += (u32)buffer[i];
  
  vhd_footer->checksum = temp_checksum;
  
  return ~checksum;
}

void vinil_bswap_vhd_footer(VHDFooter* vhd_footer) {
  vhd_footer->features = htonl(vhd_footer->features);
	vhd_footer->file_format_version = htonl(vhd_footer->file_format_version);
	vhd_footer->data_offset = htonll(vhd_footer->data_offset);
	vhd_footer->timestamp = htonl(vhd_footer->timestamp);
	vhd_footer->creator_version = htonl(vhd_footer->creator_version);
	vhd_footer->creator_host_os = htonl(vhd_footer->creator_host_os);
	vhd_footer->original_size = htonll(vhd_footer->original_size);
	vhd_footer->current_size = htonll(vhd_footer->current_size);
	vhd_footer->disk_geometry = htonl(vhd_footer->disk_geometry);
	vhd_footer->disk_type = htonl(vhd_footer->disk_type);
  vhd_footer->checksum = htonl(vhd_footer->checksum);
}
