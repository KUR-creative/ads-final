# map <F4> :wa<CR>:!rm pyprop;make;python prop_test.py<CR>
import random
import subprocess as sp

import yaml
import funcy as F
from hypothesis import given, example
from hypothesis import strategies as st
import matplotlib.pyplot as plt

tup = lambda f: lambda argtup: f(*argtup)
def unzip(seq):
    return zip(*seq)

#def 
@st.composite
def gen_small(draw):
    # Points to add
    ixys = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=1000000),
            st.integers(min_value=1, max_value=2147483647),#2**31
            st.integers(min_value=1, max_value=2147483647),#2**31
        ),
        min_size=2, max_size=1000000,
        unique_by=lambda ixy:ixy[0]
    ))
    
    # Points to delete 
    k = draw(st.integers(min_value=0, max_value=len(ixys) - 2))
    rm_positions = random.sample(list(range(len(ixys))), k)
    rm_ixys = [ixys[pos] for pos in rm_positions]
    
    valid_ixys = F.lremove(
        lambda i: i is None,
        [None if ixy in set(rm_ixys) else ixy
         for ixy in ixys])
    
    # Queries
    xyrs = draw(st.lists(
        st.tuples(
            st.integers(min_value=1, max_value=2147483647),#2**31
            st.integers(min_value=1, max_value=2147483647),#2**31
            st.integers(min_value=1, max_value=2**15)),
        min_size=2
    ))
    
    # Make Answer: O(N^2)
    @F.curry
    def is_inside(xyr, ixy):
        i,x,y = ixy; cx,cy,r = xyr
        return (x - cx)**2 + (y - cy)**2 <= r**2
    def inside_points(xyr):
        return F.lfilter(is_inside(xyr), ixys)
    inside_ixys = F.lmap(inside_points, xyrs)
    
    def dist_sqr(xyr, ixy):
        i,x,y = ixy; cx,cy,r = xyr
        return (x - cx)**2 + (y - cy)**2
    def max_i(xyr, ixys):
        return(max(
            ixys,
            key=tup(lambda i,x,y: 
                (dist_sqr(xyr, (i,x,y)), -i))
        )[0] if ixys else None)
        
    answers = F.lmap(
        lambda in_ixys, xyr:
        (len(in_ixys), max_i(xyr, in_ixys)),
        inside_ixys, xyrs)

    return dict(
        ixys=ixys,
        rm_positions=rm_positions,
        xyrs=xyrs,
        inside_ixys=inside_ixys,
        answers=answers)
        #rm_ixys=rm_ixys,
        #valid_ixys=valid_ixys,

def prob2inp(prob):
    max_len = len(str(max(F.flatten(prob['ixys']))))
    fmt = '{} {:>%d} {:>%d} {:>%d}' % ((max_len,) * 3)
    rm_fmt = '- {:>%d}' % max_len
    
    add_lines = F.lmap(
        lambda ixy: fmt.format('+', *ixy), prob['ixys'])
    rm_lines = F.lmap(
        lambda pos: rm_fmt.format(pos), prob['rm_positions'])
    query_lines = F.lmap(
        lambda xyr: fmt.format('?', *xyr), prob['xyrs'])
    
    lines = add_lines + rm_lines + query_lines
    random.shuffle(lines)
    return '\n'.join(lines)

def run_pyprop(inp):
    p = sp.run(
        ['./pyprop'], stdout=sp.PIPE,
        input=inp, encoding='ascii'
    )
    try:
        nis = yaml.safe_load(p.stdout)
        out = [(n, i) for n,i in nis]
        return p.returncode, out
    except:
        return p.returncode, p.stdout

@given(gen_small())
def test_t(prob):
    inp = prob2inp(prob)

    ret, out = run_pyprop(inp)
    assert ret == 0, out

    ans = prob['answers']
    assert ans == out

def plot(prob):
    circles = F.lmap(
        tup(lambda x,y,r: plt.Circle((x,y), r, alpha=0.15)),
        prob['xyrs'])
    _,xs,ys = unzip(prob['ixys'])
    
    fig, ax = plt.subplots()
    ax.set(xlim=(min(xs) - 100, max(xs) + 100),
           ylim=(min(ys) - 100, max(ys) + 100))
    
    for _,x,y in prob['ixys']:
        plt.scatter(x, y, c='k')#, c=
    for i,x,y in prob['ixys']: ax.annotate(str(i), (x, y))#, c=
        
    for x,y,_ in prob['xyrs']:
        plt.scatter(x, y, c='g')#, c=
    for x,y,r in prob['xyrs']:
        ax.annotate(str(r), (x, y), c='r')#, c=
    for cir in circles:
        ax.add_artist(cir)
        
    for inside_ixys in prob['inside_ixys']:
        for _,x,y in inside_ixys:
            plt.scatter(x, y, c='r')#, c=
    plt.show()

'''
prob={'answers': [(2, 1), (2, 1)],
 'inside_ixys': [[(1, 1, 1), (2, 1, 1)], [(1, 1, 1), (2, 1, 1)]],
 'ixys': [(1, 1, 1), (2, 1, 1)],
 'rm_positions': [],
 'xyrs': [(1, 1, 1), (1, 1, 1)]}
'''
prob={'answers': [(3, 1), (3, 1)],
     'inside_ixys': [[(1, 1, 1), (2, 1, 1), (3, 1, 1)],
      [(1, 1, 1), (2, 1, 1), (3, 1, 1)]],
     'ixys': [(1, 1, 1), (2, 1, 1), (3, 1, 1)],
     'rm_positions': [1],
     'xyrs': [(1, 1, 1), (1, 1, 1)]}
print(prob2inp(prob))
print(prob['answers'])
print(run_pyprop(prob2inp(prob))[1])
#plot(prob)
