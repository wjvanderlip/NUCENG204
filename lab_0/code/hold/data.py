import numpy as np
import matplotlib.pyplot as plt
from numpy import sqrt, pi, exp, linspace, random
import scipy as sci
from scipy.optimize import curve_fit

data = np.loadtxt('lab0_spectral_data.txt')
for counts in data:
    Am241 = counts[0:1]
    Ba133 = counts[1:2]
    Cs137 = counts[2:3]
    Co60 = counts[3:4]
    Eu152 = counts[4:5]

    plt.hist(Am241, 10000, normed=1)
    with open('hyadeserr.txt', 'r') as f:
    reader = csv.reader(f, 'ssv')
    for row in reader:
        floats = [float(column) for column in row]
        data.append(floats)

print data
