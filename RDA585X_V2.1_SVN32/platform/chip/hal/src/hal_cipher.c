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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_cipher.c $ //
//    $Author: admin $                                                       // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_cipher                                                          //
/// HAL cipher command driver                                                 //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////// 

#include "cs_types.h"

#include "global_macros.h"
#include "cipher.h"

#include "hal_cipher.h"
#include "hal_error.h"

// =============================================================================
// hal_CipherKcInit
// -----------------------------------------------------------------------------
/// Initialize Kc
/// @param kcLow   Low part of the Kc Key
/// @param kcHigh  High part of the Kc Key
/// @return #HAL_ERR_RESOURCE_BUSY if the ciphering is already running or 
/// #HAL_ERR_NO if the initialization is properly done.
// =============================================================================

PUBLIC UINT8 hal_CipherKcInit(UINT32 kcLow,    UINT32 kcHigh)
{
    UINT32 error;

    //  Check if the cipher A5 is already processing 
    error = hwp_cipher->status & CIPHER_RUNNING;
    if (error) 
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else 
    {
        //  Write low bits of Kc register 
        hwp_cipher->Kc_low = kcLow;
        //  Write high bits of Kc register 
        hwp_cipher->Kc_high = kcHigh;
        return HAL_ERR_NO;
    }
}
