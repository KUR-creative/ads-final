from ctypes import CDLL, Structure
from ctypes import c_int, c_char, byref
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

NODE_props = 'key parent left right'.split()
class NODE(Structure):
    _fields_ = list(zip(NODE_props, F.repeat(c_int)))
Node = namedtuple('Node', NODE_props, defaults=[0,0,0,0])

IXY_props = 'i x y'.split()
class IXY(Structure):
    _fields_ = list(zip(IXY_props, F.repeat(c_int)))
Ixy = namedtuple('Ixy', IXY_props, defaults=[0,0,0])

PROPS = {NODE: NODE_props, IXY: IXY_props}
PYTYPE = {NODE: Node, IXY: Ixy}

MAX_LEN = 1000000

def pyobj(cobj): 
    global PROPS, PYTYPE
    ctype = type(cobj); pytype = PYTYPE[ctype]
    props = F.lmap(prop, PROPS[ctype])
    return pytype(*map(lambda p: p(cobj), props))

def sparse_array(ixys):
    dense_arr = (IXY * len(ixys))(*ixys)
    sparse_arr = (IXY * MAX_LEN)()
    bst.make_sparse(len(ixys), dense_arr, sparse_arr)
    return sparse_arr

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
def all_ixy_idxes(tup_tree):
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

def all_inodes(tup_tree):
    def inodes(node):
        v,p,l,r = node
        left  = inodes(tup_tree[l]) if l > 0 else []
        right = inodes(tup_tree[r]) if r > 0 else []
        return [*left, node, *right]
    return inodes(tup_tree[1])

'''
xs = (Ixy(), 
      Ixy(), Ixy(), Ixy(), Ixy(4,3,4), Ixy(5,3,5), 
      Ixy(), Ixy(7,8,4), Ixy(8,4,5), Ixy(), Ixy())
tt = (Node(), Node())
print(all_inodes(tt))
tt = (Node(), 
      Node(1,  0, 0, 2), Node(3,  1, 3, 4), 
      Node(3,  2,-5,-4), Node(4,  1,-8,-7), Node())
print(all_inodes(tt))
tt = (Node(), (1, 0, -1, 0), Node())
print(all_inodes(tt))
tt = [(0, 0, 0, 0), (1, 0, 2, -2), (1, 1, -1, -3), 
      (0, 0, 0, 0), (0, 0, 0, 0)]
print(all_inodes(tt))
tt = [(0, 0, 0, 0), 
      (1102486390, 0, 2, 9), (10, 1, -919041, 3), 
      (56580, 2, 5, 4), (1088844602, 3, 6, -266226), 
      (25473, 3, 7, -66049), (588888909, 4, -385079, -871170), 
      (18858, 5, 8, -408489), (18182, 7, -708360, -54656), 
      (1115410972, 1, -558753, -32704), (0, 0, 0, 0)]
tt1= [(0, 0, 0, 0), 
      (1, 0, 2, 9), (2, 1, -919041, 3), 
      (3, 2, 5, 4), (4, 3, 6, -266226), 
      (5, 3, 7, -66049), (6, 4, -385079, -871170), 
      (7, 5, 8, -408489), (8, 7, -708360, -54656), 
      (9, 1, -558753, -32704), (0, 0, 0, 0), 
      (0, 0, 0, 0), (0, 0, 0, 0)]
from pprint import pprint
pprint(all_inodes(tt))
'''
#--------------------------------------------------------
def test_leaf_ixy_idxes():
    xs = (Ixy(), 
          Ixy(), Ixy(), Ixy(), Ixy(4,3,4), Ixy(5,3,5), 
          Ixy(), Ixy(7,8,4), Ixy(8,4,5), Ixy(), Ixy())
    tt = (Node(), Node())
    assert all_ixy_idxes(tt) == ()
    tt = (Node(), 
          Node(1,  0, 0, 2), Node(3,  1, 3, 4), 
          Node(3,  2,-5,-4), Node(4,  1,-8,-7), Node())
    assert all_ixy_idxes(tt) == (-5, -4, -8, -7)
    tt = (Node(), (1, 0, -1, 0), Node())
    assert all_ixy_idxes(tt) == (-1,)
    tt = (Node(), (1, 0, -1, -4), Node())
    assert all_ixy_idxes(tt) == (-1, -4)
    tt = [(0, 0, 0, 0), (1, 0, 2, -2), (1, 1, -1, -3), 
          (0, 0, 0, 0), (0, 0, 0, 0)]
    assert all_ixy_idxes(tt) == (-1, -3, -2)
    #print(F.lmap(lambda i: xs[abs(i)], all_ixy_idxes(tt)))

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
    return dict(ixys=ixys, mode=mode, ixy_map=ixy_map)

@given(gen_ixys_mode_map())
def test_prop__bst_insert(gen):
    ixys, mode, ixy_map = gen['ixys'], gen['mode'], gen['ixy_map']
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
        # Parent must be positive value except root.
        for i,node in enumerate(tree[1:n_inserted+1]):
            assert node.parent >= 0, (n_inserted, i, pyobj(node))
                                   
    #   2. Get ixy idxes from tree structure
        ixy_idxes = all_ixy_idxes(
            tup_tree(tree[:n_inserted+4]))
        # Inserted number of ixys preserved?
        no0idxes = F.compact([abs(i) for i in ixy_idxes])
        assert n_inserted == len(no0idxes), \
            'ixy_idxes = {}, tup_tree = {}'.format(
                ixy_idxes, tup_tree(tree[:n_inserted+4]))
        # All ixy have unique index.
        assert len(set(no0idxes)) == n_inserted
        # All leaves point ixy(neg idx), not inode.
        assert all(idx <= 0 for idx in ixy_idxes), \
            'ixy_idxes = {}, tree = {}'.format(
                ixy_idxes, tup_tree(tree[:n_inserted+4]))

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

    # All inodes must be sorted in ascending order.
        inodes = all_inodes(tup_tree(tree[:n_inserted+4]))
        for n1, n2 in F.pairwise(inodes):
            k1 = n1[0]; k2 = n2[0]
            assert k1 <= k2
        # Inserted ixys are sorted in ascending order.
        neg_idxeseq = F.mapcat(tup(
            lambda k,p,l,r: 
            ((l,) if l < 0 else ()) + ((r,) if r < 0 else ())),
            inodes)
        ixy_idxes = F.map(abs, neg_idxeseq)
        saved_ixys = F.map(lambda i: pyobj(ixy_arr[i]), ixy_idxes)
        keys = F.lmap(key, saved_ixys)
        for k1,k2 in F.pairwise(keys):
            assert k1 <= k2

    # Largest (x/y) val of left subtree = root inode val.
    # (When left subtree is not empty.)
    # ㄴis this true? or important? TODO: remove it?

    #print('-------------------')
    #print(ixys)
    #print(F.lmap(cobj2tuple,tree[:len(ixys)+3]))

def bst_tree(ixys, xORy):
    n_node = 0 # empty
    tree = (NODE * MAX_LEN)()
    ixy_arr = sparse_array(ixys)
    for n_inserted, (i,x,y) in enumerate(ixys, start=1):
        n_node = bst.insert(
            n_node, tree, xORy, i, ixy_arr)
    return n_node, ixy_arr, tree, n_inserted

'''
@st.composite
def gen_subtree_ixys_data(draw):
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

    #idxes = F.lmap(F.first, ixys)
    xORy = c_char(mode.encode())
    n_node, ixy_arr, c_bst, n_inserted = bst_tree(ixys, xORy)

    tup_bst = tup_tree(c_bst[:n_node+4])
    beg_idx = draw(st.sampled_from(range(1, n_node+1)))
    ixy_idxes = [-i for i in all_ixy_idxes(tup_bst[beg_idx:])]

    return dict(ixys=ixys, mode=mode, ixy_map=ixy_map, 
                c_bst=c_bst, n_inserted=n_inserted,
                beg_idx=beg_idx, ixy_idxes=ixy_idxes)

@given(gen_subtree_ixys_data())
def test_prop__subtree_ixys(gen):
    ixys = gen['ixys']; mode = gen['mode']
    c_bst = gen['c_bst']; n_inserted = gen['n_inserted']
    beg_idx = gen['beg_idx']; ixy_idxes = gen['ixy_idxes']

    ret_idx_arr = (c_int * n_inserted)()
    n_ixys = bst.ixy_idxes(byref(c_bst[beg_idx]), ret_idx_arr)
    actual = [int(i) for i in ret_idx_arr[:n_ixys]]
    print('=--=')
    print(ret_idx_arr)
    print(actual)
    assert n_ixys == len(ixy_idxes)
    #assert False
'''

@st.composite
def gen_range_search_data(draw):
    global MAX_LEN
    max_key = 2147483647 #2**31
    
    mode = random.choice('xy')
    key = prop(mode)
    
    ixys = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=MAX_LEN),
            st.integers(min_value=1, max_value=max_key),
            st.integers(min_value=1, max_value=max_key),
        ),
        min_size=2, max_size=MAX_LEN,
        unique_by=lambda ixy:ixy[0]
    ))
    
    nt_ixys = F.lmap(tup(Ixy), ixys)
    
    max_val_ = key(max(nt_ixys, key=key))
    max_val = min(max_val_ + max_val_ // 2, max_key)
    min_val_ = key(min(nt_ixys, key=key))
    min_val = max(min_val_ - min_val_ // 2, 1)
    
    min_key, max_key = sorted([
        draw(st.integers(
            min_value=min_val, max_value=max_val)),
        draw(st.integers(
            min_value=min_val, max_value=max_val))
    ])

    def included(ixy):
        return min_key <= key(ixy) <= max_key
    def excluded(ixy):
        return key(ixy) < min_key or max_key < key(ixy)
    includeds = [tuple(ixy) for ixy in 
        sorted(filter(included,nt_ixys), key=key)]
               #key=lambda ixy:(key(ixy), ixy[0]))]
    excludeds = [tuple(ixy) for ixy in filter(excluded,nt_ixys)]
    
    ixy_map = F.zipdict(
        map(F.first, ixys), map(tup(Ixy), ixys))
    return dict(
        ixys=ixys, mode=mode, ixy_map=ixy_map,
        min_key=min_key, max_key=max_key,
        includeds=includeds, excludeds=excludeds
    )

#@pytest.mark.skip(reason='not now')
@given(gen_range_search_data())
def test_prop__range_search(gen):
    ixys = gen['ixys']
    mode = gen['mode']; xORy = c_char(mode.encode())
    min_key = gen['min_key']
    max_key = gen['max_key']
    includeds = gen['includeds']

    n_node, ixy_arr, tree, n_inserted = bst_tree(ixys, xORy)
    
    ixy_idxes = (c_int * n_inserted)()
    n_included = bst.includeds1d(
        n_node, tree, min_key, max_key, ixy_idxes)

    actual_idxes = [int(i) for i in ixy_idxes]
    expect_idxes = F.lmap(F.first, includeds)
    #actual_ixys = F.lmap(cobj2tuple, ixy_idxes)

    #assert n_included == len(includeds)
    assert actual_idxes == expect_idxes
