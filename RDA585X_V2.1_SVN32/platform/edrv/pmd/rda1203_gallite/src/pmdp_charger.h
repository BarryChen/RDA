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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/pmd/rda1203_gallite/src/pmdp_charger.h $ //
//	$Author: huazeng $                                                        // 
//	$Date: 2010-07-07 18:05:49 +0800 (星期三, 07 七月 2010) $                     //   
//	$Revision: 252 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file pmdp_charger.h  
/// This file contain the private PMD charger driver API.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "pmd_config.h"
#include "pmd_m.h"

// ============================================================================
//  MACROS
// ============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_CHARGER_STATE_T
// -----------------------------------------------------------------------------
/// This structure groups all the global states used by the charger
/// Must be in sync with pmd_map.xmd PMD_MAP_CHARGER_STATE_T
// =============================================================================
typedef struct
{
    PMD_CHARGER_STATUS_T status;
    PMD_CHARGE_CURRENT_T current;
    PMD_CHARGER_HANDLER_T handler;
    UINT16 batteryLevel;
    UINT16 pulsedOnCount;
    UINT16 pulsedOffCount;

    UINT16 pulsedCycleCount;
    UINT16 pulsedOnMeanCount;
    UINT16 pulsedOffMeanCount;

    UINT32 startTime;
    UINT32 lastWTime;

    BOOL highActivityState;
} PMD_CHARGER_STATE_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

/// State structure of the charger
EXPORT PROTECTED PMD_CHARGER_STATE_T g_pmdChargerState;

// ============================================================================
//  FUNCTIONS
// ============================================================================


// ============================================================================
// pmd_DcOnHandler
// ----------------------------------------------------------------------------
/// Registerred handler for GPIO_0 used as DC-ON Detect.
/// 
// ============================================================================
VOID pmd_DcOnHandler(BOOL on);
VOID pmd_InitCharger(VOID);
VOID pmd_ChargerPlugIn(VOID);


// ============================================================================
//  PROTECTED types & functions from pmd.c used in pmd_charger.c
// ============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// OPAL_PROFILE_MODE_T
// -----------------------------------------------------------------------------
/// Profile register selection.
/// Only low power and normal mode (using profile 2 & 1) are used in the driver.
// =============================================================================
typedef enum
{
    OPAL_PROFILE_MODE_NORMAL = 0,
    OPAL_PROFILE_MODE_LOWPOWER,

    OPAL_PROFILE_MODE_QTY,

} OPAL_PROFILE_MODE_T;


// ============================================================================
//  FUNCTIONS
// ============================================================================


// ============================================================================
// pmd_EnableOpalLdo
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param ldo the id of the ldo to switch from enum #PMD_LDO_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
/// @param profile the profile applied.
/// @param user the LDO user set by POWER_LDO_USER or LEVEL_LDO_USER macro.
// ============================================================================
PROTECTED VOID pmd_EnableOpalLdo(PMD_LDO_ID_T ldo, BOOL on, OPAL_PROFILE_MODE_T profile, UINT32 user);



