/** 
 *  @file       vhd.c
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#include "vhd.h"

#include <stdlib.h>

uint32_t vinil_checksum_vhd_footer(VinilVHDFooter* vhd_footer) {
  unsigned char* buffer;
  buffer = (unsigned char*)vhd_footer;
  
  uint32_t temp_checksum = vhd_footer->checksum;
  vhd_footer->checksum = 0;
  
  uint32_t checksum = 0;
  int i;
  for (i = 0; i < sizeof(VinilVHDFooter); i++)
    checksum += (uint32_t)buffer[i];
  
  vhd_footer->checksum = temp_checksum;
  
  return ~checksum;
}

uint32_t vinil_compute_chs(uint64_t size) {
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

uint32_t vinil_geometry_encode(uint32_t cylinders, uint32_t heads, uint32_t sectors_per_track) {
  return (cylinders << 16) | (heads << 8) | sectors_per_track;
}

uint32_t vinil_geometry_get_cylinders(uint32_t geometry) {
  return (geometry >> 16) & 0xffff;
}

uint32_t vinil_geometry_get_head(uint32_t geometry) {
  return (geometry >> 8)  & 0xff;
}

uint32_t vinil_geometry_get_sectors_per_track(uint32_t geometry) {
  return geometry & 0xff;
}

static inline uint32_t byte_swap_32(uint32_t x)
{
    return
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |
     (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24));
}

static inline uint64_t byte_swap_64(uint64_t x)
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

void vinil_vhd_footer_byte_swap(VinilVHDFooter* vhd_footer) {
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

VinilVHD* vinil_vhd_open(const char* filename) {
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
  
  VinilVHD* vhd = (VinilVHD*)malloc(sizeof(VinilVHD));
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

void vinil_vhd_close(VinilVHD* vhd) {
  fclose(vhd->fd);
  vinil_vhd_footer_destroy(vhd->footer);
  free(vhd);
}

int vinil_vhd_footer_read(FILE* fd, VinilVHDFooter* vhd_footer) {
  int error;
  
  error = fseek(fd, 0, SEEK_END);
  if (error)
    return FALSE;
  
  error = fseek(fd, ftell(fd) - sizeof(VinilVHDFooter), SEEK_SET);
  if (error)
    return FALSE;
  
  int b = fread(vhd_footer, sizeof(char), sizeof(VinilVHDFooter), fd);
  if (b != sizeof(VinilVHDFooter))
    return FALSE;
  
  vinil_vhd_footer_byte_swap(vhd_footer);
  
  return TRUE;
}

VinilVHDFooter* vinil_vhd_footer_create() {
  int error;
  
  VinilVHDFooter* footer = (VinilVHDFooter*)malloc(sizeof(VinilVHDFooter));
  if (footer == NULL) {
    vinil_vhd_footer_destroy(footer);
    return NULL;
  }
  
  return footer;
}

void vinil_vhd_footer_destroy(VinilVHDFooter* vhd_footer) {
  free(vhd_footer);
}

int vinil_vhd_read(VinilVHD* vhd, void* buffer, int count) {
  if (ftell(vhd->fd) > (vhd->footer->current_size - 512*count))
    return FALSE;
  
  int bytes = fread(buffer, sizeof(char), 512*count, vhd->fd);
  
  return bytes == 512*count ? TRUE : FALSE;
}

int vinil_vhd_write(VinilVHD* vhd, void* buffer, int count) {
  if (ftell(vhd->fd) > (vhd->footer->current_size - 512*count))
    return FALSE;
  
  int bytes = fwrite(buffer, sizeof(char), 512*count, vhd->fd);
  
  return bytes == 512*count ? TRUE : FALSE;
}

long vinil_vhd_tell(VinilVHD* vhd) {
  return ftell(vhd->fd)/512;
}

int vinil_vhd_seek(VinilVHD* vhd, long offset, int origin) {
  if (origin == SEEK_END) {
    int error = 0;
    
    error = fseek(vhd->fd, 0, SEEK_END);
    if (error) {
      return FALSE;
    }

    error = fseek(vhd->fd, ftell(vhd->fd) - sizeof(VinilVHDFooter), SEEK_SET);
    if (error) {
      return FALSE;
    }
    
    return TRUE;
  }  
  return fseek(vhd->fd, offset*512, origin) ? FALSE : TRUE;
}

int vinil_vhd_flush(VinilVHD* vhd) {
  return fflush(vhd->fd) ? FALSE : TRUE;
}

int vinil_vhd_commit_structural_changes(VinilVHD* vhd) {
  int error = fseek(vhd->fd, vhd->footer->current_size, SEEK_SET);
  if (error) {
    return FALSE;
  }
  
  vinil_vhd_footer_byte_swap(vhd->footer);
  
  int b = fwrite(vhd->footer, 1, sizeof(VinilVHDFooter), vhd->fd);
  if (b != sizeof(VinilVHDFooter))
    return FALSE;
  
  vinil_vhd_footer_byte_swap(vhd->footer);
    
  error = fseek(vhd->fd, 0, SEEK_SET);
  if (error) {
    return FALSE;
  }
  
  return TRUE;
}
