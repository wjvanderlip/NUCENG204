# coding: utf-8
from modelling import gaus
import numpy as np
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

samples = np.random.normal(100, 2, 1000)


plt.hist(samples, bins = 20, histtype="step");
v, be, _ = plt.hist(samples, bins = 20, histtype="step");

bc = be[:-1] + np.diff(be)[0] / 2.
plt.plot(bc, v, 'o')
plt.show()
a0 = np.max(v)
b0 = bc[np.argmax(v)]
c0 = 2.0

curve_help = (a0, b0, c0)
#curve_fit(gaus, bc, v)

print(curve_fit(gaus, bc, v, p0 = curve_help))
