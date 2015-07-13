/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2006, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vppp_speech_asm.c                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This contains the VoC assembly code for the Speech.               */
/*                                                                       */
/*************************************************************************/
///
///   @file vppp_speech.h
///   This contains the VoC assembly code for the Speech.
///

#include "cs_types.h"
//#include "chip_id.h"

#define MOVE_AEC_CODE_TO_RAM 0
#ifndef MICRON_ALIGN
#define MICRON_ALIGN(a)
#endif


 

#define SPEECH_SRAMDATA_INTERNAL


PROTECTED CONST INT32   g_vppBTSpeechMainCode[] MICRON_ALIGN(32)= {
#ifdef USE_VOC_CVSD

#include "vpp_speech_asm_main_voc_cvsd.tab"
#else

#include "vpp_speech_asm_main.tab"
#endif
};

PROTECTED CONST INT32   g_vppBTAecConstantX[] MICRON_ALIGN(32)= {
#include "vpp_aec_const.tab"
};

PROTECTED CONST INT32   g_vppBTAecCode[] MICRON_ALIGN(32)=
{
#ifdef USE_VOC_CVSD
#include "vpp_speech_asm_aec_voc_cvsd.tab"
#else
#include "vpp_speech_asm_aec.tab"
#endif
};

PROTECTED CONST INT32   g_vppSpeex_aec_const[] MICRON_ALIGN(32)=
{
#include "speex_aec_const.tab"
};

PROTECTED CONST INT32   g_vppNoiseSuppressCode[] MICRON_ALIGN(32)=
{
#include "vpp_speech_asm_noisesuppress.tab"
};
PROTECTED CONST INT32   g_vppNoiseSuppressConstantX[] MICRON_ALIGN(32)=
{
#include "vpp_noisesuppress_const_x.tab"
};

PROTECTED CONST INT32 g_vppBTSpeechMainCodeSize MICRON_ALIGN(32)= sizeof(g_vppBTSpeechMainCode);


