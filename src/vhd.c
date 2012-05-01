/** 
 *  @file       vhd.c
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#include "vhd.h"

#include <stdlib.h>

#define htonll(value) ((u64)(htonl((u32)(value & 0xFFFFFFFFLL))) << 32) | htonl((u32)(value >> 32))
#define ntohll(value) ((u64)(ntohl((u32)(value & 0xFFFFFFFFLL))) << 32) | ntohl((u32)(value >> 32))

u32 vinil_checksum_vhd_footer(VHDFooter* vhd_footer) {
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

u32 vinil_compute_chs(u64 size) {
  uint32_t sectors, cylinders, heads, sectors_per_track, cylinder_times_head;

  sectors = size/512;

  if (sectors > 65535 * 16 * 255)
    sectors = 65535 * 16 * 255;

  if (sectors >= 65535 * 16 * 63) {
    sectors_per_track   = 255;
    cylinder_times_head   = sectors / sectors_per_track;
    heads = 16;
  } else {
    sectors_per_track   = 17;
    cylinder_times_head   = sectors / sectors_per_track;
    heads = (cylinder_times_head + 1023) / 1024;

    if (heads < 4)
      heads = 4;

    if (cylinder_times_head >= (heads * 1024) || heads > 16) {
      sectors_per_track   = 31;
      cylinder_times_head   = sectors / sectors_per_track;
      heads = 16;
    }

    if (cylinder_times_head >= heads * 1024) {
      sectors_per_track   = 63;
      cylinder_times_head   = sectors / sectors_per_track;
      heads = 16;
    }
  }

  cylinders = cylinder_times_head / heads;

  return vinil_geometry_encode(cylinders, heads, sectors_per_track);
}

u32 vinil_geometry_encode(u32 cylinders, u32 heads, u32 sectors_per_track) {
  return (cylinders << 16) | (heads << 8) | sectors_per_track;
}

u32 vinil_geometry_get_cylinders(u32 geometry) {
  return (geometry >> 16) & 0xffff;
}

u32 vinil_geometry_get_head(u32 geometry) {
  return (geometry >> 8)  & 0xff;
}

u32 vinil_geometry_get_sectors_per_track(u32 geometry) {
  return geometry & 0xff;
}

static inline u32 byte_swap_32(u32 x)
{
    return
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |
     (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24));
}

static inline u64 byte_swap_64(u64 x)
{
    return
    ((((x) & 0xff00000000000000ULL) >> 56) |
     (((x) & 0x00ff000000000000ULL) >> 40) |
     (((x) & 0x0000ff0000000000ULL) >> 24) |
     (((x) & 0x000000ff00000000ULL) >>  8) |
     (((x) & 0x00000000ff000000ULL) <<  8) |
     (((x) & 0x0000000000ff0000ULL) << 24) |
     (((x) & 0x000000000000ff00ULL) << 40) |
     (((x) & 0x00000000000000ffULL) << 56));
}

void vinil_vhd_footer_byte_swap(VHDFooter* vhd_footer) {
  vhd_footer->features = byte_swap_32(vhd_footer->features);
  vhd_footer->file_format_version = byte_swap_32(vhd_footer->file_format_version);
  vhd_footer->data_offset = byte_swap_64(vhd_footer->data_offset);
  vhd_footer->timestamp = byte_swap_32(vhd_footer->timestamp);
  vhd_footer->creator_version = byte_swap_32(vhd_footer->creator_version);
  vhd_footer->creator_host_os = byte_swap_32(vhd_footer->creator_host_os);
  vhd_footer->original_size = byte_swap_64(vhd_footer->original_size);
  vhd_footer->current_size = byte_swap_64(vhd_footer->current_size);
  vhd_footer->disk_geometry = byte_swap_32(vhd_footer->disk_geometry);
  vhd_footer->disk_type = byte_swap_32(vhd_footer->disk_type);
  vhd_footer->checksum = byte_swap_32(vhd_footer->checksum);
}

VHD* vinil_vhd_open(const char* filename) {
  int error;
  int file_exists = 0;
  
  FILE* f = fopen(filename, "r");
  if (f) {
    file_exists = 1;
    fclose(f);
  } else {
    f = fopen(filename, "w");
    if (f)
      fclose(f);
    else
      return NULL;
  }
  
  VHD* vhd = (VHD*)malloc(sizeof(VHD));
  if (vhd == NULL)
    return NULL;
  
  vhd->fd = fopen(filename, "rb+");
  if (vhd->fd == NULL) {
    vinil_vhd_close(vhd);
    return NULL;
  }
  
  vhd->footer = vinil_vhd_footer_create();
  if (vhd->footer == NULL) {
    vinil_vhd_close(vhd);
    return NULL;
  }
  
  if (file_exists) {
    if (!vinil_vhd_footer_read(vhd->fd, vhd->footer)) {
      vinil_vhd_close(vhd);
      return NULL;
    }
    
    if (vinil_checksum_vhd_footer(vhd->footer) != vhd->footer->checksum) {
      vinil_vhd_close(vhd);
      return NULL;
    }
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

int vinil_vhd_footer_read(FILE* fd, VHDFooter* vhd_footer) {
  int error;
  
  error = fseek(fd, 0, SEEK_END);
  if (error)
    return FALSE;
  
  error = fseek(fd, ftell(fd) - sizeof(VHDFooter), SEEK_SET);
  if (error)
    return FALSE;
  
  int b = fread(vhd_footer, sizeof(char), sizeof(VHDFooter), fd);
  if (b != sizeof(VHDFooter))
    return FALSE;
  
  vinil_vhd_footer_byte_swap(vhd_footer);
  
  return TRUE;
}

VHDFooter* vinil_vhd_footer_create() {
  int error;
  
  VHDFooter* footer = (VHDFooter*)malloc(sizeof(VHDFooter));
  if (footer == NULL) {
    vinil_vhd_footer_destroy(footer);
    return NULL;
  }
  
  return footer;
}

void vinil_vhd_footer_destroy(VHDFooter* vhd_footer) {
  free(vhd_footer);
}

int vinil_vhd_read(VHD* vhd, void* buffer, int count) {
  if (ftell(vhd->fd) > (vhd->footer->current_size - 512*count))
    return FALSE;
  
  int bytes = fread(buffer, sizeof(char), 512*count, vhd->fd);
  
  return bytes == 512*count ? TRUE : FALSE;
}

int vinil_vhd_write(VHD* vhd, void* buffer, int count) {
  if (ftell(vhd->fd) > (vhd->footer->current_size - 512*count))
    return FALSE;
  
  int bytes = fwrite(buffer, sizeof(char), 512*count, vhd->fd);
  
  return bytes == 512*count ? TRUE : FALSE;
}

long vinil_vhd_tell(VHD* vhd) {
  return ftell(vhd->fd)/512;
}

int vinil_vhd_seek(VHD* vhd, long offset, int origin) {
  if (origin == SEEK_END) {
    int error = 0;
    
    error = fseek(vhd->fd, 0, SEEK_END);
    if (error) {
      return FALSE;
    }

    error = fseek(vhd->fd, ftell(vhd->fd) - sizeof(VHDFooter), SEEK_SET);
    if (error) {
      return FALSE;
    }
    
    return TRUE;
  }  
  return fseek(vhd->fd, offset*512, origin) ? FALSE : TRUE;
}

int vinil_vhd_flush(VHD* vhd) {
  return fflush(vhd->fd) ? FALSE : TRUE;
}

int vinil_vhd_commit_structural_changes(VHD* vhd) {
  int error = fseek(vhd->fd, vhd->footer->current_size, SEEK_SET);
  if (error) {
    return FALSE;
  }
  
  vinil_vhd_footer_byte_swap(vhd->footer);
  
  int b = fwrite(vhd->footer, 1, sizeof(VHDFooter), vhd->fd);
  if (b != sizeof(VHDFooter))
    return FALSE;
  
  vinil_vhd_footer_byte_swap(vhd->footer);
    
  error = fseek(vhd->fd, 0, SEEK_SET);
  if (error) {
    return FALSE;
  }
  
  return TRUE;
}
