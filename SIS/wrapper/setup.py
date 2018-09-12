from distutils.core import setup, Extension
import numpy

setup(name='sis3302',
    description='A module to readout the SIS 3302 card',
    version='0.1.0',
    ext_modules = [
        Extension('sis',
            ['src/pywrapper.c','src/SIS3150.c','src/SIS3302Configuration.c',\
             'src/VMECalls.c', 'src/loadCalibration.c'],
            include_dirs = [numpy.get_include(),'include'],
            library_dirs = ['/home/joe/SIS/lib'],
            runtime_library_dirs = ['/home/joe/SIS/lib'],
            libraries = ['sis3150'],
            extra_compile_args=["-O3","-std=c99"],
            extra_link_args=["-O3"],
        ),
    ]
)
