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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/pmd/include/pmd_m.h $ //
//  $Author: admin $                                                        // 
//  $Date: 2012-11-09 18:22:51 +0800 (周五, 09 十一月 2012) $                     //   
//  $Revision: 17608 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file pmd_m.h  
/// This file contain the PMD driver API.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


/// @defgroup pmd EDRV Power Management Driver (PMD)
/// 
/// This page describes the power management driver.
/// 
/// @{

#ifndef _PMD_M_H_
#define _PMD_M_H_

#include "cs_types.h"



// ============================================================================
//  MACROS
// ============================================================================

// =============================================================================
// PMD_BAD_VALUE
// -----------------------------------------------------------------------------
/// Value returned when the battery level is not available yet
// =============================================================================
#define PMD_BAD_VALUE  0xFFFF



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_POWERMODE_T
// -----------------------------------------------------------------------------
/// PMIC power mode
// =============================================================================
typedef enum {
    /// Low Power Mode - Power configuration for Sleep (Baseband at 32k)
    PMD_LOWPOWER,
    /// Idle Power Mode - Power configuration for Idle Mode
    PMD_IDLEPOWER
} PMD_POWERMODE_T;


// =============================================================================
// PMD_POWER_ID_T
// -----------------------------------------------------------------------------
/// PMIC power ID
/// ID used by #pmd_EnablePower to enable or disable a power.
// =============================================================================
typedef enum {
    /// Micro bias enable
    PMD_POWER_MIC,
    /// Camera LDO enable
    PMD_POWER_CAMERA,
    /// Audio LDO enable
    PMD_POWER_AUDIO,
    /// Stereo DAC LDO enable
    PMD_POWER_STEREO_DAC,
    /// Loud Speaker enable
    PMD_POWER_LOUD_SPEAKER,
    /// RF Power Amplifier
    PMD_POWER_PA,
    /// USB LDOs enable
    PMD_POWER_USB,
    /// SD/MMC LDO enable
    PMD_POWER_SDMMC,
    /// FM LDO enable
    PMD_POWER_FM,
    /// Ear Piece Micro bias enable
    PMD_POWER_EARPIECE,
    /// BlueTooth LDOs enable
    PMD_POWER_BT,
    /// Camera flash Light enable
    PMD_POWER_CAMERA_FLASH,
    /// (main) LCD LDO enable
    PMD_POWER_LCD,
    /// I2C LDO enable
    PMD_POWER_I2C,

    // must be in sync with pmd_map.xmd PMD_MAP_POWER_ID_QTY
    PMD_POWER_ID_QTY
} PMD_POWER_ID_T;

// =============================================================================
// PMD_LEVEL_ID_T
// -----------------------------------------------------------------------------
/// PMIC level ID
/// ID used by #pmd_SetLevel.
// =============================================================================
typedef enum {
    /// KeyPad Back Light level
    PMD_LEVEL_KEYPAD,
    /// (main) LCD Back Light level
    PMD_LEVEL_LCD,
    /// Sub LCD Back Light level
    PMD_LEVEL_SUB_LCD,
    /// LED0 Light level
    PMD_LEVEL_LED0,
    /// LED1 Light level
    PMD_LEVEL_LED1,
    /// LED2 Light level
    PMD_LEVEL_LED2,
    /// LED3 Light level
    PMD_LEVEL_LED3,
    /// Vibrator control level
    PMD_LEVEL_VIBRATOR,
    /// loudspeaker gain
    PMD_LEVEL_LOUD_SPEAKER,

    // must be in sync with pmd_map.xmd PMD_MAP_LEVEL_ID_QTY
    PMD_LEVEL_ID_QTY
} PMD_LEVEL_ID_T;



// ============================================================================
// PMD_SIM_VOLTAGE_CLASS_T
// ----------------------------------------------------------------------------
/// This enum lists the supported voltages classes for a SIM card.
/// The level class is set with the function #pmd_SetLevel(#PMD_LEVEL_SIM, \c level).
/// Voltage class must \b _ALWAYS_ be set to #PMD_SIM_VOLTAGE_0V0 before 
/// re-programming voltage
// ============================================================================
typedef enum
{
    /// Sci voltage class is 0
    PMD_SIM_VOLTAGE_0V0,

    /// Sci voltage class is 1,8V
    PMD_SIM_VOLTAGE_1V8,

    /// Sci voltage class is 3V
    PMD_SIM_VOLTAGE_3V0,

    /// Sci voltage class is 5V
    PMD_SIM_VOLTAGE_5V0,

    PMD_SIM_VOLTAGE_QTY
} PMD_SIM_VOLTAGE_CLASS_T;

// ============================================================================
// PMD_BACK_LIGHT_LEVEL_T
// ----------------------------------------------------------------------------
/// This enum lists the supported back light levels for LED drivers
/// The level is set with the function #pmd_SetLevel.
// ============================================================================
typedef enum
{
    /// Back light level 0 (off)
    PMD_BACK_LIGHT_LEVEL_0,

    /// Back light level 1
    PMD_BACK_LIGHT_LEVEL_1,

    /// Back light level 2
    PMD_BACK_LIGHT_LEVEL_2,

    /// Back light level 3
    PMD_BACK_LIGHT_LEVEL_3,

    /// Back light level 4
    PMD_BACK_LIGHT_LEVEL_4,

    /// Back light level 5
    PMD_BACK_LIGHT_LEVEL_5,

    /// Back light level 6
    PMD_BACK_LIGHT_LEVEL_6,

    /// Back light level 7 (full on)
    PMD_BACK_LIGHT_LEVEL_7,

    PMD_BACK_LIGHT_LEVEL_QTY
} PMD_BACK_LIGHT_LEVEL_T;


// ============================================================================
// PMD_CHARGER_STATUS_T
// ----------------------------------------------------------------------------
/// This enum lists the charger status.
/// This values are returned by pmd_GetChargerStatus()
/// or gived as parameter to the handler #PMD_CHARGER_HANDLER_T
/// Any values at or above #PMD_CHARGER_PLUGGED means that a
/// charger is plugged.
// ============================================================================
typedef enum
{
    /// Starting state before the charger detection has occured once
    PMD_CHARGER_UNKNOWN,            // 0
    /// No Charger is plugged
    PMD_CHARGER_UNPLUGGED,          // 1
    /// Charger is plugged, but charge is not running
    PMD_CHARGER_PLUGGED,            // 2
    /// Charger is plugged, charge in precharge phase (whe souldn't be on anyway)
    PMD_CHARGER_PRECHARGE,          // 3
    /// Charger is plugged, charge in fast mode (constant current)
    PMD_CHARGER_FAST_CHARGE,        // 4
    /// Charger is plugged, charge in pulsed mode (constant voltage, active)
    PMD_CHARGER_PULSED_CHARGE,      // 5
    /// Charger is plugged, charge in pulsed mode (constant voltage, inactive)
    PMD_CHARGER_PULSED_CHARGE_WAIT, // 6
    /// Charger is plugged, charge done.
    PMD_CHARGER_FULL_CHARGE,        // 7
    /// Charger is plugged, charge stopped for safety (error).
    PMD_CHARGER_ERROR,              // 8
    /// Charger is plugged, charge stopped for safety (timeout).
    PMD_CHARGER_TIMED_OUT,          // 9
    /// Charger is plugged, charge stopped for safety (temperature).
    PMD_CHARGER_TEMPERATURE_ERROR,  // 10
    /// Charger is plugged, charge stopped for safety (voltage).
    PMD_CHARGER_VOLTAGE_ERROR,      // 11

    PMD_CHARGER_STATUS_QTY
} PMD_CHARGER_STATUS_T;

// =============================================================================
// PMD_CHARGER_HANDLER_T
// -----------------------------------------------------------------------------
/// Type use to define the user handling function called when a (known) change
/// related to the charger occurs. The new \p status is given as parameter
// =============================================================================
typedef VOID (*PMD_CHARGER_HANDLER_T)(PMD_CHARGER_STATUS_T status);


// ============================================================================
// PMD_CHARGE_CURRENT_T
// ----------------------------------------------------------------------------
/// This enum lists the supported charge current
/// The level is set with the function #pmd_SetChargeCurrent
// ============================================================================
typedef enum
{
    /// Charge is forced OFF
    PMD_CHARGE_OFF,
    /// Charge with a  50 mA current
    PMD_CHARGER_50MA,
    /// Charge with a 100 mA current
    PMD_CHARGER_100MA,
    /// Charge with a 150 mA current
    PMD_CHARGER_150MA,
    /// Charge with a 200 mA current
    PMD_CHARGER_200MA,
    /// Charge with a 300 mA current
    PMD_CHARGER_300MA,
    /// Charge with a 400 mA current
    PMD_CHARGER_400MA,
    /// Charge with a 500 mA current
    PMD_CHARGER_500MA,
    /// Charge with a 600 mA current
    PMD_CHARGER_600MA,
    /// Charge with a 700 mA current
    PMD_CHARGER_700MA,
    /// Charge with a 800 mA current
    PMD_CHARGER_800MA,

    PMD_CHARGE_CURRENT_QTY
} PMD_CHARGE_CURRENT_T;



// ============================================================================
// PMD_CORE_VOLTAGE_T
// ----------------------------------------------------------------------------
/// This enum lists the supported core voltage levels in active/idle mode.
/// The level is set with the function #pmd_SetCoreVoltage
// ============================================================================
typedef enum
{
    /// The core voltage is low
    PMD_CORE_VOLTAGE_LOW,
    /// The core voltage is medium
    PMD_CORE_VOLTAGE_MEDIUM,
    /// The core voltage is high
    PMD_CORE_VOLTAGE_HIGH,

    PMD_CORE_VOLTAGE_QTY
} PMD_CORE_VOLTAGE_T;



// ============================================================================
// PMD_CORE_VOLTAGE_USER_ID_T
// ----------------------------------------------------------------------------
/// This enum lists the users that request core voltage levels.
// ============================================================================
typedef enum
{
    /// System frequency
    PMD_CORE_VOLTAGE_USER_SYS,
    /// VOC frequency
    PMD_CORE_VOLTAGE_USER_VOC,
    /// Speaker
    PMD_CORE_VOLTAGE_USER_SPEAKER,

    PMD_CORE_VOLTAGE_USER_ID_QTY
} PMD_CORE_VOLTAGE_USER_ID_T;




// ============================================================================
// PMD_SPEAKER_PA_MODE_T
// ----------------------------------------------------------------------------
/// This enum lists the supported speaker PA mode.
/// The mode is set with the function #pmd_SpeakerPaSetMode
// ============================================================================
typedef enum
{
    /// The speaker PA mode is class AB
    PMD_SPEAKER_PA_MODE_CLASS_AB,
    /// The speaker PA mode is class D
    PMD_SPEAKER_PA_MODE_CLASS_D,
    /// The speaker PA mode is class K
    PMD_SPEAKER_PA_MODE_CLASS_K,

    PMD_SPEAKER_PA_MODE_QTY
} PMD_SPEAKER_PA_MODE_T;



// =============================================================================
// PMD_CONFIG_T
// -----------------------------------------------------------------------------
/// This is the anonymous declaration of the structure used to describes the 
/// configuration of the PMD driver. The actual content is specifc to the pmd 
/// driver of the actual PM Chip
// =============================================================================
typedef struct PMD_CONFIG_STRUCT_T PMD_CONFIG_T;



// ============================================================================
//  FUNCTIONS
// ============================================================================

EXTERN_C_START

// =============================================================================
// pmd_Open
// -----------------------------------------------------------------------------
/// Open the PMD driver. This function configure the devices and set the 
/// global variables needed for proper operation of the PMD driver.
/// It must be called before any other function of the PMD driver. That's why 
/// this function is called in the end of #hal_Open. That way, the PMD driver
/// functions should always be usable by the user program, as soon as its main
/// function is launched by HAL.
///
/// @param pmdConfig Pointer to the PMD configuration structure as obtained
/// through the TGT API (#tgt_GetPmdConfig).
// =============================================================================
PUBLIC VOID pmd_Open(CONST PMD_CONFIG_T* pmdConfig);



// =============================================================================
// pmd_Close
// -----------------------------------------------------------------------------
/// Closes the PMD driver.
// =============================================================================
PUBLIC VOID pmd_Close(VOID);


// ============================================================================
// pmd_SetPowerMode
// ----------------------------------------------------------------------------
/// Set Power Mode to the desired mode
// ============================================================================
PUBLIC VOID pmd_SetPowerMode(PMD_POWERMODE_T mode);

// ============================================================================
// pmd_ConfigureRfLowPower
// ----------------------------------------------------------------------------
/// Configure the Low Power state for the RF
///
/// @param xcvUseVRF \c TRUE if the transciever use V_RF from PMD.
/// @param xcvLowPowerKeepVRFOn \c TRUE if the power on V_RF must stay on 
///         during low power.
/// @return configuration for LPS (only fieds controlling LPSCO_0 and LPSCO_1)
///         actual type is #HAL_LPS_POWERUP_CTRL_T.
// ============================================================================
PUBLIC UINT32 pmd_ConfigureRfLowPower(BOOL xcvUseVRF, BOOL xcvLowPowerKeepVRFOn);


// ============================================================================
// pmd_EnablePower
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param id the id of the power to switch from enum #PMD_POWER_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_EnablePower(PMD_POWER_ID_T id, BOOL on);


// ============================================================================
// pmd_SetLevel
// ----------------------------------------------------------------------------
/// Set a Level to desired value.
///
/// @param id the id of the level to set from enum #PMD_LEVEL_ID_T
/// @param level the desired value, its acutal type depends on the \p id
// ============================================================================
PUBLIC VOID pmd_SetLevel(PMD_LEVEL_ID_T id, UINT32 level);



// ============================================================================
// pmd_SetHighActivity
// ----------------------------------------------------------------------------
/// Set the high activity state for battery measurement compensation.
///
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_SetHighActivity(BOOL on);



// ============================================================================
// pmd_GetBatteryLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery level 
///
/// @param pPercent receive a precentage of battery charge (can be NULL)
/// @return the battery level in mV or 
/// #PMD_BAD_VALUE if no result is available yet, in that case \p pPercent is
/// not updated.
// ============================================================================
PUBLIC UINT16 pmd_GetBatteryLevel(UINT8 * pPercent);


// ============================================================================
// pmd_BatteryLevelToPercent
// ----------------------------------------------------------------------------
/// Return the \p batteryLevel converted into Percent
///
/// @param batteryLevel the battery level in mV
/// @return a precentage of the given battery level
// ============================================================================
PUBLIC UINT8 pmd_BatteryLevelToPercent(UINT16 batteryLevel);



// ============================================================================
// pmd_GetBatteryTemperature
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery temperature
///
/// @return the battery temperature in °C or 
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetBatteryTemperature(VOID);


// ============================================================================
// pmd_GetChargerStatus
// ----------------------------------------------------------------------------
/// Return the charger status
///
/// @return the charger status
// ============================================================================
PUBLIC PMD_CHARGER_STATUS_T pmd_GetChargerStatus(VOID);


// ============================================================================
// pmd_SetChargerStatusHandler
// ----------------------------------------------------------------------------
/// Set the handler used to detect charger status (plugged, charge ...)
// ============================================================================
PUBLIC VOID pmd_SetChargerStatusHandler(PMD_CHARGER_HANDLER_T handler);


// ============================================================================
// pmd_SetChargeCurrent
// ----------------------------------------------------------------------------
/// Set the current for the charge.
// ============================================================================
PUBLIC VOID pmd_SetChargeCurrent(PMD_CHARGE_CURRENT_T current);


// ============================================================================
// pmd_IrqHandler
// ----------------------------------------------------------------------------
/// Handle PMU IRQ
// ============================================================================
PUBLIC VOID pmd_IrqHandler(UINT8 interruptId);


// ============================================================================
// pmd_SetCoreVoltage
// ----------------------------------------------------------------------------
/// Set the core voltage level in active/idle mode
// ============================================================================
PUBLIC VOID pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_ID_T user,
                               PMD_CORE_VOLTAGE_T coreVolt);


// ============================================================================
// pmd_EnableMemoryPower
// ----------------------------------------------------------------------------
/// Power up/down memory
// ============================================================================
PUBLIC VOID pmd_EnableMemoryPower(BOOL on);


// ============================================================================

PUBLIC UINT32 pmd_GetBatteryGpadcChannel(VOID);
PUBLIC UINT16 pmd_GetGpadcBatteryLevel(VOID);

PUBLIC UINT8 pmd_GetPowerOnVolt(VOID);
PUBLIC UINT8 pmd_GetPowerDownVolt(VOID);
PUBLIC UINT16 pmd_GetBackLightOffsetVolt(VOID);

#ifdef PMD_TWO_BATTARY

typedef VOID SWTICH_MAIN_BATTERY_T(VOID);
typedef VOID SWTICH_SUB_BATTERY_T(VOID);

VOID pmd_TwoBatteryCallback(SWTICH_MAIN_BATTERY_T*  setmain,SWTICH_SUB_BATTERY_T* setsub);

typedef enum
{
    PMD_BATTERY_MAIN,
    PMD_BATTERY_SUB,
    PMD_BATTERY_QTY
} PMD_BATTERY_NUMBER_T;

VOID pmd_SetCurrentBattery(PMD_BATTERY_NUMBER_T batterynum);
PMD_BATTERY_NUMBER_T pmd_GetCurrentBattery(VOID);

#endif

PUBLIC BOOL pmd_ChargerChrVoltMonEnabled(VOID);
PUBLIC UINT32 pmd_GetChargerGpadcChannel(VOID);
PUBLIC UINT16 pmd_GetGpadcChargerLevel(VOID);

PUBLIC UINT16 pmd_GetScreenAntiFlickerVolt(VOID);
PUBLIC VOID pmd_EnableScreenAntiFlicker(BOOL enable);


// ============================================================================
//  Earphone detection stuff
// ============================================================================

typedef enum
{
    PMD_EAR_STATUS_OUT,
    PMD_EAR_STATUS_IN,
    PMD_EAR_STATUS_UNKNOWN,
} PMD_EAR_STATUS_T;

typedef VOID (*PMD_EAR_HANDLER_T)(PMD_EAR_STATUS_T ear_status);

#define PMD_EAR_DETECT_INT 2
#define PMD_EAR_DETECT_POLLING 3
UINT8 pmd_GetEarModeStatus(VOID);
PUBLIC VOID pmd_SetEarDetectHandler(PMD_EAR_HANDLER_T handler);
PUBLIC UINT32 pmd_GetEarpieceGpadcChannel(VOID);


// ============================================================================
// Speaker PA stuff
// ============================================================================
PUBLIC BOOL pmd_AudExtPaExists(VOID);
PUBLIC VOID pmd_SpeakerPaPreEnable(BOOL enable);
PUBLIC VOID pmd_SpeakerPaPostEnable(BOOL enable);
PUBLIC VOID pmd_SpeakerPaSelectChannel(BOOL leftChanEn, BOOL rightChanEn);
PUBLIC VOID pmd_SpeakerPaSetMode(PMD_SPEAKER_PA_MODE_T leftChanMode,
                                 PMD_SPEAKER_PA_MODE_T rightChanMode);
PUBLIC VOID pmd_SpeakerPaRestoreMode(VOID);


// =============================================================================
//  GPADC stuff (only for 8808 or later)
// =============================================================================

PUBLIC VOID pmd_GpadcSetInterval(UINT32 interval);
PUBLIC VOID pmd_GpadcEnableChannel(UINT32 channel);
PUBLIC VOID pmd_GpadcDisable(VOID);
PUBLIC BOOL pmd_GpadcChannelEnabled(UINT32 channel);

PUBLIC UINT32 pmd_GetEncryptUid();
EXTERN_C_END

/// @} // <-- End of pmd group


#endif //_PMD_M_H_

