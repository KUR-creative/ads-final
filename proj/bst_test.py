from ctypes import CDLL, Structure
from ctypes import c_int, c_char
from collections import namedtuple
import random

import funcy as F
import pytest
from hypothesis import given, example
from hypothesis import strategies as st

pipe = F.rcompose
def prop(p, obj=None):
    return(getattr(obj, p) if (isinstance(obj,tuple) and 
                               isinstance(p,str))
      else obj[p] if hasattr(obj,'__getitem__')
      else getattr(obj, p) if obj 
      else lambda o: prop(p,o))

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
def test_insert_1elem_mode_x():
    mode = c_char(b'x');
    for n_node in [0,-1]:
        tree = (NODE * 10)()
        ixys = (IXY * 10)(Ixy(), (1, 9,3))

        n_node = bst.insert(n_node, tree, mode, 1, ixys)

        #print(*map(pipe(pyobj,tuple),tree))
        assert n_node == 1
        assert pyobj(tree[1]) == Node(9, 0,-1,0)
def test_insert_1elem_mode_y():
    mode = c_char(b'y');
    for n_node in [0,-1]:
        tree = (NODE * 10)()
        ixys = (IXY * 10)(Ixy(), (1, 9,3))

        n_node = bst.insert(n_node, tree, mode, 1, ixys)

        assert n_node == 1
        assert pyobj(tree[1]) == Node(3, 0,-1,0)

@st.composite
def gen_ixys_mode(draw):
    ixys = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=MAX_LEN),
            st.integers(min_value=1, max_value=2147483647),#2**31
            st.integers(min_value=1, max_value=2147483647),#2**31
        ),
        min_size=2, max_size=MAX_LEN,
        unique_by=lambda ixy:ixy[0]
    ))
    mode = c_char(random.choice(b'xy'))
    return ixys, mode

def sparse_array(ixys):
    dense_arr = (IXY * len(ixys))(*ixys)
    sparse_arr = (IXY * MAX_LEN)()
    bst.make_sparse(len(ixys), dense_arr, sparse_arr)
    return sparse_arr

@given(gen_ixys_mode())
def test_all_inserted_elems_are_in_tree(ixys_mode):
    '''
    print(ixys)
    for no,ixy in enumerate(ixy_arr):
        if ixy.i != 0:
            print(no, ':', ixy.i, ixy.x, ixy.y)
    '''
    ixys, mode = ixys_mode
    ixy_arr = sparse_array(ixys)
    tree = (NODE * MAX_LEN)()
    n_node = -1
    for i,x,y in ixys:
        n_node = bst.insert(
            n_node, tree, mode, i, ixy_arr)
    assert False


@pytest.mark.skip(reason='not now')
def test_insert_2elem_mode_x_inc_order():
    mode = c_char(b'x');
    for n_node in [0,-1]:
        tree = (NODE * 10)()
        ixys = (IXY * 10)(
            Ixy(), (1, 9,3), Ixy(), (3, 10,2))

        n_node = bst.insert(n_node, tree, mode, 1, ixys)
        n_node = bst.insert(n_node, tree, mode, 3, ixys)

        assert n_node == 1
        assert pyobj(tree[1]) == Node(9, 0,-1,-3)

