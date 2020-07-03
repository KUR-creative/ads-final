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
// all leaves sorted in ascending in same criterion(x/y).
// all_leaves(root) point ixy(neg idx), not inode.
// parent must be positive value except root.
// largest (x/y) val of left subtree = root inode value.
// All ixy have unique index.
// all leaves point 2 sorted ixy.

// delete elem not in the bst, no change.
// range_search property

//Structure
// Use array<val,left,right,parent> to hold nodes
// inode struct points inode or elems in ixys.
// point reference value is index of array.
// if neg idx, ref leaf. pos val, ref inode.
// inode holds split value of x or y.(how to generalize?)

// naive
// treap

#include <stdlib.h>
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

static inline
int is_full(const Node* node){
    return (node->left && node->right);
}

// pos in tree
// Memory management function for tree
static inline
int next_pos(int n_node, const Node* tree, const IXY* ixy){
    return n_node + (is_full(tree + n_node) ? 1 : 0);
}

// if no value, then set to 0 (ixys[0] are {0, 0, 0}).
void leaf_value(const Node* leaf, const IXY ixys[], 
                int* l_val, int* r_val, char xORy){
    int l = abs(leaf->left);
    int r = abs(leaf->right);
    *l_val = GET_VAL(ixys[l], xORy);
    *r_val = GET_VAL(ixys[r], xORy);
}

// args:
//  n_node: number of inner nodes in tree.
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
    IXY new_ixy = ixys[iidx];
    int pos = 1;
    if(n_node){
        // Search inode
        pos = next_pos(n_node, tree, &new_ixy);
                //PRNd(pos);
        // Insert leaf
        if(tree[pos].left && tree[pos].right){
        }else if(tree[pos].left){
            tree[pos].right = leaf_index(new_ixy.i);
            int l_val, r_val;
            leaf_value(tree + pos, ixys, 
                       &l_val, &r_val, xORy);
            if(l_val > r_val){
                SWAP(tree[pos].left, tree[pos].right, int);
            }
        }else{
            // link new elem with parent
            // TODO: temporary implementation 
            // just pass n = 3 != 2 case..
            int new_val = GET_VAL(new_ixy, xORy);
            tree[pos].value = new_val;
            tree[pos].left = leaf_index(new_ixy.i);
        }
    }else{ //empty
        int new_val = GET_VAL(new_ixy, xORy);
        tree[pos].value = new_val;
        tree[pos].left = leaf_index(new_ixy.i);
    }

    /* // print tree for dbg
    printf("----------- %d -----------\n", pos);
    printf("ixy: {%d %d %d}\n", 
        new_ixy.i, new_ixy.x, new_ixy.y);
    for(int i = 0; i < n_node + 5; i++){
        Node n = tree[i];
        printf("[%d: %d %d %d] ", 
            n.value, n.parent, n.left, n.right);
    }
    puts("");
    */

    return pos;
}

void make_sparse(int len_dense, IXY dense_arr[], 
                 IXY sparse_arr[])
{
    for(int i = 0; i < len_dense; i++){
        int idx = dense_arr[i].i;
        sparse_arr[idx] = dense_arr[i];
    }
}
