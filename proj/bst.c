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
// parent must be positive idx except root.
// largest (x/y) val of left subtree = root inode key.
// All ixy have unique index.
// all leaves point 2 sorted ixy.

// delete elem not in the bst, no change.
// range_search property

//Structure
// Use array<val,left,right,parent> to hold nodes
// inode struct points inode or elems in ixys.
// point reference is index of array.
// if neg idx, ref leaf. pos val, ref inode.
// inode holds split key of x or y.(how to generalize?)

// naive
// treap

#include <stdlib.h>
#include "bst.h"


void print_arr(int n, Node arr[])
{
    for(int i = 0; i < n; i++){
        printf("%d %d %d %d \n", arr[i].key,
            arr[i].parent, arr[i].left, arr[i].right);
    }
}

void make_sparse(int len_dense, IXY dense_arr[], 
                 IXY sparse_arr[])
{
    for(int i = 0; i < len_dense; i++){
        int idx = dense_arr[i].i;
        sparse_arr[idx] = dense_arr[i];
    }
}

//--------------------------------------------------------
// if no value, then set to 0 (ixys[0] are {0, 0, 0}).
// TODO: remove it..
void leaf_key(const Node* leaf, const IXY ixys[],
              char xORy, int* l_val, int* r_val){
    int l = abs(leaf->left);
    int r = abs(leaf->right);
    *l_val = KEY(ixys[l], xORy);
    *r_val = KEY(ixys[r], xORy);
}

// next node in bst searching for new_key
// ret: left or right
// ret: (-)leaf, (0)empty leaf, (+)inode - go deeper
// TODO: remove it..
static inline
int next_idx(const Node* n, int new_key){
    return (n->key < new_key ? n->right : n->left);
}

//--------------------------------------------------------
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
    IXY ixy = ixys[iidx];
    int nkey = KEY(ixy, xORy); // new key

    // Root case
    if(n_node == 0){
        tree[1].key = nkey;
        tree[1].left = -ixy.i;
        return 1;
    }

    /*   + <= prev
     *  / \
     * ?   + <= curr  (loop, curr is idx of tree.)
     *
     *   + <= prev
     *  / \
     * ?   -/0 = curr (break, curr is idx of ixys.)
     */
    // Search indexes to insert ixy
    int prev = 0;
    int curr = 1; // root, curr = index of tree(+) or ixys(-)
    char lORr;
    while(curr > 0){
        prev = curr;
        if(tree[curr].key < nkey){
            curr = tree[curr].right; lORr = 'r';
        }else{
            curr = tree[curr].left; lORr = 'l';
        }
    }
    // Now, curr is index of ixy or 0(empty) to insert ixy.
    
    if(curr == 0){ // empty
        /*   + <= prev
         *  / \
         * ?   0 = curr
         */
        if(lORr == 'l'){
            tree[prev].left = -iidx;
        }else{
            tree[prev].right = -iidx;
        }
    }else{ // curr < 0 (full): Create new inode.
        /*   + <= prev
         *  / \
         * ?   -old = curr (idx of ixys)
         */
        n_node++; // Assign memory.
        int new = n_node;

        // Link new node
        if(lORr == 'l'){
            tree[prev].left = new;
        }else{
            tree[prev].right = new;
        }

        // Write new node
        int ckey = KEY(ixys[-curr], xORy);
        tree[new].parent = prev;
        if(ckey <= nkey){ // old_ixy <- old_key -> new_ixy
            tree[new].key = ckey;
            tree[new].left = curr;
            tree[new].right = -iidx;
        }else{            // new_ixy <- new_key -> old_ixy
            tree[new].key = nkey;
            tree[new].left = -iidx;
            tree[new].right = curr;
        }

        /*   + <= prev
         *  / \
         * ?   k <= new
         *    / \
         *  -i   -o = curr (idx of ixys)
         */
    }

    return n_node;
}

// return: number of included in range [min, max]
//         included coordinates are saved in `ixys`
int includeds1d(int n_node, const Node tree[],
                int min, int max, IXY ixys[])
{
    return 0;
}
