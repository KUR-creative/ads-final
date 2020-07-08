#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>

#define SUCCESS  0
#define FAILURE  1
#define MAX_LEN  1000000
#define MAX_VAL  2147483647L

#define PRNld(x)  do{ printf(#x ": %ld ", x);   fflush(stdout); }while(0)
#define PRNLld(x) do{ printf(#x ": %ld \n", x); fflush(stdout); }while(0)
#define PRNd(x)   do{ printf(#x ": %d ", x);   fflush(stdout); }while(0)
#define PRNLd(x)  do{ printf(#x ": %d \n", x); fflush(stdout); }while(0)
#define PRNp(x)   do{ printf(#x ": %p ", x);   fflush(stdout); }while(0)
#define PRNLp(x)  do{ printf(#x ": %p \n", x); fflush(stdout); }while(0)
#define PRNs(s)   do{ printf(#s ": %s ", s);   fflush(stdout); }while(0)
#define PRNLs(s)  do{ printf(#s ": %s \n", s); fflush(stdout); }while(0)

#define SWAP(x, y, T) \
    do { T SWAP = x; x = y; y = SWAP; } while (0)


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

void line2nums(char buf[], int nums[4]);

int set_ixy(int nums[4], IXY ixys[]);
int del_ixy(int nums[4], IXY ixys[]);

void print_ixys(const IXY ixys[], int len, const char* sep);
void print_xyrs(const XYR xyrs[], int len, const char* sep);

#endif//_COMMON_
