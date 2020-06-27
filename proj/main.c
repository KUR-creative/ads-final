#include <stdio.h>
#include <stdlib.h>
#include "rg_tree.h"
 
typedef struct {
    int y;
    int x;
    int i;
} YXI;

YXI arr[1000000]; 
YXI* x_sorted[1000000]; // x sorted
YXI* y_sorted[1000000]; // y sorted

int main(void){
    printf("start\n");
    printf("[%d]", sizeof(int));
    printf("[%d]", sizeof(arr));
    printf("[%d]", sizeof(x_sorted));
    printf("[%d]", sizeof(y_sorted));

    //YXI* arr = malloc(sizeof(YXI) * 1000000);

    printf("end\n");
    foo();
    return 0;
}
