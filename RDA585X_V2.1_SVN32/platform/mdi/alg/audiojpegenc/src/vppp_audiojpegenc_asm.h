#ifndef VPPP_AUDIOJPEGENC_ASM_H
#define VPPP_AUDIOJPEGENC_ASM_H


#include "cs_types.h"

#ifndef CS_PLATFORM_SIMU

 #define RAM_X_BEGIN_ADDR 0x0
 #define RAM_Y_BEGIN_ADDR 0x4000

#endif

#define VPP_AUDIOJPEGENC_MAIN_ENTRY           0
#define VPP_AUDIOJPEGENC_CRITICAL_SECTION_MIN 0
#define VPP_AUDIOJPEGENC_CRITICAL_SECTION_MAX 0


//common
EXPORT const INT32 G_VppCommonEncCode[];

//mp3
EXPORT const INT32 G_VppMp3EncCode[];
EXPORT const INT32 G_VppMp3EncConstX[];
EXPORT const INT32 G_VppMp3EncConstY[];
EXPORT const INT32 G_VppMp3EncConst_rqmy[];
EXPORT const INT32 G_VppMp3EncConst_Zig[];

//amrjpeg
EXPORT const INT32 G_VppAmrJpegEncCode[];
EXPORT const INT32 G_VppAmrJpegEncConstX[];
EXPORT const INT32 G_VppAmrJpegEncConstY[];

EXPORT CONST INT32 G_VppAudJpegMainCodeSize;

#endif  // VPPP_AUDIOJPEGENC_ASM_H
