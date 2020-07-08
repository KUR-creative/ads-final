#ifndef _RT2D_
#define _RT2D_

#include "common.h"

typedef struct {
    int key, parent, left, right, second;
    // key is x or y split key.
    // p,l,r is index of array.
    // valid index > 0, 0 index means NULL.
    //  if l or r is (-), then idx of ixys.
    //  if p or l or r is (+), then idx of tree array.
    // n_inode: as root of subtree, number of inode. 
    // second: index to second tree
} N2D;
