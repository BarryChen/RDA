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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/pmd/rda1203_gallite/src/pmd_charger.c $
//  $Author: huazeng $
//  $Date: 2011-04-09 11:27:00 +0800 (星期六, 09 四月 2011) $
//  $Revision: 6989 $
//
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file pmd_charger.c 
/// This file contain the PMD driver code related to the charger on RDA1203.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "pmd_config.h"
#include "pmd_m.h"
#include "pmdp_debug.h"
#include "pmdp_charger.h"
#include "pmdp.h"

#include "hal_ana_gpadc.h"
#include "hal_timers.h"
#include "rfd_xcv.h"
#include "opal.h"
#include "hal_lps.h"

// for function timers
#include "sxr_tim.h"

#include "hal_host.h"
#define PMD_FIX_EFFUSE_VALUES 1

#define SECOND        * HAL_TICK1S

#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR

#define PMD_TIME_CHG_MON_INTERVAL (2 SECONDS)



#define PMD_COUNT_CHG_STATUS_STABLE (10)

#define PMD_MV_OFFSET_CHG_RECHARGE (50)
#define PMD_MV_OFFSET_CHG_PHASE_2 (50)
#define PMD_MV_OFFSET_CHG_PHASE_3 (10)
#define PMD_MV_OFFSET_CHG_FULL_COMPENSATION (0) //(-10)

#if (PMD_MV_OFFSET_CHG_FULL_COMPENSATION >= PMD_MV_OFFSET_CHG_PHASE_3) || \
    (PMD_MV_OFFSET_CHG_PHASE_3 >= PMD_MV_OFFSET_CHG_PHASE_2)
#error "Incorrect charger voltage offsets"
#endif

#define PMD_INTERVAL_CHG_ON_LONG (9 MINUTES)
#define PMD_INTERVAL_CHG_ON_MEDIUM (6 MINUTES)
#define PMD_INTERVAL_CHG_ON_SHORT (3 MINUTES)
#define PMD_INTERVAL_CHG_MEAS_STABLE (1 MINUTE)

#define PMD_COUNT_CHG_CHR_VOLT_STABLE (5)

#define PMD_MV_OFFSET_CHG_CHR_VOLT (-30)


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PMD_CHARGE_EVENT_T
// ----------------------------------------------------------------------------
/// This enum lists the supported charge current
/// The level is set with the function #pmd_SetChargeCurrent
// ============================================================================
typedef enum
{
    /// Charger is disconnected
    PMD_CHARGE_DC_OFF,
    /// Charger is connected
    PMD_CHARGE_DC_ON,
    /// Charger called by funtion timer
    PMD_CHARGE_TIMER,
    /// Charger called by pmd_SetChargeCurrent (manual on/off of charge)
    PMD_CHARGE_CURRENT,
    
    PMD_CHARGE_EVENT_QTY
} PMD_CHARGE_EVENT_T;

// ============================================================================
// PMD_CHARGING_STAGE_T
// ----------------------------------------------------------------------------
/// This enum lists the stages during charging phase
// ============================================================================
typedef enum
{
    PMD_CHARGING_STAGE_OFF,
    PMD_CHARGING_STAGE_MEAS_DELAY,
    PMD_CHARGING_STAGE_MEAS,
    PMD_CHARGING_STAGE_PULSE,
    PMD_CHARGING_STAGE_ON,
} PMD_CHARGING_STAGE_T;

// ============================================================================
// PMD_EFFUSE_PAGE_T
// ----------------------------------------------------------------------------
/// This enum lists the effuse pages
// ============================================================================
typedef enum
{
    PMD_EFFUSE_PAGE_CHARGER = 0,
    PMD_EFFUSE_PAGE_VBAT_3P4 = 1,
    PMD_EFFUSE_PAGE_VBAT_4P2 = 2,
} PMD_EFFUSE_PAGE_T;

// ============================================================================
// PMD_CHARGER_VOLTAGE_VALIDITY_T
// ----------------------------------------------------------------------------
/// This enum lists the validity status
// ============================================================================
typedef enum
{
    PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN,
    PMD_CHARGER_VOLTAGE_VALIDITY_VALID,
    PMD_CHARGER_VOLTAGE_VALIDITY_INVALID,
} PMD_CHARGER_VOLTAGE_VALIDITY_T;

// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// Whether charger is plugged-in when powering on phone
PRIVATE BOOL g_pmdChargerPluginAtPowerOn = FALSE;

PROTECTED PMD_CHARGER_STATE_T g_pmdChargerState = 
    {
        .status = PMD_CHARGER_UNKNOWN,
        .current = PMD_CHARGER_100MA,
        .handler = NULL,
        .batteryLevel = PMD_BAD_VALUE,
        .highActivityState = FALSE,
    };

// RDA_ADDR_CHARGER_SETTING1 = 0x12
PRIVATE UINT16 g_pmdChargerSetting1 = 
    RDA_PMU_CHR_AUXADC_SEL(0)|
    RDA_PMU_UV_SEL(2)|
    RDA_PMU_IREF_CTL_CHARGER(3)|
    RDA_PMU_BG_CAL_TC_BIT1(26);

// RDA_ADDR_CHARGER_SETTING2 = 0x13
PRIVATE UINT16 g_pmdChargerSetting2 = 
    RDA_PMU_CHR_CC_IBIT_REG(6)|
    RDA_PMU_CHR_PRE_IBIT_REG(7)|
    RDA_PMU_CHR_VFB_SEL_REG(8);

// RDA_ADDR_CHARGER_CONTROL = 0x15
PRIVATE UINT16 g_pmdChargerControl = 
    RDA_PMU_CHR_EN_FREQ(1)|
    RDA_PMU_CHR_EN_DUTY_CYCLE(1);

// RDA_ADDR_CALIBRATION_SETTING1 = 0x16
PRIVATE UINT16 g_pmdCalibSetting1 = 
    RDA_PMU_BGAP_CAL_POLARITY|
    RDA_PMU_BGAP_CAL_BIT_REG(32);

// RDA_ADDR_THERMAL_CALIBRATION = 0x36
PRIVATE UINT16 g_pmdThermalCalib = 
    RDA_PMU_LP_MODE_BG_LP|
    RDA_PMU_CHR_CALMODE_EN_NONCAL|
    RDA_PMU_CALMODE_EN_BG_CAL|
    RDA_PMU_THERMO_SEL(3)|
    RDA_PMU_PU_THERMO_PROTECT|
    RDA_PMU_PD_VD_LDO;

// BG LP mode registry
PRIVATE BOOL g_pmdBgLpModeRegistry[PMD_BG_LP_MODE_USER_ID_QTY];
// Whether BG LP mode is enabled
PRIVATE BOOL g_pmdBgLpModeEn = TRUE;

// Whether the charger voltage monitor is enabled
PRIVATE BOOL g_pmdChargerChrVoltMonEnabled = FALSE;
// Previous charger voltage status
PRIVATE PMD_CHARGER_VOLTAGE_VALIDITY_T g_pmdChargerPrevChrVoltValidity = PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
// Charger voltage array
PRIVATE UINT16 g_pmdChrVoltAry[PMD_COUNT_CHG_CHR_VOLT_STABLE];
// Charger voltage array index
PRIVATE UINT16 g_pmdChrVoltIdx = 0;

// Battery voltage array
PRIVATE UINT16 g_pmdBattVoltAry[PMD_COUNT_CHG_STATUS_STABLE];
// Battery voltage array index
PRIVATE UINT16 g_pmdBattVoltIdx = 0;
// Battery measurement Failure counts
PRIVATE UINT16 g_pmdBattMeasFails = 0;
// Charger charging stage
PRIVATE PMD_CHARGING_STAGE_T g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
// Charger charging duration between the measurement operations
PRIVATE UINT32 g_pmdChargerOnInterval = 0;
// Charger pulse charging period (one stop every x runs)
PRIVATE UINT32 g_pmdChargerPulsePeriod = 6;
// Whether it is the first time to check the battery voltage
PRIVATE BOOL g_pmdChargerFirstVoltageCheck = FALSE;
// Whether the charge measurement threshold is reached
PRIVATE BOOL g_pmdChargerMeasThreshReachedFlag = FALSE;
// The time when the measurement threshold is first reached
PRIVATE UINT32 g_pmdChargerMeasThreshReachedTime = 0;
// The start time of the measurement delay
PRIVATE UINT32 g_pmdChargerMeasDelayStartTime = 0;
// Charger retuning is needed if phone is power on at low voltage, and is needed once only

// ============================================================================
//  PRIVATE FUNCTIONS PROTOTYPING
// ============================================================================


// ============================================================================
//  FUNCTIONS
// ============================================================================

// ============================================================================
// pmd_SetHighActivity 
// ----------------------------------------------------------------------------
/// Set the high activity state for battery measurement compensation.
///
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_SetHighActivity(BOOL on)
{
    g_pmdChargerState.highActivityState = on;
}

// ============================================================================
// pmd_GetGpadcChargerLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the charger level 
///
/// @return the charger level in mV or 
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetGpadcChargerLevel(VOID)
{
    UINT16 readBat = hal_AnaGpadcGet(g_pmdConfig->chargerGpadcChannel);
    if (HAL_ANA_GPADC_BAD_VALUE == readBat)
    {
        return PMD_BAD_VALUE;
    }
    else
    {
        UINT16 factor = 6;
        UINT32 chipId = rfd_XcvGetChipId();
        if (chipId == XCV_CHIP_ID_8808_V2)
        {
            factor = 4;
        }
        return (factor*readBat);
    }
}

// ============================================================================
// pmd_CheckChargerVoltageValidity
// ----------------------------------------------------------------------------
/// Check the status of the charger voltage.
///
/// @return PMD_CHARGER_VOLTAGE_VALIDITY_T.
// ============================================================================
PRIVATE PMD_CHARGER_VOLTAGE_VALIDITY_T pmd_CheckChargerVoltageValidity(VOID)
{
    // Get current charger voltage
    UINT16 curChargerVolt = pmd_GetGpadcChargerLevel();
    if (curChargerVolt != PMD_BAD_VALUE)
    {
        g_pmdChrVoltAry[g_pmdChrVoltIdx++%PMD_COUNT_CHG_CHR_VOLT_STABLE] = curChargerVolt;
        if (g_pmdChrVoltIdx == 0) // handle wrap-around issue
        {
            g_pmdChrVoltIdx = PMD_COUNT_CHG_CHR_VOLT_STABLE;
        }

        if (g_pmdChrVoltIdx >= PMD_COUNT_CHG_CHR_VOLT_STABLE)
        {
            UINT32 meanVolt = 0;
            for (int i=0; i<PMD_COUNT_CHG_CHR_VOLT_STABLE; i++)
            {
                meanVolt += g_pmdChrVoltAry[i];
            }
            meanVolt /= PMD_COUNT_CHG_CHR_VOLT_STABLE;

            if (meanVolt+PMD_MV_OFFSET_CHG_CHR_VOLT >= g_pmdConfig->chargerLevelUpperLimit)
            {
                return PMD_CHARGER_VOLTAGE_VALIDITY_INVALID;
            }
            else
            {
                return PMD_CHARGER_VOLTAGE_VALIDITY_VALID;
            }
        }
    }
    // Current status unknown yet
    return PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
}

// ============================================================================
// pmd_GetGpadcBatteryLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery level 
///
/// @return the battery level in mV or 
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetGpadcBatteryLevel(VOID)
{
    //PMD_TRACE(PMD_INFO_TRC, 0, "pmd: highActivityState %d", g_pmdChargerState.highActivityState);

    // Starting from Gallite, battery measurement is stable at the time the system gets out from LP mode.
    // That is because, LDO power supply of GPADC is controlled by h/w LP state machine, and
    // there are 2+ ms between applying LDO power and LP power up done.

    // VBAT_VOLT Stable: measure it
    UINT16 readBat = hal_AnaGpadcGet(g_pmdConfig->batteryGpadcChannel);
    if (HAL_ANA_GPADC_BAD_VALUE == readBat)
    {
        return PMD_BAD_VALUE;
    }
    else
    {
        // Battery voltage divider is 2 : 1
        if ( g_pmdChargerState.highActivityState &&
             g_pmdChargingStage != PMD_CHARGING_STAGE_ON &&
             g_pmdChargingStage != PMD_CHARGING_STAGE_PULSE )
        {
            return (3*readBat) + g_pmdConfig->batteryOffsetHighActivityMV;
        }
        else
        {
            return (3*readBat);
        }
    }
}

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
PUBLIC UINT16 pmd_GetBatteryLevel(UINT8 * pPercent)
{
    UINT16 batLevel;
    // get battery level from the charger during charge,
    // else direct read the GPADC
    // this ensure the level is not changing high and low during pulsed charge.
    // also report only 95% of actual value during fast charge as the charge is
    // fully active (this avoid going to 100% and back at start of pulse charge)
    g_pmdChargerState.batteryLevel = pmd_GetGpadcBatteryLevel();
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_GetBatteryLevel  %dmV", g_pmdChargerState.batteryLevel);
    switch(g_pmdChargerState.status)
    {
        case PMD_CHARGER_FAST_CHARGE:
            if ( ( g_pmdChargingStage == PMD_CHARGING_STAGE_ON ||
                   g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE ) &&
                 (g_pmdChargerState.batteryLevel != PMD_BAD_VALUE) )
            {
                batLevel = ((g_pmdChargerState.batteryLevel - g_pmdConfig->powerDownVoltageMV) * 95 / 100) +
                            g_pmdConfig->powerDownVoltageMV;
            }
            else
            {
                batLevel = g_pmdChargerState.batteryLevel;
            }
            break;
        case PMD_CHARGER_PULSED_CHARGE:
        default:
            batLevel = g_pmdChargerState.batteryLevel;
            break;
    }

    if ((batLevel != PMD_BAD_VALUE) && (pPercent != NULL))
    {
        *pPercent = pmd_BatteryLevelToPercent(batLevel);
    }
    
    return batLevel;
}

// ============================================================================
// pmd_BatteryLevelToPercent
// ----------------------------------------------------------------------------
/// Return the \p batteryLevel converted into Percent
///
/// @param batteryLevel the battery level in mV
/// @return a precentage of the given battery level
// ============================================================================
PUBLIC UINT8 pmd_BatteryLevelToPercent(UINT16 batteryLevel)
{
    INT32 percent;
    // build a % value
    percent = 100 * (batteryLevel 
                        - g_pmdConfig->powerDownVoltageMV)
              / (g_pmdConfig->batteryLevelFullMV 
                        - g_pmdConfig->powerDownVoltageMV);
    if (percent <= 0)
    {
        return 0;
    }
    else if (percent >= 100)
    {
        return 100;
    }
    else
    {
        return percent;
    }
}



// ============================================================================
// pmd_GetBatteryTemperature
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery temperature
///
/// @return the battery temperature in °C or 
/// #PMD_BAD_VALUE if no result is available yet.
/// @todo implement
// ============================================================================
PUBLIC UINT16 pmd_GetBatteryTemperature(VOID)
{
    return PMD_BAD_VALUE;
}


// ============================================================================
// pmd_GetChargerStatus
// ----------------------------------------------------------------------------
/// Return the charger status
///
/// @return the charger status
// ============================================================================
PUBLIC PMD_CHARGER_STATUS_T pmd_GetChargerStatus(VOID)
{
    return g_pmdChargerState.status;
}


// ============================================================================
// pmd_SetChargerStatusHandler
// ----------------------------------------------------------------------------
/// Set the handler used to detect charger status (plugged, charge ...)
// ============================================================================
PUBLIC VOID pmd_SetChargerStatusHandler(PMD_CHARGER_HANDLER_T handler)
{
    g_pmdChargerState.handler = handler;

    // Avoid reentrance of pmd_DcOnHandler/pmd_ChargerManager
    // coming from PMU/TIMER IRQ
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (g_pmdChargerPluginAtPowerOn)
    {
        g_pmdChargerPluginAtPowerOn = FALSE;
        pmd_DcOnHandler(TRUE);
    }
    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
// pmd_SetChargeCurrent
// ----------------------------------------------------------------------------
/// Set the current for the charge.
// ============================================================================
PROTECTED VOID pmd_SetChargeCurrent(PMD_CHARGE_CURRENT_T current)
{
    UINT32 regVal = 0;
    // should get bandgap from global/calib
    switch (current)
    {
        case PMD_CHARGER_50MA:
            regVal = RDA_PMU_CHR_CC_IBIT_50MA;
            break;
        case PMD_CHARGER_100MA:
            regVal = RDA_PMU_CHR_CC_IBIT_100MA;
            break;
        case PMD_CHARGER_150MA:
            regVal = RDA_PMU_CHR_CC_IBIT_150MA;
            break;
        case PMD_CHARGER_200MA:
            regVal = RDA_PMU_CHR_CC_IBIT_150MA;
            break;
        case PMD_CHARGER_300MA:
            regVal = RDA_PMU_CHR_CC_IBIT_250MA;
            break;
        case PMD_CHARGER_400MA:
            regVal = RDA_PMU_CHR_CC_IBIT_350MA;
            break;
        case PMD_CHARGER_500MA:
            regVal = RDA_PMU_CHR_CC_IBIT_500MA;
            break;
        case PMD_CHARGER_600MA:
            regVal = RDA_PMU_CHR_CC_IBIT_650MA;
            break;
        case PMD_CHARGER_700MA:
            regVal = RDA_PMU_CHR_CC_IBIT_650MA;
            break;
        case PMD_CHARGER_800MA:
            regVal = RDA_PMU_CHR_CC_IBIT_650MA;
            break;
        default:
            PMD_ASSERT(FALSE, "Unsupported charge current: %d", current);
            break;
    }
    g_pmdChargerSetting2 = ( g_pmdChargerSetting2 &
                             ~RDA_PMU_CHR_CC_IBIT_REG_MASK ) |
                           regVal |
                           RDA_PMU_CHR_CC_IBIT_DR;
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
    g_pmdChargerState.current = current;
}

PROTECTED VOID pmd_TuneCharger(VOID)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_TuneCharger");

    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdCalibSetting1 = (g_pmdCalibSetting1&~RDA_PMU_RESETN_BGAP_CAL_REG) |
                  RDA_PMU_RESETN_BGAP_CAL_DR;
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);

    g_pmdCalibSetting1 &= ~(RDA_PMU_RESETN_BGAP_CAL_DR|RDA_PMU_RESETN_BGAP_CAL_REG);
    // Disable bandgap calib, for it might cause LDO output burr
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);
    hal_SysExitCriticalSection(scStatus);
    hal_TimDelay(2 MS_WAITING);
}

PRIVATE VOID pmd_ForceChargerFinished(BOOL stop)
{
    if (stop)
    {
        // Direct-reg to disable charging
        g_pmdChargerControl &= ~RDA_PMU_CHR_ENABLE_REG;
        g_pmdChargerControl |= RDA_PMU_CHR_ENABLE_DR;
    }
    else
    {
        // Direct-reg to enable charging
        // 1) Bypass pulse charging on 8808 (no pulse charging since 8808S)
        // 2) Bypass PMU charge timeout control (about 8 hours)
        g_pmdChargerControl |= (RDA_PMU_CHR_ENABLE_DR|
                                RDA_PMU_CHR_ENABLE_REG);
    }
    pmd_RDAWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);
}

PRIVATE VOID pmd_RestoreChargerControl(VOID)
{
    // Not to direct-reg charging
    g_pmdChargerControl &= ~(RDA_PMU_CHR_ENABLE_DR|
                             RDA_PMU_CHR_ENABLE_REG);
    pmd_RDAWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);
}

PROTECTED RDA_PMU_CHR_STATUS_T pmd_GetChargerHwStatus(VOID)
{
    RDA_PMU_CHR_STATUS_T status = CHR_STATUS_UNKNOWN;

    UINT16 ret;
    if (FALSE == pmd_SpiCsNonblockingActivate(FALSE))
    {
        return status;
    }

    ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    //PMD_TRACE(PMD_DBG_TRC, 0, "charger: charge stautus  0x%x",ret);

    if ((ret & RDA_PMU_CHR_AC_ON) == 0)
    { 
        //PMD_TRACE(PMD_DBG_TRC, 0, "charger: adapter off");
        status = CHR_STATUS_AC_OFF;
        pmd_RegIrqSettingClr(RDA_PMU_INT_CHR_MASK);
    }
    else
    {
        status = CHR_STATUS_AC_ON;
    }

    pmd_OpalCsDeactivate(FALSE);
    return status;
}

// ============================================================================
//  pmd_ChargerSetBgLpMode
// ============================================================================
PROTECTED VOID pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_ID_T user, BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdBgLpModeRegistry[user] = on;

    if (on)
    {
        // Enable BG LP mode only when all the users request so.
        for (UINT32 i=0; i<PMD_BG_LP_MODE_USER_ID_QTY; i++)
        {
            if (!g_pmdBgLpModeRegistry[i])
            {
                on = FALSE;
                break;
            }
        }
    }

    if (g_pmdBgLpModeEn != on)
    {
        g_pmdBgLpModeEn = on;

        if (on)
        {
            g_pmdThermalCalib |= RDA_PMU_LP_MODE_BG_LP;
        }
        else
        {
            g_pmdThermalCalib &= ~RDA_PMU_LP_MODE_BG_LP;
        }
        pmd_RDAWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
    }

    hal_SysExitCriticalSection(scStatus);
}

PROTECTED PMD_CHARGER_STATUS_T pmd_ChargerChargingState(RDA_PMU_CHR_STATUS_T status)
{
    // Taper and term status are not checked any more, as these h/w signals are
    // inaccurate on some 8805 chipsets, and the case is even worse for 8806

    int i;
    PMD_CHARGER_STATUS_T nextState = g_pmdChargerState.status;
    UINT32 meanBattVolt = 0;
    BOOL needMeas = FALSE;
    UINT32 curTime = hal_TimGetUpTime();

    if (g_pmdChargingStage == PMD_CHARGING_STAGE_MEAS_DELAY)
    {
        if (curTime - g_pmdChargerMeasDelayStartTime >= PMD_INTERVAL_CHG_MEAS_STABLE)
        {
            g_pmdBattVoltIdx = 0;
            g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS;
        }

        return nextState;
    }

    if (g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE)
    {
        if (g_pmdChargerPulsePeriod < 2)
        {
            // At least 50% charging-active ratio
            g_pmdChargerPulsePeriod = 2;
        }

        if ((g_pmdBattVoltIdx+g_pmdBattMeasFails)%g_pmdChargerPulsePeriod == 0)
        {
            // Force to disable charging
            pmd_ForceChargerFinished(TRUE);
        }
        else
        {
            // Enable charging (h/w thresholds take effect)
            pmd_ForceChargerFinished(FALSE);
        }
    }

    if (g_pmdBattVoltIdx < PMD_COUNT_CHG_STATUS_STABLE)
    {
        return nextState;
    }

    // Get the mean battery voltage
    for (i=0; i<PMD_COUNT_CHG_STATUS_STABLE; i++)
    {
        meanBattVolt += g_pmdBattVoltAry[i];
    }
    meanBattVolt /= PMD_COUNT_CHG_STATUS_STABLE;

    switch (g_pmdChargingStage)
    {
    case PMD_CHARGING_STAGE_OFF:
        if (meanBattVolt+PMD_MV_OFFSET_CHG_RECHARGE >= g_pmdConfig->batteryLevelFullMV)
        {
            break;
        }
        // Otherwise going through measurement stage
        g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS;
    case PMD_CHARGING_STAGE_MEAS:
        // Reset voltage array index
        g_pmdBattVoltIdx = 0;
        // Check the mean voltage
        if (meanBattVolt+PMD_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmdConfig->batteryLevelFullMV)
        {
            // Compensate the measurement error (due to stable time, etc)
            // to make sure the battery is charged full
            g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
            // Set next state to full
            nextState = PMD_CHARGER_FULL_CHARGE;
        }
        else // charge is needed
        {
            if (meanBattVolt+PMD_MV_OFFSET_CHG_PHASE_3 >= g_pmdConfig->batteryLevelFullMV)
            {
                g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_SHORT;
#if 0
                // Pulsed charging with a relatively small average current
                g_pmdChargerPulsePeriod = 6;
                g_pmdChargingStage = PMD_CHARGING_STAGE_PULSE;
#else
                g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
#endif
                // Lower the charger current to 150mA
                pmd_SetChargeCurrent(PMD_CHARGER_150MA);
            }
            else if (meanBattVolt+PMD_MV_OFFSET_CHG_PHASE_2 >= g_pmdConfig->batteryLevelFullMV)
            {
                g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_MEDIUM;
#if 0
                // Pulsed charging with a relatively large average current
                g_pmdChargerPulsePeriod = 10;
                g_pmdChargingStage = PMD_CHARGING_STAGE_PULSE;
#else
                g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
#endif
                // Lower the charger current to 300mA
                pmd_SetChargeCurrent(PMD_CHARGER_300MA);
            }
            else
            {
                // Fast charging
                g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_LONG;
                g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
                // Set the charger current
                pmd_SetChargeCurrent(g_pmdConfig->batteryChargeCurrent);
            }

            // Reset the measurement threshold reached flag
            g_pmdChargerMeasThreshReachedFlag = FALSE;
            // Enable charging (h/w thresholds take effect)
            pmd_ForceChargerFinished(FALSE);
            // Set next state to fast but keep full state
            if (nextState != PMD_CHARGER_FULL_CHARGE)
            {
                nextState = PMD_CHARGER_FAST_CHARGE;
            }
        }
        break;
    case PMD_CHARGING_STAGE_PULSE:
    case PMD_CHARGING_STAGE_ON:
        if (g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE)
        {
            if (meanBattVolt+PMD_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmdConfig->batteryLevelFullMV)
            {
                needMeas = TRUE;
            }
        }
        else if (g_pmdChargingStage == PMD_CHARGING_STAGE_ON)
        {
            if (meanBattVolt+PMD_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmdConfig->batteryLevelFullMV)
            {
                needMeas = TRUE;
            }
        }

        if(needMeas)
        {
            if (!g_pmdChargerMeasThreshReachedFlag)
            {
                // Save the measurement threshold reached time
                g_pmdChargerMeasThreshReachedTime = curTime;
                g_pmdChargerMeasThreshReachedFlag = TRUE;
            }

            if (g_pmdChargerFirstVoltageCheck ||
                curTime - g_pmdChargerMeasThreshReachedTime >= g_pmdChargerOnInterval)
            {
                g_pmdBattVoltIdx = 0;
                g_pmdChargerMeasDelayStartTime = curTime;
                g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS_DELAY;
                // Force to disable charging
                pmd_ForceChargerFinished(TRUE);
            }
        }

        g_pmdChargerFirstVoltageCheck = FALSE;
        break;
    default:
        break;
    }

    // For bad or old battery
    if (curTime - g_pmdChargerState.startTime > g_pmdConfig->batteryChargeTimeout)
    {
        nextState = PMD_CHARGER_FULL_CHARGE;
    }

    return nextState;
}

// ============================================================================
// pmd_ChargerManager
// ----------------------------------------------------------------------------
/// The charger manager is only called on timer interrupt this way there is 
/// no risk of reentrance and so no critical section is needed.
///
// note: sxr_StopFunctionTimer(pmd_ChargerManager); not needed in this function
// as it is allways called by an expired timer.
// ============================================================================
PROTECTED VOID pmd_ChargerManager(PMD_CHARGE_EVENT_T event)
{
    PMD_CHARGER_STATUS_T nextState = g_pmdChargerState.status;

    if (event == PMD_CHARGE_DC_OFF)
    {
        // charger DC off will be handled by timer event
        return;
    }
    else if (event == PMD_CHARGE_DC_ON)
    {
        g_pmdChargerState.startTime = hal_TimGetUpTime();
        g_pmdBattMeasFails = 0;
        g_pmdBattVoltIdx = 0;
        // First check the battery voltage while charger is active
        g_pmdChargerFirstVoltageCheck = TRUE;
        g_pmdChargerMeasThreshReachedFlag = FALSE;
        g_pmdChargingStage = PMD_CHARGING_STAGE_ON;

        // Start charger voltage monitor
        g_pmdChrVoltIdx = 0;
        g_pmdChargerPrevChrVoltValidity = PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
        if (g_pmdChargerChrVoltMonEnabled)
        {
            hal_AnaGpadcOpen(g_pmdConfig->chargerGpadcChannel, HAL_ANA_GPADC_ATP_2S);
        }
        // Set charge current to lowest to avoid the damage of P-channel MOSFET
        // when the charger voltage is too high
        pmd_SetChargeCurrent(PMD_CHARGER_50MA);

        // Disable bandgap LP mode to allow charging in LP
        pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_CHARGER, FALSE);
        // Enable charging (h/w thresholds take effect)
        pmd_ForceChargerFinished(FALSE);
        // Start a timer to read the acutal charger status in non-blocking mode
        // (PMD_TIME_CHG_MON_INTERVAL>>4) is the debounce time
        sxr_StopFunctionTimer(pmd_ChargerManager);
        sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL>>4, pmd_ChargerManager, PMD_CHARGE_TIMER, 0);
    }
    else if (event == PMD_CHARGE_TIMER)
    {
        // Get current battery voltage
        UINT16 curBattVolt = pmd_GetGpadcBatteryLevel();

        if (curBattVolt == PMD_BAD_VALUE)
        {
            g_pmdBattMeasFails++;
        }
        else
        {
            g_pmdBattVoltAry[g_pmdBattVoltIdx++%PMD_COUNT_CHG_STATUS_STABLE] = curBattVolt;
            if (g_pmdBattVoltIdx == 0) // handle wrap-around issue
            {
                g_pmdBattVoltIdx = PMD_COUNT_CHG_STATUS_STABLE;

        // TODO: use the mean value or the first measurement value?
            }
        }

        RDA_PMU_CHR_STATUS_T status = pmd_GetChargerHwStatus();
        if (status == CHR_STATUS_AC_OFF)
        {
            nextState = PMD_CHARGER_UNPLUGGED;
            g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;

            // Stop charger voltage monitor
            if (g_pmdChargerChrVoltMonEnabled)
            {
                hal_AnaGpadcClose(g_pmdConfig->chargerGpadcChannel);
            }
            // Set charge current to lowest to avoid the damage of P-channel MOSFET
            // when the charger voltage is too high
            pmd_SetChargeCurrent(PMD_CHARGER_50MA);

            // Restore the default charging config (charging enabled)
            pmd_ForceChargerFinished(FALSE);
            // Enable bandgap LP mode to save power
            pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_CHARGER, TRUE);

            sxr_StopFunctionTimer(pmd_ChargerManager);
        }
        else // Charger is unknown or plugged in, and the timer should be alive
        {
            if (status == CHR_STATUS_UNKNOWN)
            {
                // Do nothing but wait for the next monitor interval to check charger h/w status
            }
            else // Charger is plugged in
            {
                if (nextState == PMD_CHARGER_UNKNOWN ||
                    nextState == PMD_CHARGER_UNPLUGGED)
                {
                    // Upper layer thinks there is no charger at this time
                    // Notify upper layer that charger is just plugged in
                    nextState = PMD_CHARGER_PLUGGED;
                }
                else
                {
                    // Upper layer has known that charger is plugged in

                    PMD_CHARGER_VOLTAGE_VALIDITY_T validity = PMD_CHARGER_VOLTAGE_VALIDITY_VALID;
                    if (g_pmdChargerChrVoltMonEnabled)
                    {
                        validity = pmd_CheckChargerVoltageValidity();
                        // Check charger voltage
                        if (validity == PMD_CHARGER_VOLTAGE_VALIDITY_VALID)
                        {
                            if (g_pmdChargerPrevChrVoltValidity != validity)
                            {
                                g_pmdChargerPrevChrVoltValidity = validity;
                                // pmd_ChargerChargingState will take care of everything.
                                // Current stage is measurement-delay if the charger voltage was invalid,
                                // or, current stage is charging-on if it was unknown.
                            }
                        }
                        else if (validity == PMD_CHARGER_VOLTAGE_VALIDITY_INVALID)
                        {
                            if (g_pmdChargerPrevChrVoltValidity != validity)
                            {
                                g_pmdChargerPrevChrVoltValidity = validity;
                                // Set to measurement-delay stage
                                g_pmdBattVoltIdx = 0;
                                g_pmdChargerMeasDelayStartTime = hal_TimGetUpTime();
                                g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS_DELAY;
                                // Notify upper layer that charger voltage is too high
                                nextState = PMD_CHARGER_VOLTAGE_ERROR;
                                // Force to disable charging
                                pmd_ForceChargerFinished(TRUE);
                            }
                        }
                        else // PMD_CHARGER_VOLTAGE_VALIDITY_UNKNOWN
                        {
                        }
                    }

                    if (validity == PMD_CHARGER_VOLTAGE_VALIDITY_VALID)
                    {
                        if (nextState == PMD_CHARGER_PLUGGED)
                        {
                            // Restore the charger current.
                            // These codes are executed only once each time the charger is plugged in.
                            // (The state will be changed from PLUGGED to FAST_CHARGE or something else below)

                            // Set the charger current to user-configured max value
                            pmd_SetChargeCurrent(g_pmdConfig->batteryChargeCurrent);
                            // Re-calculate the battery mean volatge as the charger current is restored
                            g_pmdBattVoltIdx = 0;
                        }

                        // Report charging progress to upper layer
                        nextState = pmd_ChargerChargingState(status);
                        if (nextState == PMD_CHARGER_PLUGGED)
                        {
                            nextState = PMD_CHARGER_FAST_CHARGE;
                        }
        			    else if(nextState == PMD_CHARGER_FULL_CHARGE)
    			    	{
    			    		PM_SetBattery(100);
    			    	}
                    }
                }
            }

            sxr_StopFunctionTimer(pmd_ChargerManager);
            sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL, pmd_ChargerManager, PMD_CHARGE_TIMER, 0);
        }
    }

    PMD_TRACE(PMD_INFO_TRC, 0, "charger: nextState=%d ,oldstatus=%d, pulseCnt=%d ",
                        nextState, g_pmdChargerState.status, g_pmdChargerState.pulsedOnCount);

    if (nextState != g_pmdChargerState.status)
    {
        g_pmdChargerState.status = nextState;
        if (g_pmdChargerState.handler != NULL)
        {
            g_pmdChargerState.handler(nextState);
        }
    }
}

// ============================================================================
// pmd_DcOnHandler
// ----------------------------------------------------------------------------
/// Registerred handler for GPIO_0 used as DC-ON Detect.
/// 
// ============================================================================
PUBLIC VOID pmd_DcOnHandler(BOOL on)
{
    PMD_TRACE(PMD_WARN_TRC, 0, "charger: DC-ON : %d",on);
    if (on == FALSE)
    {
        // wait for DC-ON rising edge
        // tell the charger manager that there is no charger
        pmd_ChargerManager(PMD_CHARGE_DC_OFF);
        PMD_TRACE(PMD_WARN_TRC, 0, "charger: DC-ON : here1 ");
    }
    else
    {
        // wait for DC-ON falling edge
        // tell the charger manager that the charger is plugged
        pmd_ChargerManager(PMD_CHARGE_DC_ON);
        PMD_TRACE(PMD_WARN_TRC, 0, "charger: DC-ON : plug in  ");
        bal_SetSleepState(FALSE); // wake up bal task
    }
}

// ============================================================================
// pmd_GetEffuse
// ----------------------------------------------------------------------------
/// Read effuse value of the given page.
/// 
// ============================================================================
PRIVATE UINT16 pmd_GetEffuseValue(PMD_EFFUSE_PAGE_T page)
{
    UINT16 value;
    value = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value = PMU_SET_BITFIELD(value, RDA_PMU_EFUSE_SEL_WORD_REG, page);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);

    value |= (RDA_PMU_EFUSE_SENSE_DR|RDA_PMU_EFUSE_SENSE_REG);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);
    hal_TimDelay(2 MS_WAITING);
    value &= ~(RDA_PMU_EFUSE_SENSE_DR|RDA_PMU_EFUSE_SENSE_REG);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);

    value = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OUT);

#if (PMD_FIX_EFFUSE_VALUES)
    if (page == PMD_EFFUSE_PAGE_VBAT_3P4 || page == PMD_EFFUSE_PAGE_VBAT_4P2)
    {
        value &= 0x3ff;
    }
    else
#endif
    {
        value = ( value |
                  ( (((value&0x3c00)>>10) == 0) ? 0 :
                        (1<<(((value&0x3c00)>>10)-1))
                  )
                ) & 0x3ff;
    }

    return value;
}


UINT32 pmd_GetEncryptUid()
{
	UINT32 val =(pmd_GetEffuseValue(PMD_EFFUSE_PAGE_VBAT_4P2)&0x3FF)<<20;	
	val +=(pmd_GetEffuseValue(PMD_EFFUSE_PAGE_VBAT_3P4)&0x3FF)<<10;
	val +=(pmd_GetEffuseValue(PMD_EFFUSE_PAGE_CHARGER)&0x3FF);

	return val;
}

UINT32 pmd_GetEncryptUid2()
{
	return 0x58505850;
}

#if (PMD_FIX_EFFUSE_VALUES)
PRIVATE VOID pmd_GetEffuseDeltaRoundValues(UINT32 delta, UINT32 *pRoundUp,
                                           UINT32 *pRoundDown)
{
    UINT32 i = 0;
    while(delta > 0)
    {
        delta >>= 1;
        i++;
    }
    *pRoundUp = 1<<i;
    if (i > 0)
        *pRoundDown = 1<<(i-1);
    else
        *pRoundDown = *pRoundUp;
}

PRIVATE VOID pmd_FixEffuseValues(UINT32 oldHigh, UINT32 oldLow,
                                 UINT32 *pNewHigh, UINT32 *pNewLow)
{
    UINT32 high, low;

    high = oldHigh;
    low = oldLow;

    hal_HstSendEvent(SYS_EVENT,0xeff00000|high);
    hal_HstSendEvent(SYS_EVENT,0xeff10000|low);

#define MEAN_GPADC_CALIB_HIGH (777)
#define MEAN_GPADC_CALIB_LOW (625)
#define MEAN_GPADC_CALIB_DIFF (152)

#define MAX_GPADC_VALUE_DEVIATION (20)
// valueDeviation/calibValue ~= diffDeviation/diff
#define MAX_GPADC_DIFF_DEVIATION (10)
#define ERROR_DIFF_DEVIATION (10)

#define GPADC_HIGH_VALID(value) \
    (value < MEAN_GPADC_CALIB_HIGH + MAX_GPADC_VALUE_DEVIATION && \
     value > MEAN_GPADC_CALIB_HIGH - MAX_GPADC_VALUE_DEVIATION)

#define GPADC_LOW_VALID(value) \
    (value < MEAN_GPADC_CALIB_LOW + MAX_GPADC_VALUE_DEVIATION && \
     value > MEAN_GPADC_CALIB_LOW - MAX_GPADC_VALUE_DEVIATION)

#define GPADC_DIFF_VALID(value) \
    (value < MEAN_GPADC_CALIB_DIFF + MAX_GPADC_DIFF_DEVIATION && \
     value > MEAN_GPADC_CALIB_DIFF - MAX_GPADC_DIFF_DEVIATION)

    BOOL highValid = FALSE;
    BOOL lowValid = FALSE;
    BOOL bad = FALSE;
    UINT32 delta, deltaHigh1, deltaHigh2, deltaLow1, deltaLow2;
    UINT32 diff;

    if (high >=  MEAN_GPADC_CALIB_HIGH + MAX_GPADC_VALUE_DEVIATION ||
        low >= MEAN_GPADC_CALIB_LOW + MAX_GPADC_VALUE_DEVIATION)
    {
        bad = TRUE;
    }
    else
    {
        if (high > MEAN_GPADC_CALIB_HIGH - MAX_GPADC_VALUE_DEVIATION)
        {
            highValid = TRUE;
        }

        if (low > MEAN_GPADC_CALIB_LOW - MAX_GPADC_VALUE_DEVIATION)
        {
            lowValid = TRUE;
        }

        if (!highValid && lowValid)
        {
            delta = MEAN_GPADC_CALIB_HIGH - high;
            pmd_GetEffuseDeltaRoundValues(delta, &deltaHigh1, &deltaHigh2);

            if ((high & deltaHigh1) == 0 &&
                GPADC_HIGH_VALID(high + deltaHigh1) &&
                GPADC_DIFF_VALID(high + deltaHigh1 - low))
            {
                high += deltaHigh1;
            }
            else if ((high & deltaHigh2) == 0 &&
                GPADC_HIGH_VALID(high + deltaHigh2) &&
                GPADC_DIFF_VALID(high + deltaHigh2 - low))
            {
                high += deltaHigh2;
            }
            else
            {
                bad = TRUE;
            }
        }
        else if (highValid && !lowValid)
        {
            delta = MEAN_GPADC_CALIB_LOW - low;
            pmd_GetEffuseDeltaRoundValues(delta, &deltaLow1, &deltaLow2);

            if ((low & deltaLow1) == 0 &&
                GPADC_LOW_VALID(low + deltaLow1) &&
                GPADC_DIFF_VALID(high - low - deltaLow1))
            {
                low += deltaLow1;
            }
            else if ((low & deltaLow2) == 0 &&
                GPADC_LOW_VALID(low + deltaLow2) &&
                GPADC_DIFF_VALID(high - low - deltaLow2))
            {
                low += deltaLow2;
            }
            else
            {
                bad = TRUE;
            }
        }
        else if (!highValid && !lowValid)
        {
            delta = MEAN_GPADC_CALIB_HIGH - high;
            pmd_GetEffuseDeltaRoundValues(delta, &deltaHigh1, &deltaHigh2);

            delta = MEAN_GPADC_CALIB_LOW - low;
            pmd_GetEffuseDeltaRoundValues(delta, &deltaLow1, &deltaLow2);

            BOOL cont = FALSE;
            if ((high & deltaHigh1) == 0 &&
                GPADC_HIGH_VALID(high + deltaHigh1))
            {
                if ((low & deltaLow1) == 0 &&
                    GPADC_LOW_VALID(low + deltaLow1) &&
                    GPADC_DIFF_VALID(high + deltaHigh1 - low - deltaLow1))
                {
                    high += deltaHigh1;
                    low += deltaLow1;
                }
                else if ((low & deltaLow2) == 0 &&
                    GPADC_LOW_VALID(low + deltaLow2) &&
                    GPADC_DIFF_VALID(high + deltaHigh1 - low - deltaLow2))
                {
                    high += deltaHigh1;
                    low += deltaLow2;
                }
                else
                {
                    cont = TRUE;
                }
            }
            else
            {
                cont = TRUE;
            }

            if (cont)
            {
                if ((high & deltaHigh2) == 0 &&
                    GPADC_HIGH_VALID(high + deltaHigh2))
                {
                    if ((low & deltaLow1) == 0 &&
                        GPADC_LOW_VALID(low + deltaLow1) &&
                        GPADC_DIFF_VALID(high + deltaHigh2 - low - deltaLow1))
                    {
                        high += deltaHigh2;
                        low += deltaLow1;
                    }
                    else if ((low & deltaLow2) == 0 &&
                        GPADC_LOW_VALID(low + deltaLow2) &&
                        GPADC_DIFF_VALID(high + deltaHigh2 - low - deltaLow2))
                    {
                        high += deltaHigh2;
                        low += deltaLow2;
                    }
                    else
                    {
                        bad = TRUE;
                    }
                }
                else
                {
                    bad = TRUE;
                }
            }
        }
        else // highValid && lowValid
        {
            diff = high - low;
            if (diff <= MEAN_GPADC_CALIB_DIFF - ERROR_DIFF_DEVIATION)
            {
                delta = low + MEAN_GPADC_CALIB_DIFF - high;
                pmd_GetEffuseDeltaRoundValues(delta, &deltaHigh1, &deltaHigh2);

                if ((high & deltaHigh1) == 0 &&
                    GPADC_HIGH_VALID(high + deltaHigh1) &&
                    GPADC_DIFF_VALID(high + deltaHigh1 - low))
                {
                    high += deltaHigh1;
                }
                else if ((high & deltaHigh2) == 0 &&
                    GPADC_HIGH_VALID(high + deltaHigh2) &&
                    GPADC_DIFF_VALID(high + deltaHigh2 - low))
                {
                    high += deltaHigh2;
                }
                else
                {
                    bad = TRUE;
                }
            }
            else if (diff >= MEAN_GPADC_CALIB_DIFF + ERROR_DIFF_DEVIATION)
            {
                delta = high - MEAN_GPADC_CALIB_DIFF - low;
                pmd_GetEffuseDeltaRoundValues(delta, &deltaLow1, &deltaLow2);

                if ((low & deltaLow1) == 0 &&
                    GPADC_LOW_VALID(low + deltaLow1) &&
                    GPADC_DIFF_VALID(high - low - deltaLow1))
                {
                    low += deltaLow1;
                }
                else if ((low & deltaLow2) == 0 &&
                    GPADC_LOW_VALID(low + deltaLow2) &&
                    GPADC_DIFF_VALID(high - low - deltaLow2))
                {
                    low += deltaLow2;
                }
                else
                {
                    bad = TRUE;
                }
            }
            else
            {
                // The values are consider valid.
            }
        }
    }

    if (bad)
    {
        high = MEAN_GPADC_CALIB_HIGH;
        low = MEAN_GPADC_CALIB_LOW;
    }

    if (oldHigh != high || oldLow != low)
    {
        hal_HstSendEvent(SYS_EVENT,0xeff20000|high);
        hal_HstSendEvent(SYS_EVENT,0xeff30000|low);
    }

    *pNewHigh = high;
    *pNewLow = low;
}
#endif

// ============================================================================
// pmd_GetEffuseGpadc
// ----------------------------------------------------------------------------
/// Read effuse value of GPADC calib sample.
/// 
// ============================================================================
PRIVATE VOID pmd_GetEffuseGpadc(VOID)
{
    UINT32 high, low;
    high = pmd_GetEffuseValue(PMD_EFFUSE_PAGE_VBAT_4P2);
    low = pmd_GetEffuseValue(PMD_EFFUSE_PAGE_VBAT_3P4);

#if (PMD_FIX_EFFUSE_VALUES)
    pmd_FixEffuseValues(high, low, &high, &low);
#endif

    *hal_AnaGpadcGetCalibHigh() = high;
    *hal_AnaGpadcGetCalibLow() = low;
}

PRIVATE VOID pmd_EnableLowBattVoltShutdown(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (on)
    {
        // uvSel->voltage: 0->2.6V, 1->2.7V, 2->2.9V, 3->3.0V
        UINT32 uvSel = 0x3;
        g_pmdChargerSetting1 = PMU_SET_BITFIELD(g_pmdChargerSetting1,
                                                RDA_PMU_UV_SEL,
                                                uvSel);
        pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, g_pmdChargerSetting1);
        // Enable vbat 3.2V detection
        g_pmdCalibSetting1 &= ~RDA_PMU_VBAT_OVER_3P2_BYPASS;
    }
    else
    {
        // Disable Vbat 3.2V check
        g_pmdCalibSetting1 |= RDA_PMU_VBAT_OVER_3P2_BYPASS;
    }
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);
    hal_SysExitCriticalSection(scStatus);
}

PRIVATE VOID pmd_ChargerBatteryDetection(VOID)
{
#ifdef CHARGER_BATTERY_CHECK_BY_CRASH

    // If battery is not in place, the system will crash soon after
    // the charger current is cut off.
    // Since the charging is disabled, the system will lose all the power gradually,
    // and all the h/w setting will be restored to the default values.
    // Then the system will be booted up again, for there is a charger connected.
    // This senario will repeat again and again, and at last the system might
    // enter a stable state with a small current leakage.

    #define CHARGER_BATTERY_CHECKING_TIME (100 MS_WAITING)

    // Disable charging
    pmd_ForceChargerFinished(TRUE);
    // Check if the system will crash during the following wait
    hal_TimDelay(CHARGER_BATTERY_CHECKING_TIME);
    // There is a battery. Enable charing again
    pmd_ForceChargerFinished(FALSE);

#else // !CHARGER_BATTERY_CHECK_BY_CRASH

    // PREREQUISITE: Charger current is no larger than 50 mA.

    // If battery is not in place, the battery voltage will change quickly
    // according to the VFB setting, and its value will equal the charing 
    // cut-off voltage of the VFB.
    // The max VFB cut-off voltage should be larger than 4200 mV, and
    // the delta of cut-off voltage between max VFB and min VFB should
    // be larger than 200 mV.
    // A small charger current can avoid the measurement error introduced
    // by battery internal resistor.
    // When battery is not detected, the system will be continously rebooted.

    #define BATT_DETECT_RETRY_TIME (500 MS_WAITING)
    #define VFB_VOLT_STABLE_TIME (50 MS_WAITING)

    #define MAX_VFB_VOLT_MV 4200
    #define MAX_MIN_VFB_VOLT_DELTA_MV 200

    #define BATT_VOLT_SAMPLE_NUM 500

    UINT32 vfbSetting = g_pmdChargerSetting2;
    UINT32 chr_vfb_sel;
    UINT32 maxVfbVolt, minVfbVolt;
    UINT32 i;
    BOOL batteryInPlace;

    // Set low gain to reduce the fluctuation of the battery voltage
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, 
        (g_pmdChargerSetting1 | RDA_PMU_CHR_CV_LOWGAIN));

    do
    {
        batteryInPlace = TRUE;

        // Max VFB value
        chr_vfb_sel = 0x0;
        vfbSetting = PMU_SET_BITFIELD(vfbSetting,
                                    RDA_PMU_CHR_VFB_SEL_REG,
                                    chr_vfb_sel) |
                    RDA_PMU_CHR_VFB_SEL_DR;
        pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, vfbSetting);
        hal_TimDelay(VFB_VOLT_STABLE_TIME);

        // If battery is not in place, the battery voltage might wave like
        // a sine signal and the period is about 400 us
        maxVfbVolt = 0;
        for (i=0; i<BATT_VOLT_SAMPLE_NUM; i++)
        {
            maxVfbVolt += pmd_GetBatteryLevel(NULL);
        }
        maxVfbVolt /= BATT_VOLT_SAMPLE_NUM;

        if (maxVfbVolt > MAX_VFB_VOLT_MV)
        {
            // Min VFB value
            chr_vfb_sel = 0xe;
            vfbSetting = PMU_SET_BITFIELD(vfbSetting,
                                        RDA_PMU_CHR_VFB_SEL_REG,
                                        chr_vfb_sel) |
                        RDA_PMU_CHR_VFB_SEL_DR;
            pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, vfbSetting);
            hal_TimDelay(VFB_VOLT_STABLE_TIME);

            // If battery is not in place, the battery voltage might wave like
            // a sine signal and the period is about 400 us
            minVfbVolt = 0;
            for (i=0; i<BATT_VOLT_SAMPLE_NUM; i++)
            {
                minVfbVolt += pmd_GetBatteryLevel(NULL);
            }
            minVfbVolt /= BATT_VOLT_SAMPLE_NUM;

            if (maxVfbVolt > minVfbVolt + MAX_MIN_VFB_VOLT_DELTA_MV)
            {
                // Battery is not in place
                batteryInPlace = FALSE;
#ifdef SHUTDOWN_FOR_CHARGER_WITHOUT_BATTERY
                break;
#else
                hal_TimDelay(BATT_DETECT_RETRY_TIME);
#endif
            }
        }
    } while (!batteryInPlace);

    // Restore the original charger setting.
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
    // Restore the original low gain setting
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, g_pmdChargerSetting1);

#ifdef SHUTDOWN_FOR_CHARGER_WITHOUT_BATTERY
    if (!batteryInPlace)
    {
        // Shutdown the system.
        // As there is a charger connected, the system will be booted up soon.
        hal_SysShutdown();
    }
#endif

#endif // !CHARGER_BATTERY_CHECK_BY_CRASH
}

PUBLIC VOID pmd_InitCharger(VOID)
{
    g_pmdMapAccess.chargerState = (PMD_MAP_CHARGER_STATE_T*)&g_pmdChargerState;

    // Init BG LP mode registry (default on)
    for (UINT32 i=0; i<PMD_BG_LP_MODE_USER_ID_QTY; i++)
    {
        g_pmdBgLpModeRegistry[i] = TRUE;
    }
    // Init BG LP mode
    pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);

    // Check whether to enable charger voltage monitoring
    if (g_pmdConfig->chargerGpadcChannel >= HAL_ANA_GPADC_CHAN_0 &&
        g_pmdConfig->chargerGpadcChannel < HAL_ANA_GPADC_CHAN_QTY &&
        g_pmdConfig->chargerLevelUpperLimit != 0)
    {
        g_pmdChargerChrVoltMonEnabled = TRUE;
    }
    // Allow GPADC channel 6 to monitor the charger voltage inside chip
    g_pmdChargerSetting1 = PMU_SET_BITFIELD(g_pmdChargerSetting1, 
                    RDA_PMU_CHR_AUXADC_SEL,
                    1);
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, g_pmdChargerSetting1);

    pmd_GetEffuseGpadc();


#if 1
    // Enable Vbat 3.2V check
    pmd_EnableLowBattVoltShutdown(TRUE);
#else
    // Disable Vbat 3.2V check
    pmd_EnableLowBattVoltShutdown(FALSE);
#endif

    // Set effuse
    UINT32 effuse = pmd_GetEffuseValue(PMD_EFFUSE_PAGE_CHARGER);
    UINT32 chr_pre_ibit = PMU_GET_BITFIELD(effuse, RDA_PMU_CHR_PRE_IBIT_REG);
    UINT32 chr_vfb_sel = PMU_GET_BITFIELD(effuse, RDA_PMU_CHR_VFB_SEL_REG);
#if 0
    // Lower EFFUSE_CHR_VFB_SEL by 2 to increase the hw target charge voltage,
    // while not enlarging the charge current too much.
    // This is safe as sw will stop charging if the battery voltage is over 4.2V.
    chr_vfb_sel = (chr_vfb_sel<=2) ? 0 : chr_vfb_sel-2;
#else
    // Force EFFUSE_CHR_VFB_SEL to 0.
    // Charge process is fully controlled by sw, so as to avoid the
    // different hw threshold issue among individual chips.
    chr_vfb_sel = 0;
#endif
    g_pmdChargerSetting2 = PMU_SET_BITFIELD(g_pmdChargerSetting2,
                                            RDA_PMU_CHR_PRE_IBIT_REG,
                                            chr_pre_ibit) |
                           RDA_PMU_CHR_PRE_IBIT_DR;
    g_pmdChargerSetting2 = PMU_SET_BITFIELD(g_pmdChargerSetting2,
                                            RDA_PMU_CHR_VFB_SEL_REG,
                                            chr_vfb_sel) |
                           RDA_PMU_CHR_VFB_SEL_DR;
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
    // End of effuse setting

    // Set charge current to lowest to avoid the damage of P-channel MOSFET
    // when the charger voltage is too high
    pmd_SetChargeCurrent(PMD_CHARGER_50MA);

    UINT16 ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    if ((ret & RDA_PMU_CHR_AC_ON) == RDA_PMU_CHR_AC_ON)
    {
        // Detect whether battery is in place
        pmd_ChargerBatteryDetection();
        // Battery is in place. Go on ...
        g_pmdChargerState.status = PMD_CHARGER_PLUGGED;
        g_pmdChargerPluginAtPowerOn = TRUE;
        // PMD IRQ setting register has been initialized in pmd_InitRdaPmu()
        pmd_RegIrqSettingSet(RDA_PMU_INT_CHR_MASK|RDA_PMU_INT_CHR_CLEAR);
    }
    else
    {
        g_pmdChargerState.status = PMD_CHARGER_UNPLUGGED;
    }
}

PUBLIC VOID pmd_RestoreChargerAtPowerOff(VOID)
{
    // Set charge current to lowest to avoid the damage of P-channel MOSFET
    // when the charger voltage is too high
    pmd_SetChargeCurrent(PMD_CHARGER_50MA);
    // Restore charge control setting (especailly for precharging)
    pmd_RestoreChargerControl();
    // Enable vbat 3.2V detection
    pmd_EnableLowBattVoltShutdown(TRUE);
}

PUBLIC UINT8 pmd_GetPowerOnVolt(VOID)
{
    return pmd_BatteryLevelToPercent(g_pmdConfig->powerOnVoltageMV);
}

PUBLIC UINT8 pmd_GetPowerDownVolt(VOID)
{
    return pmd_BatteryLevelToPercent(g_pmdConfig->powerDownVoltageMV);
}

PUBLIC UINT16 pmd_GetBackLightOffsetVolt(VOID)
{
    //((g_pmdChargerState.status >= PMD_CHARGER_PRECHARGE)&&(g_pmdChargerState.status < PMD_CHARGER_FULL_CHARGE))
    if ( g_pmdChargerState.highActivityState &&
         g_pmdChargingStage != PMD_CHARGING_STAGE_ON &&
         g_pmdChargingStage != PMD_CHARGING_STAGE_PULSE )
    {
        return  g_pmdConfig->batteryOffsetHighActivityMV;
    }
    else
    {
        return 0;
    }
}

PUBLIC UINT32 pmd_GetChargerGpadcChannel(VOID)
{
    return g_pmdConfig->chargerGpadcChannel;
}

PUBLIC BOOL pmd_ChargerChrVoltMonEnabled(VOID)
{
    return g_pmdChargerChrVoltMonEnabled;
}

