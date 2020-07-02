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

void print_arr(int n, Node arr[])
{
    for(int i = 0; i < n; i++){
        printf("%d %d %d %d \n", arr[i].value,
            arr[i].parent, arr[i].left, arr[i].right);
    }
}

static inline 
int leaf_index(int idx){
    return (idx > 0 ? -idx : idx);
}

static inline
int num_nodes(Node tree[]){
    int n = 0;
    for(int idx = 1; 
        tree[idx].parent != 0 && tree[idx].left != 0;
        idx++){
        n++;
    }
    return n;
}

// args:
//  n_node: number of inner nodes in tree.
//          -1 then it automatically calculated( O(N) )
//          n_node + 1 = index to insert new inode.
//  tree:   tree.
//  xORy:   sort mode. 'x' or 'y'
//  iidx:   idx of inserted elem in ixys.
//  ixys:   ixy array.
//
// return: 
//  number of inner nodes in tree after insertion.
//  
// precondition:
//  n_node == # of elems in tree
//  tree must be sorted in consistent 'x' or 'y'
//
// postcondition:
//  ret >= n_node
//
// empty tree is 0 filled array of Node.
// insert then: before length <= after length
int insert(int n_node, Node tree[], char xORy,
           int iidx, IXY ixys[])
{
    int new_pos = (
        n_node == -1 ? num_nodes(tree) : n_node
    ) + 1;
    IXY new_ixy = ixys[iidx];
    int new_val = (xORy == 'x' ? new_ixy.x : new_ixy.y);

    tree[new_pos].value = new_val;
    tree[new_pos].left = leaf_index(new_ixy.i);

    return 1;
}

void make_sparse(int len_dense, IXY dense_arr[], 
                 IXY sparse_arr[])
{
    for(int i = 0; i < len_dense; i++){
        int idx = dense_arr[i].i;
        sparse_arr[idx] = dense_arr[i];
    }
}
