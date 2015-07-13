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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Sources/edrv/trunk/pmd/rda1203_gallite/src/pmd.c $ //
//  $Author: yangtt $                                                        // 
//  $Date: 2011-04-22 16:02:34 +0800 (星期五, 22 四月 2011) $                     //   
//  $Revision: 7207 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file pmd.c  
/// This file contain the PMD driver code related to the eco mode.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "pmd_config.h"
#include "pmd_m.h"
#include "pmd_map.h"
//#include "pmd_calib.h"
#include "pmdp_debug.h"
#include "pmdp_charger.h"
#include "pmdp.h"
//#include "drv_dualsim.h"
#include "hal_rda_abb.h "

// Opal hardware
#include "opal.h"

// hal drivers
#include "hal_pwm.h"
#include "hal_ispi.h"
#include "hal_lps.h"
#include "hal_timers.h"
#include "hal_sys.h"
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#include "hal_rfspi.h"
#endif // (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

#ifdef DCDC_FREQ_DIV_WORKAROUND
#include "hal_rda_abb.h"
#endif // DCDC_FREQ_DIV_WORKAROUND

// sx timers
#include "sxr_tls.h"
#include "sxr_tim.h"

#ifdef VMIC_POWER_ON_WORKAROUND
extern VOID aud_InitEarpieceSetting(VOID);
extern VOID aud_EnableEarpieceSetting(BOOL enable);
#endif // VMIC_POWER_ON_WORKAROUND

//
// =============================================================================
//  DEFINES
// =============================================================================

#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef SPI_REG_DEBUG
#endif

#define PMD_FUNC_INTERNAL   // __attribute__((section(".sramtext")))

#if defined(PRJ_K8) || defined (PRJ_M11_AP_P0)
#define CONFIG_CLASS_K_SPEAKER_PA 1
#else
#define CONFIG_CLASS_K_SPEAKER_PA 0
#endif
#define DYNAMIC_DCDC_VCORE_ADJUSTMENT 0

#define CHECK_LDO_USER_BITMAP 1

#if (CHECK_LDO_USER_BITMAP == 1)
#if (PMD_POWER_ID_QTY + PMD_LEVEL_ID_QTY > 31)
#error "Too many LDO users for current LDO user bitmap"
#endif // (PMD_POWER_ID_QTY + PMD_LEVEL_ID_QTY > 31)
#endif // (CHECK_LDO_USER_BITMAP == 1)

#define POWER_LDO_USER(id) (1<<id)
#define LEVEL_LDO_USER(id) (1<<(id+PMD_POWER_ID_QTY))
#define DEFAULT_LDO_USER   (1<<31)
PUBLIC BOOL pmd_RegRead(RDA_REG_MAP_T regIdx, UINT32 *pData);
PUBLIC BOOL pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT32 data);
PRIVATE VOID pmd_EarpDetectGpioInit(HAL_GPIO_GPIO_ID_T gpioId);

//
// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// OPAL_BUCK_MODE_T
// -----------------------------------------------------------------------------
/// Setup of operation mode of buck DC-DC converter.
// =============================================================================
typedef enum
{
    OPAL_BUCK_PWM_MODE = 0,
    OPAL_BUCK_PFM_MODE,
} OPAL_BUCK_MODE_T;

// =============================================================================
// OPAL_BUCK_VCORE_MODE_T
// -----------------------------------------------------------------------------
/// Setup of power output mode.
// =============================================================================
typedef enum buck_vcore_mode
{
    OPAL_BUCK_DCDC = 0,
    OPAL_CORE_LDO,
} OPAL_BUCK_VCORE_MODE_T;

// =============================================================================
// OPAL_PROFILE_CFG_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef enum
{
    LP_MODE = 0,
    NORMAL_MODE = 1
} PROFILE_MODE_T;

typedef enum
{
    POWER_ON = 0,
    POWER_OFF = 1
} POWER_STATE_T;

typedef union
{
    struct {
        PROFILE_MODE_T mode:1;
        POWER_STATE_T  Vsim2:1;
        POWER_STATE_T  Vsim1:1;
        POWER_STATE_T  VMic:1;
        POWER_STATE_T  VUSB:1;
        POWER_STATE_T  Vibr:1;
        POWER_STATE_T  Vmmc:1;
        POWER_STATE_T  Vlcd:1;
        POWER_STATE_T  Vcamera:1;
        POWER_STATE_T  VRF:1;
        POWER_STATE_T  Vacc:1;
        POWER_STATE_T  Vpad:1;
        POWER_STATE_T  Vmemory:1;
        POWER_STATE_T  VDcdcCore:1;
        POWER_STATE_T  VLdoCore:1;
        BOOL           VRtcHigh:1;
    };
    UINT16 reg;
} OPAL_PROFILE_CFG_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================
/// This array contain the value of the corresponding Opal registers
PRIVATE OPAL_PROFILE_CFG_T g_pmdProfileRegs[OPAL_PROFILE_MODE_QTY] =
{
    {{  // profile 1 : OPAL_PROFILE_MODE_NORMAL
        .mode = NORMAL_MODE,
        .Vsim2 = POWER_OFF,
        .Vsim1 = POWER_OFF,
        .VMic = POWER_OFF,
        .VUSB = 
#ifdef FASTPF_USE_USB
                POWER_ON, // For USB flash programmer
#else // !FASTPF_USE_USB
                POWER_OFF,
#endif // !FASTPF_USE_USB
        .Vibr = POWER_OFF,
        .Vmmc = POWER_OFF,
        .Vlcd = POWER_OFF,
        .Vcamera = POWER_OFF,
        .VRF = POWER_OFF,
        // For ABB_GPADC
        .Vacc = POWER_ON,
        .Vpad = POWER_ON,
        .Vmemory = POWER_ON,
        .VDcdcCore = POWER_ON,
        .VLdoCore = POWER_OFF,
        .VRtcHigh = TRUE,
    }},
    {{  // profile 2 : OPAL_PROFILE_MODE_LOWPOWER
        .mode = LP_MODE,
        .Vsim2 = POWER_OFF,
        .Vsim1 = POWER_OFF,
        .VMic = POWER_OFF,
        .VUSB = POWER_OFF,
        .Vibr = POWER_OFF,
        .Vmmc = POWER_OFF,
        .Vlcd = POWER_OFF,
        .Vcamera = POWER_OFF,
        .VRF = POWER_OFF,
        .Vacc = POWER_OFF,
        .Vpad = POWER_ON,
        .Vmemory = POWER_ON,
        .VDcdcCore = POWER_OFF,
        .VLdoCore = POWER_ON,
        .VRtcHigh = TRUE,
    }},
};

// VCORE values
PRIVATE UINT8 g_pmdLdoVcoreValueActive;
PRIVATE UINT8 g_pmdLdoVcoreValueLp;
PRIVATE UINT8 g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_QTY];
PRIVATE UINT8 g_pmdDcdcVcoreValueLp;

PRIVATE UINT8 g_pmdDcdcVcoreActRegistry[PMD_CORE_VOLTAGE_USER_ID_QTY];
PRIVATE UINT8 g_pmdDcdcVcoreActCurIndex;

#if (CHECK_LDO_USER_BITMAP == 1)
PRIVATE UINT32 g_pmdLdoUserBitmap[OPAL_PROFILE_MODE_QTY][PMD_LDO_QTY];
#endif // (CHECK_LDO_USER_BITMAP == 1)

// Power down keypad and LCD backlight, disable boost mode (0),
// LCD backlight PWM controlled by PMU, direct LCD backlight control,
// LCD backlight always on (0), enable keypad backlight PWM on PMU,
// keypad backlight always on (0)
PRIVATE UINT16 g_pmdLcd_Ctrl = 
                RDA1203_PD_KP_PON|RDA1203_PD_KP_ACT|RDA1203_PD_KP_LP|
                RDA1203_PD_BL_PON|RDA1203_PD_BL_ACT|RDA1203_PD_BL_LP|
                RDA1203_DIM_BL_DR|RDA1203_PWM_KP_ENABLE;

// LED setting 3
PRIVATE UINT16 g_pmdLedSetting3 = 0x4210;

// SIM setting
PRIVATE UINT16 g_pmdSimSelect = 
                RDA1203_PULLUP_ENABLE_1|RDA1203_UNSEL_RST_VAL_1|
                RDA1203_PULLUP_ENABLE_2|RDA1203_UNSEL_RST_VAL_2|
                RDA1203_TIMER_BB_RESETN_SEL_60MS;

// VCORE active setting
PRIVATE UINT32 g_pmdLdoActiveSetting3 = 0;

/// value of the Opal register LDO_Out_V_Ctrl
PRIVATE UINT16 g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_QTY] =
{
    // OPAL_PROFILE_MODE_NORMAL, normal working voltage
    0x801c,
    // OPAL_PROFILE_MODE_LOWPOWER
    //0xa01c, // lower vpad to 1.8V in case that lcd is wired to it
    0x801c,
};

/// Configuration structure for TARGET
PROTECTED CONST PMD_CONFIG_T* g_pmdConfig;

/// Variable used to store the default values of the PMD calibration.
/// This is not used in the CT1010, be used by Opal.
//tianwq PRIVATE CONST CALIB_PMD_T g_pmdCalibDefault = {0x2901, 0};


/// To avoid SPI deactivation/activation during Open
PRIVATE BOOL g_pmdInitDone = FALSE;

/// To track activation status
PRIVATE VOLATILE UINT8 g_pmdActivated = 0;
PRIVATE VOLATILE UINT8 g_tsdActivated = 0;
PRIVATE VOLATILE UINT8 g_SimActivated = 0;

// Workaround to disable PMU IRQ without blocking in PMU IRQ handler
PRIVATE UINT32 g_pmdIrqSettingLatest = 0;

PRIVATE BOOL g_pmdBackLightOn = TRUE;

PRIVATE BOOL g_pmdAudExtPaExists = FALSE;

extern BOOL g_pmdChargerDcOnAtInit ;

VOID pmd_EarpDetectGpioInit (HAL_GPIO_GPIO_ID_T gpioId);

// ============================================================================
//  PRIVATE FUNCTIONS
// ============================================================================

//=============================================================================
// pmd_OpalCsBlockingActivate
//-----------------------------------------------------------------------------
/// Activate the SPI bus for Opal chip.
/// This function will poll until the CS is activated
//=============================================================================
PUBLIC VOID pmd_OpalCsBlockingActivate(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return;
    }

    PMD_PROFILE_FUNCTION_ENTER(pmd_OpalCsBlockingActivate);
    
    BOOL status = FALSE;
    UINT32 scStatus;
    while(FALSE == status)
    {
        scStatus = hal_SysEnterCriticalSection();
        if (1 == g_pmdActivated || 1 == g_tsdActivated || 1 == g_SimActivated)
        {
            status = FALSE;
            if (1 == g_pmdActivated)
            {
                PMD_TRACE(PMD_INFO_TRC, 0, "PMU has been activated. Be care of infinite loop.");
            }
        }
        else
        {
            status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
            if (status)
            {
                g_pmdActivated = 1;
            }
        }
        hal_SysExitCriticalSection(scStatus);
        
        if(FALSE == status)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, " pmd_OpalCsBlockingActivate, Sleeping...");
            sxr_Sleep(HAL_TICK1S / 10);
        }
    }
    PMD_PROFILE_FUNCTION_EXIT(pmd_OpalCsBlockingActivate);   
}

//=============================================================================
// pmd_OpalCsDeactivate
//-----------------------------------------------------------------------------
/// Activate the SPI bus for Opal chip.
/// This function will poll until the CS is activated
//=============================================================================
PUBLIC VOID pmd_OpalCsDeactivate(BOOL singleWrite)
{
     // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return;
    }

    PMD_PROFILE_FUNCTION_ENTER(pmd_OpalCsDeactivate);
    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdActivated = 0;
    hal_SysExitCriticalSection(scStatus);
    PMD_PROFILE_FUNCTION_EXIT(pmd_OpalCsDeactivate);
}


//=============================================================================
// pmd_OpalSpiActivateCs called  by TSD moudle
//-----------------------------------------------------------------------------
/// Activate Opal SPI. refer to hal_SpiActivateCs() for details.
/// @return \c TRUE if the cs has been successfully activated
/// \c FALSE when another cs is already active
// =============================================================================
PUBLIC BOOL pmd_OpalSpiActivateCs(VOID)  //called  by TSD moudle
{
    PMD_ASSERT(g_pmdConfig != NULL, "g_pmdConfig must be not NULL.");

    PMD_PROFILE_FUNCTION_ENTER(pmd_OpalSpiActivateCs);
    
    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (1 == g_pmdActivated || 1 == g_tsdActivated || 1 == g_SimActivated)
    {
        status = FALSE;
        if (1 == g_tsdActivated)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, "PMU-TS has been activated. Be care of infinite loop.");
        }
    }
    else
    {
        status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
        if (status)
        {
            g_tsdActivated = 1;
        }
    }
    hal_SysExitCriticalSection(scStatus);

    PMD_PROFILE_FUNCTION_EXIT(pmd_OpalSpiActivateCs);

    return status;
}

//=============================================================================
// pmd_OpalSpiDeactivateCs
//-----------------------------------------------------------------------------
/// Deactivate Opal SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PUBLIC VOID pmd_OpalSpiDeactivateCs(VOID)
{
    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_tsdActivated  = 0;
    hal_SysExitCriticalSection(scStatus);
    
}

PROTECTED BOOL pmd_SIMSpiActivateCs(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return TRUE;
    }

    PMD_ASSERT(g_pmdConfig != NULL, "g_pmdConfig must be not NULL.");

    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (1 == g_pmdActivated || 1 == g_tsdActivated || 1 == g_SimActivated)
    {
        status = FALSE;
        if (1 == g_SimActivated)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, "PMU-SIM has been activated. Be care of infinite loop.");
        }
    }
    else
    {
        status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
        if (status)
        {
            g_SimActivated = 1;
        }
    }
    hal_SysExitCriticalSection(scStatus);

    return status;
}

//=============================================================================
// pmd_OpalSpiDeactivateCs
//-----------------------------------------------------------------------------
/// Deactivate Opal SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PROTECTED BOOL pmd_SIMSpiDeactivateCs(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return TRUE;
    }

    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_SimActivated = 0;
    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}

//=============================================================================
// pmd_OpalSpiRead
//-----------------------------------------------------------------------------
/// Read Opal register on SPI, the CS must already be activated
/// @param regIdx register to read
/// @return register value                 0 write 1 read
//=============================================================================
PUBLIC UINT16 pmd_OpalSpiRead(RDA_REG_MAP_T regIdx)
{
    UINT32 wrData, rdData;
    UINT32 count;
    UINT16 value;

    wrData = (1<<25) | ((regIdx & 0x1ff) << 16) | 0;
    
    while(hal_IspiTxFifoAvail(HAL_ISPI_CS_PMU) < 1 ||
            hal_IspiSendData(HAL_ISPI_CS_PMU, wrData, TRUE) == 0);

    //wait until any previous transfers have ended
    while(!hal_IspiTxFinished(HAL_ISPI_CS_PMU));
    count = hal_IspiGetData(HAL_ISPI_CS_PMU, &rdData);
    PMD_ASSERT(1 == count, "PMD expected 1 data got %d !", count);

    value = (UINT16)(rdData & 0xffff);
    PMD_TRACE(PMD_INFO_TRC, 0, "REG 0x%x: 0x%x", regIdx, value);

    return value;
}

//=============================================================================
// pmd_OpalSpiWrite
//-----------------------------------------------------------------------------
/// Write Opal register on SPI, the CS must already be activated
/// @param regIdx register to read
/// @param value register value
//=============================================================================
PUBLIC VOID pmd_OpalSpiWrite(RDA_REG_MAP_T regIdx, UINT16 value)
{
    //PMD_TRACE(PMD_INFO_TRC, 0, " write RDA addr = 0x%x data =0x%0x ",regIdx,value);

    UINT32 wrData;

    if (regIdx == RDA_ADDR_IRQ_SETTINGS)
    {
        g_pmdIrqSettingLatest = value;
    }
    
    wrData = (0<<25) | ((regIdx & 0x1ff) << 16) | (value);

    while(hal_IspiTxFifoAvail(HAL_ISPI_CS_PMU) < 1 ||
            hal_IspiSendData(HAL_ISPI_CS_PMU, wrData, FALSE) == 0);

    //wait until any previous transfers have ended
    while(!hal_IspiTxFinished(HAL_ISPI_CS_PMU));    
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingSet(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdIrqSettingLatest |= flags;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);
    // Never save the clear bits
    g_pmdIrqSettingLatest &= ~(RDA1203_INT_CHR_CLEAR|RDA1203_PENIRQ_CLEAR|
                               RDA1203_EOMIRQ_CLEAR);

    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingClr(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdIrqSettingLatest &= ~flags;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);

    hal_SysExitCriticalSection(scStatus);
}
PUBLIC VOID pmd_RDAWrite(RDA_REG_MAP_T regIdx,UINT16 data)
{
#if 0
    if (g_pmdInitDone)
    {
        pmd_OpalCsBlockingActivate(TRUE);
    }
#endif

    pmd_OpalSpiWrite(regIdx, data);

#if 0
    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(TRUE);
    }
#endif
}


BOOL pmd_SimSPIWrite(RDA_REG_MAP_T regIdx,UINT16 data)
{
#if 0
    if (FALSE == pmd_SIMSpiActivateCs(TRUE))
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "spi SIM BUSY");   
        return FALSE;
    }
#endif

    pmd_OpalSpiWrite(regIdx, data);

#if 0
    pmd_SIMSpiDeactivateCs(TRUE);
#endif

    return TRUE;
}

BOOL pmd_TSDGetSPI(VOID)
{
    if (FALSE == pmd_OpalSpiActivateCs())
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "spi TSD BUSY");   
        return FALSE;
    }
    return TRUE;
    
}
BOOL pmd_TSDReleaseSPI(VOID)
{
    pmd_OpalSpiDeactivateCs();
    return TRUE;
    
}


//=============================================================================
// pmd_RDARead
//-----------------------------------------------------------------------------
/// Read Opal register on SPI
/// the CS will be activated and deactivated automatically
/// @param regIdx register to read
/// @return register value
//=============================================================================
PUBLIC UINT16 pmd_RDARead(RDA_REG_MAP_T regIdx)
{
    u16 readVal = 0;

    if (g_pmdInitDone)
    {
        pmd_OpalCsBlockingActivate(FALSE);
    }

    readVal = pmd_OpalSpiRead(regIdx);

    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(FALSE);
    }
    
    return readVal;
}



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
PROTECTED VOID pmd_EnableOpalLdo(PMD_LDO_ID_T ldo, BOOL on,
                                 OPAL_PROFILE_MODE_T profile, UINT32 user)
{
    //PMD_TRACE(PMD_DBG_TRC, 0, "pmd_EnableOpalLdo: profile=%d on=%d  ldo=0x%x",profile,on,ldo);

    PMD_ASSERT(profile < OPAL_PROFILE_MODE_QTY, "pmd_EnableOpalLdo: Invalid profile number (%d)", profile);
    PMD_ASSERT(ldo.type == PMD_LDO_ID_TYPE_OPAL, "pmd_EnableOpalLdo: using ID of non LDO resource (%08x) ", ldo);
    PMD_ASSERT((ldo.id & (~(OPAL_LDO_EN_MASK) & 0xffff)) == 0,
        "pmd_EnableOpalLdo: invalid LDO Id %08x ", ldo);

    // VABB (va, vacc) is always on in active mode on Gallite
    // (in order to supply power to ABB_GPADC), so we do NOT need to
    // consider ABB state in case of MIC power supply change.
#if 0
#if (CHECK_LDO_USER_BITMAP == 1)
    // VMIC is derived from VACC(VABB) on Gallite
    if (ldo.id & PMD_LDO_MIC)
    {
        ldo.opal |= PMD_LDO_ACC;
    }
#endif // (CHECK_LDO_USER_BITMAP == 1)
#endif

    if ( (ldo.id & PMD_LDO_ACC) && (profile == OPAL_PROFILE_MODE_NORMAL) )
    {
        // Never power off VABB in active mode
        ldo.id &= ~PMD_LDO_ACC;
    }
    if (ldo.id == 0)
    {
        return;
    }

    UINT32 reg_offset = 0;
    UINT32 idx = 0;
    UINT32 ldoId;
    UINT32 scStatus = hal_SysEnterCriticalSection();

#if (CHECK_LDO_USER_BITMAP == 1)
    ldoId = ldo.id;
    while (ldoId)
    {
        if (ldoId & 0x1)
        {
            if (on)
            {
                if (g_pmdLdoUserBitmap[profile][idx] != 0)
                {
                    // This LDO has been enabled before, and we do not need to enable it again.
                    ldo.id &= ~(1<<idx);
                }
                g_pmdLdoUserBitmap[profile][idx] |= user;
            }
            else
            {
                g_pmdLdoUserBitmap[profile][idx] &= ~user;
                if (g_pmdLdoUserBitmap[profile][idx] != 0)
                {
                    // This LDO is still used by another user, and we can NOT disable it.
                    ldo.id &= ~(1<<idx);
                }
            }
        }
        ldoId >>= 1;
        idx++;
    }

    if (ldo.id == 0)
    {
        hal_SysExitCriticalSection(scStatus);
        return;
    }
#endif // (CHECK_LDO_USER_BITMAP == 1)

    if (on)
    {
        g_pmdProfileRegs[profile].reg &= ~(ldo.id & OPAL_LDO_EN_MASK);
    }
    else
    {
        g_pmdProfileRegs[profile].reg |= ldo.id & OPAL_LDO_EN_MASK;
    }
    
    if (profile == OPAL_PROFILE_MODE_LOWPOWER)
    {
        reg_offset = 5;
        
        // if LDO is on in LP mode, copy the voltage selection from active mode to LP mode
        if (on)
        {
            UINT32 ldoVoltSelLpMask = 0;
            UINT32 ldoVoltSelLp = 0;
            
            ldoId = ldo.id;
            if (ldoId & PMD_LDO_MIC)
            {
                ldoVoltSelLpMask |= RDA1203_ACT_MIC_SEL_MASK;
            }
            if (ldoId & PMD_LDO_VIBR)
            {
                ldoVoltSelLpMask |= RDA1203_ACT_VIBRATE_SEL_MASK;
            }
            if (ldoId & PMD_LDO_MMC)
            {
                ldoVoltSelLpMask |= RDA1203_ACT_MMC_SEL_MASK;
            }
            if (ldoId & PMD_LDO_LCD)
            {
                ldoVoltSelLpMask |= RDA1203_ACT_LCD_SEL_MASK;
            }
            if (ldoId & PMD_LDO_CAM)
            {
                ldoVoltSelLpMask |= RDA1203_ACT_CAMERA_SEL_MASK;
            }
            if (ldoId & PMD_LDO_RF)
            {
                ldoVoltSelLpMask |= RDA1203_ACT_RF_SEL_MASK;
            }
            
            ldoVoltSelLp = g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] & ldoVoltSelLpMask;
            g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] &= ~ldoVoltSelLpMask;
            g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] |= ldoVoltSelLp;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER]);
        }
    }

    // profile registers are in same order so can use (OPAL_ADDR_PROFILE_1+profile) as
    // register
   // during pmd_Open, don't write to Opal Profile reg for each LDO change
   // pmd_Open will write the final value before enabling the profile
    if (g_pmdInitDone)
    {
       //PMD_TRACE(PMD_DBG_TRC, 0, "pmd_EnableOpalLdo: reg=0x%x offset=%d",g_pmdProfileRegs[profile].reg,reg_offset);
       pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1+reg_offset,g_pmdProfileRegs[profile].reg);
    } 

    hal_SysExitCriticalSection(scStatus);
}




// =============================================================================
//  Special API with lower level SPI handling (for multiple register access)
// =============================================================================


//=============================================================================
// pmd_OpalSpiSendCommand
//-----------------------------------------------------------------------------
/// Send a burst of commands to Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results ensure the command has been effectively send).
/// @param cmd array of bytes containing the command, use #PMD_OPAL_SPI_BUILD_WRITE_COMMAND and #PMD_OPAL_SPI_BUILD_READ_COMMAND.
/// @param length number of SPI command to send (cmd array must by 3 time that)
//=============================================================================
PROTECTED VOID pmd_OpalSpiSendCommand(CONST UINT32* cmd, UINT32 length)
{
    UINT32 count = 0;
    while (count < length)
    {
        count += hal_IspiSendDataBuffer(HAL_ISPI_CS_PMU,
                                                         cmd + count,
                                                         length - count);
    }
}

//=============================================================================
// pmd_OpalSpiGetData
//-----------------------------------------------------------------------------
/// Read a burst of data from Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results before sending too much new commands).
/// @param data array of bytes to receive the data, use #PMD_OPAL_SPI_EXTRACT_DATA
/// @param length number of SPI command recieve (data array must by 3 time that)
///
/// This function is blocking, it will pool to get all the expected data, 
/// use with care!
//=============================================================================
PROTECTED VOID pmd_OpalSpiGetData(UINT32* data, UINT32 length)
{
    UINT32 count;
    count = 0;
    while (count < length)
    {
        count += hal_IspiGetDataBuffer(HAL_ISPI_CS_PMU,
                                                       data + count,
                                                       length - count);
    }
}

// ============================================================================
//  FUNCTIONS
// ============================================================================

#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1)
void _pmd_inc_CoreVoltage()
{
    pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_HIGH);
}

void _pmd_dec_CoreVoltage()
{
    pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_LOW);
}
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1

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
// -----------------------------------------------------------------------------
// array to store information for shared pins:
// UINT32 g_pmdMapAccess.powerInfo[PMD_POWER_ID_QTY];
// the first ID of a share has bit 31 = 0 and lower bits represent resource used
// the other IDs have bit 31 = 1 and lower bits are the ID of the first one
// note: if more that 31 IDs are needed
// change the bit usage: instead of indexing with the bits with ID use bit 0
// for first, then add the bit to the LINK information (need 5 bits)
// then we can use IDs as we want but a shared group is limited to 31 IDs for
// the same pin.
// =============================================================================
PUBLIC VOID pmd_Open(CONST PMD_CONFIG_T* pmdConfig)
{
#ifdef SPI_REG_DEBUG
    extern VOID pmd_RegisterSpiRegCheckFunc(VOID);
    pmd_RegisterSpiRegCheckFunc();
#endif // SPI_REG_DEBUG

    PMD_ASSERT(PMD_POWER_ID_QTY < 31,
        "Expected PMD_POWER_ID_QTY < 31, implementation of g_pmdMapAccess.powerInfo must be updated to support more IDs",
        PMD_POWER_ID_QTY);
    PMD_ASSERT(PMD_POWER_ID_QTY + PMD_LEVEL_ID_QTY <= 31,
        "Too many LDO users for current LDO user bitmap: powerIds=%d, levelIds=%d",
        PMD_POWER_ID_QTY, PMD_LEVEL_ID_QTY);
	 PMD_ASSERT(PMD_POWER_ID_QTY == PMD_MAP_POWER_ID_QTY, "must be in sync with pmd_map.xmd PMD_MAP_POWER_ID_QTY");
      PMD_ASSERT(PMD_LEVEL_ID_QTY == PMD_MAP_LEVEL_ID_QTY, "must be in sync with pmd_map.xmd PMD_MAP_LEVEL_ID_QTY");
    PMD_ASSERT(pmdConfig != NULL, "pmdConfig must be not NULL.");
    g_pmdConfig = pmdConfig;
    g_pmdInitDone = FALSE;

    // LDO VCORE value->voltage (estimated):
    // 7->0.88, 6->0.99, 5->1.10, 4->1.22, 3->1.33, 2->1.44, 1->1.55, 0->1.66
    // NOTE:
    // On Gallite chip with version 6 or before, it might lead to unintended power-off
    // if the difference between DCDC VCORE and LDO VCORE is too large.
    // LDO VCORE will be powered on and set to the highest voltage once entering active mode.
    // (and will be powered off AFTER entering active mode to save energy) --
    // see pmd_SetPowerMode().
    // Otherwise, there might be a current shortage in the interval switching from
    // low LDO VCORE voltage (LP mode) to high DCDC VCORE voltage (active mode),
    // as DCDC VCORE needs a period of stable time.
    g_pmdLdoVcoreValueActive = 0;
    g_pmdLdoVcoreValueLp = 7;

    // DCDC VCORE value->voltage:
    // 0->1.00, 1->1.17, 2->1.32, 3->1.45, 4->1.59, 5->1.77, 6->1.93, 7->2.03
#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1)
    // A lower DCDC VCORE value in dyn mode leads to power-off after the first deep sleep,
    // and automatic hung-up during a call after the first screen sleep.
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW]  = 2;
#else // DYNAMIC_DCDC_VCORE_ADJUSTMENT != 1
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW]  = 1;
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT != 1
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = 3;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH]  = 4;
    g_pmdDcdcVcoreValueLp = 1;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    // Check chip version
#if 0
    const UINT32 GALLITE_CHIP_ID_VERSION_5 = 0x11351001;
    const UINT32 GALLITE_CHIP_ID_VERSION_6 = 0x11351001;
    UINT32 chipId = GALLITE_CHIP_ID_VERSION_5;
    chipId = hal_SysGetChipVersion();
#else
    const UINT32 GALLITE_CHIP_ID_VERSION_5 = 0x18880;
    const UINT32 GALLITE_CHIP_ID_VERSION_6 = 0x188A0;
    extern VOID xcvBuildSpiCmd (HAL_RFSPI_CMD_T *cmd, UINT8 address, UINT32 data);
    extern VOID xcvReadReg (UINT16 address, UINT8 *output);
//tianwq    extern const HAL_RFSPI_CFG_T g_spiCfg;
//tianwq    hal_RfspiOpen(&g_spiCfg, HAL_RFSPI_19_5M_CLK);
#ifndef _T_UPGRADE_PROGRAMMER
//tianwq    HAL_RFSPI_CMD_T* confCmd = hal_RfspiNewSingleCmd();
//tianwq        xcvBuildSpiCmd (confCmd, 0x06, 0x3f333); // SPI-Mode Select
//tianwq        hal_RfspiSendCmd(confCmd);
#endif
    UINT32 chipId = GALLITE_CHIP_ID_VERSION_5;
//tianwq        xcvReadReg(0, (UINT8*)&chipId);
#endif

    if (chipId == GALLITE_CHIP_ID_8805_V5)
    {
        // --- Gallite chip version is 5 or before ---
        // V5 Gallite chip is the first version that sold to the customers
        // with different DCDC VCORE value/voltage table.
        // DCDC VCORE value->voltage:
        // 2->1.08, 3->1.23, 4->1.38, 5->1.54, 6->1.69, 7->1.86
        g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW]  = 3;
        g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH]  = 5;
        g_pmdDcdcVcoreValueLp = 3;
        // high RTC voltage will lead to BFI.
        g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].VRtcHigh = FALSE;
        g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].VRtcHigh = FALSE;
    }
    else if (chipId == GALLITE_CHIP_ID_8805_V6)
    {
        // --- Gallite chip version is 6 or after ---
    }

    // Set default LDO on, more will be updated by power[] and level[]
    g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].reg &=
                            ~(g_pmdConfig->ldoEnableNormal & OPAL_LDO_EN_MASK);
    g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].reg &=
                            ~(g_pmdConfig->ldoEnableLowPower & OPAL_LDO_EN_MASK);

    {
        UINT32 ldoVoltSelMaskActive = 0;
        UINT32 ldoVoltSelActive = 0;
        
        UINT32 ldoVoltSelMaskLp = 0;
        UINT32 ldoVoltSelLp = 0;
        
        UINT32 voltSel = 0;
        
        ldoVoltSelMaskActive = RDA1203_ACT_MIC_SEL_MASK|RDA1203_ACT_VIBRATE_SEL_MASK|
                                            RDA1203_ACT_MMC_SEL_MASK|RDA1203_ACT_LCD_SEL_MASK|
                                            RDA1203_ACT_CAMERA_SEL_MASK|RDA1203_ACT_RF_SEL_MASK;
        
        voltSel = (g_pmdConfig->ldoMicIs2_8) ? RDA1203_ACT_MIC_SEL1V57 : RDA1203_ACT_MIC_SEL1V8;
        ldoVoltSelActive |= voltSel;
        if (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].VMic == POWER_ON)
        {
            ldoVoltSelMaskLp |= RDA1203_ACT_MIC_SEL_MASK;
            ldoVoltSelLp |= voltSel;
        }
        voltSel = (g_pmdConfig->ldoIbrIs2_8) ? RDA1203_ACT_VIBRATE_SEL2V8 : RDA1203_ACT_VIBRATE_SEL1V8;
        ldoVoltSelActive |= voltSel;
        if (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vibr == POWER_ON)
        {
            ldoVoltSelMaskLp |= RDA1203_ACT_VIBRATE_SEL_MASK;
            ldoVoltSelLp |= voltSel;
        }
        voltSel = (g_pmdConfig->ldoMMCIs2_8) ? RDA1203_ACT_MMC_SEL2V8 : RDA1203_ACT_MMC_SEL1V8;
        ldoVoltSelActive |= voltSel;
        if (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vmmc == POWER_ON)
        {
            ldoVoltSelMaskLp |= RDA1203_ACT_MMC_SEL_MASK;
            ldoVoltSelLp |= voltSel;
        }
        voltSel = (g_pmdConfig->ldoLcdIs2_8) ? RDA1203_ACT_LCD_SEL2V8 : RDA1203_ACT_LCD_SEL1V8;
        ldoVoltSelActive |= voltSel;
        if (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vlcd == POWER_ON)
        {
            ldoVoltSelMaskLp |= RDA1203_ACT_LCD_SEL_MASK;
            ldoVoltSelLp |= voltSel;
        }
        voltSel = (g_pmdConfig->ldoCamIs2_8) ? RDA1203_ACT_CAMERA_SEL2V8 : RDA1203_ACT_CAMERA_SEL1V8;
        ldoVoltSelActive |= voltSel;
        if (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vcamera == POWER_ON)
        {
            ldoVoltSelMaskLp |= RDA1203_ACT_CAMERA_SEL_MASK;
            ldoVoltSelLp |= voltSel;
        }
        voltSel = (g_pmdConfig->ldoRfIs2_8) ? RDA1203_ACT_RF_SEL2V8 : RDA1203_ACT_RF_SEL1V8;
        ldoVoltSelActive |= voltSel;
        if (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].VRF == POWER_ON)
        {
            ldoVoltSelMaskLp |= RDA1203_ACT_RF_SEL_MASK;
            ldoVoltSelLp |= voltSel;
        }

        g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] &= ~ldoVoltSelMaskActive;
        g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] |= ldoVoltSelActive;
        g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] &= ~ldoVoltSelMaskLp;
        g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] |= ldoVoltSelLp;
    }

#if (CHECK_LDO_USER_BITMAP == 1)
    // Init LDO user bitmap
    for (UINT32 i=0; i<PMD_LDO_QTY; i++)
    {
        g_pmdLdoUserBitmap[OPAL_PROFILE_MODE_NORMAL][i] = 0;
        g_pmdLdoUserBitmap[OPAL_PROFILE_MODE_LOWPOWER][i] = 0;
    }
#endif // (CHECK_LDO_USER_BITMAP == 1)

    BOOL activateStatus = pmd_SpiCsNonblockingActivate(FALSE);
    PMD_ASSERT(activateStatus, "PMD: Activation failled, ISPI must be free for pmd_Open !");

    // Init LCD/LED setting
    pmd_OpalSpiWrite(RDA_ADDR_LED_SETTING1, g_pmdLcd_Ctrl);
    // Init SIM setting
    pmd_OpalSpiWrite(RDA_ADDR_SIM_INTERFACE, g_pmdSimSelect);
    // Init voltage selection
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL]);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER]);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING4, 0x493c);
    UINT32 id;
    // power
    for (id = 0; id < PMD_POWER_ID_QTY; id++)
    {
#if (CHECK_LDO_USER_BITMAP == 1)
        g_pmdMapAccess.powerInfo[id] = 0;
#else // (CHECK_LDO_USER_BITMAP != 1)
        // also prepare the g_pmdMapAccess.powerInfo data
        if ( g_pmdConfig->power[id].shared )
        {
            PMD_POWER_ID_T firstId=0;
            // uses .opal to get a valid number but as it's an union it's just the numerical value here.
            while ((firstId <= id) && (g_pmdConfig->power[id].ldo.opal != g_pmdConfig->power[firstId].ldo.opal))
            {
                firstId++;
            }
            PMD_ASSERT( g_pmdConfig->power[firstId].shared == TRUE, "PMD: ID %d is using same ldo as ID %d but the first is not set as SHARED!", firstId, id)
            if (firstId == id)
            { // the first
                g_pmdMapAccess.powerInfo[id] = 0;
            }
            else
            { // the others link to the first
                g_pmdMapAccess.powerInfo[id] = PMD_MAP_ACCESS_T_POWER_SHARED_LINK_FLAG | firstId;
                PMD_ASSERT( g_pmdConfig->power[firstId].polarity == g_pmdConfig->power[id].polarity, "PMD: shared ID %d and ID %d do not have the same POLARITY!", firstId, id)
            }
        }
#endif // (CHECK_LDO_USER_BITMAP != 1)

        // initial state as described by powerOnState from config
        BOOL powerOn = g_pmdConfig->power[id].powerOnState;
#ifdef FASTPF_USE_USB
        if (id == PMD_POWER_USB)
        {
            powerOn = TRUE; // For USB flash programmer
        }
#endif // FASTPF_USE_USB
        pmd_EnablePower(id, powerOn);
        // For GPIO, set it to output mode
        if (g_pmdConfig->power[id].ldo.type == PMD_LDO_ID_TYPE_IO)
        {
            hal_GpioSetOut(g_pmdConfig->power[id].ldo.pin.gpioId);
        }
    }

    // level
    for (id = 0; id < PMD_LEVEL_ID_QTY; id++)
    {
        // initial state as described by powerOnState from config
        pmd_SetLevel(id, g_pmdConfig->level[id].powerOnState);
        // For GPIO, set it to output mode
        if (g_pmdConfig->level[id].type == PMD_LEVEL_TYPE_LDO &&
            g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_IO)
        {
            hal_GpioSetOut(g_pmdConfig->level[id].ldo.pin.gpioId);
        }
    }

    // Step2 we need set the relevant profile register
    // Profile register 2 for LP mode use
    // Profile register 1 for normal use
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].reg);

    // battery monitor
    hal_AnaGpadcOpen(g_pmdConfig->batteryGpadcChannel, HAL_ANA_GPADC_ATP_2S);

    g_pmdMapAccess.chargerState = (PMD_MAP_CHARGER_STATE_T*)&g_pmdChargerState;

    pmd_InitRda1203();

    // Init touch screen setting -- disable ch1
    pmd_OpalSpiWrite(RDA_ADDR_TOUCH_SCREEN_CONTROL, 0xB81);
    UINT16 TSD_temp0 = pmd_OpalSpiRead(RDA_ADDR_AUDIO_CONTROL);
    TSD_temp0 |=0x3006;
    pmd_OpalSpiWrite(RDA_ADDR_AUDIO_CONTROL, TSD_temp0);

    pmd_OpalCsDeactivate(FALSE);
    g_pmdInitDone = TRUE;

    // Check if external audio PA exists
    if (g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].ldo.type != HAL_GPIO_TYPE_NONE)
    {
        g_pmdAudExtPaExists = TRUE;
    }

    // Earpiece detection
    pmd_EarpDetectGpioInit(g_pmdConfig->earpieceDetectGpio);
}


// =============================================================================
// pmd_Close
// -----------------------------------------------------------------------------
/// Closes the PMD driver.
// =============================================================================
PUBLIC VOID pmd_Close(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    // Turn off LCD backlight
    pmd_SetLevel(PMD_LEVEL_LCD, 0);
    pmd_SetLevel(PMD_LEVEL_SUB_LCD, 0);
    // Turn off keypad lights
    pmd_SetLevel(PMD_LEVEL_KEYPAD, 0);
    pmd_SetLevel(PMD_LEVEL_LED0, 0);
    pmd_SetLevel(PMD_LEVEL_LED1, 0);
    pmd_SetLevel(PMD_LEVEL_LED2, 0);
    pmd_SetLevel(PMD_LEVEL_LED3, 0);

    // ABB cannot be powered off, otherwise exception will occur
    // Fortunately, there is no pop sound in earpiece even if ABB is not powered off

    // Power off audio module
    hal_AudForcePowerDown();
    // Restore charger h/w setting
    pmd_RestoreChargerAtPowerOff();
    // 1) Disable EOM, PEN, Charger IRQs;
    // 2) Enter the power down mode (power controlled by PMU default profile).
    g_pmdIrqSettingLatest = 0;
    pmd_RegIrqSettingSet(RDA1203_EOMIRQ_MASK|RDA1203_PENIRQ_MASK|RDA1203_INT_CHR_MASK);

    hal_SysExitCriticalSection(scStatus);
}



// ============================================================================
// pmd_SetPowerMode
// ----------------------------------------------------------------------------
/// Set Power Mode to the desired mode
// ============================================================================
PUBLIC VOID PMD_FUNC_INTERNAL pmd_SetPowerMode(PMD_POWERMODE_T powerMode)
{
    // this function is in internal ram because it is called in critical conditions (ECO Mode) so NO TRACE IN HERE !
    // PMD_TRACE(PMD_INFO_TRC, 0, __func__);

    // the actual toggling of g_pmdConfig->ecoMode is done in hal_LpsDeepSleep
    // according to the setting done with hal_LpsSetControlPin()
    // This function is now called far before the critical point of entering 
    // the deepest low power mode and is responsible only for preparing
    // other stuff (like controlling V_ANA as bellow)

#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
    UINT32 vcore = 0;
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

    UINT32 scStatus = hal_SysEnterCriticalSection();

    switch (powerMode)
    {
        case PMD_LOWPOWER:
#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
            // DCDC VCORE voltage is decreased to reduce the gap between itself and
            // LDO VCORE voltage in LP mode.
            // The gap is smaller, it is less possible to trigger unintended power-off.
            vcore = g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW];
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

            // LDO VCORE will be powered on and set to the highest voltage once entering active mode.
            g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].VLdoCore = POWER_ON;

            break;
        case PMD_IDLEPOWER:
#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
            // DCDC VCORE voltage is increased back to a level that can support applications
            // with a high-system-frequency requirement.
            vcore = g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH];
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

            // LDO VCORE can be powered off since the active mode has been setup, and
            // DCDC VCORE should have been stable.
            // This can save about 10mA energy.
            g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].VLdoCore = POWER_OFF;

            // we are out of EcoMode, so we can do some checks using external
            // rams (data or functions)
            break;
        default:
            break;
    }

#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
    // DCDC VCORE voltage selection
    // NOTE:
    // If DCDC VCORE adjustment in pmd_SetCoreVoltage() is allowed, the same adjustment
    // should be removed here, since they are conflicted with each other.
    g_pmdLdoActiveSetting3 = (g_pmdLdoActiveSetting3 &
                                ~RDA1203_DCDC_VCORE_CURRENT_MASK) |
                             RDA1203_DCDC_VCORE_CURRENT(vcore);
    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoActiveSetting3);
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

    // LDO VCORE on/off
    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].reg);

    hal_SysExitCriticalSection(scStatus);
}



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
PUBLIC UINT32 pmd_ConfigureRfLowPower(BOOL xcvUseVRF, BOOL xcvLowPowerKeepVRFOn)
{
    union
    {
        UINT32 reg;
        HAL_LPS_POWERUP_CTRL_T bitfield;
    } puctrl;
    
    puctrl.reg = 0;

    // If called before pmd_Open (hal_BoardSetup) we don't configure the LDOs
    if (g_pmdInitDone)
    {
        // V_RF is not used by XCV on Gallite. But it might be used to control another component.
        // So we do NOT configure RF LDO here.
#if 0
        // assume VRF1 is the one connected to the xcvr
        // this will have to be changed to handle digrf LDO configuration
        const PMD_LDO_ID_T rfLdo = {.opal=PMD_LDO_RF};

        // on opal V_RF is an internal LDO
        // configure V_RF
        if (xcvUseVRF)
        {
            if (xcvLowPowerKeepVRFOn)
            {
                pmd_EnableOpalLdo(rfLdo, TRUE, OPAL_PROFILE_MODE_LOWPOWER, 0);
            }
            else
            {
                pmd_EnableOpalLdo(rfLdo, FALSE, OPAL_PROFILE_MODE_LOWPOWER, 0);
            }
            pmd_EnableOpalLdo(rfLdo, TRUE, OPAL_PROFILE_MODE_NORMAL, 0);
        }
        else
        {
            pmd_EnableOpalLdo(rfLdo, FALSE, OPAL_PROFILE_MODE_LOWPOWER, 0);
            pmd_EnableOpalLdo(rfLdo, FALSE, OPAL_PROFILE_MODE_NORMAL, 0);
        }
#endif
        // let LPSCO_0 be controlled by LPS FSM
        puctrl.bitfield.puCO0ForceOn = 0;
        // LP_MODE is active (low power) High
        // LPSCO_0 is active during normal mode, so invert the pin polarity
            puctrl.bitfield.puCO0Pol = 1;
    }
    else
    {
        // force off
        puctrl.bitfield.puCO0ForceOn = 1;
        puctrl.bitfield.puCO0Pol = 1;
    }
    // LPSCO_1 is not used by Opal, should be set as gpio in hal tgt
    // force off
    puctrl.bitfield.puCO1ForceOn = 1;
    puctrl.bitfield.puCO1Pol = 1;

    return puctrl.reg;
}



// ============================================================================
// pmd_SimSetVoltageClass
// ----------------------------------------------------------------------------
/// Set the voltage class
///     
/// The #pmd_SimVoltageOpen function controlling the level shifter must have 
/// previously called. \n
/// Voltage class must \b _ALWAYS_ be set to zero before re-programming 
/// voltage
/// @param voltClass Voltage Class to set
// ============================================================================
PRIVATE VOID pmd_SimSetVoltageClass(PMD_SIM_VOLTAGE_CLASS_T voltClass)
{
    BOOL enableLDO;
    const PMD_LDO_ID_T simLdo = {.opal=PMD_LDO_SIM1};

    UINT32 simVoltSel = 0;

    switch (voltClass)
    {
        case PMD_SIM_VOLTAGE_0V0:
            enableLDO = FALSE;
            simVoltSel |= RDA1203_ACT_SIM1_SEL1V8;    
            PMD_TRACE(PMD_INFO_TRC, 0, "sim voltage:0V");    
            break;

        case PMD_SIM_VOLTAGE_1V8:
            enableLDO = TRUE;
            simVoltSel |= RDA1203_ACT_SIM1_SEL1V8;    
            PMD_TRACE(PMD_INFO_TRC, 0, "sim voltage:1V8");           
            break;

        case PMD_SIM_VOLTAGE_3V0:
            enableLDO = TRUE;
            simVoltSel |= RDA1203_ACT_SIM1_SEL2V8;        
            PMD_TRACE(PMD_INFO_TRC, 0, "sim voltage:3V");
            break;

        case PMD_SIM_VOLTAGE_5V0:
            enableLDO = TRUE;
            simVoltSel |= RDA1203_ACT_SIM1_SEL2V8;        
            PMD_TRACE(PMD_INFO_TRC, 0, "sim voltage:5V");
            break;

        default:
            enableLDO = FALSE;
            simVoltSel |= RDA1203_ACT_SIM1_SEL1V8;    
            PMD_ASSERT(FALSE, "sim voltage: invalid voltage class %d", voltClass);                    
        break;
    }

    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (voltClass == PMD_SIM_VOLTAGE_0V0)
    {
        hal_SimDisableSci();
    }

    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] &= ~(RDA1203_ACT_SIM1_SEL_MASK|RDA1203_ACT_SIM2_SEL_MASK);
    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] |= simVoltSel;
    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] &= ~(RDA1203_ACT_SIM1_SEL_MASK|RDA1203_ACT_SIM2_SEL_MASK);
    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] |= simVoltSel;

    pmd_SimSPIWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL]); 
    pmd_SimSPIWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER]); 

    hal_SysExitCriticalSection(scStatus);

    if( enableLDO !=
        (g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vsim1 == POWER_ON) )
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "LOWPOWER:USIM En=%d, OldState=%d",
                enableLDO, g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vsim1);
        pmd_EnableOpalLdo(simLdo, enableLDO, OPAL_PROFILE_MODE_LOWPOWER, LEVEL_LDO_USER(PMD_LEVEL_SIM));
    }

    if( enableLDO !=
        (g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].Vsim1 == POWER_ON) )
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "NORMAL:USIM En=%d, OldState=%d",
                enableLDO, g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].Vsim1);
        pmd_EnableOpalLdo(simLdo, enableLDO, OPAL_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(PMD_LEVEL_SIM));
    }
    if (voltClass != PMD_SIM_VOLTAGE_0V0)
    {
        hal_SimDisableSci();
}

#if 0 // tianwq
DUALSIM_ERR_T pmd_DualSimSetVoltageClass(DUALSIM_SIM_VOLTAGE_T voltClass0,DUALSIM_SIM_VOLTAGE_T voltClass1)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_DualSimSetVoltageClass classA = %d , classB = %d ",voltClass0,voltClass1);

    BOOL enableSim1LDO = TRUE, enableSim2LDO = TRUE;
    UINT32 simVoltSel = 0;

    switch (voltClass0)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            enableSim1LDO = FALSE;
            simVoltSel |= RDA1203_ACT_SIM1_SEL1V8;    
            PMD_TRACE(PMD_INFO_TRC, 0, "sim0 voltage:0V");    
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            simVoltSel |= RDA1203_ACT_SIM1_SEL1V8;    
            PMD_TRACE(PMD_INFO_TRC, 0, "sim0 voltage:1V8");           
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            simVoltSel |= RDA1203_ACT_SIM1_SEL2V8;        
            PMD_TRACE(PMD_INFO_TRC, 0, "sim0 voltage:3V");
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            simVoltSel |= RDA1203_ACT_SIM1_SEL2V8;        
            PMD_TRACE(PMD_INFO_TRC, 0, "sim0 voltage:5V");
            break;

        default:
            PMD_ASSERT(FALSE, "sim0 voltage: invalid voltage class %d", voltClass0);                    
        break;
    }

        
    switch (voltClass1)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            enableSim2LDO = FALSE;
            simVoltSel |= RDA1203_ACT_SIM2_SEL1V8;    
            PMD_TRACE(PMD_INFO_TRC, 0, "sim1 voltage:0V");    
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            simVoltSel |= RDA1203_ACT_SIM2_SEL1V8;    
            PMD_TRACE(PMD_INFO_TRC, 0, "sim1 voltage:1V8");           
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            simVoltSel |= RDA1203_ACT_SIM2_SEL2V8;        
            PMD_TRACE(PMD_INFO_TRC, 0, "sim1 voltage:3V");
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            simVoltSel |= RDA1203_ACT_SIM2_SEL2V8;        
            PMD_TRACE(PMD_INFO_TRC, 0, "sim1 voltage:5V");
            break;

        default:
            PMD_ASSERT(FALSE, "sim1 voltage: invalid voltage class %d", voltClass1);                    
        break;
    }

    UINT32 scStatus = hal_SysEnterCriticalSection();
    if ( voltClass0 == PMD_SIM_VOLTAGE_0V0 && voltClass1 == PMD_SIM_VOLTAGE_0V0)
    {
        hal_SimDisableSci();
    }

    g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].Vsim1 =
                        (enableSim1LDO) ? POWER_ON : POWER_OFF;
    g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vsim1 =
                        (enableSim1LDO) ? POWER_ON : POWER_OFF;
    g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].Vsim2 =
                        (enableSim2LDO) ? POWER_ON : POWER_OFF;
    g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].Vsim2 =
                        (enableSim2LDO) ? POWER_ON : POWER_OFF;

    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] &= ~(RDA1203_ACT_SIM1_SEL_MASK|RDA1203_ACT_SIM2_SEL_MASK);
    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL] |= simVoltSel;
    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] &= ~(RDA1203_ACT_SIM1_SEL_MASK|RDA1203_ACT_SIM2_SEL_MASK);
    g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER] |= simVoltSel;

    pmd_SimSPIWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdProfileRegs[OPAL_PROFILE_MODE_NORMAL].reg);
    pmd_SimSPIWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdProfileRegs[OPAL_PROFILE_MODE_LOWPOWER].reg);
    pmd_SimSPIWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_NORMAL]);
    pmd_SimSPIWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLDO_Out_V_Ctrl[OPAL_PROFILE_MODE_LOWPOWER]);
    if ( ! (voltClass0 == PMD_SIM_VOLTAGE_0V0 && voltClass1 == PMD_SIM_VOLTAGE_0V0) )
    {
        hal_SimEnableSci();
    }

    hal_SysExitCriticalSection(scStatus);

    return DUALSIM_ERR_NO;
}


BOOL pmd_SelectSimCard(UINT8 sim_card)
{
    if (sim_card == 0)
    {
        g_pmdSimSelect = g_pmdSimSelect & ~(RDA1203_SIM_SELECT);
    }
    else
    {
        g_pmdSimSelect = g_pmdSimSelect | RDA1203_SIM_SELECT;
    }
    if (pmd_SimSPIWrite(RDA_ADDR_SIM_INTERFACE, g_pmdSimSelect) == FALSE)
    {
        return FALSE;
    }
    
    return TRUE;
}
#endif

#if (CONFIG_CLASS_K_SPEAKER_PA)
VOID pmd_EnableClassKLoudSpeaker(VOID)
{
    HAL_APO_ID_T apo = g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].ldo.pin;

    VOID (*gpioActive)(HAL_APO_ID_T apo);
    VOID (*gpioInactive)(HAL_APO_ID_T apo);
    if (g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].polarity)
    {
        gpioActive = &hal_GpioSet;
        gpioInactive = &hal_GpioClr;
    }
    else
    {
        gpioActive = &hal_GpioClr;
        gpioInactive = &hal_GpioSet;
    }

    // Generate a pulse sequence to configure class K audio PA

    // TODO:
    //     Change the following codes to meet your PA specification.
    // CAUSION:
    //     hal_TimDelay(1) is about 61 us. If your PA needs a smaller
    // pulse width, please use for(...)/while(...), assuming the CPU
    // is running on 104 MHz.
    volatile int i=0;
    (*gpioInactive)(apo);
    for(i=0;i<30;i++);
    (*gpioActive)(apo);
    for(i=0;i<30;i++);
    (*gpioInactive)(apo);
    for(i=0;i<30;i++);
    (*gpioActive)(apo);
    for(i=0;i<30;i++);
    (*gpioInactive)(apo);
    for(i=0;i<30;i++);
    (*gpioActive)(apo);
    for(i=0;i<30;i++);
    (*gpioInactive)(apo);
    for(i=0;i<30;i++);
    (*gpioActive)(apo);
}
#endif // CONFIG_CLASS_K_SPEAKER_PA

#ifdef VMIC_POWER_ON_WORKAROUND
#include "hal_rda_audio.h"
#endif // VMIC_POWER_ON_WORKAROUND

// ============================================================================
// pmd_EnablePower 
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param id the id of the power to switch from enum #PMD_POWER_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_EnablePower(PMD_POWER_ID_T id, BOOL on)
{
    BOOL newState;
    UINT32 callerAddr = 0;

    SXS_GET_RA(&callerAddr);
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_EnablePower id=%i on=%i from 0x%x", id, on, callerAddr);

#if (CHECK_LDO_USER_BITMAP == 1)
    newState = on;
    g_pmdMapAccess.powerInfo[id] = newState;
#else // (CHECK_LDO_USER_BITMAP != 1)
    if ( g_pmdConfig->power[id].shared )
    { // shared pin
        PMD_POWER_ID_T firstId = id;
        // find the first
        if (g_pmdMapAccess.powerInfo[id] & PMD_MAP_ACCESS_T_POWER_SHARED_LINK_FLAG)
        {
            firstId = g_pmdMapAccess.powerInfo[id] & ~PMD_MAP_ACCESS_T_POWER_SHARED_LINK_FLAG;
        }
        // update info status
        if (on)
        {
            g_pmdMapAccess.powerInfo[firstId] |= (1 << id);
        }
        else
        {
            g_pmdMapAccess.powerInfo[firstId] &= ~(1 << id);
        }
        // update LDO
        newState = (g_pmdMapAccess.powerInfo[firstId])?TRUE:FALSE;
    }
    else
    { // direct LDO (no sharing)
        newState = on;
        // update info status (to be Coolwatchable)
        g_pmdMapAccess.powerInfo[id] = newState;
    }
#endif // (CHECK_LDO_USER_BITMAP != 1)

    if (id == PMD_POWER_USB)
    {
        // Power on/off USB PHY
        // If v_usb is shared by other modules, it might NOT be powered down when
        // USB is requested to be turned off. To avoid impacting USB reset, USB PHY
        // must be directly controlled (not by baseband USB module).
        if (on)
        {
            //hal_AbbRegWrite(USB_CONTROL, 0x2418);
        }
        else
        {
            //hal_AbbRegWrite(USB_CONTROL, 0x2419);
        }
    }

    // actual update according to newState.
    if (g_pmdConfig->power[id].ldo.type == PMD_LDO_ID_TYPE_OPAL)
    {
        pmd_EnableOpalLdo(g_pmdConfig->power[id].ldo, newState,
                                        OPAL_PROFILE_MODE_NORMAL, POWER_LDO_USER(id));
        
        if (id == PMD_POWER_EARPIECE)
        {
            // VMIC needs 15 ms to be stable, so it can NOT always be off during low power
            pmd_EnableOpalLdo(g_pmdConfig->power[id].ldo, newState,
                                            OPAL_PROFILE_MODE_LOWPOWER, POWER_LDO_USER(id));
            
#ifdef VMIC_POWER_ON_WORKAROUND
            if (g_pmdInitDone)
            {
                // After PMD is initialized, PMD_POWER_EARPIECE can NOT be on/off in the context
                // of interrupt or idle task, as it needs to call sxr_Sleep to retry.
                if (on)
                {
                    while (!hal_AudOpen(HAL_AUD_USER_EARPIECE, NULL))
                    {
                        sxr_Sleep(AUD_TIME_RETRY);
                    }
                }
                else
                {
                    // Release aud resource.
                    while (!hal_AudClose(HAL_AUD_USER_EARPIECE))
                    {
                        sxr_Sleep(AUD_TIME_RETRY);
                    }
                }
            }
            else
            {
                if (on)
                {
                    // When PMD is not initialized, OS is not ready either, and we cannot call sxr_Sleep.
                    extern BOOL hal_AudEarpieceDetectInit(VOID);
                    hal_AudEarpieceDetectInit();
                }
            }
#endif // VMIC_POWER_ON_WORKAROUND
        }
    }
    else
    {   // Pin
        // For pin-controlled power items, e.g., external loud speaker
#if (CONFIG_CLASS_K_SPEAKER_PA)
        if (id == PMD_POWER_LOUD_SPEAKER && newState)
        {
            pmd_EnableClassKLoudSpeaker();
        }
        else
#endif // CONFIG_CLASS_K_SPEAKER_PA
        {
        if ( (newState && !g_pmdConfig->power[id].polarity) ||
              (!newState && g_pmdConfig->power[id].polarity) )
        {
            hal_GpioClr(g_pmdConfig->power[id].ldo.pin);
        }
        else
        {
            hal_GpioSet(g_pmdConfig->power[id].ldo.pin);
        }
    }
}
}


// ============================================================================
// pmd_SetLevel(PMD_LEVEL_LOUD_SPEAKER)
// ----------------------------------------------------------------------------
/// Set a Level to desired value. PMD_LEVEL_LCD
///
/// @param id the id of the level to set from enum #PMD_LEVEL_ID_T
/// @param level the desired value, its acutal type depends on the \p id
// ============================================================================
PUBLIC VOID pmd_SetLevel(PMD_LEVEL_ID_T id, UINT32 level)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SetLevel id=%i level=%i", id, level); 
    
    UINT32 status;
    
    switch (g_pmdConfig->level[id].type)
    {
        case PMD_LEVEL_TYPE_NONE:
            break;
        case PMD_LEVEL_TYPE_PWL0:
            if (level > 7)
            {
                level = 7;
            }
            // HAL PWL0 is connected to LCD backlight control and is DISABLED
            // by direct register in this driver.
            hal_PwlSelLevel(HAL_PWL_0,g_pmdConfig->lightLevelToPwm0[level]);
            break;
        case PMD_LEVEL_TYPE_PWL1:
            if (level > 7)
            {
                level = 7;
            }
            hal_PwlSelLevel(HAL_PWL_1,g_pmdConfig->lightLevelToPwm1[level]);
            break;
        case PMD_LEVEL_TYPE_LPG:
            PMD_ASSERT(FALSE, "TODO impelment LPG", id);
            break;
        case PMD_LEVEL_TYPE_LDO:
            PMD_ASSERT(g_pmdConfig->level[id].ldo.type != PMD_LDO_ID_TYPE_INVALID,
                                    "Invalid LDO type: id=%d, type=%d",
                                    id, g_pmdConfig->level[id].ldo.type);
            if (g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_OPAL)
            {   // Opal LDO
                pmd_EnableOpalLdo(g_pmdConfig->level[id].ldo, (0 == level)?FALSE:TRUE,
                                  OPAL_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));

                if (id == PMD_LEVEL_VIBRATOR)
                {
                    // Allow vibrator to work in LP mode
                    pmd_EnableOpalLdo(g_pmdConfig->level[id].ldo, (0 == level)?FALSE:TRUE,
                                      OPAL_PROFILE_MODE_LOWPOWER, LEVEL_LDO_USER(id));
                }
            }
            else
            {   // Pin
                switch (id)
                {
                    case PMD_LEVEL_LCD:
                    case PMD_LEVEL_KEYPAD:
                    case PMD_LEVEL_LED0:
                    case PMD_LEVEL_LED1:
                    case PMD_LEVEL_LED2:
                    case PMD_LEVEL_LED3:
                        if (0 == level)
                        {
                            hal_GpioClr(g_pmdConfig->level[id].ldo.pin);
                        }
                        else
                        {
                            hal_GpioSet(g_pmdConfig->level[id].ldo.pin);
                        }
                        break;
                    default:
                        PMD_ASSERT(g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_NONE,
                                                "Invalid level LDO config: id=%d, type=%d",
                                                id, g_pmdConfig->level[id].ldo.type);
                        break;
                }
            }
            break;
        case PMD_LEVEL_TYPE_OPAL:
            switch (id)
            {
                case PMD_LEVEL_SIM:
                    pmd_SimSetVoltageClass(level);
                    break;
                case PMD_LEVEL_KEYPAD:
                case PMD_LEVEL_LED0:
                case PMD_LEVEL_LED1:
                case PMD_LEVEL_LED2:
                case PMD_LEVEL_LED3:
                    {
                        // Keypad LED backlight control is driven by PMU directly instead of HAL PWL0.
                        if (level > 7)
                        {
                            level = 7;
                        }
                        // Get a value from the PWM0 config in the target
                        level = g_pmdConfig->lightLevelToPwm0[level];
                        // Change value range from [0, 255] (for HAL PWM) to [0, 31] (for PMU PWM)
                        level = (level & 0xff) >> 3;
                        
                        status = hal_SysEnterCriticalSection();
                        if (0 == level)
                        {
                            g_pmdLcd_Ctrl |= (RDA1203_PD_KP_ACT);
                        }
                        else
                        {
                            g_pmdLcd_Ctrl &= ~(RDA1203_PD_KP_ACT);
                            g_pmdLedSetting3 = (g_pmdLedSetting3&~RDA1203_PWM_KP_DUTY_CYCLE_MASK) |
                                    RDA1203_PWM_KP_DUTY_CYCLE(level);
                        }
                        hal_SysExitCriticalSection(status);
                        
                        pmd_RDAWrite(RDA_ADDR_LED_SETTING1, g_pmdLcd_Ctrl);
                        if (0 != level)
                        {
                            pmd_RDAWrite(RDA_ADDR_LED_SETTING3, g_pmdLedSetting3);
                        }
                    }
                    break;
                case PMD_LEVEL_LOUD_SPEAKER:
                    // Speaker level is controlled by ABB rather than PMU on Gallite
                default:
                    PMD_ASSERT(FALSE, "Trying to use an invalid id for type PMD_LEVEL_TYPE_OPAL", id);
                    break;
            }
            break;
        case PMD_LEVEL_TYPE_BACKLIGHT:
            {
                if (level > 7)
                {
                    level = 7;
                }
                //PMD_TRACE(PMD_DBG_TRC|TSTDOUT, 0,  "PMD_LEVEL_TYPE_BACKLIGHT level=%d",level);

                status = hal_SysEnterCriticalSection();
                if (0 == level)
                {
                    g_pmdLcd_Ctrl |= (RDA1203_PD_BL_ACT);
                }
                else
                {
                    g_pmdLcd_Ctrl &= ~(RDA1203_PD_BL_ACT);
                }
                hal_SysExitCriticalSection(status);
                
                if (0 == level)
                {
                    pmd_RDAWrite(RDA_ADDR_LED_SETTING1,g_pmdLcd_Ctrl);
                }
                else
                {
#if 0
                    if (hal_GpioGet(HAL_GPIO_0) == 1)
                    {
                         sxr_StartFunctionTimer(2 * HAL_TICK1S,tsd_EnableRdaPen,(VOID*)NULL,0x03);
                    }
#endif
                    pmd_RDAWrite(RDA_ADDR_LED_SETTING1,g_pmdLcd_Ctrl);
                    pmd_RDAWrite(RDA_ADDR_LED_SETTING2,g_pmdConfig->lightLevelToRda1203[level]);
                }
            }
            break;
        default:
            PMD_ASSERT(FALSE, "Trying to use an invalid type of level", g_pmdConfig->level[id].type);
            break;
    }

    if (id == PMD_LEVEL_LCD)
    {
        if (0 == level)
        {
            g_pmdBackLightOn = FALSE;
            pmd_SetHighActivity(FALSE);
        }
        else
        {
            g_pmdBackLightOn = TRUE;
            pmd_SetHighActivity(TRUE);

        }
    }

    g_pmdMapAccess.levelInfo[id]=level;
}

#if 0 // tianwq
PUBLIC UINT32 pmd_CalibStartCustom(UINT32 command,
                                   CALIB_PMD_T* pmdCalibStruct)
{
  return 1;
}


// ============================================================================
// pmd_CalibGetDefault
// ----------------------------------------------------------------------------
/// Returns the default values of the PMD calibration.
/// Used by the Calib module to get the default of each PMD implementation.
///
/// @return Default values for the PMD calib parameters.
// ============================================================================
PUBLIC CONST CALIB_PMD_T* pmd_CalibGetDefault(VOID)
{
    return &g_pmdCalibDefault;
}
#endif

VOID pmd_InitLdoSetting(VOID)
{ 
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, RDA1203_LDO_SIM2 | RDA1203_LDO_SIM1 |
                        RDA1203_LDO_MMC | RDA1203_LDO_ABB | RDA1203_LDO_RF |
                        RDA1203_LDO_USB | RDA1203_LDO_VIBRATE | RDA1203_LDO_MIC |
                        RDA1203_LDO_CAMERA | RDA1203_LDO_LCD);
}


// ============================================================================
// pmd_EnableMemoryPower
// ----------------------------------------------------------------------------
/// Power up/down memory
// ============================================================================
PUBLIC VOID pmd_EnableMemoryPower(BOOL on)
{
    PMD_LDO_ID_T ldo;
    ldo.opal = PMD_LDO_MEM;
    pmd_EnableOpalLdo(ldo, on, OPAL_PROFILE_MODE_NORMAL, DEFAULT_LDO_USER);
}


PUBLIC BOOL pmd_RegRead(RDA_REG_MAP_T regIdx, UINT32 *pData);
PUBLIC BOOL pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT32 data);

// ============================================================================
// pmd_SetCoreVoltage
// ----------------------------------------------------------------------------
/// Set the core voltage level in active/idle mode
// ============================================================================
PUBLIC VOID pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_T coreVolt)
{
#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
    // It has risks to dynamically change DCDC VCORE, as
    // DCDC VCORE voltage stable time is about 100 us in a voltage change.
    // NOTE:
    // If this change is allowed, DCDC VCORE voltage adjustment in pmd_SetPowerMode()
    // should be removed since they are conflicted with each other.
    if (g_pmdInitDone) return;
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

    UINT32 ret;
    // Non-blocking register read (to avoid issues in an IRQ)
    if (pmd_RegRead(RDA_ADDR_LDO_ACTIVE_SETTING3, &ret))
    {
        g_pmdLdoActiveSetting3 = ret;
#if 0
        CONST UINT32 vio = 0, vmem = 0;
        g_pmdLdoActiveSetting3 = (g_pmdLdoActiveSetting3 & ~(0x7 << 3)) | ((vio & 0x7) << 3);
        g_pmdLdoActiveSetting3 = (g_pmdLdoActiveSetting3 & ~(0x7 << 6)) | ((vmem & 0x7) << 6);
#endif
    }

    // Set DCDC VCORE
    UINT32 vcore;
    vcore = g_pmdDcdcVcoreValueActiveAry[coreVolt];
    g_pmdLdoActiveSetting3 = (g_pmdLdoActiveSetting3 &
                                ~RDA1203_DCDC_VCORE_CURRENT_MASK) |
                             RDA1203_DCDC_VCORE_CURRENT(vcore);
    // Set LDO VCORE
    vcore = g_pmdLdoVcoreValueActive;
    g_pmdLdoActiveSetting3 = (g_pmdLdoActiveSetting3 &
                                ~RDA1203_LDO_VCORE_CURRENT_MASK) |
                             RDA1203_LDO_VCORE_CURRENT(vcore);

    pmd_RegWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoActiveSetting3);

#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1)
#if 0
    // Wait until the voltage becomes stable
    if (g_pmdInitDone)
    {
        // DCDC VCORE voltage stable time is about 100 us in a voltage change
        // Freq might be changed in IRQ context, e.g., PAL
        hal_TimDelay(2);
    }
#endif
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1
}


VOID pmd_InitRda1203(VOID)
{
    // Set VCORE voltage in active mode
    pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_HIGH);

    // Set VCORE voltage in LP mode
    UINT16 ret;
    ret = pmd_OpalSpiRead(RDA_ADDR_LDO_LP_SETTING3);
    
    UINT32 vcore;
    vcore = g_pmdDcdcVcoreValueLp;
    ret = (ret & ~RDA1203_DCDC_VCORE_CURRENT_MASK) |
            RDA1203_DCDC_VCORE_CURRENT(vcore);
    vcore = g_pmdLdoVcoreValueLp;
    ret = (ret & ~RDA1203_LDO_VCORE_CURRENT_MASK) |
            RDA1203_LDO_VCORE_CURRENT(vcore);
    
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING3, ret);

    // Disable touch screen irqs (they will be enabled later if needed)
    // Enter active mode
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS,
            RDA1203_EOMIRQ_MASK|RDA1203_PENIRQ_MASK|RDA1203_PD_MODE_SEL);

    pmd_InitLdoSetting();
    pmd_InitCharger();
}

PUBLIC UINT32 pmd_GetBatteryGpadcChannel(VOID)
{
    return g_pmdConfig->batteryGpadcChannel;
}

VOID pmd_TsdClearEomIrqInternal(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    UINT16 ret = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
    ret = ret | RDA1203_EOMIRQ_CLEAR;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS,ret);

    hal_SysExitCriticalSection(scStatus);
    return;
}

VOID pmd_TsdResetTouch(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    UINT16 ret = pmd_OpalSpiRead(RDA_ADDR_MISC);
    ret = ret | (1<<12);
    ret = ret & ~ (1<<11);
    pmd_OpalSpiWrite(RDA_ADDR_MISC,ret);
    ret = ret | (1<<11);
    ret = ret & ~ (1<<12);
    pmd_OpalSpiWrite(RDA_ADDR_MISC,ret);

    hal_SysExitCriticalSection(scStatus);
    return;
}

BOOL pmd_ResetTouch(VOID)
{   
    if (FALSE == pmd_TSDGetSPI())
    {
        return FALSE;
    }
    pmd_TsdResetTouch();
    return pmd_TSDReleaseSPI();
}

VOID pmd_TsdClearTouchIrqInternal(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    UINT16 ret = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
    ret = ret | RDA1203_EOMIRQ_CLEAR |RDA1203_PENIRQ_CLEAR;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS,ret);

    hal_SysExitCriticalSection(scStatus);
    return;
}

BOOL pmd_TsdClearTouchIrq(VOID)
{   
    if (FALSE == pmd_TSDGetSPI())
    {
        return FALSE;
    }
    pmd_TsdClearTouchIrqInternal();
    return pmd_TSDReleaseSPI();
}

BOOL pmd_TsdEnableIrq(VOID)
{
    if (FALSE == pmd_TSDGetSPI())
    {
        return FALSE;
    }

    UINT16 ret;
    ret = pmd_OpalSpiRead(RDA_ADDR_LDO_SETTINGS);
    // enable touch screen
    ret |= RDA1203_LDO_TSC;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS,ret);

    // clear previous pen irq
    pmd_TsdClearTouchIrqInternal();
    
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    ret = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
    // unmask pen irq
    ret &= ~RDA1203_PENIRQ_MASK;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS,ret);

    hal_SysExitCriticalSection(scStatus);
    
    return pmd_TSDReleaseSPI();
}

BOOL pmd_TsdDisableIrq(VOID)
{
    if (FALSE == pmd_TSDGetSPI())
    {
        return FALSE;
    }

    UINT16 ret;

    ret = pmd_OpalSpiRead(RDA_ADDR_LDO_SETTINGS);
    // disable touch screen
    ret &= ~RDA1203_LDO_TSC;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS,ret);

    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    ret = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
    ret |= RDA1203_PENIRQ_MASK;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS,ret);

    hal_SysExitCriticalSection(scStatus);

    return pmd_TSDReleaseSPI();
}


PUBLIC VOID pmd_IrqHandler(UINT8 interruptId)
{
    // IRQ handler cannot be blocked.
    // Here we write to PMU directly as charger IRQ is the only source
    // (EOM IRQ has been masked).
    UINT16 value = g_pmdIrqSettingLatest & 0xffff;
    value |= RDA1203_INT_CHR_MASK|RDA1203_INT_CHR_CLEAR;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, value);
    // Charger IRQ will be re-enabled once charger is plugged out
    // (in pmd_GetChargerHwStatus)

    // A timer will be started, and it will read the acutal charger status in non-blocking mode
    pmd_DcOnHandler(TRUE);
    
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_IrqHandler");
    return;
}


PROTECTED RDA1203_CHR_STATUS_T pmd_GetChargerHwStatus(VOID)
{
    RDA1203_CHR_STATUS_T status = CHR_STATUS_UNKNOWN;
    
    UINT16 ret;
    if (FALSE == pmd_TSDGetSPI())
    {
        return status;
    }
    
    ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    PMD_TRACE(PMD_DBG_TRC, 0, "charger: charge stautus  0x%x",ret);

    if ((ret & RDA1203_CHR_AC_ON) == 0)
    { 
        PMD_TRACE(PMD_DBG_TRC | TSTDOUT, 0, "charger: adapter off");
        status = CHR_STATUS_DC_OFF;
        
        UINT32 scStatus;
        scStatus = hal_SysEnterCriticalSection();

        UINT16 ret2;
        ret2 = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
        ret2 = ret2 & (~RDA1203_INT_CHR_MASK);
        pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, ret2);

        hal_SysExitCriticalSection(scStatus);
    }
    else
    {
        // RDA1203_CHR_VREG is not accurate and often gives fake signals.
        // Use RDA1203_CHR_TERM instead.
        if (ret & RDA1203_CHR_TERM)
        {
             //PMD_TRACE(PMD_DBG_TRC | TSTDOUT, 0, "charger: full and stop");
             status = CHR_STATUS_FULL;
        }
        else if (ret & RDA1203_CHR_TAPER)
        {
             //PMD_TRACE(PMD_DBG_TRC | TSTDOUT, 0, "charger: full 50ma");
             status = CHR_STATUS_TAPER;
        }
        else // current is greater than 50mA
        {
            status = CHR_STATUS_FAST;
        }
    }
    
    pmd_TSDReleaseSPI();
    return status;
}


BOOL pmd_TsdReadCoordinatesInternal(UINT16* gpadc_x, UINT16* gpadc_y)
{
  
   UINT16   ret = pmd_OpalSpiRead(RDA_ADDR_TOUCH_SCREEN_RESULTS1);

   if((ret&(1<<10)) != 0)
   {
    *gpadc_x = (ret&0x3ff);
   }
   else
   {
    return FALSE;
   }
  ret = pmd_OpalSpiRead(RDA_ADDR_TOUCH_SCREEN_RESULTS2);
  if((ret&(1<<10)) != 0)
   {
    *gpadc_y = (ret&0x3ff);
   }
   else
   {
    return FALSE;
   }
   return TRUE;
}


PUBLIC BOOL pmd_SpiCsNonblockingActivate(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return TRUE;
    }

    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (1 == g_pmdActivated || 1 == g_tsdActivated || 1 == g_SimActivated)
    {
        status = FALSE;
        if (1 == g_pmdActivated)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, "PMU has been activated. Be care of infinite loop.");
        }
    }
    else
    {
        status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
        if (status)
        {
            g_pmdActivated = 1;
        }
    }
    hal_SysExitCriticalSection(scStatus);

    return status;
}

PUBLIC BOOL pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT32 data)
{
#if 0
    if (g_pmdInitDone)
    {
        if (!pmd_SpiCsNonblockingActivate(TRUE))
        {
            return FALSE;
        }
    }
#endif

    pmd_OpalSpiWrite(regIdx, (UINT16)data);

#if 0
    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(TRUE);
    }
#endif

    return TRUE;
}

PUBLIC BOOL pmd_RegRead(RDA_REG_MAP_T regIdx, UINT32 *pData)
{
    if (g_pmdInitDone)
    {
        if (!pmd_SpiCsNonblockingActivate(FALSE))
        {
            return FALSE;
        }
    }

    *pData = pmd_OpalSpiRead(regIdx);

    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(FALSE);
    }
    
    return TRUE;
}


#ifdef SPI_REG_DEBUG
// ======================================================
// Debug: Read and Write Registers via SPI

#define PMD_SPI_REG_VALUE_LOCATION __attribute__((section(".pmu_reg_value")))

#define SPI_REG_WRITE_FLAG (1<<31)
#define SPI_REG_READ_FLAG (1<<30)
#define SPI_REG_FORCE_WRITE_FLAG (1<<29)
#define SPI_REG_FORCE_READ_FLAG (1<<28)
#define SPI_REG_DATA_MASK (0xffff)

#define PMU_REG_NUM (0x38)
UINT32 PMD_SPI_REG_VALUE_LOCATION g_pmdSpiRegValue[PMU_REG_NUM];

PROTECTED VOID pmd_SpiRegValueInit(VOID)
{
    for (int i=0; i<PMU_REG_NUM; i++)
    {
        g_pmdSpiRegValue[i] = 0;
    }
}

PUBLIC VOID pmd_SpiRegCheck(VOID)
{
    static BOOL initDone = FALSE;
    if (!initDone)
    {
        pmd_SpiRegValueInit();
        initDone = TRUE;
    }
    
    for (int i=0; i<PMU_REG_NUM; i++)
    {
        if (g_pmdSpiRegValue[i] & SPI_REG_WRITE_FLAG)
        {
            if (!pmd_RegWrite(i, g_pmdSpiRegValue[i]&SPI_REG_DATA_MASK))
            {
                continue;
            }
            g_pmdSpiRegValue[i] &= ~SPI_REG_WRITE_FLAG;
        }
        if (g_pmdSpiRegValue[i] & SPI_REG_READ_FLAG)
        {
             UINT32 data;
             if (!pmd_RegRead(i, &data))
             {
                continue;
             }
            g_pmdSpiRegValue[i] &= ~(SPI_REG_READ_FLAG|SPI_REG_DATA_MASK);
            g_pmdSpiRegValue[i] |= data&SPI_REG_DATA_MASK;
        }
    }
}

PUBLIC VOID pmd_RegisterSpiRegCheckFunc(VOID)
{
    static BOOL registerIdleHook = FALSE;
    if (!registerIdleHook)
    {
        registerIdleHook = TRUE;
        extern bool sxs_RegisterDebugIdleHookFunc(void (*funcPtr)(void));
        sxs_RegisterDebugIdleHookFunc(&pmd_SpiRegCheck);
    }
}
#endif // SPI_REG_DEBUG

// =============================================================================
//  Earphone detection stuff
// =============================================================================

PMD_EAR_HANDLER_T ear_handler = NULL;
HAL_GPIO_GPIO_ID_T g_ear_gpio = HAL_GPIO_NONE ; // to detect the earphone use gpio

VOID pmd_EarDetectDelay(VOID)
{
    HAL_GPIO_CFG_T eraCfg;
    if (ear_handler != NULL)
    {
        if (hal_GpioGet(g_ear_gpio))
        {
            (*ear_handler)(FALSE);
            PMD_TRACE(PMD_WARN_TRC, 0, "pmd/opal:ear Delay gpio = 1");
        }
        else
        {
            (*ear_handler)(TRUE);
            PMD_TRACE(PMD_WARN_TRC, 0, "pmd/opal:ear Delay gpio = 0");
        }
    }
    eraCfg.irqMask.rising = TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(g_ear_gpio);
}

VOID pmd_EarDetectHandler(VOID)
{
    PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear EarDetectHandler gpio");
    
    HAL_GPIO_CFG_T eraCfg;
    eraCfg.irqMask.rising = FALSE;
    eraCfg.irqMask.falling = FALSE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(g_ear_gpio);
    sxr_StopFunctionTimer(pmd_EarDetectDelay);
    sxr_StartFunctionTimer(HAL_TICK1S, pmd_EarDetectDelay, NULL, 0);
}

VOID pmd_EarpDetectGpioInit (HAL_GPIO_GPIO_ID_T gpioId)
{
    if ( gpioId != HAL_GPIO_NONE &&
         (gpioId < HAL_GPIO_1 || gpioId > HAL_GPIO_7) )
    {
        PMD_ASSERT(FALSE, "Earpice detect GPIO should be none or from 1 to 7: 0x%08x", gpioId);
    }
    
    g_ear_gpio = gpioId;
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return;
    }
    
    HAL_GPIO_CFG_T eraCfg;
    eraCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    eraCfg.irqHandler = pmd_EarDetectHandler;
    eraCfg.irqMask.rising=TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level=FALSE;
    hal_GpioOpen(g_ear_gpio,&eraCfg);
}

PUBLIC VOID pmd_SetEarDetectHandler(PMD_EAR_HANDLER_T handler)
{
    ear_handler = handler;
}

UINT8 pmd_GetEarModeStatus(VOID)
{
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return PMD_EAR_DETECT_POLLING;
    }
    else
    {
        return hal_GpioGet(g_ear_gpio);
    }
}

BOOL pmd_Gpadc_stable(VOID)
{
    return TRUE;
}

