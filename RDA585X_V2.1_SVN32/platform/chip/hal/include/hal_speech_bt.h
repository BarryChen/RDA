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
//    THIS FILE WAS GENERATED FROM ITS CORRESPONDING XML VERSION WITH COOLXML.  
//                                                                              
//                       !!! PLEASE DO NOT EDIT !!!                             
//                                                                              
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_speech.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:28:03 +0800 (星期三, 07 七月 2010) $                                                                      
//  $Revision: 269 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
/// That file contains speech buffer structures and related functions. 
///  
///  @defgroup speech HAL Speech FIFO 
///  @par Operation A speech FIFO is used to convey audio data from the Baseband
/// side to acoustic world. PAL accesses this FIFO through #hal_SpeechBtPushRxPopTx,
/// which puts received audio data in the Fifo and takes out audio data to send.
/// 
///  @par On the other side, #hal_SpeechBtPopRxPushTx is aimed to be used by the VOIS
/// service. It takes received data from the Fifo (to decode them and play them)
/// and puts in it audio data to send (recorded from a mic and encoded), 
///  @par Thus, we see that the speech FIFO and its related mechanisms only manipulate
/// encoded data. 
///  @{
//
//==============================================================================

#ifndef _HAL_SPEECH_BT_H_
#define _HAL_SPEECH_BT_H_


#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================
/// FIFO depth.
#define HAL_SPEECH_BT_FIFO_DEPTH                    (3)


// ============================================================================
// HAL_SPEECH_BT_FRAME_SIZE_T
// -----------------------------------------------------------------------------
/// Size of the frame in bytes, for the various supported codecs.
// =============================================================================
typedef enum
{
    HAL_SPEECH_BT_FRAME_SIZE_AMR475                = 0x0000000C,
    HAL_SPEECH_BT_FRAME_SIZE_AMR515                = 0x00000010,
    HAL_SPEECH_BT_FRAME_SIZE_AMR59                 = 0x00000010,
    HAL_SPEECH_BT_FRAME_SIZE_AMR67                 = 0x00000014,
    HAL_SPEECH_BT_FRAME_SIZE_AMR74                 = 0x00000014,
    HAL_SPEECH_BT_FRAME_SIZE_AMR795                = 0x00000014,
    HAL_SPEECH_BT_FRAME_SIZE_AMR102                = 0x0000001C,
    HAL_SPEECH_BT_FRAME_SIZE_AMR122                = 0x00000020,
    HAL_SPEECH_BT_FRAME_SIZE_AMR_RING              = 0x00000020,
    HAL_SPEECH_BT_FRAME_SIZE_EFR                   = 0x00000020,
    HAL_SPEECH_BT_FRAME_SIZE_FR                    = 0x00000024,
    HAL_SPEECH_BT_FRAME_SIZE_HR                    = 0x00000010,
    HAL_SPEECH_BT_FRAME_SIZE_COD_BUF               = 0x00000024,
    HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF               = 0x00000140
} HAL_SPEECH_BT_FRAME_SIZE_T;


typedef struct
{
    UINT16                         encOutBuf[HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2]; //0x00000008
} HAL_SPEECH_BT_ENC_OUT_T; //Size : 0x2C



// ============================================================================
// HAL_SPEECH_DEC_IN_T
// -----------------------------------------------------------------------------
/// Structure used to configure VOC frame decode
// =============================================================================
typedef struct
{
    /// Decoder input buffer (coded frame in dec_mode)
    UINT16                         decInBuf[HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2]; //0x00000010
} HAL_SPEECH_BT_DEC_IN_T; //Size : 0x34

// ============================================================================
// HAL_SPEECH_BT_ENC_OUT_T
// -----------------------------------------------------------------------------
/// Structure used to configure VOC frame encode
// =============================================================================
typedef struct
{
    UINT16                         OutBuf[HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2]; //0x00000008
} HAL_SPEECH_BT_OUT_T; //Size : 0x2C



// ============================================================================
// HAL_SPEECH_BT_DEC_IN_T
// -----------------------------------------------------------------------------
/// Structure used to configure VOC frame decode
// =============================================================================
typedef struct
{
    /// Decoder input buffer (coded frame in dec_mode)
    UINT16                         InBuf[HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2]; //0x00000010
} HAL_SPEECH_BT_IN_T; //Size : 0x34



// ============================================================================
// HAL_SPEECH_BT_PCM_HALF_BUF_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef UINT16 HAL_SPEECH_BT_PCM_HALF_BUF_T[HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2];


// ============================================================================
// HAL_SPEECH_BT_PCM_BUF_T
// -----------------------------------------------------------------------------
/// Structure defining the Speech PCM swap buffers used in Speech encoding and decoding
// =============================================================================
typedef struct
{
    HAL_SPEECH_BT_PCM_HALF_BUF_T      pcmBuf[2];                    //0x00000000
} HAL_SPEECH_BT_PCM_BUF_T; //Size : 0x280


// ============================================================================
// HAL_SPEECH_BT_FIFO_ELM_T
// -----------------------------------------------------------------------------
/// This types describes a Fifo section. Rx and Tx are joined together because the
/// we always read and write one Rx and a Tx (or inversly) at the same time.
// =============================================================================
typedef struct
{
    /// Received encoded frame
    UINT16            rx[HAL_SPEECH_BT_FIFO_DEPTH*HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2];                           //0x00000000
    /// Encoded Frame to transmit
    UINT16           tx[HAL_SPEECH_BT_FIFO_DEPTH*HAL_SPEECH_BT_FRAME_SIZE_PCM_BUF/2];                           //0x00000034
} HAL_SPEECH_BT_FIFO_ELM_T; //Size : 0x60



// ============================================================================
// HAL_SPEECH_BT_FIFO_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    /// This types describes a Fifo section. Rx and Tx are joined together because
    /// the we always read and write one Rx and a Tx (or inversly) at the same time.
    HAL_SPEECH_BT_FIFO_ELM_T          fifo;  //0x00000000
    /// That boolean is set to TRUE when the Speech Fifo Rx has overflowed, which
    /// also corresponds to a Tx underflow. The reset is manual through coolwatcher.
    BOOL                           rxOverflowTxUnderflow;        //0x00000180
    /// That boolean is set to TRUE when the Speech Fifo Tx has overflowed, which
    /// also corresponds to a Rx underflow. This is normally expected in case of
    /// handover, for example. The reset is manual through coolwatcher.
    BOOL                           txOverflowRxUnderflow;        //0x00000181
} HAL_SPEECH_BT_FIFO_T; //Size : 0x182





// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_SpeechBtFifoReset
// -----------------------------------------------------------------------------
/// Initializes the speech related mechanisms in HAL (Speech Fifo, ...)
// =============================================================================
PUBLIC VOID hal_SpeechBtFifoReset(VOID);

// =============================================================================
// hal_SpeechBtPushRxPopTx
// -----------------------------------------------------------------------------
/// PAL side of the FIFO accessor.
///
/// @param pushedRx Pointer to the received trame to push in the FIFO
/// @param popedTx Pointer to the buffer where the trame to send will
/// be copied.
// =============================================================================
PUBLIC VOID hal_Speech_BtPushRxPopTx(
CONST UINT8* pushedRx, UINT8* popedTx,UINT16 length);

// =============================================================================
// hal_SpeechBtPopRxPushTx
// -----------------------------------------------------------------------------
/// Vois side of the FIFO accessor.
///
/// @param popedRx Pointer where to copy the received trame.
/// @param pushedTx Pointer to the trame to send to push in the FIFO.
// =============================================================================
PUBLIC VOID hal_SpeechBtPopRxPushTx(UINT8* popedRx,
CONST UINT8* pushedTx);

/// @} // --> End of the Speech group.



#endif

