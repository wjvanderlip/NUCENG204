import numpy as np

from code.modelling import gauss

def test_gaus_symmetry():
    amp = 100
    cent = 0
    sd = 1
    xr = 2.0
    lx = -xr
    assert gaus(xr, amp, cent, sd) == gaus(xl, amp, cent, sd)
