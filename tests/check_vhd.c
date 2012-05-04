/** 
 *  @file       check_vhd.c
 *  @author     Igor Bonadio
 *  @copyright  MIT License. See Copyright Notice in LICENSE.txt
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <check.h>

#include "vhd.h"

START_TEST (test_vinil_checksum_vhd_footer) {
  char *vhd_files[] = {"vhd_test_y.vhd", 
                       "vhd_test_zero.vhd"};
  
  char vhd_path[256];
  char error_msg[256];
  
  int i;
  for (i = 0; i < 2; i++) {
    sprintf(vhd_path, "../tests/data/%s", vhd_files[i]);
    
    FILE *fd = fopen(vhd_path, "r");
    
    sprintf(error_msg, "Cannot open %s", vhd_files[i]);
    fail_unless(fd != NULL, error_msg);

    VinilVHDFooter* footer = vinil_vhd_footer_create();
    sprintf(error_msg, "Cannot create vhd_footer for %s", vhd_files[i]);
    fail_unless(footer != NULL, error_msg);
    
    int ok = vinil_vhd_footer_read(fd, footer);
    sprintf(error_msg, "Cannot read %s's footer", vhd_files[i]);
    fail_unless(ok, error_msg);
    
    sprintf(error_msg, "%s has an invalid checksum", vhd_files[i]);
    fail_unless(vinil_checksum_vhd_footer(footer) == footer->checksum, error_msg);
    
    vinil_vhd_footer_destroy(footer);
    
    fclose(fd);
  }
} END_TEST

START_TEST (test_vinil_open) {
  char *vhd_files[] = {"vhd_test_y.vhd", 
                       "vhd_test_zero.vhd"};
  
  char vhd_path[256];
  char error_msg[256];
  
  int i;
  for (i = 0; i < 2; i++) {
    sprintf(vhd_path, "../tests/data/%s", vhd_files[i]);
    
    VHD* vhd = vinil_vhd_open(vhd_path);
    
    sprintf(error_msg, "Cannot open %s", vhd_files[i]);
    fail_unless(vhd != NULL, error_msg);
    
    sprintf(error_msg, "Cannot open %s file descriptor", vhd_files[i]);
    fail_unless(vhd->fd != NULL, error_msg);
    
    sprintf(error_msg, "Cannot open %s footer", vhd_files[i]);
    fail_unless(vhd->footer != NULL, error_msg);
    
    vinil_vhd_close(vhd);
  }
} END_TEST

START_TEST (test_vinil_read) {
  char *vhd_files[] = {"vhd_test_y.vhd", 
                       "vhd_test_zero.vhd"};
  
  char vhd_path[256];
  char error_msg[256];
  
  int i;
  for (i = 0; i < 2; i++) {
    sprintf(vhd_path, "../tests/data/%s", vhd_files[i]);
    
    VHD* vhd = vinil_vhd_open(vhd_path);
    
    sprintf(error_msg, "Cannot open %s", vhd_files[i]);
    fail_unless(vhd != NULL, error_msg);
    
    char sector[512];
    int count = 0;
    while (vinil_vhd_read(vhd, sector, 1))
      count++;
    
    sprintf(error_msg, "Wrong number of sectors in %s", vhd_files[i]);
    fail_unless(count*512 == vhd->footer->current_size, error_msg);
    
    vinil_vhd_close(vhd);
  }
} END_TEST

START_TEST (test_vinil_tell) {
  char *vhd_files[] = {"vhd_test_y.vhd", 
                       "vhd_test_zero.vhd"};
  
  char vhd_path[256];
  char error_msg[256];
  
  int i;
  for (i = 0; i < 2; i++) {
    sprintf(vhd_path, "../tests/data/%s", vhd_files[i]);
    
    VHD* vhd = vinil_vhd_open(vhd_path);
    
    sprintf(error_msg, "Cannot open %s", vhd_files[i]);
    fail_unless(vhd != NULL, error_msg);
    
    char sector[512];
    sprintf(error_msg, "Cannot read %s", vhd_files[i]);
    fail_unless(vinil_vhd_read(vhd, sector, 1), error_msg);
    
    sprintf(error_msg, "Cannot read %s", vhd_files[i]);
    fail_unless(vinil_vhd_read(vhd, sector, 1), error_msg);
    
    sprintf(error_msg, "Cannot read %s", vhd_files[i]);
    fail_unless(vinil_vhd_read(vhd, sector, 1), error_msg);
    
    sprintf(error_msg, "vinil_vhd_tell function returns a wrong sector number %s", vhd_files[i]);
    fail_unless(vinil_vhd_tell(vhd) == 3, error_msg);
    
    vinil_vhd_close(vhd);
  }
} END_TEST

START_TEST (test_vinil_seek) {
  char *vhd_files[] = {"vhd_test_y.vhd", 
                       "vhd_test_zero.vhd"};
  
  char vhd_path[256];
  char error_msg[256];
  
  int i;
  for (i = 0; i < 2; i++) {
    sprintf(vhd_path, "../tests/data/%s", vhd_files[i]);
    
    VHD* vhd = vinil_vhd_open(vhd_path);
    
    sprintf(error_msg, "Cannot open %s", vhd_files[i]);
    fail_unless(vhd != NULL, error_msg);
    
    sprintf(error_msg, "Cannot execute vinil_vhd_seek in %s", vhd_files[i]);
    fail_unless(vinil_vhd_seek(vhd, 0, SEEK_END), error_msg);
    
    sprintf(error_msg, "It is not the last sector of %s", vhd_files[i]);
    fail_unless(vinil_vhd_tell(vhd) == vhd->footer->current_size/512, error_msg);
    
    sprintf(error_msg, "Cannot execute vinil_vhd_seek in %s", vhd_files[i]);
    fail_unless(vinil_vhd_seek(vhd, vhd->footer->current_size/512 - 1, SEEK_SET), error_msg);
    
    sprintf(error_msg, "It is not the last sector of %s", vhd_files[i]);
    fail_unless(vinil_vhd_tell(vhd) == vhd->footer->current_size/512 - 1, error_msg);
    
    sprintf(error_msg, "Cannot execute vinil_vhd_seek in %s", vhd_files[i]);
    fail_unless(vinil_vhd_seek(vhd, 1, SEEK_CUR), error_msg);
    
    sprintf(error_msg, "It is not the last sector of %s", vhd_files[i]);
    fail_unless(vinil_vhd_tell(vhd) == vhd->footer->current_size/512, error_msg);
    
    vinil_vhd_close(vhd);
  }
} END_TEST

START_TEST (test_vinil_vhd_commit_structural_changes) {
  char vhd_file[] = "new_vhd_file.vhd";
  
  char vhd_path[256];
  sprintf(vhd_path, "../tests/data/%s", vhd_file);
  
  FILE* f = fopen(vhd_path, "r");
  if (f) {
    fclose(f);
    remove(vhd_path);
  }
  
  f = fopen(vhd_path, "r");
  fail_unless(f == NULL, "new_vhd_file.vhd already exists");
  
  VHD* vhd = vinil_vhd_open(vhd_path);
  fail_unless(vhd != NULL, "Cannot open new_vhd_file.vhd");
  
  memcpy(vhd->footer->cookie, "conectix", 9);
  vhd->footer->features = 0;
  vhd->footer->file_format_version = 0x00010000;
  vhd->footer->data_offset = 0xFFFFFFFF;
  vhd->footer->timestamp = time(NULL);
  memcpy(vhd->footer->creator_application, "vnil", 4);
  vhd->footer->creator_version = 0x00000001;
  vhd->footer->creator_host_os = 0x4D616320;                    // Mac OS X
  vhd->footer->original_size = 4*1024*1024;                     // 4MB
  vhd->footer->current_size = 4*1024*1024;                      // 4MB
  vhd->footer->disk_geometry = vinil_compute_chs(4*1024*1024);
  vhd->footer->disk_type = 2;                                   // Fixed
  vinil_uuid_generate(&vhd->footer->uuid);
  vhd->footer->saved_state = 0;
  vhd->footer->checksum = vinil_checksum_vhd_footer(vhd->footer);
  
  fail_unless(vinil_vhd_commit_structural_changes(vhd), "Cannot commit changes in new_vhd_file.vhd");
  
  unsigned char buffer[512];
  int i;
  for (i = 0; i < 512; i++)
    buffer[i] = 'a';
  fail_unless(vinil_vhd_write(vhd, buffer, 1), "Cannot write 1 sector to new_vhd_file.vhd");
  
  unsigned char buffer2[1024];
  for (i = 0; i < 1024; i++)
    buffer2[i] = 'b';
  fail_unless(vinil_vhd_write(vhd, buffer2, 2), "Cannot write 2 sectors to new_vhd_file.vhd");
  
  fail_unless(vinil_vhd_seek(vhd, 0, SEEK_SET), "Cannot execute vinil_vhd_seek in new_vhd_file.vhd");
  
  int count = 0;
  while (vinil_vhd_write(vhd, buffer, 1))
    count++;
  
  fail_unless(count*512 == vhd->footer->current_size, "Wrong number of sectors in new_vhd_file.vhd");
  
  vinil_vhd_close(vhd);
  
  // checking the new file...
  VHD* vhd2 = vinil_vhd_open(vhd_path);
  fail_unless(vhd2 != NULL, "Cannot reopen new_vhd_file.vhd");
  vinil_vhd_close(vhd2);
  
} END_TEST

START_TEST (test_vinil_geometry_encode) {
  char *vhd_files[] = {"vhd_test_y.vhd", 
                       "vhd_test_zero.vhd"};
  
  char vhd_path[256];
  char error_msg[256];
  
  int i;
  for (i = 0; i < 2; i++) {
    sprintf(vhd_path, "../tests/data/%s", vhd_files[i]);
    
    VHD* vhd = vinil_vhd_open(vhd_path);
    
    sprintf(error_msg, "Cannot open %s", vhd_files[i]);
    fail_unless(vhd != NULL, error_msg);
    
    sprintf(error_msg, "Invalid geometry of %s", vhd_files[i]);
    fail_unless(vinil_compute_chs(vhd->footer->current_size) == vhd->footer->disk_geometry, error_msg);
    
    vinil_vhd_close(vhd);
  }
} END_TEST

Suite* func_suite(void) {
  Suite *s = suite_create ("vinil");
  TCase *tc_core = tcase_create ("VHD");
  tcase_add_test (tc_core, test_vinil_checksum_vhd_footer);
  tcase_add_test (tc_core, test_vinil_open);
  tcase_add_test (tc_core, test_vinil_read);
  tcase_add_test (tc_core, test_vinil_tell);
  tcase_add_test (tc_core, test_vinil_seek);
  tcase_add_test (tc_core, test_vinil_vhd_commit_structural_changes);
  tcase_add_test (tc_core, test_vinil_geometry_encode);
  suite_add_tcase (s, tc_core);
  return s;
}

int main() {
  int number_failed;
  Suite *s = func_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
