/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2006, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*  vppp_audiojpegenc_asm.c                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*  This contains the VoC assembly code for the audio and jpeg encoder.  */
/*                                                                       */
/*************************************************************************/

#include "cs_types.h"

//common code



#ifdef VOCCODE_IN_PSRAM

//common code

#if 0//defined (_CAM_SMALL_MEM_) ||defined ( _CAM_SPECIAL_GC6113_)
 INT32 G_VppCommonEncCode[] = 
{
#include "audiojpeg_enc_common_qvga.tab"
};
#else
 INT32 G_VppCommonEncCode[] = 
{
#include "vpp_audiojpegenc_asm_main.tab"
};
#endif

//mp3

 INT32 G_VppMp3EncCode[] =
{
0//#include "vpp_audiojpegenc_asm_mp3enc.tab"
};

 INT32 G_VppMp3EncConstX[] = 
{
0//#include "vpp_mp3_enc_const_x.tab"
};

 INT32 G_VppMp3EncConstY[] = 
{
0//#include "vpp_mp3_enc_const_y.tab"
};


 INT32 G_VppMp3EncConst_rqmy[] = 
{
0//#include "vpp_mp3_enc_const_rqmy.tab"
};

 INT32 G_VppMp3EncConst_Zig[] = 
{
0//#include "vpp_mp3_enc_const_zig.tab"
};





//amrjpeg

#if  0//defined(_CAM_SMALL_MEM_) ||defined( _CAM_SPECIAL_GC6113_)
INT32 G_VppAmrJpegEncCode[] =
{
 #include "amrjpegenc_qvga.tab"
};
#else
INT32 G_VppAmrJpegEncCode[] =
{
 #include "vpp_audiojpegenc_asm_amrjpegenc.tab"
};
#endif


 INT32 G_VppAmrJpegEncConstX[] = 
{
#include "vpp_amrjpeg_enc_const_x.tab"
};

 INT32 G_VppAmrJpegEncConstY[] = 
{
#include "vpp_amrjpeg_enc_const_y.tab"
};


PROTECTED  INT32 G_VppAudJpegMainCodeSize = sizeof(G_VppCommonEncCode);


#else





//common code

#if  0//defined(_CAM_SMALL_MEM_) ||defined( _CAM_SPECIAL_GC6113_)
const INT32 G_VppCommonEncCode[] MICRON_ALIGN(32)= 
{
#include "audiojpeg_enc_common_qvga.tab"
};
#else
const INT32 G_VppCommonEncCode[] MICRON_ALIGN(32)= 
{
#include "vpp_audiojpegenc_asm_main.tab"
};
#endif




//mp3

const INT32 G_VppMp3EncCode[] MICRON_ALIGN(32)=
{
0//#include "vpp_audiojpegenc_asm_mp3enc.tab"
};

const INT32 G_VppMp3EncConstX[] MICRON_ALIGN(32)= 
{
0//#include "vpp_mp3_enc_const_x.tab"
};

const INT32 G_VppMp3EncConstY[] MICRON_ALIGN(32)= 
{
0//#include "vpp_mp3_enc_const_y.tab"
};


const INT32 G_VppMp3EncConst_rqmy[] MICRON_ALIGN(32)= 
{
0//#include "vpp_mp3_enc_const_rqmy.tab"
};

const INT32 G_VppMp3EncConst_Zig[] MICRON_ALIGN(32)= 
{
0//#include "vpp_mp3_enc_const_zig.tab"
};



//amrjpeg

#if  0//defined(_CAM_SMALL_MEM_) ||defined( _CAM_SPECIAL_GC6113_)
const INT32 G_VppAmrJpegEncCode[] MICRON_ALIGN(32)=
{
 #include "amrjpegenc_qvga.tab"
};
#else
const INT32 G_VppAmrJpegEncCode[] MICRON_ALIGN(32)=
{
 #include "vpp_audiojpegenc_asm_amrjpegenc.tab"
};
#endif


const INT32 G_VppAmrJpegEncConstX[] MICRON_ALIGN(32)= 
{
#include "vpp_amrjpeg_enc_const_x.tab"
};

const INT32 G_VppAmrJpegEncConstY[] MICRON_ALIGN(32)= 
{
#include "vpp_amrjpeg_enc_const_y.tab"
};

PROTECTED CONST INT32 G_VppAudJpegMainCodeSize MICRON_ALIGN(32)= sizeof(G_VppCommonEncCode);

#endif


