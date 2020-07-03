from ctypes import CDLL, Structure
from ctypes import c_int, c_char
from collections import namedtuple
import random

import funcy as F
import pytest
from hypothesis import given, example
from hypothesis import strategies as st


tup = lambda f: lambda argtup: f(*argtup)
pipe = F.rcompose
def go(x, *fs):
    return pipe(*fs)(x)

def prop(p, obj=None):
    return(getattr(obj, p) if (isinstance(obj,tuple) and 
                               isinstance(p,str))
      else obj[p] if hasattr(obj,'__getitem__')
      else getattr(obj, p) if obj 
      else lambda o: prop(p,o))

def unzip(seq):
    return zip(*seq)

#--------------------------------------------------------
bst = CDLL('./libbst.so')

NODE_props = 'value parent left right'.split()
class NODE(Structure):
    _fields_ = list(zip(NODE_props, F.repeat(c_int)))
Node = namedtuple('Node', NODE_props, defaults=[0,0,0,0])

IXY_props = 'i x y'.split()
class IXY(Structure):
    _fields_ = list(zip(IXY_props, F.repeat(c_int)))
Ixy = namedtuple('Ixy', IXY_props, defaults=[0,0,0])

PROPS = {NODE: NODE_props, IXY: IXY_props}
PYTYPE = {NODE: Node, IXY: Ixy}

def pyobj(cobj): 
    global PROPS, PYTYPE
    ctype = type(cobj); pytype = PYTYPE[ctype]
    props = F.lmap(prop, PROPS[ctype])
    return pytype(*map(lambda p: p(cobj), props))

MAX_LEN = 1000000

#--------------------------------------------------------
def num_leaf(NODE):
    return int(NODE.left < 0) + int(NODE.right < 0)
def is_leaf(NODE):
    return NODE.left <= 0 and NODE.right <= 0

def cobj2tuple(cobj): return pipe(pyobj,tuple)(cobj)
def tup_tree(tree): return F.lmap(cobj2tuple, tree)

def is_tup_leaf(node):
    v,p,l,r = node
    return l <= 0 and r <= 0
def leaf_ixy_idxes(tup_tree):
    ''' Gey ixy indexes in (tuple mapped) tree '''
    def tup_or_empty(i):
        return (i,) if i else ()
    def leaf(l, r):
        return (tup_or_empty(l) + tup_or_empty(r))
    def node_or_leaf(i):
        return(ixy_idxes(tup_tree[i]) if i > 0 
          else tup_or_empty(i))
    def ixy_idxes(node):
        v,p,l,r = node
        return(leaf(l, r) if is_tup_leaf(node)
          else node_or_leaf(l) + node_or_leaf(r))
    return ixy_idxes(tup_tree[1])

#--------------------------------------------------------

def test_leaf_ixy_idxes():
    xs = (Ixy(), 
          Ixy(), Ixy(), Ixy(), Ixy(4,3,4), Ixy(5,3,5), 
          Ixy(), Ixy(7,8,4), Ixy(8,4,5), Ixy(), Ixy())
    tt = (Node(), Node())
    assert leaf_ixy_idxes(tt) == ()
    tt = (Node(), 
          Node(1,  0, 0, 2), Node(3,  1, 3, 4), 
          Node(3,  2,-5,-4), Node(4,  1,-8,-7), Node())
    assert leaf_ixy_idxes(tt) == (-5, -4, -8, -7)
    tt = (Node(), (1, 0, -1, 0), Node())
    assert leaf_ixy_idxes(tt) == (-1,)
    tt = (Node(), (1, 0, -1, -4), Node())
    assert leaf_ixy_idxes(tt) == (-1, -4)
    tt = [(0, 0, 0, 0), (1, 0, 2, -2), (1, 1, -1, -3), 
          (0, 0, 0, 0), (0, 0, 0, 0)]
    assert leaf_ixy_idxes(tt) == (-1, -3, -2)
    #print(F.lmap(lambda i: xs[abs(i)], leaf_ixy_idxes(tt)))

@st.composite
def gen_ixys_mode_map(draw):
    ixys = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=MAX_LEN),
            st.integers(min_value=1, max_value=2147483647),#2**31
            st.integers(min_value=1, max_value=2147483647),#2**31
        ),
        min_size=2, max_size=MAX_LEN,
        unique_by=lambda ixy:ixy[0]
    ))
    mode = random.choice('xy')
    ixy_map = F.zipdict(
        map(F.first, ixys), map(tup(Ixy), ixys))
    return ixys, mode, ixy_map

def sparse_array(ixys):
    dense_arr = (IXY * len(ixys))(*ixys)
    sparse_arr = (IXY * MAX_LEN)()
    bst.make_sparse(len(ixys), dense_arr, sparse_arr)
    return sparse_arr

@given(gen_ixys_mode_map())
def test_prop__bst_insert(ixys_mode_map):
    ixys, mode, ixy_map = ixys_mode_map
    xORy = c_char(mode.encode())
    ixy_arr = sparse_array(ixys)

    key = prop(mode)
    n_node = 0 # empty
    tree = (NODE * MAX_LEN)()
    for n_inserted, (i,x,y) in enumerate(ixys, start=1):
        n_node = bst.insert(
            n_node, tree, xORy, i, ixy_arr)

    # Num of leaves ixy ref = num of inserted ixys
    #   1. Get ixy idxs from memory
        num_leaves = sum(
            map(num_leaf, tree[1:n_inserted+1]))
        assert n_inserted == num_leaves, \
            'n_inserted = {}, tree = {}'.format(
                n_inserted, tup_tree(tree[:n_inserted+4]))
                                   
    #   2. Get ixy idxes from tree structure
        ixy_idxes = leaf_ixy_idxes(
            tup_tree(tree[:n_inserted+4]))
        # Inserted number of ixys preserved?
        no0idxes = F.compact([abs(i) for i in ixy_idxes])
        assert n_inserted == len(no0idxes), \
            'ixy_idxes = {}, tup_tree = {}'.format(
                ixy_idxes, tup_tree(tree[:n_inserted+4]))
        # All ixy have unique index.
        assert len(set(no0idxes)) == n_inserted
        # Inserted ixys are sorted in ascending order.
        inserted_ixys = F.lmap(
            lambda i: ixy_arr[abs(i)], ixy_idxes)
        for ixy1, ixy2 in F.pairwise(inserted_ixys): 
            assert key(ixy1) <= key(ixy2), 'tree = {}' \
                .format(tup_tree(tree[:n_inserted+4]))

    # All leaves: l <= r
        leaves = F.lfilter(is_leaf, tree[:n_inserted+4])
        for leaf in leaves:
            l = leaf.left; r = leaf.right
            if l and r:
                l_val = key(ixy_map[abs(l)])
                r_val = key(ixy_map[abs(r)])
                assert l_val <= r_val  

        #print(F.lmap(cobj2tuple,tree[:len(ixys)+3]))

    # All inodes must be sorted in ascending order.
    # All leaves point ixy(neg idx), not inode.
    # Parent must be positive value except root.
    # Largest (x/y) val of left subtree = root inode val.
    # (When left subtree is not empty.)

    #print('-------------------')
    #print(ixys)
    #print(F.lmap(cobj2tuple,tree[:len(ixys)+3]))
