#ifndef _BST_
#define _BST_

#include "common.h"

typedef struct {
    int key, parent, left, right;
    // key is x or y split key.
    // p,l,r is index of array.
    // valid index > 0, 0 index means NULL.
    //  if l or r is (-), then idx of ixys.
    //  if p or l or r is (+), then idx of tree array.
    // n_inode: as root of subtree, number of inode. 
} Node;
// Binary Search Tree is array of Node.

#define KEY(ixy, mode) \
    (((mode) == 'x') ? (ixy).x : (ixy).y)

void print_tree(int n_node, Node tree[]);
void print_arr(int n, int arr[]);
void print_stack(int top, int stack[]);
void make_sparse(int len_dense, IXY dense_arr[], 
                 IXY sparse_arr[]);

int insert(int n_node, Node tree[], char xORy,
           int iidx, IXY ixys[]);
int delete(int n_node, Node tree[], int iidx, IXY ixys[], 
           int ixy_idxes[], int prevs[], int stack[]);

int includeds1d(const Node tree[], const IXY ixys[], 
                char xORy, int min, int max, 
                int ixy_idxes[], int stack[]);
int includeds2d(const Node tree[], const IXY ixys[], 
                int min_x, int max_x, 
                int min_y, int max_y, 
                int ret_arr[], int arr[]);
#endif//_BST_
