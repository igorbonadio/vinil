/** 
 *  @file	    vhd.h
 *  @brief    Virtual Hard Disk I/O interface.
 *  @author   Igor Bonadio
 */

#ifndef VINIL_VHD_H_
#define VINIL_VHD_H_

#include <stdint.h>
#include <uuid/uuid.h>

typedef uint32_t u32;
typedef uint64_t u64;
typedef uuid_t vhd_uuid;

/** @brief Stores basic informations which is shared by all the VHDs types */
typedef struct {
  char cookie[8];
  u32 features;
  u32 file_format_version;
  u64 data_offset;
  u32 timestamp;
  char creator_application[4];
  u32 creator_version;
  u32 creator_host_os;
  u64 original_size;
  u64 current_size;
  u32 disk_geometry;
  u32 disk_type;
  u32 checksum;
  vhd_uuid uuid;
  char saved_state;
  char reserved[427];
} VHDFooter;

/** @brief  Calculates VDH Footer's checksum
 *
 *  @param    vhd_footer      VHD Footer
 *
 *  @return   the checksum
 */
int vinil_checksum_vhd_footer(VHDFooter* vhd_footer);

/** @brief  Function for byte order swapping
 *
 *  @param    vhd_footer      VHD Footer
 */
void vinil_bswap_vhd_footer(VHDFooter* vhd_footer);

#endif
