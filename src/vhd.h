/** 
 *  @file	      vhd.h
 *  @brief      Virtual Hard Disk I/O interface.
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#ifndef VINIL_VHD_H_
#define VINIL_VHD_H_

#include <stdio.h>
#include <stdint.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <uuid/uuid.h>
#endif

#include "util.h"

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

/** @brief  Creates a new VHDFooter object
 *
 *  @return   a new VHDFooter object
 */
VHDFooter* vinil_vhd_footer_create();

/** @brief  Reads a VHD footer
 *
 *  @param    fd          a VHD file descriptor
 *
 *  @param    vhd_footer  a VHDFooter object that will contain vhd footer's information
 *
 *  @return   Returns TRUE if successful or FALSE otherwise.
 */
int vinil_vhd_footer_read(FILE* fd, VHDFooter* vhd_footer);

/** @brief  Destroys a VHDFooter object
 *
 *  @param    vhd_footer  a VHDFooter object to be destroyed
 */
void vinil_vhd_footer_destroy(VHDFooter* vhd_footer);

/** @brief  Calculates VDH Footer's checksum
 *
 *  @param    vhd_footer      VHD Footer
 *
 *  @return   the checksum
 */
u32 vinil_checksum_vhd_footer(VHDFooter* vhd_footer);

/** @brief  As described in VHD specification, this function calculates CHS
 *
 *  @param    size      Disk's size
 *
 *  @return   CHS
 */
u32 vinil_compute_chs(u64 size);

/** @brief  CHS is composed by cylinders (2 bytes), tracks (1 byte),
 *          and secotrs-per-track (1 byte)
 *
 *  @param    cylinders             number of cylinders
 *
 *  @param    heads                 number of heads
 *
 *  @param    sectors_per_track     number of sectors per track
 *
 *  @return   CHS encoded
 */
u32 vinil_geometry_encode(u32 cylinders, u32 heads, u32 sectors_per_track);

/** @brief  Gets the number of cylinders
 *
 *  @param    geometry      Encoded CHS
 *
 *  @return   number of cylinders
 */
u32 vinil_geometry_get_cylinders(u32 geometry);

/** @brief  Gets the number of heads
 *
 *  @param    geometry      Encoded CHS
 *
 *  @return   number of heads
 */
u32 vinil_geometry_get_head(u32 geometry);

/** @brief  Gets the number of sectors per track
 *
 *  @param    geometry      Encoded CHS
 *
 *  @return   number of sectors per track
 */
u32 vinil_geometry_get_sectors_per_track(u32 geometry);

/** @brief  Function for byte order swapping
 *
 *  @param    vhd_footer      VHD Footer
 */
void vinil_vhd_footer_byte_swap(VHDFooter* vhd_footer);


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

/** @brief  Reads a sector from the VHD object
 *
 *  @param    vhd       VHD object
 *
 *  @param    buffer    a (512*count) bytes buffer
 *
 *  @param    count     number of sectors to read
 *
 *  @return   If the operation was succesfully executed this function will return TRUE.
 *            Otherwise, FALSE will be returned.
 */
int vinil_vhd_read(VHD* vhd, void* buffer, int count);

/** @brief  Writes a sector to a virtual hard disk file
 *
 *  @param    vhd       VHD object
 *
 *  @param    buffer    a (512*count) bytes buffer
 *
 *  @param    count     number of sectors to write
 *
 *  @return   If the operation was succesfully executed this function will return TRUE.
 *            Otherwise, FALSE will be returned.
 */
int vinil_vhd_write(VHD* vhd, void* buffer, int count);

/** @brief  Returns the current sector number
 *
 *  @param    vhd       VHD object
 *
 *  @return   On success, the current sector number is returned. 
 *            If an error occurs, -1L is returned.
 */
long vinil_vhd_tell(VHD* vhd);

/** @brief  Sets the position indicator associated with the VHD object to a new
 *          position defined by the number of sector indicated by offset to a 
 *          reference position specified by origin.
 *
 *  @param    vhd       VHD object
 *
 *  @param    offset    number of sector to offset from origin
 *
 *  @param    origin    Default Positions defined by <cstdio>:
 *                      <table border> 
 *                      <tr> 
 *                         <td> SEEK_SET </td> 
 *                         <td> Beginning of file </td> 
 *                      </tr> 
 *                      <tr> 
 *                         <td> SEEK_CUR </td> 
 *                         <td> Current position of the file pointer </td> 
 *                      </tr> 
 *                      <tr> 
 *                         <td> SEEK_END </td> 
 *                         <td> End of file </td> 
 *                      </tr>
 *                      </table>
 *
 *  @return   On success, the function returns TRUE.
 *            If an error occurs, it returns FALSE.
 */
int vinil_vhd_seek(VHD* vhd, long offset, int origin);

/** @brief  It is like fflush C function.
 *
 *  @param    vhd       VHD object
 *
 *  @return   On success, the function return TRUE.
 *            If an error occurs, it return FALSE
 */
int vinil_vhd_flush(VHD* vhd);

/** @brief  If necessary it changes the virtual hard disk's size 
 *          and write the VHDFooter struct at the end of file
 *
 *  @param    vhd       VHD object
 *
 *  @return   On success, the function return TRUE.
 *            If an error occurs, it return FALSE
 */
int vinil_vhd_commit_structural_changes(VHD* vhd);

#endif
