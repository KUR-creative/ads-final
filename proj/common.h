#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>

#define SUCCESS  0
#define FAILURE  1
#define MAX_LEN  1000000

#define PRNd(x)  printf(#x ": %d ", x)   
#define PRNLd(x) printf(#x ": %d \n", x)   
#define PRNs(s)  printf(#s ": %s ", s)   
#define PRNLs(s) printf(#s ": %s \n", s)   


typedef struct {
    int i, x, y; // i = 0 means no ixy
} IXY;

typedef struct {
    int x, y, r; // r = 0 means no xyr
} XYR; // union? QoA (Query or Answer?)


int read_inp_f(FILE* fp, IXY ixys[], XYR xyrs[]);
int read_inp(const char* path, IXY ixys[], XYR xyrs[]);
long d_sub_r(long x, long y, long cx, long cy, long r);
int solve_n_square(const IXY ixys[], XYR xyrs[]);

void print_ixys(const IXY ixys[], int len, const char* sep);
void print_xyrs(const XYR xyrs[], int len, const char* sep);

#endif//_COMMON_
