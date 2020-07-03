#ifndef _BST_
#define _BST_

#include "common.h"

typedef struct {
    int value, parent, left, right;
    // value is x or y split value.
    // p,l,r is index of array.
    // valid index > 0, 0 index means NULL.
    //  if l or r is (-), then idx of ixys.
    //  if p or l or r is (+), then idx of tree array.
} Node;
// Binary Search Tree is array of Node.

#define KEY(ixy, mode) \
    (((mode) == 'x') ? (ixy).x : (ixy).y)
int insert(int n_node, Node tree[], char xORy,
           int iidx, IXY ixys[]);

#endif//_BST_
