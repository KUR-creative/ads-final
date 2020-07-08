#include <stdlib.h>
#include <string.h>

#include "common.h"

int set_ixy(int nums[4], IXY ixys[]) // +
{
    int i = nums[1]; ixys[i].i = i;
    int x = nums[2]; ixys[i].x = x;
    int y = nums[3]; ixys[i].y = y;
    return i;
}
int del_ixy(int nums[4], IXY ixys[]) // -
{
    int i = nums[1]; ixys[i].i = 0; // i = 0 means no ixy
    return i;
}

//----------------------------------------------------------------
int tok2int(const char* tok) {
    if(tok == NULL){ return -1; }
    switch(tok[0]){
        case '+':
        case '-':
        case '?': return (int)tok[0];
        default: 
                  return atoi(tok);
    }
}

// first number is (int)char: type of line.
void line2nums(char buf[], int nums[4]) 
{
    char* tok = strtok(buf, " ");
    int idx = 0;
    while(tok) {
        nums[idx++] = tok2int(tok);
        tok = strtok(NULL, " ");
    }
}

//----------------------------------------------------------------
void print_ixys(const IXY ixys[], int len, const char* sep)
{
    for(int i = 0; i < len; i++){
        IXY yxi = ixys[i];
        printf("i:%d x:%d y:%d%s", 
                yxi.i, yxi.x, yxi.y, sep);
    }
}
