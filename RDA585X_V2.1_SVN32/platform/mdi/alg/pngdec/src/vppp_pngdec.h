
/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2008, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vppp_pngdec.h                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This is private header of VPP Multimedia.                         */
/*                                                                       */
/*************************************************************************/
///
///   @file vppp_pngdec.h
///   This is private header of VPP Multimedia.
///

///
///  @mainpage Granite VOC Processing Control API
///  Granite VOC Processing Control (VPP) API description.
///  @author Coolsand Technologies, Inc.
///  @version 1.0
///  @date    march 2007
///
///  @par Purpose
///
///  This document describes the Granite software VPP layer.
///
///  @par
///
///  VPP MULTIMEDIA API is composed of the following division :
///  - @ref vppp_pngdec_struct "vpp private multimedia structures and defines" \n
///

#ifndef VPPP_PNGDEC_H
#define VPPP_PNGDEC_H

#include "cs_types.h"

///@defgroup vppp_pngdec_struct
///@{

// ============================================================================
// Types
// ============================================================================


// ============================================================================
// VPP_PNGDEC_CODE_CFG_T
// ----------------------------------------------------------------------------
/// Internal VPP PngDec configuration structure
// ============================================================================

typedef struct
{
    /// pointer to the hi code
    INT32* hiPtr;
    /// pointer to the bye code
    INT32* byePtr;
    /// pointer to the data constants
    INT32* constPtr;
    
} VPP_PNGDEC_CODE_CFG_T;


///  @} <- End of the vppp_pngdec_struct group


#endif  // VPPP_PNGDEC_H
