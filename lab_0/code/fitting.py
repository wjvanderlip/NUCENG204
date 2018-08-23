import numpy as np

def gaus(x, A, B, C):
    """
    Gaussian distribution.
    A = amplitude
    B = centroid
    C = std. deviation (sigma)
    """
    return A * np.exp(-(x - B)**2 / (2*C**2))
