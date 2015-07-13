////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2010-2010, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/include/hal_rda_audio.h $ //
//    $Author: admin $                                                        //
//    $Date: 2012-07-28 15:28:08 +0800 (星期六, 28 七月 2012) $                     //
//    $Revision: 16516 $                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_rda_audio.h                                                           //
///                                                                           //
/// This file decribes the RDA Audio driver API.         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _HAL_RDA_AUDIO_H_
#define _HAL_RDA_AUDIO_H_

#include "cs_types.h"
#include "chip_id.h"
#include "hal_gpio.h"

// =============================================================================
//  MACROS
// =============================================================================

// ----------------
// Default volumns
// ----------------
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

#if defined(GALLITE_IS_8805)
// Default ABB DAC digital gain is -3dB for receiver
#define DEFAULT_ABB_DAC_GAIN_RECEIVER (-3)
// Default ABB DAC digital gain is -3dB for speaker
#define DEFAULT_ABB_DAC_GAIN_SPEAKER (-3)
// Earpiece output energy is too large
// Reduce default ABB DAC digital gain to -15dB
#define DEFAULT_ABB_DAC_GAIN_HEAD (-15)
// Default ABB ADC (MIC) digital gain is -10dB
#define DEFAULT_ABB_ADC_GAIN (-10)
#else // 8806
// Default ABB DAC digital gain is -5dB for receiver
#define DEFAULT_ABB_DAC_GAIN_RECEIVER (-5)
// Default ABB DAC digital gain is -5dB for speaker
#define DEFAULT_ABB_DAC_GAIN_SPEAKER (-5)
// Earpiece output energy is too large
// Reduce default ABB DAC digital gain to -9dB
#define DEFAULT_ABB_DAC_GAIN_HEAD (-9)
// Default ABB ADC (MIC) digital gain is -6dB
#define DEFAULT_ABB_ADC_GAIN (-6)
#endif

// Max ABB DAC digital gain is -3dB for earpiece
#define MAX_ABB_DAC_GAIN_HEAD (-3)
// Max ABB ADC (MIC) digital gain is -2dB
#define MAX_ABB_ADC_GAIN (-2)

#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)

// Default ABB DAC digital gain is -5dB for receiver
#define DEFAULT_ABB_DAC_GAIN_RECEIVER (-5)
// Default ABB DAC digital gain is -5dB for speaker
#define DEFAULT_ABB_DAC_GAIN_SPEAKER (-5)
// Earpiece output energy is too large
// Reduce default ABB DAC digital gain to -9dB
#define DEFAULT_ABB_DAC_GAIN_HEAD (-9)
// Default ABB ADC (MIC) digital gain is 0dB
#define DEFAULT_ABB_ADC_GAIN (0)

// Max ABB DAC digital gain is -3dB for earpiece
#define MAX_ABB_DAC_GAIN_HEAD (-3)
#define MAX_ABB_ADC_GAIN (12)
#else // 8809 or later
#define DEFAULT_ABB_DAC_GAIN_RECEIVER (-2)
#define DEFAULT_ABB_DAC_GAIN_SPEAKER (-2)
#define DEFAULT_ABB_DAC_GAIN_HEAD (-9)
#define DEFAULT_ABB_ADC_GAIN (0)
#define MAX_ABB_DAC_GAIN_HEAD (-2)
// Max ABB ADC (MIC) digital gain is 12dB
#define MAX_ABB_ADC_GAIN (12)

#endif // 8808 or later


// ----------------
// Times
// ----------------
#define AUD_TIME_POWER_ON_DEEP   (1 MS_WAITING)
#define AUD_TIME_POWER_ON_REF    (11) // 500 us, upper_int(HAL_TICK1S/1000/2) + 2
#define AUD_TIME_POWER_ON_OTHER  (6) // 200 us, upper_int(HAL_TICK1S/1000/5) + 2
#define AUD_TIME_SET_FM_MODE     (1 MS_WAITING)

#define AUD_TIME_RETRY           (2 MS_WAITING)


// =============================================================================
//  TYPES
// =============================================================================

typedef VOID (*HAL_AUD_CALLBACK_T)(VOID);

typedef enum
{
    // Identifiers for internal only
    HAL_AUD_USER_NULL,
    // Identifiers for API parameter
    HAL_AUD_USER_CODEC,
    HAL_AUD_USER_FM,
    HAL_AUD_USER_LINEIN,
    HAL_AUD_USER_AIF_ONLY,
#ifdef VMIC_POWER_ON_WORKAROUND
    HAL_AUD_USER_EARPIECE,
#endif // VMIC_POWER_ON_WORKAROUND
} HAL_AUD_USER_T;

typedef union
{
    UINT32 reg;
    struct
    {
        BOOL noDeepPd : 1;
        BOOL noRefPd : 1;
        BOOL noMicPd : 1;
        BOOL noAuxMicPd : 1;
        BOOL noAdcPd : 1;
        BOOL noDacPd : 1;
        UINT32 : 2;
        BOOL noDacReset : 1;
    };
} HAL_AUD_PD_T;
#define HAL_AUD_PD_MASK (0x13f)

typedef union
{
    UINT32 reg;
    struct
     {
        BOOL auxMicSel : 1;
        BOOL spkSel : 1;
        BOOL spkMonoSel : 1;
        BOOL rcvSel : 1;
        BOOL headSel : 1;
        BOOL fmSel : 1;
    };
} HAL_AUD_SEL_T;

typedef union
{
    UINT32 reg;
    struct
    {
        UINT32 micGain : 4;
        BOOL noMicMute : 1;
    };
} HAL_AUD_MIC_CFG_T;

typedef union
{
    UINT32 reg;
    struct
    {
        UINT32 spkGain : 4;
        BOOL noSpkMute : 1;
    };
} HAL_AUD_SPK_CFG_T;

typedef enum
{
    HP_DETECT_REQ_SRC_AUD = 0,
    HP_DETECT_REQ_SRC_FM,
} HP_DETECT_REQ_SRC_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//  

// =============================================================================
//  FUNCTIONS
// =============================================================================

PUBLIC VOID hal_AudConfigExtSpkPa(HAL_APO_ID_T paCtrl, BOOL pinActiveLow);

PUBLIC BOOL hal_AudOpen(HAL_AUD_USER_T user, HAL_AUD_CALLBACK_T cbFunc);
PUBLIC BOOL hal_AudClose(HAL_AUD_USER_T user);
PUBLIC VOID hal_AudPowerOn(HAL_AUD_PD_T pdStatus);
PUBLIC VOID hal_AudPowerDown(HAL_AUD_PD_T pdStatus);
PUBLIC VOID hal_AudDeepPowerDown(VOID);
PUBLIC VOID hal_AudForcePowerDown(VOID);

/// Must wait for AUD_TIME_SET_FM_MODE if setting to non-FM mode
PUBLIC VOID hal_AudSetFmMode(BOOL isFmMode);
PUBLIC VOID hal_AudSetVoiceMode(BOOL isVoiceMode);
PUBLIC VOID hal_AudEnableDac(BOOL on);
PUBLIC VOID hal_AudEnableCodecDsp(BOOL on);
PUBLIC VOID hal_AudSetSampleRate(UINT32 selReg, UINT32 divReg);
PUBLIC VOID hal_AudEnableHpDetect(BOOL on, HP_DETECT_REQ_SRC_T src);
PUBLIC VOID hal_AudBypassNotchFilterReq(BOOL bypass);

PUBLIC VOID hal_AudSetDacVolumn(UINT32 vol);
PUBLIC VOID hal_AudSetAdcVolumn(UINT32 vol);

PUBLIC VOID hal_AudForceReceiverMicSelection(BOOL on);

PUBLIC VOID hal_AudSetSelCfg(HAL_AUD_SEL_T selCfg);
PUBLIC VOID hal_AudSetMicCfg(HAL_AUD_MIC_CFG_T micCfg);
PUBLIC VOID hal_AudSetSpkCfg(HAL_AUD_SPK_CFG_T spkCfg);
PUBLIC VOID hal_AudSetRcvGain(UINT32 gain);
PUBLIC VOID hal_AudSetHeadGain(UINT32 gain);

PUBLIC HAL_AUD_PD_T hal_AudGetPowerStatus(VOID);
PUBLIC HAL_AUD_SEL_T hal_AudGetSelCfg(VOID);
PUBLIC HAL_AUD_MIC_CFG_T hal_AudGetMicCfg(VOID);
PUBLIC HAL_AUD_SPK_CFG_T hal_AudGetSpkCfg(VOID);
PUBLIC UINT32 hal_AudGetRcvGain(VOID);
PUBLIC UINT32 hal_AudGetHeadGain(VOID);

PUBLIC UINT32 hal_AudRcvGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudHeadGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudSpkGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudMicGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudDacGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudAdcGainDb2Val(INT32 db);
PUBLIC VOID hal_AudEnableDSSI(BOOL on);

#endif // _HAL_RDA_AUDIO_H_

