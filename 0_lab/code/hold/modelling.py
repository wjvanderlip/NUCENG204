import numpy as np


def gaus(x, amp, cent, sd):
    """
    This is a 3-parameter gaussian function
    amp = amplitiude
    cent = centroid
    sd = std. dev.

    This is a doc string, python is self documenting
    """
    return amp * np.exp(-(x-cent)**2/(2*sd**2))

#the doc string will show when you go gaus?
#the way to import this code in ipython

#from modellig import gaus
