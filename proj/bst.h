#ifndef _BST_
#define _BST_

typedef struct {
    int value, parent, left, right;
    // value is x or y split value.
    // p,l,r is index of array.
    // if l or r is (-), then idx of ixys.
    //  (no 0 index in ixys)
    // if p or l or r is (+), then idx of tree array.
} Node;
// Binary Search Tree is array of Node.

#endif//_BST_
