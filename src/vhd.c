/** 
 *  @file     vhd.c
 *  @author   Igor Bonadio
 */

#include "vhd.h"

#include <stdlib.h>

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

void vinil_vhd_footer_to_little_endian(VHDFooter* vhd_footer) {
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

VHD* vinil_vhd_open(const char* filename) {
  int error;
  
  VHD* vhd = (VHD*)malloc(sizeof(VHD));
  if (vhd == NULL)
    return NULL;
  
  vhd->fd = fopen(filename, "rb+");
  if (vhd->fd == NULL) {
    vinil_vhd_close(vhd);
    return NULL;
  }
  
  vhd->footer = vinil_vhd_footer_create(vhd->fd);
  if (vhd->footer == NULL) {
    vinil_vhd_close(vhd);
    return NULL;
  }
  
  if (vinil_checksum_vhd_footer(vhd->footer) != vhd->footer->checksum) {
    vinil_vhd_close(vhd);
    return NULL;
  }
  
  error = fseek(vhd->fd, 0, SEEK_SET);
  if (error) {
    vinil_vhd_close(vhd);
    return NULL;
  }
  
  return vhd;
}

void vinil_vhd_close(VHD* vhd) {
  fclose(vhd->fd);
  vinil_vhd_footer_destroy(vhd->footer);
  free(vhd);
}

VHDFooter* vinil_vhd_footer_create(FILE* fd) {
  int error;
  
  VHDFooter* footer = (VHDFooter*)malloc(sizeof(VHDFooter));
  if (footer == NULL) {
    vinil_vhd_footer_destroy(footer);
    return NULL;
  }
  
  error = fseek(fd, 0, SEEK_END);
  if (error) {
    vinil_vhd_footer_destroy(footer);
    return NULL;
  }
  
  error = fseek(fd, ftell(fd) - sizeof(VHDFooter), SEEK_SET);
  if (error) {
    vinil_vhd_footer_destroy(footer);
    return NULL;
  }
  
  int b = fread(footer, sizeof(char), sizeof(VHDFooter), fd);
  if (b != sizeof(VHDFooter)) {
    vinil_vhd_footer_destroy(footer);
    return NULL;
  }
  
  vinil_vhd_footer_to_little_endian(footer);
  
  return footer;
}

void vinil_vhd_footer_destroy(VHDFooter* vhd_footer) {
  free(vhd_footer);
}

int vinil_vhd_read(VHD* vhd, void* buffer) {
  if (ftell(vhd->fd) == vhd->footer->original_size)
    return 0;
  
  int bytes = fread(buffer, sizeof(char), 512, vhd->fd);
  
  return bytes == 512 ? 1 : 0;
}

long vinil_vhd_tell(VHD* vhd) {
  return ftell(vhd->fd)/512;
}

int vinil_vhd_seek(VHD* vhd, long offset, int origin) {
  if (origin == SEEK_END) {
    int error = 0;
    
    error = fseek(vhd->fd, 0, SEEK_END);
    if (error) {
      return 1;
    }

    error = fseek(vhd->fd, ftell(vhd->fd) - sizeof(VHDFooter), SEEK_SET);
    if (error) {
      return 1;
    }
    
    return 0;
  }
  return fseek(vhd->fd, offset*512, origin);
}
