import pandas as pd
import numpy as np
import kmeans_pp as kmpp
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris
import mykmeanssp

# random_state = 0 ??

def calculateInertia(X, centroids):
    inertia = 0
    for x in X:
        minDist = float("inf")
        for c in centroids:
            minDist = min(minDist, np.linalg.norm(x - centroids[-1][1]) ** 2)
        inertia += minDist
    return inertia

def plotGraph(interiaForK):
    x = np.linspace(1,10)
    fig,ax = plt.subplots()
    plt.title = "Elbow Method for selection of optimal 'K' clusters"
    plt.xlabel = "K"
    plt.ylabel = "Averge Dispersion"
    plt.grid(True)
    ax.plot(x,interiaForK[x]) ## need to be changed!!! 
    fig.show()

def main():
    datapoints = load_iris().data
    N, d = datapoints.shape
    max_iter = 200 # ??
    interiaForK = []

    for k in range(1,10):
        centroids = kmpp.kmeans_pp(datapoints, k)
        centroidsArr = list(map(lambda x: x[1].tolist(), centroids))
        datapointArr = list(map(lambda x: x.tolist(), datapoints))
        centroids = mykmeanssp.fit(k, d, max_iter, datapointArr, centroidsArr)
        interia = calculateInertia(datapoints, centroids)
        interiaForK.append(interia)
    
    print(interiaForK)
    plotGraph(interiaForK)

main()