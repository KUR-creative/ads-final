from ctypes import CDLL, Structure
from ctypes import c_int, c_char
from collections import namedtuple

import funcy as F
import pytest

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

#--------------------------------------------------------
def test_insert_1elem_then_len_is_1():
    n_node = 0
    tree = (NODE * 10)()
    ixys = (IXY * 10)(Ixy(), (1, 9,3))
    mode = c_char(b'x');

    n_node = bst.insert(n_node, tree, mode, 1, ixys)

    print(*map(pipe(pyobj,tuple),tree))
    assert n_node == 1
    assert pyobj(tree[1]) == Node(9, 0,-1,0)

#@pytest.mark.skip(reason='not now')
def test_insert_1elem_then_len_is_1_1():
    n_node = -1
    tree = (NODE * 10)()
    ixys = (IXY * 10)(Ixy(), (1, 9,3))
    mode = c_char(b'x');

    n_node = bst.insert(n_node, tree, mode, 1, ixys)

    print(*map(pipe(pyobj,tuple),tree))
    assert n_node == 1
    assert pyobj(tree[1]) == Node(9, 0,-1,0)

#test_insert_1elem_then_len_is_1()
print(pyobj(NODE(1,2,3,-4)))
