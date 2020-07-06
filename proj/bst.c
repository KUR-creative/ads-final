//map <F4> :wa<CR>:!rm libbst.so;make;python bst_test.py<CR>
//map <F5> :wa<CR>:!rm libbst.so;make;pytest . -vv<CR>
//map <F8> :wa<CR>:!rm libbst.so;make;pytest . -s -vv<CR>

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


void print_arr(int n, int arr[])
{
    for(int i = 0; i < n; i++){
        printf("{%d}", arr[i]);
    }
}

void print_stack(int top, int stack[])
{
    PRNd(top);
    if(top > -1){
        printf("stack: ");
        for(int i = 0; i < top + 1; i++){
            printf("[%d]", stack[i]);
        }
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

// if idx > 0, return key of node from tree.
// if idx < 0, return key of ixy from ixys.
// if idx = 0, return 0.
static inline
int node_key(int idx, const Node tree[], const IXY ixys[], 
             char xORy){
    if(idx > 0){ 
        return tree[idx].key; 
    }else if(idx < 0){ 
        return KEY(ixys[-idx], xORy); 
    }else{ 
        return 0; 
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

int delete(int n_node, Node tree[], char xORy,
           int iidx, IXY ixys[])
{
    return n_node;
}

// Inorder traversal from Node: tree[beg_idx].
// Report all ixys in the tree[beg_idx].
// `stack` is just int array, Managed in this function.
// 
// return: number of ixys in subtree.
//  `idxes` is returned indexes(+) of ixy in ixys.
//  So idxes are positive!
int ixy_indexes(const Node tree[], int beg_idx, 
                int idxes[], int stack[]){
    int top = -1; // stack top
    int n_ixy = 0;
    int curr = beg_idx;
    do{
        while(curr > 0){
            stack[++top] = curr;
            curr = tree[curr].left;
        }
        if(top > -1){
            int popped = stack[top--]; // popped: inode index
            // log ixy indexes
            int left = tree[popped].left;
            if(left < 0){
                idxes[n_ixy++] = -left;
            }
            int right = tree[popped].right;
            if(right < 0){
                idxes[n_ixy++] = -right;
            }else if(right > 0){
                curr = right;
            }
        }
    } while(top > -1 || curr > 0);
    // End if stack is empty && can't go deep
    return n_ixy;
}

// return: number of included in range [min, max]
//         included idxes of coordinates are saved in `ixys`
//         `stack` is just int array, Managed in this function.
int includeds1d(const Node tree[], const IXY ixys[], 
                char xORy, int min, int max, 
                int ixy_idxes[], int stack[])
{
    int top = -1; // stack top.
    int n_ixy = 0;
    int split = 1;

    // Find split node idx.
    while(split > 0 && // split is not a leaf
         (tree[split].key < min || // KEY < min <= max
          max < tree[split].key)){ // or    min <= max < KEY
        int k = tree[split].key;
        int l = tree[split].left; 
        int r = tree[split].right;
        split = (max <= k ? l : r);
    }
    // Now, split is idx of tree(+) or idx of ixys(-) or 0.
    
    //PRNd(split);puts("");
    if(split < 0){
        int k = KEY(ixys[-split], xORy);
        if(min <= k && k <= max){
            ixy_idxes[n_ixy++] = -split;
        }
        return n_ixy;
    }else if(split == 0){
        return n_ixy; // Do nothing.
    }

    // Left path
    int v = tree[split].left; 
    while(v > 0){
        int vr = tree[v].right;
        if(min <= tree[v].key){
            stack[++top] = vr;
            v = tree[v].left;
        }else{
            v = vr;
        }
    }
    // Now, v <= 0, check v is included in [min,max]
    int vk = KEY(ixys[-v], xORy);
    if(v < 0 && min <= vk && vk <= max){
        stack[++top] = v;
    }
    // Save begin index of left vertices.
    int beg_left_vs = top; 

    // Right path
    v = tree[split].right;
    while(v > 0){ 
        int vl = tree[v].left;
        if(tree[v].key <= max){
            stack[++top] = vl;
            v = tree[v].right;
        }else{
            v = vl;
        }
    }
    // Now, v <= 0, check v is included in [min,max]
    vk = KEY(ixys[-v], xORy);
    if(v < 0 && min <= vk && vk <= max){
        stack[++top] = v;
    }

    // Copy left_vs from stack to vla array in reverse.
    int n_vs = top + 1;
    int vs[n_vs]; int i_vs = 0;
    for(int i = beg_left_vs; i >= 0; i--){
        vs[i_vs++] = stack[i];
    }
    // Copy right_vs from stack to vla array.
    for(int i = beg_left_vs + 1; i < n_vs; i++){
        vs[i_vs++] = stack[i];
    }

    // Write ixy_idxes.
    for(int i = 0; i < n_vs; i++){
        v = vs[i];
        if(v > 0){
            n_ixy += ixy_indexes(
                tree, v, ixy_idxes + n_ixy, stack);
        }else if(v < 0){
            ixy_idxes[n_ixy++] = -v;
        }
    }
    return n_ixy;
}


// return: number of included in range [min, max]
//  included idxes of coordinates are saved in `ret_arr`
//  `arr` is just int array, Managed in this function.
//  ret_arr is sorted in x, not y.
int includeds2d(const Node tree[], const IXY ixys[], 
                int min_x, int max_x, 
                int min_y, int max_y, 
                int ret_arr[], int arr[])
{
    // Use ret_arr as stack of includeds1d TEMPORARILY 
    int* x_sorted = arr;
    int* stack = ret_arr; // temporary calc space
    int n_xsorted = includeds1d(
        tree, ixys, 'x', min_x, max_x, x_sorted, stack);

    // Use arr as just an int array
    int n_ret = 0;
    for(int i = 0; i < n_xsorted; i++){
        int idx = x_sorted[i];
        if(min_y <= ixys[idx].y && ixys[idx].y <= max_y){
            ret_arr[n_ret++] = idx;
        }
    }
    return n_ret;
}
