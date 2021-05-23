import sys
import pandas as pd
import numpy as np
import random

DEBUG_INPUT = False # set to False 

def readArgs():
    assert(4 <= len(sys.argv) <= 5)
    try:
        k = int(sys.argv[1])
    except ValueError:
        print("K is not integer, exits...")
        exit(0)

    try:
        max_iter = 300 if len(sys.argv) == 4 else int(sys.argv[2])
    except ValueError:
        print("max_iter is not integer, exits...")
        exit(0)
    
    file_path1, file_path2 = "",""
    try:
        if (len(sys.argv) == 4):
            file_path1 = str(sys.argv[2])
            file_path2 = str(sys.argv[3])
        elif (len(sys.argv) == 5): 
            file_path1 = str(sys.argv[3])
            file_path2 = str(sys.argv[4])
    except ValueError:
        print(f"file_name1: '{file_path1}' or file_name2: '{file_path2}' is not String, exits...")
        exit(0)

    return k, max_iter, file_path1, file_path2

def set_header(pointsDf):
    columns = ["Index"]
    for i in range(len(pointsDf.columns) - 1): 
        columns.append(f"X{i}")
    pointsDf.columns = columns

def readPointsFromFile(file_path): 
    pointsDf = pd.read_csv(file_path, header=None)
    set_header(pointsDf)
    return pointsDf
    # print(f"first row:\n{pointsDf.loc(0)[1]}")

def kmeans_pp(datapoints, k):
    centroids = [random.choice(datapoints)]
    Z = 1
    while(Z < k):
        D = []
        for x in datapoints:
            distances = [np.linalg.norm(x - centroid) for centroid in centroids]
            D.append(min(distances))
        
        Z += 1
        centroids.append(random.choices(datapoints, weights=D))
    
    return centroids

def convert_DF_to_PDArr(pointsDF):
    arr = []
    for row in range(pointsDF.shape[0]):
        # print(f"first row:\n{join_points_DF.loc(0)[0]}")
        index = pointsDF.loc(0)[row][0]
        point = pointsDF.loc(0)[row][1:].to_numpy()
        arr.append((index,point))

def main():
    k, max_iter, file_path1, file_path2 = readArgs()
    pointsDf1 = readPointsFromFile(file_path1)
    pointsDf2 = readPointsFromFile(file_path2)
    join_points_DF = pd.merge(pointsDf1, pointsDf2, on = 'Index') 
    set_header(join_points_DF)
    
    N, d = join_points_DF.shape
    d -= 1 # if Index column is delete erase this line //////////////////////////////////////////////////

    PD_Arr = convert_DF_to_PDArr(join_points_DF)
    
    if (k >= N):
        print("K is not smaller then n, exits...")
        exit(0)
    if (DEBUG_INPUT):
        print(f"k: {k}\nmax_iter: {max_iter}\nfile_path1: {file_path1}\nfile_path2: {file_path2}")
        print("\npoints:\n")
        print(join_points_DF.to_string())

    datapoints =[np.array([0,0]), np.array([1,1]), np.array([10,10])]
    if (DEBUG_INPUT):
        print(kmeans_pp(datapoints,2))

main() 