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
#include <assert.h>


void print_tree(int n_node, Node tree[]){
    int i;
    for(i = 0; i < n_node + 1; i++){
        printf("[%d %d %d] ", 
            tree[i].parent, tree[i].left, tree[i].right);
        if(i == 0 || i % 5 == 0){
            puts("");
        }
    }
    if(i && i % 5){
        puts("");
    }
}

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
// TODO: remove it..
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

// Inorder traversal from Node: tree[beg_idx].
// Report all ixys in the tree[beg_idx].
// `stack` is just int array, Managed in this function.
// 
// return: number of ixys in subtree.
//  `idxes` is returned indexes(+) of ixy in ixys.
//  So idxes are positive!
int ixy_indexes(const Node tree[], int beg_idx, 
                int ixy_idxes[], int prevs[], 
                int stack[]){
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
                if(prevs){ prevs[n_ixy] = popped; }
                //PRNd(popped); PRNd(n_ixy);
                ixy_idxes[n_ixy++] = -left;
            }
            int right = tree[popped].right;
            if(right < 0){
                if(prevs){ prevs[n_ixy] = popped; }
                //PRNd(popped);
                ixy_idxes[n_ixy++] = -right;
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
                tree, v, ixy_idxes + n_ixy, NULL, stack);
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

// Change a (p|l|r) of tree[inode] to new.
// old is value linked to tree[inode].
//
// Return where the child was attached.
// If no change, return null char.
static inline
char set_ref(Node tree[], int inode, int old, int new){
    if(tree[inode].parent == old){
        tree[inode].parent = new;
        return 'p';
    }else if(tree[inode].left == old){
        tree[inode].left = new;
        return 'l';
    }else if(tree[inode].right == old){
        tree[inode].right = new;
        return 'r';
    }
    return '\0';
}

// Return number of node in tree
// search tree by 'x', then search with 'y'.
//  ixy_idxes, prevs, stack are just space for calc
int delete(int n_node, Node tree[], int iidx, IXY ixys[], 
           int ixy_idxes[], int prevs[], int stack[])
{
    IXY want = ixys[iidx];
    int inode = 0; // inode of want ixy.

    // Search by x in tree.
    int prev = 0;
    int curr = 1;
    while(curr > 0 && // curr is not a leaf
          tree[curr].key != want.x){ // 
        prev = curr;
        int k = tree[curr].key;
        int l = tree[curr].left; 
        int r = tree[curr].right;
        curr = (want.x <= k ? l : r);
    }
    // Now, curr is idx of tree(+) or idx of ixys(-) or 0.
    
    // Get inode(= prev) of wanted ixy.
    if(curr < 0){
        IXY ixy = ixys[-curr];
        if(ixy.i == iidx){
            inode = prev;
        }else{
            return n_node; // Do nothing.
        }
    }else if(curr == 0){ 
        return n_node; // Do nothing.
    }else{
        int n_ixys = ixy_indexes(
            tree, curr, ixy_idxes, prevs, stack);
        for(int i = 0; i < n_ixys; i++){
            if(ixy_idxes[i] == iidx){
                inode = prevs[i];
            }
        }
    }
    // Now, we know inode of wanted ixy.
    
    // Delete ixy(leaf) from inode.
    set_ref(tree, inode, -iidx, 0);

    //*
    int parent = tree[inode].parent; 
    while(inode != 0 && (tree[inode].left == 0 && 
                         tree[inode].right == 0)){
        // Delete inode from parent.
        set_ref(tree, parent, inode, 0);

        // Overwrite inode memory with last inode(tree[n_node])
        int last = n_node;
        tree[inode] = tree[last];
        n_node--;

        // Update p, l or r of moved node from last.
        if(inode != last){
            const Node moved = tree[inode];
            if(moved.parent){ 
                set_ref(tree, moved.parent, last, inode);
            }
            if(moved.left){
                set_ref(tree, moved.left, last, inode);
            }
            if(moved.right){
                set_ref(tree, moved.right, last, inode);
            }
        }

        // Set next inode
        if(last != parent){ // Not changed by overwriting
            inode = parent;
        } // If last == parent, it overwrite parent to inode,
          // Therefore next parent is inode. inode := inode.
        parent = tree[inode].parent;
    }
    /*/
    int parent = tree[inode].parent; 
                    printf("====[inode=%d]====", inode);
                    puts("");
                    printf("p=%d l=%d r=%d ",
                        tree[inode].parent,
                        tree[inode].left, tree[inode].right);
                    puts("");
    while(inode != 0 &&
          (tree[inode].left == 0 && tree[inode].right == 0)){
                    printf("----[%d]----\n", inode);
                    puts("----before");
                    print_tree(n_node, tree);
        // Delete inode from parent.
        set_ref(tree, parent, inode, 0);
                    puts("----deleted");
                    print_tree(n_node, tree);
        // Overwrite inode memory with last inode(tree[n_node])
        int last = n_node;
        tree[inode] = tree[last];
        n_node--;
                    puts("----Overwritten");
                    printf("last = %d, in = %d p = %d\n", last, inode, parent);
                    print_tree(n_node, tree);
        // Update p, l or r of moved node from last.
        if(inode != last){
            const Node moved = tree[inode];
            if(moved.parent){ 
                set_ref(tree, moved.parent, last, inode);
            }
            if(moved.left){
                set_ref(tree, moved.left, last, inode);
            }
            if(moved.right){
                set_ref(tree, moved.right, last, inode);
            }
        }
                    puts("----Updated");
                    print_tree(n_node, tree);
                    printf("inode = %d",inode);
        // Set next inode
        if(last != parent){ // Not changed by overwriting
            inode = parent;
        } // If last == parent, it overwrite parent to inode,
          // Therefore next parent is inode. inode := inode.
                    printf(" -> %d \n",inode);
        parent = tree[inode].parent;
    }
    //*/

    return n_node;
}
