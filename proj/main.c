// map <F4> :wa<CR>:!rm main;make;./main;<CR>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "rg_tree.h"

 
IXY ixys[1000000]; // i x y
XYR xyrs[1000000]; // x y r
IXY* x_sorted[1000000]; // x sorted
IXY* y_sorted[1000000]; // y sorted

int main(void){
    // Read file to ixys
    //read_inp("../data/pin_1.txt", ixys, xyrs);
    read_inp("./fixtures/1.inp", ixys, xyrs);

    print_ixys(ixys, 10, "\n");
    print_xyrs(xyrs, 10, "\n");
    puts("------------");

    solve_n_square(ixys, xyrs);
    print_xyrs(xyrs, 10, "\n");

    /*
    printf("start\n");
    printf("[%d]", sizeof(int));
    printf("[%d]", sizeof(ixys));
    printf("[%d]", sizeof(x_sorted));
    */
    //printf("[%d]", sizeof(long));

    //IXY* ixys = malloc(sizeof(IXY) * 1000000);

    return 0;
}
