#ifndef _SOLVE_
#define _SOLVE_

#include "bst.h"

// idxes, arr is just calc space 
// tree, ixys are data store.
// Input: x,y,r.
// Return: out_num, out_idx.
void solve_bst1d(const Node tree[], const IXY ixys[], 
                 int idxes[], int arr[],
                 int x, int y, int r,
                 int* out_num, int* out_idx);

#endif//_SOLVE_
