import numpy as np
import time
import matplotlib
import sys
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
plt.ion()

# Import sis library. If you get an import error, wrapper has failed to
# compile correctly
import sis
SIGNAL_LENGTH= 4096   # Length of raw data: depends on configuration
READ_INTERVAL=2.0   # Sleep interval between sis.acquiredata

# Set up a plot to visualize the data coming from SIS

fig, ax_raw = plt.subplots()
ax_raw.set_title('Sample signal')
ax_raw.set_xlim(0, SIGNAL_LENGTH)
ax_raw.plot(np.zeros(SIGNAL_LENGTH))
fig.canvas.draw()

if __name__ == "__main__":
    # Required variables for sis setup
    configuration_file = 'config_new.ini'
    output_file = 'DATA_co60Cs137Am241_7trigger.dat'
    toggle_save = True
    # Connect to and configure sis
    sis.connectToDAQ()
    sis.configuration(configuration_file, output_file, toggle_save)

    # Start the acquisition. This command resets the clock via the 3150
    # so all timestamps will be relative to this command
    sis.startacquisition()
    start_time = time.time()

    # Setup a run loop
    generation = 0

    while True:
        generation += 1

        # Get some data from boards and display some info about the
        # acquired data

        ts, en, ch, trig, sigs = sis.acquireDataWithRaw(toggle_save)

        print 'Generation %s' %(generation)
        print '  %s readouts collected' %len(ts)
        uniq_ch = np.unique(ch)
        print '  %s unique channels: %s' %(len(uniq_ch), uniq_ch)
        # Update the raw signal
        sig = sigs[0,:]
        ax_raw.lines[0].set_ydata(sig)
	ax_raw.set_ylim((0.99*sig.min(), 1.01*sig.max()))
	fig.canvas.draw()
        time.sleep(READ_INTERVAL)
        print generation
