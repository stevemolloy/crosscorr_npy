#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "cc_lib.h"

int main(int argc, char *argv[]) {
  FILE *fd = NULL;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s ref_file.npy comparison_file.npy results_file.csv\n", argv[0]);
    fprintf(stderr, "Incorrect number of arguments provided.\n");
    return 1;
  }

  char *fname_cheatsheet_filename = "fname_cheatsheet.txt";
  fd = fopen(fname_cheatsheet_filename, "w");
  if (fd==NULL) {
     fprintf(stderr, "Could not open %s: %s\n", fname_cheatsheet_filename, strerror(errno));
     return -1;
  }
  for (int i=1; i<argc; i++) {
    fprintf(fd, "%d, %s\n", i, argv[i]);
  }
  fclose(fd);

  int comp_num = 0;
  int total_comparisons = ((argc-1)*(argc-1) + argc) / 2;
  for (int i=1; i<argc; i++) {
    char *ref_fname = argv[i];

    double **ref_sum_data = malloc(BPM_CNT * sizeof(double*));
    for (size_t i=0; i<BPM_CNT; i++) {
      ref_sum_data[i] = malloc(POINT_CNT * sizeof(double));
    }
    if (fill_mem_from_file(ref_fname, ref_sum_data) < 0) return 1;

    for (int j=i; j<argc; j++) {
      comp_num++;
      printf("%4d / %d :: (%2d/%2d) Comparing  %s  with  %s\n", comp_num, total_comparisons, i, j, argv[i], argv[j]);

      char *cmp_fname = argv[j];
      char sav_fname[256];
      snprintf(sav_fname, 256, "compare_%03d_%03d.txt", i, j);
      
      double **cmp_sum_data = malloc(BPM_CNT * sizeof(double*));
      for (size_t i=0; i<BPM_CNT; i++) {
        cmp_sum_data[i] = malloc(POINT_CNT * sizeof(double));
      }
      if (fill_mem_from_file(cmp_fname, cmp_sum_data) < 0) return 1;

      pthread_t threads[BPM_CNT];

      ThreadInput ti_s[BPM_CNT];
      double cmp_cor[BPM_CNT];
      for (size_t i=0; i<BPM_CNT; i++) {
        ti_s[i] = (ThreadInput) {
          ref_sum_data[i],
          cmp_sum_data[i],
          POINT_CNT,
          &cmp_cor[i],
        };

        pthread_create(&threads[i], NULL, &cross_corr, (void*)(ti_s + i));
      }

      for (size_t i=0; i<BPM_CNT; i++) {
        pthread_join(threads[i], NULL);
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
      }
      free(cmp_sum_data);

      fclose(fd);
    }
  
    for (size_t i=0; i<BPM_CNT; i++) {
      free(ref_sum_data[i]);
    }
    free(ref_sum_data);
  }

  return 0;
}
