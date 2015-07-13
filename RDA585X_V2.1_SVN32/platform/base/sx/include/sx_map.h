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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/sx/include/sx_map.h $                                                                   
//  $Author: huazeng $                                                                    
//  $Date: 2011-04-08 18:38:15 +0800 (星期五, 08 四月 2011) $                                                                      
//  $Revision: 6983 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
/// This file contains the portion of the module's memory map that will be accessible
/// through CoolWatcher (or any CoolXml tool). It is also used for the get version
/// mechanism.
//
//==============================================================================

#ifndef _SX_MAP_H_
#define _SX_MAP_H_


#include "sx_task.h"

// =============================================================================
//  MACROS
// =============================================================================
#define SXR_NB_MAX_JOB                           (12)
#define SXR_NB_MAX_JOB_EVT                       (16)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SXR_JOB_PREAL_STACK_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT32*                        stackTop;                     //0x00000000
    UINT32*                        stackBottom;                  //0x00000004
} SXR_JOB_PREAL_STACK_T; //Size : 0x8



// ============================================================================
// SXR_JOB_STACK_DESCRIPTOR_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef SXR_JOB_PREAL_STACK_T SXR_JOB_STACK_DESCRIPTOR_T[10];


// ============================================================================
// SXR_JBDESC_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT32*                        JbBody;                       //0x00000000
    UINT32*                        JbExit;                       //0x00000004
    INT8*                          Name;                         //0x00000008
    UINT16                         Id;                           //0x0000000C
    UINT8                          StackSize32;                  //0x0000000E
    UINT8                          Priority;                     //0x0000000F
} SXR_JBDESC_T; //Size : 0x10



// ============================================================================
// SXR_JBCTX_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT32*                        Sp;                           //0x00000000
    CONST SXR_JBDESC_T*            Desc;                         //0x00000004
    VOID*                          Data;                         //0x00000008
    UINT32*                        StackSwp;                     //0x0000000C
    UINT32*                        StackTop;                     //0x00000010
    UINT32*                        StackBottom;                  //0x00000014
    UINT16                         State;                        //0x00000018
    UINT8                          Free;                         //0x0000001A
    UINT8                          IdxSort;                      //0x0000001B
} SXR_JBCTX_T; //Size : 0x1C



// ============================================================================
// SXR_JOB_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          Active;                       //0x00000000
    UINT8                          LastActive;                   //0x00000001
    UINT8                          InitCnt;                      //0x00000002
    UINT8                          IdxFree;                      //0x00000003
    UINT8                          IdxQueue;                     //0x00000004
    UINT8                          Load;                         //0x00000005
    UINT8                          EvtQueue[SXR_NB_MAX_JOB_EVT]; //0x00000006
    VOID*                          EvtData[SXR_NB_MAX_JOB_EVT];  //0x00000018
    SXR_JBCTX_T                    Ctx[SXR_NB_MAX_JOB];          //0x00000058
    UINT8                          Sorted[SXR_NB_MAX_JOB];       //0x000001A8
    UINT8                          FirstReady;                   //0x000001B4
    UINT8                          SkipFrameEndEvt;              //0x000001B5
    UINT32                         BitMapReady;                  //0x000001B8
    UINT16                         EvtStatus[4];                 //0x000001BC
} SXR_JOB_T; //Size : 0x1C4



// ============================================================================
// SX_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// This global variable is the shared structure of SX.
// =============================================================================
typedef struct
{
    UINT32*                        sxExternalTraceEnablePtr;     //0x00000000
    UINT32*                        sxsDebugIdleHookEnablePtr;    //0x00000004
} SX_MAP_ACCESS_T; //Size : 0x8






// =============================================================================
// sx_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID sx_RegisterYourself(VOID);

    

#endif

