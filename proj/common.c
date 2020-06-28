#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"


int tok2int(const char* tok) {
    if(tok == NULL){ return -1; }
    switch(tok[0]){
        case '+':
        case '-':
        case '?': return (int)tok[0];
        default: return atoi(tok);
    }
}

// first number is (int)char: type of line.
void buf2nums(char buf[], int nums[4]) {
    char* tok = strtok(buf, " ");
    int idx = 0;
    while(tok) {
        nums[idx++] = tok2int(tok);
        tok = strtok(NULL, " ");
    }
}

// 동일한 index가 있을 수 있나?
// 인덱스는 다르지만 좌표는 같을 수 있나?(아마 이거일 것)
void set_ixy(int nums[4], IXY ixys[]) { // +
    int i = nums[1]; ixys[i].i = i;
    int x = nums[2]; ixys[i].x = x;
    int y = nums[3]; ixys[i].y = y;
}
void del_ixy(int nums[4], IXY ixys[]) { // -
    int i = nums[1]; ixys[i].i = 0; // i = 0 means empty ixy
}

int read_inp(const char* path, IXY ixys[])
{
    char buf[64] = {0,};
    FILE* fp = fopen(path, "r");
    while(fgets(buf, sizeof buf, fp) != NULL){
        int nums[4];
        //PRNs(buf); puts("----------");
        buf2nums(buf, nums);
        //printf(":->  %c %d %d %d \n", (char)nums[0], nums[1], nums[2], nums[3]);
        char type = (char)nums[0];
        switch(type){
            case '+': set_ixy(nums, ixys); break;
            case '-': del_ixy(nums, ixys); break;
            case '?': break;
            default: puts("DATA error or something."); abort();
        }
    }
    return SUCCESS;
}

void print_yxis(IXY ixys[], int len, const char* sep)
{
    for(int i = 0; i < len; i++){
        IXY yxi = ixys[i];
        printf("i:%d x:%d y:%d%s", 
                yxi.i, yxi.x, yxi.y, sep);
    }
}
