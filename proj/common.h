#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>

#define MAX_LEN  1000000
#define MAX_VAL  2147483647L

#define PRNld(x)  do{ printf(#x ": %ld ", x);  fflush(stdout); }while(0)
#define PRNLld(x) do{ printf(#x ": %ld \n", x);fflush(stdout); }while(0)
#define PRNd(x)   do{ printf(#x ": %d ", x);   fflush(stdout); }while(0)
#define PRNLd(x)  do{ printf(#x ": %d \n", x); fflush(stdout); }while(0)
#define PRNp(x)   do{ printf(#x ": %p ", x);   fflush(stdout); }while(0)
#define PRNLp(x)  do{ printf(#x ": %p \n", x); fflush(stdout); }while(0)
#define PRNs(s)   do{ printf(#s ": %s ", s);   fflush(stdout); }while(0)
#define PRNLs(s)  do{ printf(#s ": %s \n", s); fflush(stdout); }while(0)

#define KEY(ixy, mode) \
    (((mode) == 'x') ? (ixy).x : (ixy).y)

typedef struct {
    int i, x, y; // i = 0 means no ixy
} IXY;


void line2nums(char buf[], int nums[4]);

int set_ixy(int nums[4], IXY ixys[]);
int del_ixy(int nums[4], IXY ixys[]);

void print_ixys(const IXY ixys[], int len, const char* sep);

#endif//_COMMON_
