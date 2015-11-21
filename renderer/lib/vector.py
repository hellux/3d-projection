def vector_add(u, v):
    """returns the sum of vectors u and v"""
    return (u[0]+v[0], u[1]+v[1], u[2]+v[2])

def vector_subtract(u, v):
    """returns the difference (vector) of vectors u and v"""
    return (u[0]-v[0], u[1]-v[1], u[2]-v[2])


def inner_product(u, v):
    """returns the inner product (vector) of two vectors u and v
    or the number n and vector v"""
    product = []
    
    if isinstance(u, (tuple, list)) and isinstance(v, (tuple, list)):
        if len(u) == len(v):
                for a, b in zip(u, v):
                    product.append(a*b)
    elif isinstance(u, (float, int)) and isinstance(v, (tuple, list)):
        for n in v:
            product.append(u*n)
    else:
        print('Inner product not calculated: invalid input')
        return None

    return tuple(product)
    

def dot_product(u, v):
    """returns the dot product (rational number) of vectors u and v 
    or the rational number u and vector v"""
    product_sum = 0
    
    if isinstance(u, (tuple, list)) and isinstance(v, (tuple, list)):
        if len(u) == len(v):
                for a, b in zip(u, v):
                    product_sum += a*b
    elif isinstance(u, (float, int)) and isinstance(v, (tuple, list)):
        for n in v:
            product_sum += u*n
    else:
        print('Dot product not calculated: invalid input')
        print('u is of type {} and v is of type {}'.format(type(u), type(v)))
        return None

    return product_sum

def cross_product(u, v):
    """returns the cross product (vector) of vectors u and v"""
    if len(u) == len(v) == 3:
        return (u[1]*v[2] - u[2]*v[1],
                u[2]*v[0] - u[0]*v[2],
                u[0]*v[1] - u[1]*v[0])
    else:
        print('Cross product not calculated: dimension mismatch')
        return None

def magnitude(u):
    """returns the magnitude (length) of vector u"""
    return (u[0]**2 + u[1]**2 + u[2]**2)**0.5

def test():
    n = 4
    u = (2, 1, 0)
    v = (3, 4, 0)

    assert u == (2, 1, 0)
    assert u != v
    assert vector_add(u, v) == (5, 5, 0)
    assert vector_subtract(u, v) == (-1, -3, 0)
    assert inner_product(u, v) == (6, 4, 0)
    assert inner_product(n, u) == (8, 4, 0)
    assert dot_product(u, v) == 10
    assert dot_product(n, u) == 12
    assert cross_product(u, v) == (0, 0, 5)

    #render pixel of triangle scenario test
    from matrix import inverse, matrix_multiply

    S = (1, 0, 0)
    V = (2, 0.5, 0)

    P0 = (4, 0, 3)
    P1 = (4, 0, -3)
    P2 = (4, 6, 0)

    N = cross_product(vector_subtract(P1, P0), vector_subtract(P2, P0))
    t = -(dot_product(N, S) - dot_product(N, P0)) / dot_product(N, V)
    P = vector_add(S, inner_product(t, V))

    R = vector_subtract(P, P0)
    Q1 = vector_subtract(P1, P0)
    Q2 = vector_subtract(P2, P0)

    M1 = ((dot_product(Q1, Q1), dot_product(Q1, Q2)),
          (dot_product(Q1, Q2), dot_product(Q2, Q2)))
    M2 = ((dot_product(R, Q1),),
          (dot_product(R, Q2),))

    w = matrix_multiply(inverse(M1), M2)
    w = (1-w[0][0]-w[1][0], w[0][0], w[1][0])

    assert N == (36, 0, 0)
    assert t == 1.5
    assert P == (4, 0.75, 0)
    assert all([i > 0 for i in w])

    print('All tests ran successfully, no errors!')
    

    u = ( 1, 2, 3 )
    v = ( 4, 5, 6 )
    from time import time
    start = time()
    for i in range(1000000):
        d = vector_subtract(u, v)
    print(time() - start)

if __name__ == '__main__':
    test()
