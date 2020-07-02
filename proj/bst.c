//map <F4> :wa<CR>:!rm libbst.so;make;python bst_test.py<CR>
//map <F5> :wa<CR>:!rm libbst.so;make;pytest . -vv<CR>

//Support Operations
// insert(ixy)
// delete(ixy)
// range_search(maybe other module?)
// all_nodes(inorder walk)
// all_leaves(ixys)

//Properties
// all_nodes result must be sorted in ascending order.
// all_leaves(root) point ixy(neg idx), not inode.
// parent must be positive value.
// x/y then all leaves sorted in ascending x/y.
// largest (x/y) val of left subtree = root inode value.
// range_search property
// delete not in the bst, no change.

//Structure
// Use array<val,left,right,parent> to hold nodes
// inode struct points inode or elems in ixys.
// point reference value is index of array.
// if neg idx, ref leaf. pos val, ref inode.
// inode holds split value of x or y.(how to generalize?)

// naive
// treap

#include "common.h"
#include "bst.h"

void c_square(int n, double *array_in, double *array_out)
{ //return the square of array_in of length n in array_out
    int i;

    for (i = 0; i < n; i++)
    {
        array_out[i] = array_in[i] * array_in[i];
    }
}

void print_arr(int n, Node arr[])
{
    for(int i = 0; i < n; i++){
        printf("%d %d %d %d \n", arr[i].value,
            arr[i].parent, arr[i].left, arr[i].right);
    }
}

// args:
//  n_node: number of inner nodes in tree.
//          -1 then it automatically calculated( O(N) )
//          It can be index to insert new inode.
//  tree:   tree.
//  iidx:   idx of inserted elem in ixys.
//  ixys:   ixy array.
// return: 
//  number of inner nodes in tree after insertion.
//
// empty tree is 0 filled array of Node.
// insert then: before length <= after length
int insert(int n_node, Node tree[], int iidx, IXY ixys[])
{
    //tree[n_node] = //(IXY){1,2,3};
    tree[n_node].value = 999;
    return 1;
}
