import sys
import numpy as np
import math
import symnmfmodule
from io_utils import process_input

# Print the input matrix 
def print_mat(mat, n, k):
    for i in range(n):
        for j in range(k):
            if j == k - 1:
                print(f"{mat[i][j]:.4f}", end="")
            else:
                print(f"{mat[i][j]:.4f}", end=",")
        print()


# Calculate the matrix H
def calc_H(W, n, k, epsilon, iter):
    numpy_w = np.array(W)
    m = numpy_w.mean()
    h = np.random.uniform(0, 2*math.sqrt(m/k),size = (n,k))
    return np.array(symnmfmodule.symnmf(h.astype(float).tolist(), numpy_w.astype(float).tolist(), iter, epsilon))

# Returns the index of the max value in a given vector
def argmax(vec):
    assert(isinstance(vec, list))
    assert(len(vec) > 0)
    idx = 0
    max_val = float('-inf')
    for i in range(len(vec)):
        if vec[i] > max_val:
            max_val = vec[i]
            idx = i
    return idx

# Runs symnmf and return the points assignments to each cluster.
# Returns a list whose i'th coordinate represents the cluster that the i'th point belongs to
def symnmf(points, k, iter, epsilon):
    np.random.seed(1234)
    n = len(points)
    if k <= 1 or k >= n:
        raise ValueError("Invalid number of clusters")
    clusters_assignment = []
    norm_mat = symnmfmodule.norm(points)
    H = calc_H(norm_mat, n, k, epsilon, iter)
    for i in range(n):
        clusters_assignment.append(argmax(H[i].astype(float).tolist()))

    return np.array(clusters_assignment)



def main():
    np.random.seed(1234)
    args = sys.argv
    num_args = len(args)-1

    # Check number of inputs
    if num_args != 3:
       print("An Error Has Occurred")
       exit(1)

    goal = args[2]
    
    # Check each case
    try:
        k = int(args[1])
    except ValueError:
        print("An Error Has Occurred")
        exit(1)

    iter = 300

    # Check validity of goal
    if goal not in ['symnmf' , 'sym' , 'ddg' , 'norm']:
        print("An Error Has Occurred")
        exit(1)
    
    file_name = args[3]

    # Check correctenss of the inputs' range
    if k <= 1:
        print("An Error Has Occurred")
        exit(1)



    try:
        points = process_input(file_name)
        n = len(points)
        if k >= n:
            print("An Error Has Occurred")
            exit(1)
        sym_mat = symnmfmodule.sym(points.astype(float).tolist())

        if goal == 'sym':
            print_mat(sym_mat, n, n)

        else:
            ddg_mat = symnmfmodule.ddg(points.astype(float).tolist())
            if goal == 'ddg':
                print_mat(ddg_mat, n, n)
            else:
                norm_mat = symnmfmodule.norm(points.astype(float).tolist())
                if goal == 'norm':
                    print_mat(norm_mat, n, n)
                else:
                    H = calc_H(norm_mat, n, k, 0.0001, 300)
                    print_mat(H, n, k)
    except Exception:
        print("An Error Has Occurred")
        exit(1)

    return 0

if __name__ == "__main__":
    main()
