#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_COLS       8   // # of ADC's/Module
#define NUM_ROWS       19  // # of modules
#define NUM_CHANNELS   152 // 19 * 8
#define NUM_ENTRIES    2   // # of vals in calibration (2 for linear)

// Load calibration data from a file. In this case, the calibration must be
// linear with a slope (mval) and intercept (bval). The values are stored in
// 2-D arrays that are consistent with the data format called by the SIS 
// software (i.e. row# = module and col# = ADC). Both the module number and
// adc number start at 0.
void loadCalibration( float mAry[][NUM_COLS], float bAry[][NUM_COLS],
                      char* filename );

// Same as above, but load into 1D arrays instead of 2
void loadCalibration1D( float mAry[NUM_CHANNELS], float bAry[NUM_CHANNELS],
                      char* filename );

// A method for printing a 2-D array to stdout
void printAry( float ary[][NUM_COLS] );

// Prints an array out to a file. The dim determines whether it will be printed
// in 1D or 2D.
void printAryToFile( float ary[][NUM_COLS], char* filename, int dim );
