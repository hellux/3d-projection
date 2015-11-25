def matrix_multiply(A, B):
    """returns the product (matrix) of matrices A and B
    or number A and vector B"""
    if isinstance(A, (tuple, list)) and isinstance(B, (tuple, list)):
        return _mm_multiply(A, B)
    elif isinstance(A, (int, float)) and isinstance(B, (tuple, list)):
        return _nm_multiply(A, B)
    else:
        print('Matrix multiplication was not performed: invalid input')
        return None

def _mm_multiply(A, B):
    """calculate the product of two matrices A, B"""
    mA, nA = len(A), len(A[0]) #number of rows, cols in A
    mB, nB = len(B), len(B[0]) #number of rows, cols in B
    C = []
    if nA != mB:
        print('Matrix multiplication was not performed: invalid dimensions')
        return None
    
    for row in range(mA):
        current_row = []
        for col_B in range(nB):
            current_row.append(0)
            for col_A in range(nA):
                current_row[col_B] += A[row][col_A] * B[col_A][col_B]
        C.append(tuple(current_row))
    print(C)
    return tuple(C)

def _nm_multiply(multiplier, A):
    """calculate the product of a rational number multiplier and matrix A"""
    m, n = len(A), len(A[0])
    B = []
    for row in range(m):
        current_row = []
        for col in range(n):
            current_row.append(multiplier*A[row][col])
        B.append(tuple(current_row))
    return tuple(B)

def inverse(A):
    """returns the inverse (matrix) of a 2x2 matrix A"""
    swap_neg = ((A[1][1], -A[0][1]),
                (-A[1][0], A[0][0])) #swap 0,0 with 1,1 and negate 0,1 and 1,0
   
    determinant = A[0][0]*A[1][1] - A[0][1]*A[1][0]
    return matrix_multiply(1/determinant, swap_neg)

def test():
    A = ((1, 0, 2),
         (-1, 3, 1))
    B = ((3, 1),
         (2, 1),
         (1, 0))
    C = ((4, 7),
         (2, 6))
    
    assert matrix_multiply(A, B) == ((5, 1), (4, 2))
    assert inverse(C) == ((0.6000000000000001, -0.7000000000000001), (-0.2, 0.4))

    print('All tests were successful, no errors!')

if __name__ == '__main__':
    test()
