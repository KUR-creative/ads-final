#include <assert.h>
#include <math.h>

#include "solve.h"
#include "common.h"

// idxes, arr is just calc space 
// tree, ixys are data store.
// Input: cx,cy,r.
// Return: out_num, out_idx.
/*
void solve_bst1d(const Node tree[], const IXY ixys[], 
                 int idxes[], int arr[],
                 int cx, int cy, int r,
                 int* out_num, int* out_idx)
{
    int min_x = cx - r; //(cx - r > 1 ? cx - r : 1); 
    int max_x = cx + r; 
    int min_y = cy - r; //cy - r > 1 ? cy - r : 1); 
    int max_y = cy + r;
    int r_sqrt2 = (double)r / sqrt(2);

    // [left],[all] box
    int x1la = min_x;
    int x2la = min_x + r - r_sqrt2; // + 1; ?
    int y1la = min_y;
    int y2la = max_y;
    assert(x2la >= x1la);
    assert(y2la >= y1la);
    // [right],[all] box
    int x1ra = max_x - r + r_sqrt2; // - 1; ?
    int x2ra = max_x;
    int y1ra = min_y;
    int y2ra = max_y;
    assert(x2ra >= x1ra);
    assert(y2ra >= y1ra);
    // (mid),[up] box
    int x1mu = x2la + 1;
    int x2mu = x1ra - 1;
    int y1mu = min_y;
    int y2mu = min_y + r - r_sqrt2; // + 1; ?
    assert(x2mu > x1mu);
    assert(y2mu >= y1mu);
    // (mid),[down] box
    int x1md = x2la + 1;
    int x2md = x1ra - 1;
    int y1md = max_y - r + r_sqrt2;
    int y2md = max_y;
    assert(x2md > x1md);
    assert(y2md >= y1md);
    // (mid),(mid) box;
    int x1mm = x2la + 1;
    int x2mm = x1ra - 1;
    int y1mm = y2mu + 1;
    int y2mm = y1md - 1;
    assert(x2mm > x1mm);
    assert(y2mm > y1mm);

    // Calc [left],[all]
    int n_chks = includeds2d(tree, ixys, 
                             x1la,x2la, y1la,y2la, 
                             idxes, arr);
    // Calc [right],[all]
    n_chks += includeds2d(tree, ixys, 
                          x1ra,x2ra, y1ra,y2ra, 
                          idxes + n_chks, arr);
    // Calc (mid),[up]
    n_chks += includeds2d(tree, ixys, 
                          x1mu,x2mu, y1mu,y2mu, 
                          idxes + n_chks, arr);
    // Calc (mid),[down]
    n_chks += includeds2d(tree, ixys, 
                          x1md,x2md, y1md,y2md, 
                          idxes + n_chks, arr);

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

        //PRNld(dist2); PRNLld(r2);
        if(dist2 <= r2){
            n_in++;
            if(dist2 >= max_len2 && idx < min_idx){
                max_len2 = dist2;
                min_idx = idx;
                        //PRNd(n_in); PRNLd(min_idx);
            }
        }
    }

    // Calc (mid),(mid)
    n_in += includeds2d(tree, ixys, 
                        x1mm,x2mm, y1mm,y2mm,
                        idxes, arr);
    // Return n_in, min_idx.
    *out_num = n_in;
    *out_idx = min_idx;
                        //PRNd(n_in); PRNLd(min_idx);
}
*/

void solve_bst1d(const Node tree[], const IXY ixys[], 
                 int idxes[], int arr[],
                 int cx, int cy, int r,
                 int* out_num, int* out_idx)
{
    int min_x = cx - r; //(cx - r > 1 ? cx - r : 1); 
    int max_x = cx + r; 
    int min_y = cy - r; //cy - r > 1 ? cy - r : 1); 
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

        //PRNld(dist2); PRNLld(r2);
        if(dist2 <= r2){
            n_in++;
            if(dist2 >= max_len2 && idx < min_idx){
                max_len2 = dist2;
                min_idx = idx;
                        //PRNd(n_in); PRNLd(min_idx);
            }
        }
    }

    // Return n_in, min_idx.
    *out_num = n_in;
    *out_idx = min_idx;
                        //PRNd(n_in); PRNLd(min_idx);
}
