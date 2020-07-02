"""
Define the C-variables and functions from the C-files that are needed in Python
"""
from ctypes import CDLL, Structure
from ctypes import c_double, c_int
from collections import namedtuple

import funcy as F

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

'''
def do_square_using_c(list_in):
    """Call C function to calculate squares"""
    n = len(list_in)
    c_arr_in = (c_double * n)(*list_in)
    c_arr_out = (c_double * n)()

    bst.c_square(c_int(n), c_arr_in, c_arr_out)
    return c_arr_out[:]

def test_t():
    print(do_square_using_c([1,2,3,4]))
    assert False
'''

#arr = (NODE * 2)((1,2,3,4), (-5,6,7,8))
#bst.print_arr(2, arr)

def pyobj(cobj): 
    global PROPS, PYTYPE
    ctype = type(cobj); pytype = PYTYPE[ctype]
    props = F.lmap(prop, PROPS[ctype])
    return pytype(*map(lambda p: p(cobj), props))
#--------------------------------------------------------
def test_insert_1elem_then_len_is_1():
    tree = (NODE * 10)()
    ixys = (IXY * 10)(Ixy(), (1,2,3))
    n_node = bst.insert(0, tree, 1, ixys)
    print(list(tree))

test_insert_1elem_then_len_is_1()
print(pyobj(NODE(1,2,3,-4)))
