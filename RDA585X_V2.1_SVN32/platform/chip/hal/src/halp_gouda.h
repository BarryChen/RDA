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
/// @file halp_gouda.h
/// This file contains the lcdc private header.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if CHIP_HAS_GOUDA

#ifndef _HALP_GOUDA_H_
#define _HALP_GOUDA_H_

#include "hal_sys.h"

// =============================================================================
// hal_GoudaIrqHandler
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID hal_GoudaIrqHandler(UINT8 interruptId);


#endif // _HALP_GOUDA_H_

#endif // CHIP_HAS_GOUDA

