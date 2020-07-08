// map <F4> :wa<CR>:!rm main;make;./main;<CR>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "bst.h"
#include "solve.h"

 
// Data Store
IXY ixys[MAX_LEN];
Node tree[MAX_LEN];
// Temporary space to calc somethings
int stack[MAX_LEN]; // Used in query, delete.
int prevs[MAX_LEN]; // Used in delete.
int ixy_idxes[MAX_LEN]; // Used in query, delete.

int main(int argc, char* argv[]){
    //read_inp("../data/pin_1.txt", ixys, xyrs);
    //char* inp_path = "../data/pin_1.txt";
    char* inp_path = (
        //argc == 1 ? "../data/pin_2.txt" : argv[1]);
        argc == 1 ? "../dat/2_2q.inp" : argv[1]);
    char* out_path = (
        //argc <= 2 ? "pout.txt" : argv[2]);
        argc <= 2 ? "chk.out" : argv[2]);

    FILE* inp = fopen(inp_path, "r");
    if(!inp){
        perror("File opening failed");
        return FAILURE;
    }
    FILE* out = fopen(out_path, "w");

    // Range tree.
    int n_node = 0;

    // Read inp file line by line
    char buf[64] = {0,};
    while(fgets(buf, sizeof buf, inp) != NULL){
        //PRNLd(n_node);
        int nums[4];
        line2nums(buf, nums);
        // Operate by command(+/-/?)
        char type = (char)nums[0];
        switch(type){
        case '+':{ 
            int iidx = set_ixy(nums, ixys); 
            n_node = insert(
                n_node, tree, 'x', iidx, ixys);
            break;
        }
        case '-': {
            //int iidx = del_ixy(nums, ixys); 
            int iidx = nums[1];
            n_node = delete(
                n_node, tree, iidx, ixys, 
                ixy_idxes, prevs, stack);
            del_ixy(nums, ixys); 
            break;
        }
        case '?': {
            int x = nums[1]; int y = nums[2];
            int r = nums[3];
            //PRNd(x); PRNd(y); PRNLd(r);

            //puts("----");
            //print_tree(n_node, tree);
            //puts("");
            /*
            long r2 = (long)r * (long)r;
            for(int i = 0; i < MAX_LEN; i++){
                IXY ixy = ixys[i];
                if(ixy.i != 0){ 
                    long ix = (long)ixy.x; 
                    long iy = (long)ixy.y;
                    long dx = ix - (long)x; 
                    long dy = iy - (long)y;
                    long dist2 = dx*dx + dy*dy;
                    if(dist2 <= r2){
                        PRNd(i); PRNld(r2); PRNLld(dist2);
                    }
                }
            }
            */
            // Solve
            int out_num = 0; int out_idx = 0;
            solve_bst1d(
                tree, ixys, ixy_idxes, stack,
                x, y, r, &out_num, &out_idx);
                      //PRNd(nums[1]); PRNd(nums[2]); PRNLd(nums[3]);
            if(out_num){
                fprintf(out, "%d %d\n", out_num, out_idx);
            }else{
                fputs("0\n", out);
            }
            break;
        }
        default: puts("DATA error or something."); abort();
        }
    }

    fclose(inp);
    fclose(out);
    return 0;
}
