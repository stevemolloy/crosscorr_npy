#ifndef _CC_LIB_H
#define _CC_LIB_H

#define BPM_CNT 201
#define POINT_CNT 200000
#define SCAN_DELAY 5

typedef struct {
  double *ref_data;
  double *cmp_data;
  int length;
} ThreadInput;

void *cross_corr(void*);
// double cross_corr(double*, double*, int);
int fill_mem_from_file(char*, double**);

#endif // !_CC_LIB_H

