//==============================================================================
//                                                                              
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.              
//                            All Rights Reserved                               
//                                                                              
//      This source code is the property of Coolsand Technologies and is        
//      confidential.  Any  modification, distribution,  reproduction or        
//      exploitation  of  any content of this file is totally forbidden,        
//      except  with the  written permission  of  Coolsand Technologies.        
//                                                                              
//==============================================================================
//                                                                              
//                                                                              
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/halp_speech.h $                                                                   
//    $Author: admin $                                                                    
//    $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                                                                      
//    $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#include "hal_speech.h"
#include "hal_debug.h"
#include "halp_debug.h"

#ifndef _HALP_SPEECH_H_
#define _HALP_SPEECH_H_


EXPORT HAL_SPEECH_FIFO_T g_halSpeechFifo;


INLINE VOID hal_SpeechTraceRx(HAL_SPEECH_DEC_IN_T* popedRx)
{
    HAL_TRACE(TSTDOUT, 0, "VPP (mode,dtx) : 0x%04x%04x\n",
        popedRx->codecMode    & 0xffff,
        popedRx->dtxOn        & 0xffff);
        
    HAL_TRACE(TSTDOUT, 0, "VPP (bfi,frmT) : 0x%04x%04x\n",
        popedRx->bfi          & 0xffff,
        popedRx->decFrameType & 0xffff);
    
    HAL_TRACE(TSTDOUT, 0, "VPP (taf,sid) : 0x%04x%04x\n",
        popedRx->taf          & 0xffff,
        popedRx->sid          & 0xffff);
        
    HAL_TRACE(TSTDOUT, 0, "VPP (rsv,ufi) : 0x%04x%04x\n",
        popedRx->reserved     & 0xffff,
        popedRx->ufi          & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[ 1] & 0xffff, 
        popedRx->decInBuf[ 0] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[ 3] & 0xffff, 
        popedRx->decInBuf[ 2] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[ 5] & 0xffff, 
        popedRx->decInBuf[ 4] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[ 7] & 0xffff, 
        popedRx->decInBuf[ 6] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[ 9] & 0xffff, 
        popedRx->decInBuf[ 8] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[11] & 0xffff, 
        popedRx->decInBuf[10] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[13] & 0xffff, 
        popedRx->decInBuf[12] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[15] & 0xffff, 
        popedRx->decInBuf[14] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (decBuf) : 0x%04x%04x\n",
        popedRx->decInBuf[17] & 0xffff, 
        popedRx->decInBuf[16] & 0xffff);
}


INLINE VOID hal_SpeechTraceTx(HAL_SPEECH_ENC_OUT_T* popedTx)
{
    HAL_TRACE(TSTDOUT, 0, "VPP (frmT,mode) : 0x%04x%04x\n",
        popedTx->encFrameType & 0xffff,
        popedTx->encMode      & 0xffff);
        
    HAL_TRACE(TSTDOUT, 0, "VPP (sp,frmSkp) : 0x%04x%04x\n",
        popedTx->sp          & 0xffff,
        popedTx->echoSkipEncFrame & 0xffff);
    
    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[ 1] & 0xffff, 
        popedTx->encOutBuf[ 0] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[ 3] & 0xffff, 
        popedTx->encOutBuf[ 2] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[ 5] & 0xffff, 
        popedTx->encOutBuf[ 4] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[ 7] & 0xffff, 
        popedTx->encOutBuf[ 6] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[ 9] & 0xffff, 
        popedTx->encOutBuf[ 8] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[11] & 0xffff, 
        popedTx->encOutBuf[10] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[13] & 0xffff, 
        popedTx->encOutBuf[12] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[15] & 0xffff, 
        popedTx->encOutBuf[14] & 0xffff);

    HAL_TRACE(TSTDOUT, 0, "VPP (encBuf) : 0x%04x%04x\n",
        popedTx->encOutBuf[17] & 0xffff, 
        popedTx->encOutBuf[16] & 0xffff);
}




#endif


