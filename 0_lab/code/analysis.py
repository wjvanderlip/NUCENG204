
import sys
sys.path.append('..')
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

from fitting import gaus

def main():
    """
    Main function that generates data from a normal distribution, histograms
    the random samples, fits a Gaussian model to the data, plots the results,
    and saves the figure.
    """
    # Parameters for (non-normalized) gaussian
    A = 1
    B = 100
    C = 2
    num_samples = 1000

    # Parameters for histogram
    nbins = 15

    # Draw samples and histogram
    samples = np.random.normal(B, C, size=num_samples)
    fig, ax = plt.subplots()
    counts, bin_edges, _ = ax.hist(samples, bins=15, histtype="step")

    # Convert histogram info to x, y data for fit
    xd = bin_edges[:-1] + np.diff(bin_edges)[0] / 2.
    yd = counts

    # Visually verify fitting data
    ax.plot(xd, yd, 'bo', label='Data')

    # Determine initial guesses for fitter
    a0 = yd.max()
    b0 = xd[np.argmax(yd)]
    c0 = (xd.max() - xd.min()) / 6.
    p0 = [a0, b0, c0]

    # Fit model to data
    popt, pcov = curve_fit(gaus, xd, yd, p0=p0)

    # Plot model against data
    x = np.linspace(xd.min(), xd.max(), 100)
    ax.plot(x, gaus(x, *popt), 'g--', label='Model');

    # Make sure axes limits are set appropriately
    ax.set_xlim((0.9*xd.min(), 1.1*xd.max()))
    ax.set_ylim((0, 1.1*yd.max()))

    # Spruce up plot with title, labels, legend, etc.
    ax.set_title('Gaussian Distribution - Model vs. %s Samples' %(num_samples))
    ax.set_xlabel('X value')
    ax.set_ylabel('Y value')
    ax.legend()     # NOTE: pulls legend keys from 'label' keyword in ax.plot

    # Save our results
    plt.savefig('../images/gaus_model_fit.png')

if __name__ == "__main__":
    main()
