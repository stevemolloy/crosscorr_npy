#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "cc_lib.h"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s ref_file.npy comparison_file.npy results_file.csv\n", argv[0]);
    fprintf(stderr, "Incorrect number of arguments provided.\n");
    return 1;
  }

  char *ref_fname = argv[1];
  char *cmp_fname = argv[2];
  char *sav_fname = argv[3];
  
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

  pthread_t threads[BPM_CNT];

  double cmp_cor[BPM_CNT];
  for (size_t i=0; i<BPM_CNT; i++) {
    ThreadInput ti = (ThreadInput) {
      ref_sum_data[i],
      cmp_sum_data[i],
      BPM_CNT,
      &cmp_cor[i],
    };

    pthread_create(&threads[i], NULL, &cross_corr, (void*)&ti);
    // cmp_cor[i] = cross_corr(ref_sum_data[i], cmp_sum_data[i], POINT_CNT);
  }

  FILE *fd = fopen(sav_fname, "w");
  if (fd==NULL) {
    fprintf(stderr, "Could not open %s: %s\n", sav_fname, strerror(errno));
    return -1;
  }

  fprintf(fd, "BPM, Corr_amplitude\n");
  for (size_t i=0; i<BPM_CNT; i++) {
    fprintf(fd, "%zu, %lf\n", i+1, cmp_cor[i]);
  }

  for (size_t i=0; i<BPM_CNT; i++) {
    free(cmp_sum_data[i]);
    free(ref_sum_data[i]);
  }
  free(cmp_sum_data);
  free(ref_sum_data);

  return 0;
}
