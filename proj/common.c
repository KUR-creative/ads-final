#include <stdlib.h>
#include <string.h>

#include "common.h"

/* 
 * distance - radius
 * distance: square of distance of (x,y) ~ (cx,cy)
 * radius: r^2, square of radius of circle{(cx,cy), r}
 *
 * return:
 *  -   (x,y) in the circle:     distance < radius 
 *  0   (x,y) on the circle:     distance = radius
 *  +   (x,y) out of the circle: distance > radius 
 */
inline long d_sub_r(long x, long y, long cx, long cy, long r)
{
    long dx = x - cx; long dy = y - cy;
    return dx*dx + dy*dy - r*r;
}

// 동일한 index가 있을 수 있나?
// 인덱스는 다르지만 좌표는 같을 수 있나?(아마 이거일 것)
void set_ixy(int nums[4], IXY ixys[]) { // +
    int i = nums[1]; ixys[i].i = i;
    int x = nums[2]; ixys[i].x = x;
    int y = nums[3]; ixys[i].y = y;
}
void del_ixy(int nums[4], IXY ixys[]) { // -
    int i = nums[1]; ixys[i].i = 0; // i = 0 means no ixy
}
void set_xyr(int nums[4], XYR xyrs[], int idx) { // ?
    int x = nums[1]; xyrs[idx].x = x;
    int y = nums[2]; xyrs[idx].y = y;
    int r = nums[3]; xyrs[idx].r = r; // r = 0 means no xyr
}
// consider inlining above 3 set functions

int tok2int(const char* tok) {
    if(tok == NULL){ return -1; }
  printf("[%s]", tok);
    switch(tok[0]){
        case '+':
        case '-':
        case '?': return (int)tok[0];
        default: 
                  return atoi(tok);
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
    printf("\n---- %d %d %d %d ---- \n", 
            nums[0], nums[1], nums[2], nums[3]);
}

int read_inp_f(FILE* fp, IXY ixys[], XYR xyrs[])
{
    char buf[64] = {0,};
    int q_idx = 0;
    while(fgets(buf, sizeof buf, fp) != NULL){
        int nums[4];
        //PRNs(buf); puts("----------");
        buf2nums(buf, nums);
        //printf(":->  %c %d %d %d \n", (char)nums[0], nums[1], nums[2], nums[3]);
        char type = (char)nums[0];
        switch(type){
            case '+': set_ixy(nums, ixys); break;
            case '-': del_ixy(nums, ixys); break;
            case '?': set_xyr(nums, xyrs, q_idx++); break;;
            default: puts("DATA error or something."); abort();
        }
    }
    return SUCCESS;
}
int read_inp(const char* path, IXY ixys[], XYR xyrs[])
{
    FILE* fp = fopen(path, "r");
    if(!fp){
        perror("File opening failed");
        return FAILURE;
    }
    return read_inp_f(fp, ixys, xyrs);
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

void print_xyrs(const XYR xyrs[], int len, const char* sep)
{
    for(int i = 0; i < len; i++){
        XYR xyr = xyrs[i];
        printf("x:%d y:%d r:%d%s", 
                xyr.x, xyr.y, xyr.r, sep);
    }
}

//----------------------------------------------------------------
void query_ans_n2(const IXY ixys[], 
                  long cx, long cy, long r,
                  int n_loop,
                  int* count_inner, int* farthest_idx){
    int cnt_in = *count_inner; int far_idx = *farthest_idx;
    long max_diff_in = -r*r; //  -r^2 < diff_in <= 0
    for(int p_i = 0; p_i < n_loop; p_i++){
        long idx = ixys[p_i].i;
        long x = ixys[p_i].x;
        long y = ixys[p_i].y;
        long diff = d_sub_r(x,y, cx,cy, r);
        if(idx && diff <= 0){ // idx = 0 means invalid point
            cnt_in++;
            if(diff > max_diff_in){
                max_diff_in = diff;
                far_idx = idx;
            }else if(diff == max_diff_in){
                max_diff_in = diff;
                far_idx = (idx < far_idx ? idx : far_idx);
                //PRNd(idx); PRNd(far_idx);
            }
        }
    }
    *count_inner = cnt_in;
    *farthest_idx = far_idx;
}

/* 
 * It converts xyrs as answers. It CHANGES SEMANTIC!
 * So x = # of points, y = index of farthest point.
 * `n_square` means O(N^2) algorithm.
 */
int solve_n_square(const IXY ixys[], XYR xyrs[])
{
    for(int q_i = 0; xyrs[q_i].r != 0; q_i++){
        long cx = xyrs[q_i].x; 
        long cy = xyrs[q_i].y;
        long r = xyrs[q_i].r;

        int count_inner = 0;
        int farthest_idx = MAX_LEN + 1; // invalid idx
        query_ans_n2(ixys, cx, cy, r, MAX_LEN,
                     &count_inner, &farthest_idx);
        xyrs[q_i].x = count_inner; // Number of points in circle.
        xyrs[q_i].y = farthest_idx; // Index of farthest point.
        //printf("xyrs:"); PRNd(count_inner), PRNLd(farthest_idx);
    }
    return SUCCESS;
}
