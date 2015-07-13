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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_ana_afc.c $ //
//    $Author: admin $                                                        // 
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                     //   
//    $Revision: 269 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
///     @file hal_ana_afc.c
///     Implementation of Granite's AFC Driver
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"

#include "global_macros.h"

#ifdef BT_SHAREMEM_TRANSPORT
#include "sharemem.h"
#include "gpio.h"

#include "hal_sharemem.h"


#define AFC_NEXT_VAL_DATE 0

UINT32 *g_hal_read_addr;
UINT32 *g_hal_write_addr;

// =============================================================================
// hal_AnaAfcOpen
// -----------------------------------------------------------------------------
/// Power Up and Initialize the AFC.
/// Takes a resource.
// =============================================================================
PUBLIC void hal_ShareMemInit(void)
{
    g_hal_read_addr = hwp_ShareMem->Rx_Buff;
    g_hal_write_addr = hwp_ShareMem->Tx_Buff;
    hwp_ShareMem->Rx_Buff[0] = 0;
    hwp_ShareMem->Tx_Buff[0] = 0;
}

PUBLIC UINT16 *hal_ShareMem_GetHeader(void)
{
    if(*g_hal_read_addr == 0xffffffff)
        g_hal_read_addr = hwp_ShareMem->Rx_Buff;
        
    return (UINT16*)(g_hal_read_addr);
}

PUBLIC UINT8 *hal_ShareMem_GetData(void)
{
    return (UINT8*)(g_hal_read_addr+1);
}

PUBLIC void hal_ShareMem_NextData(UINT32 len)
{
    g_hal_read_addr += ((len+3)>>2)+1;
    //hal_HstSendEvent(g_hal_read_addr);
}

PUBLIC UINT8 hal_ShareMem_GetRxPage(void)
{
    return ((g_hal_read_addr - (UINT32*)hwp_ShareMem->Rx_Buff)>>8);
}

PUBLIC UINT8 hal_ShareMem_GetTxPage(void)
{
    return ((g_hal_write_addr - (UINT32*)hwp_ShareMem->Tx_Buff)>>6);
}

PUBLIC void hal_ShareMem_SendHeader(UINT16 *header)
{
    memcpy(g_hal_write_addr, header, 4);
    
    g_hal_write_addr += ((header[1]+3)>>2)+1;

    *g_hal_write_addr = 0x00;

    hwp_gpio->gpio_oen_set_in ^= 0x40000000; // reverse gpio 30
}

PUBLIC void hal_ShareMem_SendData(UINT8 *data, UINT32 len)
{
    if(data != NULL && len > 0)
    {
        if(g_hal_write_addr+((len+3)>>2)+3 - (UINT32*)(hwp_ShareMem->Tx_Buff) > SHAREMEM_SIZE)
        {
            hwp_ShareMem->Tx_Buff[0] = 0;
            *g_hal_write_addr = 0xffffffff;
            g_hal_write_addr = hwp_ShareMem->Tx_Buff;
        }
        memcpy((UINT8*)(g_hal_write_addr+1), data, len);
    }
}

#endif

