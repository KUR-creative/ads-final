import random

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
            key=tup(lambda i,x,y: (dist_sqr(xyr, (i,x,y)), i))
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

@given(gen_small())
def test_t(dat):
    print(dat)
    assert False

'''
print('{:>7}'.format(10))
print('{:>10}'.format(2**32))
print(prob2inp(gen_small().example()))
'''

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
prob = {'answers': [(0, None),
             (0, None),
             (1, 454277),
             (2, 454277),
             (0, None),
             (0, None),
             (0, None),
             (1, 149858),
             (0, None),
             (1, 764250),
             (0, None),
             (0, None),
             (0, None),
             (0, None),
             (3, 454277),
             (0, None),
             (1, 587462),
             (1, 454277)],
 'inside_ixys': [[],
                 [],
                 [(454277, 37256, 30910)],
                 [(587462, 154, 38698), (454277, 37256, 30910)],
                 [],
                 [],
                 [],
                 [(149858, 22987, 35)],
                 [],
                 [(764250, 59501, 11176)],
                 [],
                 [],
                 [],
                 [],
                 [(149858, 22987, 35),
                  (764250, 59501, 11176),
                  (454277, 37256, 30910)],
                 [],
                 [(587462, 154, 38698)],
                 [(454277, 37256, 30910)]],
 'ixys': [(83121, 1698401546, 1724),
          (830322, 40, 810392772),
          (191229, 617894305, 54),
          (851163, 147, 51),
          (587462, 154, 38698),
          (149858, 22987, 35),
          (764250, 59501, 11176),
          (454277, 37256, 30910)],
 'rm_ixys': [(764250, 59501, 11176), (454277, 37256, 30910), (851163, 147, 51)],
 'valid_ixys': [(83121, 1698401546, 1724),
                (830322, 40, 810392772),
                (191229, 617894305, 54),
                (587462, 154, 38698),
                (149858, 22987, 35)],
 'xyrs': [(1673170752, 64673, 12839),
          (111, 210166442, 16688),
          (37726, 32854, 17774),
          (16977, 34541, 26864),
          (1096460811, 538896356, 5027),
          (562391342, 82, 27594),
          (52, 2092109471, 31369),
          (26679, 201, 11221),
          (12572, 4921, 4196),
          (44110, 9095, 21342),
          (1499806485, 32350, 1048),
          (50033, 1771290243, 7858),
          (30, 15864, 10826),
          (1258412348, 57769, 8118),
          (39131, 10221, 23973),
          (193, 51107, 7573),
          (157, 33495, 32718),
          (29590, 42706, 17956)]}
plot(prob)
plt.show()
'''
