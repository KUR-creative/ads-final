#ifndef _COMMON_
#define _COMMON_

#define SUCCESS  0
#define FAILURE  1

#define PRNd(x)  printf(#x ": %d ", x)   
#define PRNLd(x) printf(#x ": %d \n", x)   
#define PRNs(s)  printf(#s ": %s ", s)   
#define PRNLs(s) printf(#s ": %s \n", s)   

typedef struct {
    int i;
    int x;
    int y;
} IXY;

#endif//_COMMON_

int read_inp(const char* path, IXY ixys[]);
void print_yxis(IXY ixys[], int len, const char* sep);
