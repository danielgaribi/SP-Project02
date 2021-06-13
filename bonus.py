import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as pltPatch
from numpy.core.fromnumeric import shape
from numpy.core.numeric import full, full_like
from sklearn.datasets import load_iris
from sklearn.cluster import KMeans

import matplotlib
matplotlib.use('Agg')

def plotGraph(interiaForK):
    x = np.arange(1,11,1)

    fig, ax = plt.subplots()
    ax.plot(x, interiaForK, marker = "o")
    k = 3
    yArrowLength = 60
    xArrowLength = 0.2
    yArrowMargin = 52
    xArrowMargin = 0.2

    ellipse1 = pltPatch.Ellipse((k, interiaForK[k - 1]), 0.5, 50, color='r', linestyle = '--', fill = False)
    ax.add_patch(ellipse1)
    ax.arrow(k + xArrowLength + xArrowMargin, interiaForK[k-1] + yArrowLength + yArrowMargin, -xArrowLength, -yArrowLength, head_width=0.18, head_length=20, color = 'black')

    plt.title("Elbow Method for selection of optimal 'K' clusters")
    plt.xlabel("K")
    plt.xticks(x)
    plt.ylabel("Averge Dispersion")
    plt.grid(True)
    plt.savefig('elbow.png')

def main():
    datapoints = load_iris().data
    N, d = datapoints.shape
    max_iter = 200
    interiaForK = []
    for k in range(1,11):
        kmeans = KMeans(n_clusters = k, init = 'k-means++', random_state = 0)
        kmeans.fit(datapoints)
        interiaForK.append(kmeans.inertia_)
    
    plotGraph(interiaForK)

if (__name__ == "__main__"):
    main() 