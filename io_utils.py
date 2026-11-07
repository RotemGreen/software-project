import numpy as np


# Process csv file into a matrix
def process_input(file_name):
    points = []
    with open(file_name, 'r') as file:
        for line in file:
            clean_line = line.strip()
            parts = clean_line.split(',')
            row = [float(x) for x in parts]
            points.append(row)
    return np.array(points)
