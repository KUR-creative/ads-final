from ctypes import CDLL, Structure
from ctypes import c_int, c_char
from collections import namedtuple
import random

import funcy as F
import pytest
from hypothesis import given, example, settings
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
def all_ixy_idxes(tup_tree, beg_idx=1):
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
    return ixy_idxes(tup_tree[beg_idx])

def all_inodes(tup_tree, beg_idx=1):
    def inodes(node):
        v,p,l,r = node
        left  = inodes(tup_tree[l]) if l > 0 else []
        right = inodes(tup_tree[r]) if r > 0 else []
        return [*left, node, *right]
    return inodes(tup_tree[beg_idx])

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
tt = [(0, 0, 0, 0), 
      (1, 0, 2, 9), (2, 1, -919041, 3), 
      (3, 2, 5, 4), (4, 3, 6, -266226), 
      (5, 3, 7, -66049), (6, 4, -385079, -871170), 
      (7, 5, 8, -408489), (8, 7, -708360, -54656), 
      (9, 1, -558753, -32704), (0, 0, 0, 0), 
      (0, 0, 0, 0), (0, 0, 0, 0)]
from pprint import pprint
pprint(all_inodes(tt))

pprint(all_inodes(tt, 4))
pprint(all_ixy_idxes(tt, 4))
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

def assert_valid_bst(mode, ixy_map,
                     ixy_arr, tree, n_inserted):
    ''' tree is bst '''
    key = prop(mode)
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

@given(gen_ixys_mode_map())
def test_prop__bst_insert(gen):
    ixys, mode, ixy_map = gen['ixys'], gen['mode'], gen['ixy_map']
    xORy = c_char(mode.encode())
    ixy_arr = sparse_array(ixys)

    n_node = 0 # empty
    tree = (NODE * MAX_LEN)()
    for n_inserted, (i,x,y) in enumerate(ixys, start=1):
        n_node = bst.insert(
            n_node, tree, xORy, i, ixy_arr)

        assert_valid_bst(
            mode, ixy_map, ixy_arr, tree, n_inserted)

    #print('-------------------')
    #print(ixys)
    #print(F.lmap(cobj2tuple,tree[:len(ixys)+3]))

@st.composite
def gen_bst_delete_data(draw):
    ixys = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=MAX_LEN),
            st.integers(min_value=1, max_value=2**31),
            st.integers(min_value=1, max_value=2**31),
        ),
        min_size=1, max_size=MAX_LEN,
        unique_by=lambda ixy:ixy[0]
    ))

    ixy_idxes = F.lmap(F.first, ixys)
    ixy_map = F.zipdict(
        ixy_idxes, map(tup(Ixy), ixys))

    # if tuple, ixy. if int, delete index.
    # if +: ins, -: del
    cmd_idxes = ixy_idxes + [-i for i in ixy_idxes]
    random.shuffle(cmd_idxes)

    return dict(ixys=ixys, del_idxes=ixy_idxes, 
                ixy_map=ixy_map, cmd_idxes=cmd_idxes)

#@given(gen_bst_delete_data())
def test_prop__bst_delete(gen):
    ixys, ixy_map = gen['ixys'], gen['ixy_map']
    mode = 'x'; cmd_idxes = gen['cmd_idxes']

    n_node = 0 # empty
    tree = (NODE * MAX_LEN)()

    ixy_idxes = (c_int * MAX_LEN)()
    prevs = (c_int * MAX_LEN)()
    stack = (c_int * MAX_LEN)()

    ixy_arr = sparse_array(ixys)

    inserted_idxes = []
    n_inserted = 0
    #print('-----------------')
    for idx in cmd_idxes:
        if idx > 0:
            n_node = bst.insert(
                n_node, tree, c_char(b'x'), idx, ixy_arr)

            n_inserted += 1
            inserted_idxes.append(idx)

            assert_valid_bst(
                mode, ixy_map, ixy_arr, tree, n_inserted)
        else: # no idx = 0 case
            before_ixy_idxes = all_ixy_idxes(
                tup_tree(tree[:n_inserted+4]))
            n_node = bst.delete(
                n_node, tree, -idx, ixy_arr,
                ixy_idxes, prevs, stack)

            if -idx in inserted_idxes:
                inserted_idxes.remove(-idx) 
                py_bst = tup_tree(tree[:n_inserted+4])
                after_ixy_idxes = all_ixy_idxes(py_bst)
                assert idx not in after_ixy_idxes, py_bst

                before_len = len(before_ixy_idxes)
                after_len = len(after_ixy_idxes)
                #TODO
                #assert before_len + 1 == after_len

                n_inserted -= 1

            assert_valid_bst(
                mode, ixy_map, ixy_arr, tree, n_inserted)


'''
gen={'cmd_idxes': [1, -1],
     'del_idxes': [1],
     'ixy_map': {1: Ixy(i=1, x=1, y=1)},
     'ixys': [(1, 1, 1)]}
print(gen)
test_prop__bst_delete(gen)
exit();
'''


#from pprint import pprint
#pprint(gen_bst_delete_data().example())


def bst_tree(ixys, xORy):
    '''
    n_node: num of node in tree
    n_inserted: num of inserted ixy
    '''
    n_node = 0 # empty
    tree = (NODE * MAX_LEN)()
    ixy_arr = sparse_array(ixys)
    for n_inserted, (i,x,y) in enumerate(ixys, start=1):
        n_node = bst.insert(
            n_node, tree, xORy, i, ixy_arr)
    return n_node, ixy_arr, tree, n_inserted

@st.composite
def gen_ixy_indexes_data(draw):
    ixys = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=MAX_LEN),
            st.integers(min_value=1, max_value=2147483647),#2**31
            st.integers(min_value=1, max_value=2147483647),#2**31
        ),
        min_size=2, max_size=MAX_LEN,
        unique_by=lambda ixy:ixy[0]
    ))
    mode = random.choice('xy'); xORy = c_char(mode.encode())

    n_node, ixy_arr, c_bst, n_inserted = bst_tree(ixys, xORy)
    beg_idx = random.choice(range(1, n_node+1))

    return dict(ixys=ixys, mode=mode, xORy=xORy,
                c_bst=c_bst, n_inserted=n_inserted,
                beg_idx=beg_idx)

#@settings(max_examples=500)
@given(gen_ixy_indexes_data())
def test_prop__ixy_indexes(gen):
    #print('====================')
    ixys = gen['ixys']; mode = gen['mode']; xORy=gen['xORy']
    c_bst = gen['c_bst']; n_inserted = gen['n_inserted']
    beg_idx = gen['beg_idx']

    ixy_idx_arr = (c_int * n_inserted)()
    stack = (c_int * MAX_LEN)()
    n_ixys = bst.ixy_indexes(
        c_bst, beg_idx, ixy_idx_arr, None, stack)

    actual_idxes = [int(i) for i in ixy_idx_arr[:n_ixys]]
    tup_bst = tup_tree(c_bst[:n_inserted+4])
    expect_idxes = [-i for i in all_ixy_idxes(tup_bst, beg_idx)]

    assert n_ixys == len(expect_idxes), tup_bst
    assert actual_idxes == expect_idxes, \
        f'{actual_idxes} != {expect_idxes}, {tup_bst}'
    #print('--------------------')

'''
gen={'beg_idx': 2,
     'c_bst': None,
     'ixys': [(1, 1, 1), (2, 1, 2), (3, 1, 1)],
     'mode': 'y',
     'n_inserted': 3,
     'xORy': c_char(b'y')}

gen={'beg_idx': 2,
     'c_bst': None,
     'ixys': [(2, 1, 2), (1, 1, 1), (3, 1, 2)],
     'mode': 'y',
     'n_inserted': 3,
     'xORy': c_char(b'y')}
n_node, ixy_arr, c_bst, n_inserted = bst_tree(
    gen['ixys'], gen['xORy'])
gen['c_bst'] = c_bst
test_prop__ixy_indexes(gen)
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
#@settings(max_examples=10000)
@given(gen_range_search_data() .filter(lambda g: g['max_key'] - g['min_key'] > 1))
def test_prop__range_search(gen):
    ixys = gen['ixys']
    mode = gen['mode']; xORy = c_char(mode.encode())
    min_key = gen['min_key']
    max_key = gen['max_key']
    includeds = gen['includeds']

    n_node, ixy_arr, c_bst, n_inserted = bst_tree(ixys, xORy)
    tup_bst = tup_tree(c_bst[:n_inserted+4])
    #pprint(tup_bst)
    
    ixy_idxes = (c_int * n_inserted)()
    stack = (c_int * MAX_LEN)()
    n_included = bst.includeds1d(
        c_bst, ixy_arr, xORy, min_key, max_key, ixy_idxes, stack)

    actual_idxes = [int(i) for i in ixy_idxes[:n_included]]
    expect_idxes = F.lmap(F.first, includeds)
    #actual_ixys = F.lmap(cobj2tuple, ixy_idxes)

    assert set(actual_idxes) == set(expect_idxes), \
        f'{actual_idxes} != {expect_idxes}, {tup_bst}'
    key = prop(mode)
    for i1, i2 in F.pairwise(actual_idxes):
        assert key(ixy_arr[i1]) <= key(ixy_arr[i2])
    assert n_included == len(includeds), \
        f'{n_included} != {len(includeds)}, {tup_bst}'

'''
gen = {'excludeds': [(734066, 113, 122),
               (604135, 31074, 65),
               (539554, 13632, 1877174209),
               (585501, 239, 569),
               (458877, 3, 108),
               (102289, 171, 33668)],
 'includeds': [(5515, 162, 42278),
               (312766, 6878, 45348),
               (344951, 960568721, 49395),
               (341945, 181, 63985),
               (156775, 1219835225, 203617851)],
 'ixy_map': {5515: Ixy(i=5515, x=162, y=42278),
             102289: Ixy(i=102289, x=171, y=33668),
             156775: Ixy(i=156775, x=1219835225, y=203617851),
             312766: Ixy(i=312766, x=6878, y=45348),
             341945: Ixy(i=341945, x=181, y=63985),
             344951: Ixy(i=344951, x=960568721, y=49395),
             458877: Ixy(i=458877, x=3, y=108),
             539554: Ixy(i=539554, x=13632, y=1877174209),
             585501: Ixy(i=585501, x=239, y=569),
             604135: Ixy(i=604135, x=31074, y=65),
             734066: Ixy(i=734066, x=113, y=122)},
 'ixys': [(341945, 181, 63985),
          (734066, 113, 122),
          (344951, 960568721, 49395),
          (604135, 31074, 65),
          (539554, 13632, 1877174209),
          (585501, 239, 569),
          (312766, 6878, 45348),
          (156775, 1219835225, 203617851),
          (458877, 3, 108),
          (102289, 171, 33668),
          (5515, 162, 42278)],
 'max_key': 1738622769,
 'min_key': 40826,
 'mode': 'y'}
gen={'excludeds': [],
     'includeds': [(1, 1, 1), (2, 1, 1), (3, 1, 1)],
     'ixy_map': {1: Ixy(i=1, x=1, y=1),
      2: Ixy(i=2, x=1, y=1),
      3: Ixy(i=3, x=1, y=1)},
     'ixys': [(1, 1, 1), (2, 1, 1), (3, 1, 1)],
     'max_key': 1,
     'min_key': 1,
     'mode': 'y'}
gen={'excludeds': [(1, 1, 2), (2, 1, 2)],
     'includeds': [],
     'ixy_map': {1: Ixy(i=1, x=1, y=2), 2: Ixy(i=2, x=1, y=2)},
     'ixys': [(1, 1, 2), (2, 1, 2)],
     'max_key': 1,
     'min_key': 1,
     'mode': 'y'}

ex={'excludeds': [(2, 1, 1), (3, 1, 2), (4, 1, 2), (1, 1, 172)],
 'includeds': [],
 'ixy_map': {1: Ixy(i=1, x=1, y=172),
  2: Ixy(i=2, x=1, y=1),
  3: Ixy(i=3, x=1, y=2),
  4: Ixy(i=4, x=1, y=2)},
 'ixys': [(2, 1, 1), (3, 1, 2), (4, 1, 2), (1, 1, 172)],
 'max_key': 5,
 'min_key': 3,
 'mode': 'y'}
from pprint import pprint
pprint(ex)
test_prop__range_search(ex)
'''


@st.composite
def gen_range_query2d_data(draw):
    global MAX_LEN
    max_key = 2147483647 #2**31
    
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
    # min/max x
    max_x_ = max(map(prop('x'),nt_ixys))
    max_xv = min(max_x_ + max_x_ // 2, max_key)
    min_x_ = min(map(prop('x'),nt_ixys))
    min_xv = max(min_x_ - min_x_ // 2, 1)
    min_x, max_x = sorted([
        draw(st.integers(
            min_value=min_xv, max_value=max_xv)),
        draw(st.integers(
            min_value=min_xv, max_value=max_xv))
    ])
    # min/max y
    max_y_ = max(map(prop('y'),nt_ixys))
    max_yv = min(max_y_ + max_y_ // 2, max_key)
    min_y_ = min(map(prop('y'),nt_ixys))
    min_yv = max(min_y_ - min_y_ // 2, 1)
    min_y, max_y = sorted([
        draw(st.integers(
            min_value=min_yv, max_value=max_yv)),
        draw(st.integers(
            min_value=min_yv, max_value=max_yv))
    ])
    
    def included(ixy):
        return(min_x <= ixy.x <= max_x
           and min_y <= ixy.y <= max_y)
    #def excluded(ixy): return key(ixy) < min_key or max_key < key(ixy)
    includeds = go(
        filter(included,nt_ixys),
        F.partial(sorted, key=prop('x')),
        F.curry(F.lmap)(tuple))
    #excludeds = [tuple(ixy) for ixy in filter(excluded,nt_ixys)]
    
    ixy_map = F.zipdict(
        map(F.first, ixys), map(tup(Ixy), ixys))
    return dict(
        ixys=ixys, ixy_map=ixy_map,
        min_x=min_x, max_x=max_x,
        min_y=min_y, max_y=max_y,
        includeds=includeds
    )

#-------------------------------------------------------
#@pytest.mark.skip(reason='not now')
#@settings(max_examples=10000)
@given(gen_range_query2d_data())
def test_prop__range_query2d(gen):
    ixys = gen['ixys']
    min_x = gen['min_x']; max_x = gen['max_x']
    min_y = gen['min_y']; max_y = gen['max_y']
    includeds = gen['includeds']

    n_node, ixy_arr, c_bst, n_inserted = \
        bst_tree(ixys, c_char('x'.encode()))
    tup_bst = tup_tree(c_bst[:n_inserted+4])

    ixy_idxes = (c_int * n_inserted)()
    stack = (c_int * MAX_LEN)()
    n_included = bst.includeds2d(
        c_bst, ixy_arr, min_x, max_x, min_y, max_y, 
        ixy_idxes, stack)

    actual_idxes = [int(i) for i in ixy_idxes[:n_included]]
    expect_idxes = F.lmap(F.first, includeds)
    assert set(actual_idxes) == set(expect_idxes), \
        f'{actual_idxes} != {expect_idxes}, {tup_bst}'
    for i1, i2 in F.pairwise(actual_idxes):
        assert ixy_arr[i1].x <= ixy_arr[i2].x
    assert n_included == len(includeds), \
        f'{n_included} != {len(includeds)}, {tup_bst}'
