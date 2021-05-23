import random
import numpy as np


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


datapoints =[np.array([0,0]), np.array([1,1]), np.array([10,10])]
print(kmeans_pp(datapoints,2))