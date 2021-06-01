import sys
import pandas as pd
import numpy as np
import mykmeanssp

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
    columns = []
    for i in range(len(pointsDf.columns)): 
        columns.append(f"X{i}")
    pointsDf.columns = columns

def readPointsFromFile(file_path): 
    pointsDf = pd.read_csv(file_path, header=None)
    return pointsDf.set_index(list(pointsDf.columns[[0]]))

def getDataFrame(file_path1, file_path2):
    pointsDf1 = readPointsFromFile(file_path1)
    pointsDf2 = readPointsFromFile(file_path2)
    join_points_DF = pd.merge(pointsDf1, pointsDf2, left_index=True, right_index=True) 
    set_header(join_points_DF)
    return join_points_DF.sort_index()

def kmeans_pp(datapoints, k):
    np.random.seed(0)
    r = np.random.choice(len(datapoints))
    centroids = [(r, datapoints[r])]
    D = [np.inf for i in range(len(datapoints))] 
    
    Z = 1
    while(Z < k):
        for i in range(len(datapoints)):
            x = datapoints[i]
            curDist = np.linalg.norm(x - centroids[-1][1]) ** 2
            D[i] = curDist if (curDist < D[i]) else D[i]            
        
        Z += 1
        dSum = sum(D)
        NormalizedD = list(map(lambda d: d / dSum, D))
        r = np.random.choice(len(datapoints), p=NormalizedD)
        centroids.append((r, datapoints[r]))
    
    return centroids

def printOutput(centroids): 
    str = ""
    for i in range(len(centroids)): 
        for d in range(len(centroids[0])): 
            str += "{}".format(format(centroids[i][d], '.4f'))
            str += ","
        str = str[:-1] + "\n"
    
    print(str[:-1])

def main():
    k, max_iter, file_path1, file_path2 = readArgs()
    pointsDataFrame = getDataFrame(file_path1, file_path2)

    N, d = pointsDataFrame.shape

    if (k >= N):
        print("K is not smaller then n, exits...")
        exit(0)
    if (DEBUG_INPUT):
        print(f"k: {k}\nmax_iter: {max_iter}\nfile_path1: {file_path1}\nfile_path2: {file_path2}")
        print("\npoints:\n")
        print(pointsDataFrame.to_string())

    datapoints = pointsDataFrame.to_numpy()
    centroids = kmeans_pp(datapoints, k)
    centroidsArr = list(map(lambda x: x[1].tolist(), centroids))
    datapointArr = list(map(lambda x: x.tolist(), datapoints))
    
    print(",".join([str(int(c[0])) for c in centroids]))
    centroids = mykmeanssp.fit(k, d, max_iter, datapointArr, centroidsArr)
    printOutput(centroids)

main() 