#include <Python.h>
#include <SIS3150.h>
#include "loadCalibration.h"
#include "numpy/arrayobject.h" // provides the numpy C API
#include "SISDefines.h"

PyObject *wrap_connectToDAQ(PyObject *self, PyObject *args) {
    int ret = connectToDAQ();
    return Py_BuildValue("i",ret);
}

//Wrapper for stopping acquisition RWB
PyObject *wrap_stopacquisition( PyObject *self, PyObject *args )
{
  int ret = stopDataAcquisition();
  return Py_BuildValue("i", ret);
}

PyObject *wrap_configuration(PyObject *self, PyObject *args) {
    // Parse input
    char* confFname;
    char* outFname;
    int* saveData;
    if( !PyArg_ParseTuple( args, "ssi", &confFname, &outFname, &saveData ) )
    { return NULL; }
    //Debug
//    printf("config file name = \n%s\n", confFname);
//    printf("output file name = \n%s\n", outFname);
//    printf("save data bool = %d\n", saveData );
    int ret = loadConfiguration( confFname, outFname, saveData );
    return Py_BuildValue("i",ret);
}

PyObject *wrap_startacquisition(PyObject *self, PyObject *args) {
    int ret = startDataAcquisition();
    return Py_BuildValue("i",ret);
}

PyObject *wrap_acquiredata(PyObject *self, PyObject *args) {
    // Get calibration file name from python function
    /**** CALIBRATION FUNCTIONALITY - HARD-CODED FOR CCI2
//    char* filename;
//    if( !PyArg_ParseTuple( args, "si", &filename, &saveData ) ){ return NULL; }
//    float mAry[NUM_CHANNELS];
//    float bAry[NUM_CHANNELS];
//    loadCalibration1D( mAry, bAry, filename );
//    printf("Got passed calibration loading\n");
    ****/

    /*** Get data from python ***/
    int* saveData;
    if( !PyArg_ParseTuple( args, "i", &saveData ) ){ return NULL; }
    struct SISdata *data;
    int len = 0;
    int ret = acquireData( &data, &len, saveData);
    
    /*** Setup data holders and construct python arrays ***/
    PyArrayObject *channel;
    PyArrayObject *energy;
    PyArrayObject *timestamp;	
//    PyArrayObject *nnleft;
//    PyArrayObject *nnright;
    PyArrayObject *trigger;
    npy_intp dimensions[1];
    dimensions[0] = len;
    channel = PyArray_SimpleNew(1, dimensions, NPY_UINT16);
    energy = PyArray_SimpleNew(1, dimensions, NPY_FLOAT);
    timestamp = PyArray_SimpleNew(1, dimensions, NPY_UINT64);
//    nnleft = PyArray_SimpleNew(1, dimensions, NPY_UINT);
//    nnright = PyArray_SimpleNew(1, dimensions, NPY_UINT);
    trigger = PyArray_SimpleNew(1, dimensions, NPY_UINT);

    /*** Fill up the data arrays (copy style - can be slow for truly high
     *   event rate situations. Checkout cython memoryviews for faster
     *   wrapper implementation ***/
    int i, j, ID;
    float en[len];
    for (i=0;i<len; ++i) {
        ((unsigned int*)channel->data)[i] = data[i].adcid;
        ID = data[i].adcid;
	en[i] = data[i].energy * 1.0;
//        en[i] = bAry[ID] + data[i].energy * mAry[ID];
        ((float*)energy->data)[i] = en[i];
        ((unsigned long long*)timestamp->data)[i] = data[i].timestamp;
//        ((unsigned int*)nnleft->data)[i] = data[i].neighborminus;
//        ((unsigned int*)nnright->data)[i] = data[i].neighborplus;
        ((unsigned int*)trigger->data)[i] = data[i].trigger;
    }
    
    /*** Return c-buffers as numpy arrays ***/
    free(data);
    return Py_BuildValue("NNNN", timestamp, energy, channel, trigger);
}

PyObject *wrap_acquireDataWithRaw(PyObject *self, PyObject *args) {
    /*** Calibration not implemented in general way
    // Get calibration file name from python function
    char* filename;
    int* saveData;
    if( !PyArg_ParseTuple( args, "si", &filename, &saveData ) ){ return NULL; }
    float mAry[NUM_CHANNELS];
    float bAry[NUM_CHANNELS];
    loadCalibration1D( mAry, bAry, filename );
    ***/
    /*** Get data from python into C ***/
    int* saveData;
    if(!PyArg_ParseTuple(args, "i", &saveData)){ return NULL; }
    struct SISdata *data;
    int len = 0;
    int ret = acquireData(&data, &len, saveData);

// get the raw data and put it in it's own 2D array (should be done higher up,
// this is just a test to see if pyarray_newfromdata works
    uint16_t* rdataAry = (uint16_t*) malloc( len * gl_uint_RawSampleLength * sizeof(uint16_t) );
    // Loop over data to fill the array
    int a, b;
    for( a=0; a<len; a++ )
    {
      for( b=0; b<gl_uint_RawSampleLength; b++ )
      {
        rdataAry[a * gl_uint_RawSampleLength + b] = data[a].rawdata[b];
      }
    }    //printf("%d events\n",len);

    
    /*** Set up and fill data containers ***/
    PyArrayObject *channel;
    PyArrayObject *energy;
    PyArrayObject *timestamp;	
//    PyArrayObject *nnleft;
//    PyArrayObject *nnright;
    PyArrayObject *trigger;
    PyArrayObject *rawdata;
    npy_intp dimensions[1];
    dimensions[0] = len;
    channel = PyArray_SimpleNew(1, dimensions, NPY_UINT32);
    energy = PyArray_SimpleNew(1, dimensions, NPY_FLOAT);
    timestamp = PyArray_SimpleNew(1, dimensions, NPY_UINT64);
//    nnleft = PyArray_SimpleNew(1, dimensions, NPY_UINT);
//    nnright = PyArray_SimpleNew(1, dimensions, NPY_UINT);
    trigger = PyArray_SimpleNew(1, dimensions, NPY_UINT);
    // Raw data wrapper stuff
    npy_intp rdims[2];
    rdims[0] = len;
    rdims[1] = gl_uint_RawSampleLength;
//    rawdata = PyArray_SimpleNew(2, rdims, NPY_USHORT);
    void* rdataptr;
    rdataptr = (void*) &rdataAry[0];
    // MEMLEAK BUG HERE - BASE NOT SET PROPERLY
    rawdata = PyArray_SimpleNewFromData(2, rdims, NPY_UINT16, rdataptr);
    PyArray_BASE(rawdata) = PyCObject_FromVoidPtr(rdataptr, free);

    int i, j, ID;
    float en[len];
    for (i=0;i<len; ++i) 
    {
        ((unsigned int*)channel->data)[i] = data[i].adcid;
        ID = data[i].adcid;
//        en[i] = bAry[ID] + data[i].energy * mAry[ID];
	en[i] = data[i].energy * 1.0;	// remove me when calib is implemented
        ((float*)energy->data)[i] = en[i];
        ((unsigned long long*)timestamp->data)[i] = data[i].timestamp;
//        ((unsigned int*)nnleft->data)[i] = data[i].neighborminus;
//        ((unsigned int*)nnright->data)[i] = data[i].neighborplus;
        ((unsigned int*)trigger->data)[i] = data[i].trigger;
    }
    
    free(data);
    return Py_BuildValue("OOOOO", timestamp, energy, channel, trigger,
                         rawdata);
}


static PyMethodDef SISMethods[] = {
    {"connectToDAQ", (PyCFunction)wrap_connectToDAQ, METH_VARARGS, "Connect to SIS3150"},
    {"configuration", (PyCFunction)wrap_configuration, METH_VARARGS, "Configure SIS3302"},
    {"startacquisition", (PyCFunction)wrap_startacquisition, METH_VARARGS, "Start SIS3302"},
    {"stopacquisition", (PyCFunction)wrap_stopacquisition, METH_VARARGS, "Stop SIS3302"},
    {"acquiredata", (PyCFunction)wrap_acquiredata, METH_VARARGS, "Acquire data from SIS3302"},
    {"acquireDataWithRaw", (PyCFunction)wrap_acquireDataWithRaw, METH_VARARGS, "Acquire edata and rdata from SIS3302"},
    {NULL,NULL} 
};

PyMODINIT_FUNC initsis(void) { // must be init<modulename> (init_cext => _cext)
    (void) Py_InitModule("sis", SISMethods);
    import_array(); // load numpy (effectively "import numpy") for use in this module 
};
