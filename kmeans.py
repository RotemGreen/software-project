import sys
import math
import numpy as np
from io_utils import process_input


class Cluster:
    def __init__(self, meanCords, meanDeltaCords, size):
        self.meanCords = meanCords
        self.meanDeltaCords = meanDeltaCords
        self.size = size



def init_cluster(vector):
    vec1 = [x for x in vector]
    vec2 = [x for x in vector]
    return Cluster(vec1, vec2, 1)


def calc_N(vectorsMat):
    return len(vectorsMat)

def calc_d(vectorsMat):
    return len(vectorsMat[0])


def calc_distance(vector, mean, d):
    sum = 0
    for i in range(d):
        sum += (vector[i] - mean[i]) * (vector[i] - mean[i]) 
    
    return math.sqrt(sum)



def argmin(vector, clusters_array, k, d):
    idx = -1

    for i in range(k):
        curr_distance = calc_distance(vector, clusters_array[i].meanCords, d)
        if i <= 0:
            min_distance = curr_distance
        
        if curr_distance <= min_distance:
            min_distance = curr_distance
            idx = i
    
    return idx



def is_mean_delta_smaller_than_epsilon(clusters_array, k, d, epsilon):
    for i in range(k):
        delta_cords = clusters_array[i].meanDeltaCords
        for j in range(d):
            if abs(delta_cords[j]) >= epsilon:
                return False
    return True



def print_means(clusters_array, k):
    for i in range(k):
        curr_mean = clusters_array[i].meanCords
        curr_row = [f"{val:.4f}" for val in curr_mean]
        print(",".join(curr_row))

    


def get_prev_mean(cluster, d):
    cluster_mean = cluster.meanCords
    mean = []

    for i in range(d):
        mean.append(cluster_mean[i])

    return mean


def reset_cluster_mean_and_size(cluster, d):
    curr_mean = cluster.meanCords
    cluster.size = 0

    for i in range(d):
        curr_mean[i] = 0


def calculate_mean(cluster, vectorsMat, clustId, assignments, N, d):
    curr_mean = cluster.meanCords
    for i in range(N):
        if assignments[i] == clustId:
            curr_vec = vectorsMat[i]
            for j in range(d):
                curr_mean[j] += curr_vec[j]
            cluster.size += 1

    for m in range(d):
        if cluster.size == 0:
            curr_mean[m] = 0
        else:
            curr_mean[m] = curr_mean[m] / cluster.size
        



def update_delta(cluster, prevMean, d):
    curr_mean = cluster.meanCords
    curr_delta = cluster.meanDeltaCords

    for i in range(d):
        curr_delta[i] = abs(prevMean[i] - curr_mean[i])
    
    cluster.meanDeltaCords = curr_delta


def kmeans(vectorsMat, k, iter, epsilon):   
    N = calc_N(vectorsMat)

    if k >= N:
        print("An Error Has Occurred")
        exit(1)
    
    d = calc_d(vectorsMat)

    assignments = [-1] * N

    clusters_array = []
    #initialize cluster_array && the first step of the algorithem
    for i in range(k):
        curr_clust = init_cluster(vectorsMat[i])
        assignments[i] = i
        clusters_array.append(curr_clust)


    #steps 2-5 of the algorithem
    iteration_number = 0
    while (is_mean_delta_smaller_than_epsilon(clusters_array, k, d, epsilon) == False) and (iteration_number < iter):
        for i in range(N):
            curr_xi = vectorsMat[i]
            assignments[i] = argmin(curr_xi, clusters_array, k, d)
        
        for j in range(k):
            curr_clust = clusters_array[j]
            prev_mean = get_prev_mean(curr_clust, d)

            reset_cluster_mean_and_size(curr_clust, d)
            calculate_mean(curr_clust, vectorsMat, j, assignments, N, d)
            update_delta(curr_clust, prev_mean, d)

        iteration_number += 1

    return assignments




def main():
    args = sys.argv
    num_args = len(args)-1

    #check number of inputs
    if num_args not in [2,3]:
        print("An Error Has Occurred")
        exit(1)
    
    #check each case
    try:
        k = int(args[1])
    except ValueError:
        print("An Error Has Occurred")
        exit(1)

    if num_args == 2:
        iter = 400
        file_name = args[2]
    
    else:
        file_name = args[3]
        try:
            iter = int(args[2])
        except ValueError:
            print("An Error Has Occurred")
            exit(1)


    #check correctenss of the inputs' range
    if iter <=1 or iter >=800:
        print("An Error Has Occurred")
        exit(1)

    if k <= 1:
        print("An Error Has Occurred")
        exit(1)


    #call process_input
    numpy_vectorsMat = process_input(file_name)
    vectorsMat = numpy_vectorsMat.astype(float).tolist()

    #check if process input threw an error 
    if vectorsMat == None:
        exit(1)
    

    assignments = kmeans(vectorsMat, k, iter, 0.001)


    exit(0)

if __name__ == "__main__":
    main()
