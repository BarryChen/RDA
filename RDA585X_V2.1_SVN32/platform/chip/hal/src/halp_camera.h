////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file halp_camera.h
/// This file contains the camera private header.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _HALP_CAMERA_H_
#define _HALP_CAMERA_H_

#include "hal_sys.h"
// =============================================================================
// hal_CameraIrqHandler
// -----------------------------------------------------------------------------
/// Handler called by the IRQ module when the camera module generates an
/// interrupt.
// =============================================================================
PROTECTED VOID hal_CameraIrqHandler(UINT8 interruptId);

#endif // _HALP_CAMERA_H_


