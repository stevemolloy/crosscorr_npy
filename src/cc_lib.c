#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cc_lib.h"

#define DELAY 0 // In case the data should be convolved

double signal_amp(double *sig, int N) {
  double result = 0;

  for (size_t i=0; i<(size_t)N; i++) {
    result += sig[i] * sig[i];
  }

  return sqrt(result);
}

void *cross_corr(void *ti) {
  ThreadInput *threadinput = (ThreadInput*) ti;
  *threadinput->result = 0;
  double ref_amp = signal_amp(threadinput->ref_data, threadinput->length);
  double cmp_amp = signal_amp(threadinput->cmp_data, threadinput->length);

  for (int i=0; i<threadinput->length; i++) {
    if (i-DELAY < 0 || i-DELAY >= threadinput->length) continue;
    *threadinput->result += threadinput->ref_data[i] * threadinput->cmp_data[i-DELAY];
  }

  *threadinput->result /= (ref_amp * cmp_amp);

  return NULL;
}

int fill_mem_from_file(char *fname, double **data) {
  int fd = open(fname, O_RDONLY);
  if (fd<0) {
    fprintf(stderr, "Could not open %s: %s\n", fname, strerror(errno));
    return -1;
  }

  struct stat file_stats;
  if (fstat(fd, &file_stats) < 0) {
    fprintf(stderr, "Could not stat %s: %s\n", fname, strerror(errno));
    return -1;
  }

  char *fc = (char*)mmap(NULL, (size_t)file_stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (fc == NULL) {
    fprintf(stderr, "Could not memory-map %s: %s\n", fname, strerror(errno));
    return -1;
  }

  size_t hdr_len = (size_t)*(unsigned short int*)(fc+8);
  char *data_start = fc+8+2+hdr_len;

  for (size_t i=0; i<BPM_CNT; i++) {
    memcpy(data[i], data_start + i*POINT_CNT*sizeof(double), POINT_CNT*sizeof(double));
  }

  munmap(fc, (size_t)file_stats.st_size);
  close(fd);

  return 0;
}


