/** 
 *  @file	    vhd.h
 *  @brief    Virtual Hard Disk I/O interface.
 *  @author   Igor Bonadio
 */

#ifndef VINIL_VHD_H_
#define VINIL_VHD_H_

#include <stdio.h>
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

/** @brief Represents a virtual hard disk file */
typedef struct {
  FILE* fd;
  VHDFooter* footer;
} VHD;

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
void vinil_vhd_footer_to_little_endian(VHDFooter* vhd_footer);

/** @brief  Opens a VHD file
 *
 *  @param    filename      C string containing the name of the file to be opened.
 *
 *  @return   If the operation was succesfully opened this function will return a pointer to VHD object. 
 *            Otherwise, a null pointer is returned.
 */
VHD* vinil_vhd_open(const char* filename);

/** @brief  Closes and destroy the VHD object
 *
 *  @param    vhd      VHD object
 */
void vinil_vhd_close(VHD* vhd);

#endif
