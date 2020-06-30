// map <F4> :wa<CR>:!rm pyprop;make;pytest . -vv;<CR>
// map <F5> :wa<CR>:!rm main;make;./main;<CR>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "rg_tree.h"

 
IXY ixys[1000000]; // i x y
XYR xyrs[1000000]; // x y r
IXY* x_sorted[1000000]; // x sorted
IXY* y_sorted[1000000]; // y sorted

int main(void){
    read_inp_f(stdin, ixys, xyrs);

    //for(int i = 0; xyrs[i].r; i++){
    for(int i = 0; i < 10; i++){
        printf("- [%d, %s]\n", i, "null");
    }

    return 0;
}
