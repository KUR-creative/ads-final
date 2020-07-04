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

// TODO: remove it.
static inline 
int leaf_index(int idx){
    return (idx > 0 ? -idx : idx);
    // return -abs(idx);
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
int is_empty(const Node* node){
    return (node->left == 0) && (node->right == 0);
}
static inline
int is_full(const Node* node){
    return (node->left && node->right);
}

// if no value, then set to 0 (ixys[0] are {0, 0, 0}).
void leaf_key(const Node* leaf, const IXY ixys[],
              char xORy, int* l_val, int* r_val){
    int l = abs(leaf->left);
    int r = abs(leaf->right);
    *l_val = KEY(ixys[l], xORy);
    *r_val = KEY(ixys[r], xORy);
}

// TODO: remove it..
static inline
int is_leaf(const Node* node){
    return node->left <= 0 && node->right <= 0;
}

// next node in bst searching for new_key
// ret: left or right
// ret: (-)leaf, (0)empty leaf, (+)inode - go deeper
// TODO: remove it..
static inline
int next_idx(const Node* n, int new_key){
    return (n->key < new_key ? n->right : n->left);
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
    /* 
    int dst = curr;
    if(is_full(tree + curr)){
        n_node++; // Assign memory.
        dst = n_node;
        // Implement full case
        int lkey, rkey;
        leaf_key(tree + dst, ixys, xORy, &lkey, &rkey);
        if(nkey <= lkey){       
            // C1: N <- (N) -> (l)
            //              l <- -> r
        }else if(nkey <= rkey){
            // C2: l <- (l) -> (N)
            //              N <- -> r
        }else{
            // C3: l <- (l) -> (r)
            //              r <- -> N
        }
    }else{
        // Save ixy idx
        tree[dst].parent = prev; // TODO: or curr
        if(is_empty(tree + dst)){
            tree[dst].left = -ixy.i;
        }else{ // l <- N ->( )
            tree[dst].right = -ixy.i;
            int lkey, rkey;
            leaf_key(tree + dst, ixys, xORy, &lkey, &rkey);
            if(lkey > rkey){
                SWAP(tree[dst].left, tree[dst].right, int);
            }
        }
    }
    */


    //puts("--------"); PRNd(prev);PRNd(curr);PRNLd(dst);

    //*
    /*/
    // Assign memory (when full)
    int curr = next_pos(n_node, tree, &ixy);

    // Link inode
    // TODO: not prev, but parent!
    if(curr == n_node + 1){
        int prev = n_node; // previous index 

        int prev_left = abs(tree[prev].left);
        int new_val = KEY(ixys[prev_left], xORy);

        int l_val, r_val;
        leaf_key(tree + curr, ixys, xORy, &l_val, &r_val);
        // link current node
        tree[curr].parent = prev;
        tree[curr].key = new_val;
        tree[curr].left = tree[prev].left;
        // link parent
        tree[prev].left = curr;
    }

    // Save value to current node
    if(tree[curr].left && tree[curr].right){
        // l <- N -> r
    }else if(tree[curr].left || tree[curr].right){
        // l <- N ->( )  or  ( ) <- N -> r
        tree[curr].right = -ixy.i;
        int l_val, r_val;
        leaf_key(tree + curr, ixys, xORy, &l_val, &r_val);
        if(l_val > r_val){
            SWAP(tree[curr].left, tree[curr].right, int);
        }
    }else{// ( ) <- N -> ( )
        tree[curr].left = -ixy.i;
    }
    */

    //*
    /*/ // print tree for dbg
    printf("----------- %d -----------\n", curr);
    printf("ixy: {%d %d %d}\n", 
        ixy.i, ixy.x, ixy.y);
    for(int i = 0; i < n_node + 5; i++){
        Node n = tree[i];
        printf("[%d: %d %d %d] ", 
            n.key, n.parent, n.left, n.right);
    }
    puts("");
    //*/
}

void make_sparse(int len_dense, IXY dense_arr[], 
                 IXY sparse_arr[])
{
    for(int i = 0; i < len_dense; i++){
        int idx = dense_arr[i].i;
        sparse_arr[idx] = dense_arr[i];
    }
}
