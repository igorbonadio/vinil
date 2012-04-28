#include <stdlib.h>
#include <stdio.h>

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

    VHDFooter* footer = vinil_vhd_footer_create(fd);
    
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
    
    char sector[513];
    int bytes;
    int count = 0;
    while ((bytes = vinil_vhd_read(vhd, sector)) == 512)
      count++;
    
    sprintf(error_msg, "Wrong number of sectors in %s", vhd_files[i]);
    fail_unless(count*512 == vhd->footer->original_size, error_msg);
    
    vinil_vhd_close(vhd);
  }
} END_TEST

Suite* func_suite(void) {
  Suite *s = suite_create ("vinil");
  TCase *tc_core = tcase_create ("VHD");
  tcase_add_test (tc_core, test_vinil_checksum_vhd_footer);
  tcase_add_test (tc_core, test_vinil_open);
  tcase_add_test (tc_core, test_vinil_read);
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
