"""
Define the C-variables and functions from the C-files that are needed in Python
"""
from ctypes import c_double, c_int, CDLL
import sys

bst = CDLL('./libbst.so')

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

print(do_square_using_c([1,2,3,4]))
