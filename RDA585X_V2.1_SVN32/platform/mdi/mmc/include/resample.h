/*
 * FILE: resample.h 
 */


#include "cs_types.h"

#define Nhc       8
#define Na        7
#define Np       (Nhc+Na)

#define Pmask    ((1<<Np)-1)

/* Conversion constants */
 
INT32 ResampleInit(INT32 InSampleRate,INT32 OutSampleRate,INT32 FrameLength,INT32 NbChnanel);


INT32 ResampleOneFrame(INT16 *InBuf,INT16 * OutBuf);



