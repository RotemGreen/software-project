import sys
import numpy as np
from kmeans import kmeans
from symnmf import symnmf
from sklearn.metrics import silhouette_score
from io_utils import process_input


   
def main():
    args = sys.argv
    num_args = len(args)-1

    #check number of inputs
    if num_args != 2:
        print("An Error Has Occurred")
        exit(1)
    
    #check each case
    try:
        k = int(args[1])
    except ValueError:
        print("An Error Has Occurred")
        exit(1)

    file_name = args[2]
    
    #check correctenss of the inputs' range
    if k <= 1:
        print("An Error Has Occurred")
        exit(1)

    #call process_input
    numpy_vectorsMat = process_input(file_name)
    vectorsMat = numpy_vectorsMat.astype(float).tolist()
    
    #check if process input threw an error 
    if vectorsMat == None:
        exit(1)

    assignments_kmeans = kmeans(vectorsMat, k, 300, 0.0001)
    assignments_symnmf = symnmf(vectorsMat, k, 300, 0.0001)

    kmeans_score = silhouette_score(vectorsMat, assignments_kmeans)
    synmnf_score = silhouette_score(vectorsMat, assignments_symnmf)
    print(f"nmf: {synmnf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")



if __name__ == "__main__":
    main()
