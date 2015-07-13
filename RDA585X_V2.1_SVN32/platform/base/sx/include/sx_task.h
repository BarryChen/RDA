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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/base/sx/include/sx_task.h $                                                                   
//  $Author: admin $                                                                    
//  $Date: 2010-07-07 20:26:37 +0800 (星期三, 07 七月 2010) $                                                                      
//  $Revision: 268 $                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _SX_TASK_H_
#define _SX_TASK_H_



// =============================================================================
//  MACROS
// =============================================================================
#define SXR_NO_TASK                              (0XFF)
#define SXR_FREE_TSK                             ((1 << 0))
#define SXR_ALLOCATED_TSK                        ((1 << 1))
#define SXR_PENDING_TSK                          ((1 << 2))
#define SXR_SUSPENDED_TSK                        ((1 << 3))
#define SXR_ACTIVE_TSK                           ((1 << 4))
#define SXR_WAIT_MSG_TSK                         ((1 << 5))
#define SXR_STOPPED_TSK                          (SXR_ALLOCATED_TSK)
#define SXR_NB_MAX_TASK                          (40)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SXR_TASKDESC_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT32*                        TaskBody;                     //0x00000000
    UINT32*                        TaskExit;                     //0x00000004
    CONST INT8*                    Name;                         //0x00000008
    UINT16                         StackSize;                    //0x0000000C
    UINT8                          Priority;                     //0x0000000E
} SXR_TASKDESC_T; //Size : 0x10



// ============================================================================
// SXR_TASKCTX_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT32*                        Sp;                           //0x00000000
    VOID*                          Pc;                           //0x00000004
    UINT32*                        StackTop;                     //0x00000008
    CONST SXR_TASKDESC_T*          Desc;                         //0x0000000C
    VOID*                          DataCtx;                      //0x00000010
    UINT8                          State;                        //0x00000014
    UINT8                          Id;                           //0x00000015
    UINT8                          Next;                         //0x00000016
    UINT8                          Free;                         //0x00000017
} SXR_TASKCTX_T; //Size : 0x18





#endif

