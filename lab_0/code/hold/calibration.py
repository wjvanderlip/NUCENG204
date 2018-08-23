def spectrum_calibration(channel_width, energy_list, data_2_calibrate):
    import numpy as np
    import matplotlib.pyplot as plt
    import statsmodels.api as sm

    '''
    IDs peaks then drops peak to 0 to search for other peak while retaining
    indexing
    '''

    i = 0; channel_max_list = []
    while i < len(energy_list):
        channel_max = np.argmax(data_2_calibrate)
        data_left = channel_max - channel_width
        data_right = channel_max + channel_width
        channel_max_list.append(channel_max)
        iterator = data_left
        while iterator < (data_right):
            data_2_calibrate[iterator] = 0
            iterator += 1
        i += 1

    '''
    sorting channel number so the correct channel number corresponds with
    the correct energy.
    '''
    channel_number = sorted(channel_max_list, key=int)
    energy = energy_list
    results = sm.OLS(energy,sm.add_constant(channel_number)).fit()

    slope, intercept = np.polyfit(channel_number, energy, 1)

    abline_values = [slope * i + intercept for i in channel_number]
    plt.plot(channel_number,energy, 'ro')
    plt.plot(channel_number, abline_values, 'b')
    plt.xlabel('Channel Number')
    plt.ylabel('Energy [keV]')
    plt.title('Best Fit Line')
    return slope, intercept
