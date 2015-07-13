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
//  $Author: yanshengzhang $
//  $Date: 2011-04-27 10:56:42 +0800 (星期三, 27 四月 2011) $
//  $Revision: 7261 $
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

#include "opal.h"
#include "hal_lps.h"

// for function timers
#include "sxr_tim.h"

#define SECOND        * HAL_TICK1S

#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR

#define PMD_TIME_CHG_MON_INTERVAL (2 SECONDS)

#define PMD_VOLTAGE_TUNE_NEEDED (3400)
#define PMD_VOLTAGE_TUNE_TRIGGERED (3800)

#if (PMD_VOLTAGE_TUNE_NEEDED >= PMD_VOLTAGE_TUNE_TRIGGERED)
#error "Error in charger tuning thresholds"
#endif // (PMD_VOLTAGE_TUNE_NEEDED >= PMD_VOLTAGE_TUNE_TRIGGERED)

#define PMD_COUNT_CHG_STATUS_STABLE (10)
#define PMD_COUNT_CHG_PULSE_PERIOD (6)

#define PMD_MV_OFFSET_CHG_RECHARGE (50)
#define PMD_MV_OFFSET_CHG_PULSE_LARGE_CUR (50)
#define PMD_MV_OFFSET_CHG_PULSE_SMALL_CUR (10)
#define PMD_MV_OFFSET_CHG_FULL_COMPENSATION (-10)

#if (PMD_MV_OFFSET_CHG_FULL_COMPENSATION >= PMD_MV_OFFSET_CHG_PULSE_SMALL_CUR) || \
    (PMD_MV_OFFSET_CHG_PULSE_SMALL_CUR >= PMD_MV_OFFSET_CHG_PULSE_LARGE_CUR)
#error "Incorrect charger voltage offsets"
#endif

#define PMD_INTERVAL_CHG_ON_LONG (6 MINUTES)
#define PMD_INTERVAL_CHG_ON_SHORT (3 MINUTES)
#define PMD_INTERVAL_CHG_MEAS_STABLE (1 MINUTE)

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
    PMD_CHARGING_STAGE_MEAS_WAIT,
    PMD_CHARGING_STAGE_MEAS,
    PMD_CHARGING_STAGE_PULSE,
    PMD_CHARGING_STAGE_ON,
} PMD_CHARGING_STAGE_T;

// ============================================================================
// CHG_TUNING_STAGE_T
// ----------------------------------------------------------------------------
/// This enum lists the stages when tuning charger
// ============================================================================
typedef enum
{
    CHG_TUNING_STAGE_INIT,
    CHG_TUNING_STAGE_PART1,
    CHG_TUNING_STAGE_PART2,
} CHG_TUNING_STAGE_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PROTECTED PMD_CHARGER_STATE_T g_pmdChargerState = 
    {
        .status = PMD_CHARGER_UNKNOWN,
        .current = PMD_CHARGER_100MA,
        .handler = NULL,
        .batteryLevel = PMD_BAD_VALUE,
        .highActivityState = FALSE,
    };

BOOL g_pmdChargerDcOnAtInit = FALSE;

UINT16 g_pm_tunedvolt = 0;

// Battery voltage array
PRIVATE UINT16 g_pmdBattVoltAry[PMD_COUNT_CHG_STATUS_STABLE];
// Battery voltage array index
PRIVATE UINT16 g_pmdBattVoltIdx = 0;
// Battery measurement Failure counts
PRIVATE UINT32 g_pmdBattMeasFails = 0;
// Charger charging stage
PRIVATE PMD_CHARGING_STAGE_T g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
// The start time when charging is on
PRIVATE UINT32 g_pmdChargerOnStartTime = 0;
// Charger charging duration between the measurement operations
PRIVATE UINT32 g_pmdChargerOnInterval = 0;
// The start time of the measurement
PRIVATE UINT32 g_pmdChargerMeasStartTime = 0;
// Charger retuning is needed if phone is power on at low voltage, and is needed once only
PRIVATE UINT16 g_pmdChargerStartVoltage = PMD_BAD_VALUE;

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
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd/opal: HighActivity=%d, g_pm_tunedvolt=%d", on, g_pm_tunedvolt);
}

// ============================================================================
// pmd_GetGpadcBatteryLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the battery level 
///
/// @return the battery level in mV or 
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PRIVATE UINT16 pmd_GetGpadcBatteryLevel(VOID)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_EMERALD)
    return PMD_BAD_VALUE; 
#else
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd/opal: backLight %d", g_pmdChargerState.highActivityState);

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
#endif
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
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd/opal: pmd_GetBatteryLevel  %dmV", g_pmdChargerState.batteryLevel);
    switch(g_pmdChargerState.status)
    {
        case PMD_CHARGER_FAST_CHARGE:
            if ( ( g_pmdChargingStage == PMD_CHARGING_STAGE_ON ||
                   g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE ) &&
                 (g_pmdChargerState.batteryLevel != PMD_BAD_VALUE) )
            {
                batLevel = ((g_pmdChargerState.batteryLevel - g_pmdConfig->batteryLevelPrechargeMV) * 95 / 100) +
                            g_pmdConfig->batteryLevelPrechargeMV;
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
                        - g_pmdConfig->batteryLevelPrechargeMV)
              / (g_pmdConfig->batteryLevelFullMV 
                        - g_pmdConfig->batteryLevelPrechargeMV);
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
        case PMD_CHARGER_100MA:
            regVal = RDA1203_CHG_CUR_SEL_100MA;
            break;
        case PMD_CHARGER_200MA:
            regVal = RDA1203_CHG_CUR_SEL_200MA;
            break;
        case PMD_CHARGER_300MA:
            regVal = RDA1203_CHG_CUR_SEL_300MA;
            break;
        case PMD_CHARGER_400MA:
            regVal = RDA1203_CHG_CUR_SEL_300MA;
            break;
        case PMD_CHARGER_500MA:
            regVal = RDA1203_CHG_CUR_SEL_450MA;
            break;
        case PMD_CHARGER_600MA:
            regVal = RDA1203_CHG_CUR_SEL_650MA;
            break;
        case PMD_CHARGER_700MA:
            regVal = RDA1203_CHG_CUR_SEL_650MA;
            break;
        case PMD_CHARGER_800MA:
            regVal = RDA1203_CHG_CUR_SEL_800MA;
            break;
        default:
            PMD_ASSERT(FALSE, "Unsupported charge current: %d", current);
            break;
    }
    ///@todo: keep calib value
    UINT32 oldVal = pmd_RDARead(RDA_ADDR_CHARGER_SETTING1);
    regVal = (oldVal&~(RDA1203_CHG_CUR_SEL_MASK)) | (regVal&RDA1203_CHG_CUR_SEL_MASK);
    pmd_RDAWrite(RDA_ADDR_CHARGER_SETTING1, (UINT16)regVal);
    g_pmdChargerState.current = current;
}


PRIVATE VOID pmd_TuneCharger(CHG_TUNING_STAGE_T stage)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd/opal: pmd_TuneCharger stage=%d", stage);

    UINT16 ret;

    if (stage != CHG_TUNING_STAGE_INIT)
    {
        if (!pmd_SpiCsNonblockingActivate(FALSE))
        {
            sxr_StopFunctionTimer(pmd_TuneCharger);
            sxr_StartFunctionTimer(5 MS_WAITING, pmd_TuneCharger, CHG_TUNING_STAGE_PART1, 0);
            return;
        }
    }

    if (stage != CHG_TUNING_STAGE_PART2)
    {
        ret = pmd_OpalSpiRead(RDA_ADDR_SIM_INTERFACE);
        ret = ret | RDA1203_SELF_CAL_ENABLE_DR | RDA1203_SELF_CAL_ENABLE_REG;
        pmd_OpalSpiWrite(RDA_ADDR_SIM_INTERFACE, ret);
        
        ret = pmd_OpalSpiRead(RDA_ADDR_LDO_CALIBRATION_SETTING1);
        ret = (ret | RDA1203_CHR_BGAP_CAL_RESETN_DR) &
                ~RDA1203_CHR_BGAP_CAL_RESETN_REG;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_CALIBRATION_SETTING1, ret); 
        ret = ret & ~(RDA1203_CHR_BGAP_CAL_RESETN_DR|RDA1203_CHR_BGAP_CAL_RESETN_REG);
        pmd_OpalSpiWrite(RDA_ADDR_LDO_CALIBRATION_SETTING1, ret); 
        if (stage == CHG_TUNING_STAGE_INIT)
        {
            // OS is not ready yet. Just delay
            hal_TimDelay(10 MS_WAITING);
        }
        else
        {
            sxr_StopFunctionTimer(pmd_TuneCharger);
            sxr_StartFunctionTimer(10 MS_WAITING, pmd_TuneCharger, CHG_TUNING_STAGE_PART2, 0);
            return;
        }
    }

    ret = pmd_OpalSpiRead(RDA_ADDR_SIM_INTERFACE);
    ret = ret & ~(RDA1203_SELF_CAL_ENABLE_DR|RDA1203_SELF_CAL_ENABLE_REG); 
    pmd_OpalSpiWrite(RDA_ADDR_SIM_INTERFACE, ret); 

    if (stage != CHG_TUNING_STAGE_INIT)
    {
        pmd_OpalCsDeactivate(FALSE);
    }
}

PRIVATE VOID pmd_ForceChargerFinished(BOOL stop)
{
    if (stop)
    {
        pmd_RDAWrite(RDA_ADDR_LDO_MISC_CONTROL, 0x3524);
    }
    else
    {
        pmd_RDAWrite(RDA_ADDR_LDO_MISC_CONTROL, 0x2524);
    }
}

PROTECTED PMD_CHARGER_STATUS_T pmd_ChargerChargingState(RDA1203_CHR_STATUS_T status)
{
    // Taper and term status are not checked any more, as these h/w signals are
    // inaccurate on some 8805 chipsets, and the case is even worse for 8806

    int i;
    PMD_CHARGER_STATUS_T nextState = g_pmdChargerState.status;
    UINT16 meanBattVolt = 0;
    BOOL needMeas = FALSE;
    UINT32 curTime = hal_TimGetUpTime();

    if (g_pmdChargingStage == PMD_CHARGING_STAGE_MEAS_WAIT)
    {
        if (curTime - g_pmdChargerMeasStartTime >= PMD_INTERVAL_CHG_MEAS_STABLE)
        {
            g_pmdBattVoltIdx = 0;
            g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS;
        }

        return nextState;
    }

    if (g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE)
    {
        if ((g_pmdBattVoltIdx+g_pmdBattMeasFails)%PMD_COUNT_CHG_PULSE_PERIOD == 0)
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
        g_pmdBattVoltIdx = 0;
        if (meanBattVolt+PMD_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmdConfig->batteryLevelFullMV)
        {
            // Compensate the measurement error (due to stable time, etc)
            // to make sure the battery is charged full
            g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
            // Set next state to full
            nextState = PMD_CHARGER_FULL_CHARGE;
        }
        else if (meanBattVolt+PMD_MV_OFFSET_CHG_PULSE_SMALL_CUR >= g_pmdConfig->batteryLevelFullMV)
        {
            // Pulsed charging with a relatively small average current
            g_pmdChargerOnStartTime = curTime;
            g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_SHORT;
            g_pmdChargingStage = PMD_CHARGING_STAGE_PULSE;
        }
        else if (meanBattVolt+PMD_MV_OFFSET_CHG_PULSE_LARGE_CUR >= g_pmdConfig->batteryLevelFullMV)
        {
            // Pulsed charging with a relatively large average current
            g_pmdChargerOnStartTime = curTime;
            g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_LONG;
            g_pmdChargingStage = PMD_CHARGING_STAGE_PULSE;
        }
        else
        {
            // Fast charging
            g_pmdChargerOnStartTime = curTime;
            g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_LONG;
            g_pmdChargingStage = PMD_CHARGING_STAGE_ON;
            // Enable charging (h/w thresholds take effect)
            pmd_ForceChargerFinished(FALSE);
        }
        break;
    case PMD_CHARGING_STAGE_PULSE:
    case PMD_CHARGING_STAGE_ON:
        if (g_pmdChargingStage == PMD_CHARGING_STAGE_PULSE)
        {
            if (meanBattVolt >= g_pmdConfig->batteryLevelFullMV)
            {
                needMeas = TRUE;
            }
        }
        else if (g_pmdChargingStage == PMD_CHARGING_STAGE_ON)
        {
            if (meanBattVolt >= g_pmdConfig->batteryLevelFullMV)
            {
                needMeas = TRUE;
            }
        }

        if(needMeas && (curTime - g_pmdChargerOnStartTime >= g_pmdChargerOnInterval))
        {
            g_pmdBattVoltIdx = 0;
            g_pmdChargerMeasStartTime = curTime;
            g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS_WAIT;
            // Force to disable charging
            pmd_ForceChargerFinished(TRUE);
        }
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
        g_pmdChargerStartVoltage = PMD_BAD_VALUE;
        g_pmdBattMeasFails = 0;
        g_pmdBattVoltIdx = 0;
        // -- To minimize the charging time for a battery nearing full
        g_pmdChargerOnStartTime = g_pmdChargerState.startTime + 0x80000000;
        g_pmdChargerOnInterval = PMD_INTERVAL_CHG_ON_SHORT;
        // -- End
        g_pmdChargerMeasStartTime = g_pmdChargerState.startTime + 0x80000000;
        // Measurment must be done before charging
        g_pmdChargingStage = PMD_CHARGING_STAGE_MEAS;

        // Start a timer to read the acutal charger status in non-blocking mode
        // (PMD_TIME_CHG_MON_INTERVAL>>2) is the debounce time
        sxr_StopFunctionTimer(pmd_ChargerManager);
        sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL>>2, pmd_ChargerManager, PMD_CHARGE_TIMER, 0);
    }
    else if (event == PMD_CHARGE_TIMER)
    {
        // Get current battery voltage
        UINT16 curBattVolt = pmd_GetBatteryLevel(NULL);

        if (curBattVolt == PMD_BAD_VALUE)
        {
            g_pmdBattMeasFails++;
        }
        else
        {
            g_pmdBattVoltAry[g_pmdBattVoltIdx++%PMD_COUNT_CHG_STATUS_STABLE] = curBattVolt;
        }

        // TODO: use the mean value or the first measurement value?
        if (g_pmdChargerStartVoltage == PMD_BAD_VALUE)
        {
            g_pmdChargerStartVoltage = g_pmdChargerState.batteryLevel;
        }
        else
        {
            if (g_pmdChargerStartVoltage < PMD_VOLTAGE_TUNE_NEEDED)
            {
                if (g_pmdChargerState.batteryLevel > PMD_VOLTAGE_TUNE_TRIGGERED)
                {
                    pmd_TuneCharger(CHG_TUNING_STAGE_PART1);
                    g_pmdChargerStartVoltage = PMD_VOLTAGE_TUNE_TRIGGERED;
                }
            }
        }

        RDA1203_CHR_STATUS_T status = pmd_GetChargerHwStatus();
        if (status == CHR_STATUS_DC_OFF)
        {
            nextState = PMD_CHARGER_UNPLUGGED;
            g_pmdChargingStage = PMD_CHARGING_STAGE_OFF;
            // Force to disable charging (measurement must be done before charging)
            pmd_ForceChargerFinished(TRUE);

            sxr_StopFunctionTimer(pmd_ChargerManager);
        }
        else // Charger is unknown or plugged in
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
                    // Report charging progress to upper layer
                    nextState = pmd_ChargerChargingState(status);
                }
            }

            sxr_StopFunctionTimer(pmd_ChargerManager);
            sxr_StartFunctionTimer(PMD_TIME_CHG_MON_INTERVAL, pmd_ChargerManager, PMD_CHARGE_TIMER, 0);
        }
    }

    PMD_TRACE(PMD_INFO_TRC, 0, "pmd/opal: nextState=%d ,oldstatus=%d, pulseCnt=%d ",
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
    PMD_TRACE(PMD_WARN_TRC, 0, "pmd/opal: DC-ON : %d",on);
    if (on == FALSE)
    {
        // wait for DC-ON rising edge
        // tell the charger manager that there is no charger
        pmd_ChargerManager(PMD_CHARGE_DC_OFF);
        PMD_TRACE(PMD_WARN_TRC, 0, "pmd/opal: DC-ON : here1 ");
    }
    else
    {
        // wait for DC-ON falling edge
        // tell the charger manager that the charger is plugged
        pmd_ChargerManager(PMD_CHARGE_DC_ON);
        PMD_TRACE(PMD_WARN_TRC, 0, "pmd/opal: DC-ON : plug in  ");
    }
}

PUBLIC VOID pmd_InitCharger(VOID)
{
    // Force to disable charging (measurement must be done before charging)
    pmd_ForceChargerFinished(TRUE);

    pmd_SetChargeCurrent(g_pmdConfig->batteryChargeCurrent);

    pmd_TuneCharger(CHG_TUNING_STAGE_INIT);

    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_CONTROL, 0x456a);

    // Set effuse
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING2, 0x0000);
    hal_TimDelay(1 MS_WAITING);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0x9800);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0x9000);
    UINT32 effuse = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING4);
    UINT32 prech_vsel = (effuse&RDA1203_EFFUSE_PRECH_VSEL_MASK)>>RDA1203_EFFUSE_PRECH_VSEL_SHIFT;
    UINT32 chr_vfb_sel = (effuse&RDA1203_EFFUSE_CHR_VFB_SEL_MASK)>>RDA1203_EFFUSE_CHR_VFB_SEL_SHIFT;
    // Lower EFFUSE_CHR_VFB_SEL by 2 to increase the hw target charge voltage,
    // while not enlarging the charge current too much.
    // This is safe as sw will stop charging if the battery voltage is over 4.2V.
    chr_vfb_sel = (chr_vfb_sel<=2) ? 0 : chr_vfb_sel-2;
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING2, 0x0200);
    UINT16 chg_setting1 = pmd_OpalSpiRead(RDA_ADDR_CHARGER_SETTING1);
    UINT16 chg_setting2 = pmd_OpalSpiRead(RDA_ADDR_CHARGER_SETTING2);
    chg_setting1 = ( chg_setting1 & ~(RDA1203_PRECH_VSEL_MASK) ) |
                     RDA1203_PRECH_VSEL(prech_vsel);
    chg_setting2 = ( chg_setting2 & ~(RDA1203_CHR_VFB_SEL_MASK) ) |
                     RDA1203_CHR_VFB_SEL_DR |
                     RDA1203_PRECH_VSEL_DR |
                     RDA1203_CHR_VFB_SEL(chr_vfb_sel);
    // first write the reg value and then write direct reg bit
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, chg_setting1);
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, chg_setting2);
    // End of effuse setting

    UINT16 ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    if ((ret & RDA1203_CHR_AC_ON) == RDA1203_CHR_AC_ON)
    {
        g_pmdChargerState.status = PMD_CHARGER_PLUGGED;
        g_pmdChargerDcOnAtInit = TRUE;
        
        ret = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
        ret |= RDA1203_INT_CHR_MASK|RDA1203_INT_CHR_CLEAR;
        pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, ret);
    }
    else
    {
        g_pmdChargerState.status = PMD_CHARGER_UNPLUGGED;
    }
}

PUBLIC VOID pmd_ChargerPlugIn(VOID)
{
    pmd_ChargerManager(PMD_CHARGE_DC_ON);

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
