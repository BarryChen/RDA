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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/pmd/include/pmd_map.h $                                                                   
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

#ifndef _PMD_MAP_H_
#define _PMD_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================
#define PMD_MAP_POWER_ID_QTY                     (14)
#define PMD_MAP_LEVEL_ID_QTY                     (9)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PMD_MAP_CHARGER_STATE_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT32                         status;                       //0x00000000
    UINT32                         current;                      //0x00000004
    VOID*                          handler;                      //0x00000008
    UINT16                         batteryLevel;                 //0x0000000C
    UINT16                         pulsedOnCount;                //0x0000000E
    UINT16                         pulsedOffCount;               //0x00000010
    UINT16                         pulsedCycleCount;             //0x00000012
    UINT16                         pulsedOnMeanCount;            //0x00000014
    UINT16                         pulsedOffMeanCount;           //0x00000016
    UINT32                         startTime;                    //0x00000018
    UINT32                         lastWTime;                    //0x0000001C
    BOOL                           highActivityState;            //0x00000020
} PMD_MAP_CHARGER_STATE_T; //Size : 0x24



// ============================================================================
// PMD_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// Type used to define the accessible structures of the module.
// =============================================================================
typedef struct
{
    UINT32                         powerInfo[PMD_MAP_POWER_ID_QTY]; //0x00000000
    /// Current level value.
    UINT32                         levelInfo[PMD_MAP_LEVEL_ID_QTY]; //0x00000038
    PMD_MAP_CHARGER_STATE_T*       chargerState;                 //0x00000060
} PMD_MAP_ACCESS_T; //Size : 0x64

//powerInfo
#define PMD_MAP_ACCESS_T_POWER_SHARED_LINK_FLAG (1<<31)
#define PMD_MAP_ACCESS_T_POWER_VALUE(n) (((n)&0x7FFFFFFF)<<0)






// =============================================================================
// pmd_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID pmd_RegisterYourself(VOID);

    

#endif

