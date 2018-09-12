Before anything:

You will need the following python packages: 
- numpy >=v1.7
- matplotlib v>=1.3 with TkAgg backend (note: only needed for example script)

If using native python, you can get these with pip:

sudo pip install --upgrade numpy matplotlib
 - (this will only work if you have pip installed: sudo apt-get install python-pip)

Before building:

1. In setup.py, uncomment the two lines referring to library directories and
   runtime libraries and enter the path to wherever you installed the sis
   libraries (i.e. wherever libsis3150.so is).
2. If you are planning on extracting raw signals, make sure that the raw data
   buffer length is include/SISDefines.h is consistent with whatever raw data
   length you set in the SIS configuration file. The default value is 256. If
   you configure SIS to output pulses shorter than that it may still work but
   you'll be wasting memory. If you configure SIS with longer pulses you will
   hopefully get a segfault but may just get garbage data.
3. YOU NEED A SIS CONFIGURATION FILE. The one that is included here is for 
   CCI2 which uses 19 3302 modules. You'll need to make your own sis conf file

To build:

python setup.py build\_ext -i

To run the sample program:

 * Things to change:
   * Make sure the SIGNAL\_LENGTH is correct for your configuration
   * Change the configuration\_file variable to whatever you named your
     sis config file

python example\_saveBinary.py

This will run the example script which reads out the SIS boards at roughly 2
second intervals, plots a sample raw signal, and saves the binary data to a
file called DATA.dat (or whatever you rename it to).

To stop the program, ctrl+c

The sample script only saves the binary data, so you will need to parse it in
order to have access to it afterwards. The example script should be easy
to modify to save the data you extract into whatever format you want (e.g. an
HDF5 file).

Known issues:
 - Calibration not implemented for general case (specific to CCI2 - should be
   straightforward to rework for general number of 3302 modules
 - Current way of extracting raw data leaks memory due to misuse of the
   numpy/C API. See http://blog.enthought.com/python/numpy-arrays-with-pre-allocated-memory/#.VScQxnXd\_Qo
 - The output of the channel ID will depend on how you have the SIS boards
   configured. The mapping I have works for CCI2, but is weird for the LAI and
   requires correction. See SIS manual for binary data format.
