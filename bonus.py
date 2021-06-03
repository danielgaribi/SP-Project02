import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as pltPatch
from numpy.core.fromnumeric import shape
from numpy.core.numeric import full, full_like
from sklearn.datasets import load_iris
import kmeans_pp as kmpp
import mykmeanssp

def calculateInertia(X, centroids):
    inertia = 0
    for x in X:
        minDist = float("inf")
        for c in centroids:
            minDist = min(minDist, np.linalg.norm(x - c) ** 2) 
        inertia += minDist
    return inertia

def plotGraph(interiaForK):
    x = np.arange(1,11,1)
    fig, ax = plt.subplots()
    ax.plot(x, interiaForK, marker = "o")

    ellipse1 = pltPatch.Ellipse((2, interiaForK[1]), 0.5, 50, color='r', linestyle = '--', fill = False)
    ax.add_patch(ellipse1)

    ax.arrow(2, interiaForK[1], -0.5,-50, shape = 'right') #\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ fix arrow :(

    plt.title("Elbow Method for selection of optimal 'K' clusters")
    plt.xlabel("K")
    plt.xticks(x)
    plt.ylabel("Averge Dispersion")
    plt.grid(True)
    plt.savefig('elbow.png')
    plt.show() ## ////////////////////////////////////////////////////////////////delete!

def main():
    datapoints = load_iris().data
    N, d = datapoints.shape
    max_iter = 200
    
    interiaForK = []
    for k in range(1,11):
        centroids = kmpp.kmeans_pp(datapoints, k)
        centroidsArr = list(map(lambda x: x[1].tolist(), centroids))
        datapointArr = list(map(lambda x: x.tolist(), datapoints))
        centroids = mykmeanssp.fit(k, d, max_iter, datapointArr, centroidsArr)
        interia = calculateInertia(datapoints, centroids)
        interiaForK.append(interia)
    
    plotGraph(interiaForK)

main()