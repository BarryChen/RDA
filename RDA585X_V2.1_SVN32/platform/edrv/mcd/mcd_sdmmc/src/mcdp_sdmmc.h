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
//
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/mcd/mcd_sdmmc/src/mcdp_sdmmc.h $
//  $Author: huazeng $
//  $Date: 2010-08-23 13:47:34 +0800 (星期一, 23 八月 2010) $
//  $Revision: 1532 $
//
////////////////////////////////////////////////////////////////////////////////
//
/// @file mcdp_sdmmc.h
///
/// Structures privatly used by the SDMMC module implementation of MCD
/// 
//
////////////////////////////////////////////////////////////////////////////////



// =============================================================================
// MCD_CARD_STATE_T
// -----------------------------------------------------------------------------
/// The state of the card when receiving the command. If the command execution 
/// causes a state change, it will be visible to the host in the response to 
/// the next command. The four bits are interpreted as a binary coded number 
/// between 0 and 15.
// =============================================================================
typedef enum
{
    MCD_CARD_STATE_IDLE    = 0,
    MCD_CARD_STATE_READY   = 1,
    MCD_CARD_STATE_IDENT   = 2,
    MCD_CARD_STATE_STBY    = 3,
    MCD_CARD_STATE_TRAN    = 4,
    MCD_CARD_STATE_DATA    = 5,
    MCD_CARD_STATE_RCV     = 6,
    MCD_CARD_STATE_PRG     = 7,
    MCD_CARD_STATE_DIS     = 8
} MCD_CARD_STATE_T;


// =============================================================================
// MCD_CARD_STATUS_T
// -----------------------------------------------------------------------------
/// Card status as returned by R1 reponses (spec V2 pdf p.)
// =============================================================================
typedef union
{
    UINT32 reg;
    struct
    {
        UINT32                          :3;
        UINT32 akeSeqError              :1;
        UINT32                          :1;
        UINT32 appCmd                   :1;
        UINT32                          :2;
        UINT32 readyForData             :1;
        MCD_CARD_STATE_T currentState   :4;
        UINT32 eraseReset               :1;
        UINT32 cardEccDisabled          :1;
        UINT32 wpEraseSkip              :1;
        UINT32 csdOverwrite             :1;
        UINT32                          :2;
        UINT32 error                    :1;
        UINT32 ccError                  :1;
        UINT32 cardEccFailed            :1;
        UINT32 illegalCommand           :1;
        UINT32 comCrcError              :1;
        UINT32 lockUnlockFail           :1;
        UINT32 cardIsLocked             :1;
        UINT32 wpViolation              :1;
        UINT32 eraseParam               :1;
        UINT32 eraseSeqError            :1;
        UINT32 blockLenError            :1;
        UINT32 addressError             :1;
        UINT32 outOfRange               :1;
    } fields;
} MCD_CARD_STATUS_T;



