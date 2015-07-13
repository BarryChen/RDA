////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2008, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
//  $HeadURL$
//  $Author$
//  $Date$
//  $Revision$
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file mcip_media.h
///
/// MCI internal media variable
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MCIP_CAMERA_H_
#define _MCIP_CAMERA_H_

#include "cs_types.h"
#include "lcdd_m.h"


// =============================================================================
// MACROS                                                                       
// =============================================================================
#define MCI_CAM_MAX_PREVIEW_SIZE (320*240)
#define MCI_CAM_MAX_CAPTURE_SIZE (640*480)

// =============================================================================
// TYPES                                                                        
// =============================================================================

// =============================================================================
// MCI_CAMERA_INFO_STRUCT_T
// -----------------------------------------------------------------------------
/// Type used to store some useful information about the camera settings.
/// Nota: TODO We should be able, in a certain measure, to get rid of it.
// =============================================================================
typedef struct
{
    UINT16  startX;
	UINT16  startY;
	UINT16  endX;
	UINT16  endY;
	UINT16  imageWidth;
	UINT16  imageHeight;
	UINT16  screenWidth;
	UINT16  screenHeight;	
	UINT32  previewZoom;
	INT32   imageQuality;
	BOOL    addFrame;
} MCI_CAMERA_INFO_STRUCT_T;


// =============================================================================
// GLOBAL VARIABLES                                                             
// =============================================================================


// =============================================================================
// FUNCTIONS                                                                    
// =============================================================================
PRIVATE VOID MCI_CamSaveOsd(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
PRIVATE VOID mci_CameraBlendOsd(LCDD_FBW_T* previewFbw, LCDD_FBW_T* osdFbw);
PRIVATE VOID mci_CameraPreviewHandler(LCDD_FBW_T* previewFbw);
PRIVATE VOID mci_CameraCaptureHandler(LCDD_FBW_T* dataFbw, LCDD_FBW_T* previewFbw);



#endif // _MCIP_CAMERA_H_
