import random

from hypothesis import given, example
from hypothesis import strategies as st

@st.composite
def gen_small(draw):
    # Points to add
    idxes = draw(st.lists(
        st.integers(min_value=1, max_value=1000000),
        min_size=2, unique=True))
    xs = draw(st.lists(
        st.integers(min_value=1, max_value=2147483647),#2**31
        min_size=len(idxes)))
    ys = draw(st.lists(
        st.integers(min_value=1, max_value=2147483647),#2**31
        min_size=len(idxes)))
    # Points to delete 
    k = draw(st.integers(min_value=0, max_value=len(idxes)))
    rm_idxes = random.sample(idxes, k)
    print(len(idxes), k)
    # Queries
    # Make Answer
    return [idxes, xs, ys, rm_idxes]

@st.composite
def gen_big(draw):
    idxes = draw(st.lists(
        st.integers(min_value=1, max_value=1000000),
        min_size=10000, max_size=1000000, unique=True))
    return idxes

#@given(gen_big())
@given(gen_small())
def test_t(x):
    print(x)
    assert False
