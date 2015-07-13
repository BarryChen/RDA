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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_nand.h $ //
//    $Author: huazeng $                                                       // 
//    $Date: 2012-03-14 17:44:31 +0800 (周三, 14 三月 2012) $                     //   
//    $Revision: 14120 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/// @file hal_nand                                                            //
/// HAL NAND driver                                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HAL_NAND_H_
#define _HAL_NAND_H_

/// @defgroup cipher HAL NAND Driver
///
/// @{
///

#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_NandOpen
// -----------------------------------------------------------------------------
/// Initialize NAND flash controller
/// @param None
/// @return None
// =============================================================================
PUBLIC VOID hal_NandOpen(VOID);


// =============================================================================
// hal_NandReadData
// -----------------------------------------------------------------------------
/// Read data from NAND flash
/// @param addr    The start address in NAND flash
/// @param pData   Pointing to a buffer to hold the data
/// @param len     The number of bytes to be read
/// @return None
// =============================================================================
PUBLIC VOID hal_NandReadData(UINT32 addr, VOID *pData, UINT32 len);



/// @} <-- End of the nand group

#endif // _HAL_NAND_H_

