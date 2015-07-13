
/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2006, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vppp_pngdec_asm.h                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This is private header of VPP Audio Motion JPEG VoC assembly      */
/*                                                                       */
/*************************************************************************/
///
///   @file vppp_pngdec_asm.h
///   This is private header of VPP Audio Motion JPEG VoC assembly.
///

#ifndef VPPP_PNGDEC_ASM_H
#define VPPP_PNGDEC_ASM_H

#include "cs_types.h"

#ifndef CS_PLATFORM_SIMU

 // avoid double declaration in simulation
 #define RAM_X_BEGIN_ADDR               0x0000
 #define RAM_Y_BEGIN_ADDR               0x4000

#endif

#define VPP_PNGDEC_MAIN_SIZE               g_vppPngDecCommonCodeSize
#define VPP_PNGDEC_MAIN_ENTRY              0
#define VPP_PNGDEC_CRITICAL_SECTION_MIN    0
#define VPP_PNGDEC_CRITICAL_SECTION_MAX    0

EXPORT CONST INT32 g_vppPngDecCommonCode[];
EXPORT CONST INT32 g_vppPngDecCommonCodeSize;
EXPORT CONST INT32 g_vppPngDecConst[];
EXPORT CONST INT32 g_vppPngDecLenfixDistFix[];

#endif  // VPPP_PNGDEC_ASM_H
