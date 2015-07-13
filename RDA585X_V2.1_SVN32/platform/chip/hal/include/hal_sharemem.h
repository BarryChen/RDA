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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/hal/include/hal_cipher.h $ //
//    $Author: admin $                                                       // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_cipher.h                                                        //
/// This file contains HAL cipher command driver API.                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HAL_SHAREMEM_H_
#define _HAL_SHAREMEM_H_

/// @defgroup cipher HAL Ciphering Driver
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
// hal_CipherKcInit
// -----------------------------------------------------------------------------
/// Initialize Kc
/// @param kcLow   Low part of the Kc Key
/// @param kcHigh  High part of the Kc Key
/// @return #HAL_ERR_RESOURCE_BUSY if the ciphering is already running or 
/// #HAL_ERR_NO if the initialization is properly done.
// =============================================================================

PUBLIC void hal_ShareMemInit(void);


PUBLIC UINT16 *hal_ShareMem_GetHeader(void);

PUBLIC UINT8 *hal_ShareMem_GetData(void);
PUBLIC void hal_ShareMem_NextData(UINT32 len);
PUBLIC UINT8 hal_ShareMem_GetRxPage(void);
PUBLIC UINT8 hal_ShareMem_GetTxPage(void);


PUBLIC void hal_ShareMem_SendHeader(UINT16 *header);

PUBLIC void hal_ShareMem_SendData(UINT8 *data, UINT32 len);

/// @} <-- End of the cipher group

#endif // _HAL_SHAREMEM_H_


