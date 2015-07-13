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
VOID pmd_RestoreChargerAtPowerOff(VOID);


// ============================================================================
//  PROTECTED types & functions from pmd.c used in pmd_charger.c
// ============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_PROFILE_MODE_T
// -----------------------------------------------------------------------------
/// Profile register selection.
/// Only low power and normal mode (using profile 2 & 1) are used in the driver.
// =============================================================================
typedef enum
{
    PMD_PROFILE_MODE_NORMAL = 0,
    PMD_PROFILE_MODE_LOWPOWER,

    PMD_PROFILE_MODE_QTY,

} PMD_PROFILE_MODE_T;


// =============================================================================
// PMD_BG_LP_MODE_USER_ID_T
// -----------------------------------------------------------------------------
/// BG LP mode users.
// =============================================================================
typedef enum
{
    PMD_BG_LP_MODE_USER_CHARGER,
    PMD_BG_LP_MODE_USER_KEYPAD_LED,
    PMD_BG_LP_MODE_USER_LED0,
    PMD_BG_LP_MODE_USER_LED1,
    PMD_BG_LP_MODE_USER_LED2,
    PMD_BG_LP_MODE_USER_LED3,

    PMD_BG_LP_MODE_USER_ID_QTY
} PMD_BG_LP_MODE_USER_ID_T;


// =============================================================================
// PMD_CLK4M_AVDD3_USER_ID_T
// -----------------------------------------------------------------------------
/// Clock 4M and AVDD3 users.
// =============================================================================
typedef enum
{
    PMD_CLK4M_AVDD3_USER_KEYPAD_LED,
    PMD_CLK4M_AVDD3_USER_LED0,
    PMD_CLK4M_AVDD3_USER_LED1,
    PMD_CLK4M_AVDD3_USER_LED2,
    PMD_CLK4M_AVDD3_USER_LED3,

    PMD_CLK4M_AVDD3_USER_ID_QTY
} PMD_CLK4M_AVDD3_USER_ID_T;


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
PROTECTED VOID pmd_EnableOpalLdo(PMD_LDO_ID_T ldo, BOOL on, PMD_PROFILE_MODE_T profile, UINT32 user);


PROTECTED VOID pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_ID_T user, BOOL on);


PROTECTED VOID pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_ID_T user,
                                            BOOL on);


