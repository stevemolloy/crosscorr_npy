#include <stdio.h>
#include <stdlib.h>

#include "cc_lib.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s ref_file.npy comparison_file.npy\n", argv[0]);
    fprintf(stderr, "Incorrect number of arguments provided.\n");
    return 1;
  }

  char *ref_fname = argv[1];
  char *cmp_fname = argv[2];
  
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
    // printf("auto_cor[%zu]  = %e\n", i, auto_cor[i]);
    // printf("cross_cor[%zu] = %e (normalised %e)\n", i, cmp_cor[i], cmp_cor[i] / auto_cor[i]);
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
