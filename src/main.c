#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "cc_lib.h"

int main(void) {
  char *ref_fname = "./data/postmortem_sum_2023-10-17T12_58_29.160517.npy";
  char *cmp_fname = "data/postmortem_sum_2023-10-16T08:10:36.022668.npy";
  
  double **ref_sum_data = malloc(BPM_CNT * sizeof(double*));
  for (size_t i=0; i<BPM_CNT; i++) {
    ref_sum_data[i] = malloc(POINT_CNT * sizeof(double));
  }

  double **cmp_sum_data = malloc(BPM_CNT * sizeof(double*));
  for (size_t i=0; i<BPM_CNT; i++) {
    cmp_sum_data[i] = malloc(POINT_CNT * sizeof(double));
  }

  if (fill_mem_from_file(ref_fname, ref_sum_data) < 0) return 1;
  if (fill_mem_from_file(cmp_fname, cmp_sum_data) < 0) return 1;

  double auto_cor[BPM_CNT], cmp_cor[BPM_CNT];

  for (size_t i=0; i<BPM_CNT; i++) {
    auto_cor[i] = cross_corr(ref_sum_data[i], ref_sum_data[i], POINT_CNT);
    cmp_cor[i]  = cross_corr(ref_sum_data[i], cmp_sum_data[i], POINT_CNT);
    // printf("auto_cor[%zu] = %e (normalised %e)\n", i, auto_cor[i], auto_cor[i] / auto_cor[i]);
    printf("normalised cmp_cor[%zu]  = %e\n", i, cmp_cor[i] / auto_cor[i]);
  }

  for (size_t i=0; i<BPM_CNT; i++) {
    free(cmp_sum_data[i]);
    free(ref_sum_data[i]);
  }
  free(cmp_sum_data);
  free(ref_sum_data);

  return 0;
}
