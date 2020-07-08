#include <assert.h>
#include <math.h>

#include "solve.h"
#include "common.h"


void solve_bst1d(const Node tree[], const IXY ixys[], 
                 int idxes[], int arr[],
                 int cx, int cy, int r,
                 int* out_num, int* out_idx)
{
    int min_x = cx - r;
    int max_x = cx + r; 
    int min_y = cy - r;
    int max_y = cy + r;

    // 2d query
    int n_chks = includeds2d(
        tree,ixys, min_x,max_x, min_y,max_y, idxes,arr);

    // Calc max_len2, min_idx, and n_in.
    int n_in = 0; // #ixy in circle
    long max_len2 = 0;
    int min_idx = MAX_LEN;

    long r2 = (long)r * (long)r;
    for(int i = 0; i < n_chks; i++){
        int idx = idxes[i];
        long x = (long)ixys[idx].x; 
        long y = (long)ixys[idx].y;

        long dx = x - (long)cx; 
        long dy = y - (long)cy;
        long dist2 = dx*dx + dy*dy;

        if(dist2 <= r2){
            n_in++;
            if(dist2 > max_len2 ||
               (dist2 == max_len2 && idx < min_idx)){
                max_len2 = dist2;
                min_idx = idx;
            }
        }
    }

    // Return n_in, min_idx.
    *out_num = n_in;
    *out_idx = min_idx;
}
