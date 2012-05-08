/** 
 *  @file       vhd.h
 *  @brief      Virtual Hard Disk I/O interface.
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#ifndef VINIL_VHD_H_
#define VINIL_VHD_H_

#include <stdio.h>
#include <stdint.h>

#include "util.h"
#include "crossplatform.h"

/** @brief Stores basic informations which is shared by all the VHDs types */
typedef struct {
  char      cookie[8];
  uint32_t  features;
  uint32_t  file_format_version;
  uint64_t  data_offset;
  uint32_t  timestamp;
  char      creator_application[4];
  uint32_t  creator_version;
  uint32_t  creator_host_os;
  uint64_t  original_size;
  uint64_t  current_size;
  uint32_t  disk_geometry;
  uint32_t  disk_type;
  uint32_t  checksum;
  vinil_uuid  uuid;
  char      saved_state;
  char      reserved[427];
} VinilVHDFooter;

/** @brief Represents a virtual hard disk file */
typedef struct {
  FILE* fd;
  VinilVHDFooter* footer;
} VinilVHD;

/** @brief  Creates a new VinilVHDFooter object
 *
 *  @return   a new VinilVHDFooter object
 */
VINILAPI VinilVHDFooter* vinil_vhd_footer_create();

/** @brief  Reads a VHD footer
 *
 *  @param    fd          a VHD file descriptor
 *
 *  @param    vhd_footer  a VinilVHDFooter object that will contain vhd footer's information
 *
 *  @return   Returns TRUE if successful or FALSE otherwise.
 */
VINILAPI int vinil_vhd_footer_read(FILE* fd, VinilVHDFooter* vhd_footer);

/** @brief  Destroys a VinilVHDFooter object
 *
 *  @param    vhd_footer  a VinilVHDFooter object to be destroyed
 */
VINILAPI void vinil_vhd_footer_destroy(VinilVHDFooter* vhd_footer);

/** @brief  Calculates VDH Footer's checksum
 *
 *  @param    vhd_footer      VHD Footer
 *
 *  @return   the checksum
 */
VINILAPI uint32_t vinil_checksum_vhd_footer(VinilVHDFooter* vhd_footer);

/** @brief  As described in VHD specification, this function calculates CHS
 *
 *  @param    size      Disk's size
 *
 *  @return   CHS
 */
VINILAPI uint32_t vinil_compute_chs(uint64_t size);

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
VINILAPI uint32_t vinil_geometry_encode(uint32_t cylinders, uint32_t heads, uint32_t sectors_per_track);

/** @brief  Gets the number of cylinders
 *
 *  @param    geometry      Encoded CHS
 *
 *  @return   number of cylinders
 */
VINILAPI uint32_t vinil_geometry_get_cylinders(uint32_t geometry);

/** @brief  Gets the number of heads
 *
 *  @param    geometry      Encoded CHS
 *
 *  @return   number of heads
 */
VINILAPI uint32_t vinil_geometry_get_head(uint32_t geometry);

/** @brief  Gets the number of sectors per track
 *
 *  @param    geometry      Encoded CHS
 *
 *  @return   number of sectors per track
 */
VINILAPI uint32_t vinil_geometry_get_sectors_per_track(uint32_t geometry);

/** @brief  Function for byte order swapping
 *
 *  @param    vhd_footer      VHD Footer
 */
VINILAPI void vinil_vhd_footer_byte_swap(VinilVHDFooter* vhd_footer);


/** @brief  Opens a VHD file
 *
 *  @param    filename      C string containing the name of the file to be opened.
 *
 *  @return   If the operation was succesfully opened this function will return a pointer to VHD object. 
 *            Otherwise, a null pointer is returned.
 */
VINILAPI VinilVHD* vinil_vhd_open(const char* filename);

/** @brief  Closes and destroy the VinilVHD object
 *
 *  @param    vhd      VinilVHD object
 */
VINILAPI void vinil_vhd_close(VinilVHD* vhd);

/** @brief  Reads a sector from the VinilVHD object
 *
 *  @param    vhd       VinilVHD object
 *
 *  @param    buffer    a (512*count) bytes buffer
 *
 *  @param    count     number of sectors to read
 *
 *  @return   If the operation was succesfully executed this function will return TRUE.
 *            Otherwise, FALSE will be returned.
 */
VINILAPI int vinil_vhd_read(VinilVHD* vhd, void* buffer, int count);

/** @brief  Writes a sector to a virtual hard disk file
 *
 *  @param    vhd       VinilVHD object
 *
 *  @param    buffer    a (512*count) bytes buffer
 *
 *  @param    count     number of sectors to write
 *
 *  @return   If the operation was succesfully executed this function will return TRUE.
 *            Otherwise, FALSE will be returned.
 */
VINILAPI int vinil_vhd_write(VinilVHD* vhd, void* buffer, int count);

/** @brief  Returns the current sector number
 *
 *  @param    vhd       VinilVHD object
 *
 *  @return   On success, the current sector number is returned. 
 *            If an error occurs, -1L is returned.
 */
VINILAPI long vinil_vhd_tell(VinilVHD* vhd);

/** @brief  Sets the position indicator associated with the VinilVHD object to a new
 *          position defined by the number of sector indicated by offset to a 
 *          reference position specified by origin.
 *
 *  @param    vhd       VinilVHD object
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
VINILAPI int vinil_vhd_seek(VinilVHD* vhd, int64_t offset, int origin);

/** @brief  It is like fflush C function.
 *
 *  @param    vhd       VinilVHD object
 *
 *  @return   On success, the function return TRUE.
 *            If an error occurs, it return FALSE
 */
VINILAPI int vinil_vhd_flush(VinilVHD* vhd);

/** @brief  If necessary it changes the virtual hard disk's size 
 *          and write the VinilVHDFooter struct at the end of file
 *
 *  @param    vhd       VinilVHD object
 *
 *  @return   On success, the function return TRUE.
 *            If an error occurs, it return FALSE
 */
VINILAPI int vinil_vhd_commit_structural_changes(VinilVHD* vhd);

#endif
