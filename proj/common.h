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

typedef struct {
    int x;
    int y;
    int r;
} XYR;


int read_inp(const char* path, IXY ixys[], XYR xyrs[]);
void print_ixys(IXY ixys[], int len, const char* sep);
void print_xyrs(XYR xyrs[], int len, const char* sep);

#endif//_COMMON_
