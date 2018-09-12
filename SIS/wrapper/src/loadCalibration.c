#include "loadCalibration.h" 

void loadCalibration( float mAry[][NUM_COLS], float bAry[][NUM_COLS],
                      char* filename )
{
  FILE *fr;

  int i = 0;
  int modNum = 0;
  int adcNum = 0;
  float mVal, bVal;

  fr = fopen( filename, "rt" );
  while( i < NUM_ROWS*NUM_COLS )
  {
    fscanf( fr, "%f", &bVal );
    fscanf( fr, "%f", &mVal );
    mAry[modNum][adcNum] = mVal;
    bAry[modNum][adcNum] = bVal;
    adcNum++;
    if ( (i+1) % 8 == 0)
    {
      modNum++;
      adcNum = 0;
    }
    i++;
  }
  fclose( fr );
  return;
}

void loadCalibration1D( float mAry[NUM_CHANNELS], float bAry[NUM_CHANNELS],
                        char* filename )
{
  FILE *fr;

  int i = 0;
//  int modNum = 0;
//  int adcNum = 0;
  float mVal, bVal;

  fr = fopen( filename, "rt" );
//  printf("Opened file\n");
  while( i < NUM_CHANNELS )
  {
//    printf("i = %d\n", i);
    fscanf( fr, "%f", &bVal );
//    printf("first read success\n");
    fscanf( fr, "%f", &mVal );
    mAry[i] = mVal;
    bAry[i] = bVal;
//    adcNum++;
//    if ( (i+1) % 8 == 0)
//    {
//      modNum++;
//      adcNum = 0;
//    }
    i++;
  }
  fclose( fr );
  return;
}

void printAry( float ary[][NUM_COLS] )
{
  int i,j;
  for( i = 0; i < NUM_ROWS; i++ )
  {
    for( j=0; j < NUM_COLS; j++ )
    {
      printf( "%f ", ary[i][j] );
    }
    printf( "\n" );
  }
  return;
}

void printAryToFile( float ary[][NUM_COLS], char* filename, int dim )
{
  // Open file
  FILE* fw;
  fw = fopen( filename, "w+" );
  
  int i,j;
  for( i = 0; i < NUM_ROWS; i++ )
  {
    for( j=0; j < NUM_COLS; j++ )
    {
      if( dim == 2 ) { fprintf( fw, "%f ", ary[i][j] ); }
      else { fprintf( fw, "%f\n", ary[i][j] ); }
    }
    if( dim == 2 ) { fprintf( fw, "\n" ); }
  }
  fclose( fw );
  return;
}
