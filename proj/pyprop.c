// map <F5> :wa<CR>:!rm pyprop;make;pytest . -vv;<CR>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "rg_tree.h"

    
IXY ixys[1000000]; // i x y, 
// points.
// if not compact, i == 0 means invalid.
// if compact, i == 0 means no more elems
XYR xyrs[1000000]; 
// queries or answers, last val == 0, no more elems
// if queries: (x,y,r). 
// if answers: (n,i, ). n = # of points, i = farthest idx
IXY* x_sorted[1000000]; // x sorted
IXY* y_sorted[1000000]; // y sorted

int main(void){
    read_inp_f(stdin, ixys, xyrs);

    //for(int i = 0; i < 10; i++){
    for(int i = 0; xyrs[i].r; i++){
        XYR xyr = xyrs[i];
        if(xyr.x == 0){
            printf("- [%d, %s]\n", xyr.x, "null");
        }else{
            printf("- [%d, %d]\n", xyr.x, xyr.y);
        }
    }

    return 0;
}
