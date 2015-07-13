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
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Workspaces/aint/bv5_dev/platform/chip/hal/src/hal_rda_audio.c $ //
//    $Author: admin $                                                        //
//    $Date: 2013-01-16 18:55:38 +0800 (周三, 16 一月 2013) $                     //
//    $Revision: 18736 $                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_rda_audio.c                                                           //
///                                                                           //
/// This file decribes the RDA Audio driver API.         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"
#include "cfg_regs.h"

#include "halp_debug.h"
#include "halp_sys.h"
#include "hal_sys.h"
#include "hal_rda_abb.h"
#include "hal_rda_audio.h"
#if defined(SOUND_RECORDER_SUPPORT_16K)	
#include "hal_aif.h"
#endif

#include "sxr_tls.h"
#include "pmd_m.h"
#include "rfd_xcv.h"


#if defined(AUD_SPK_ON_WITH_LCD) && defined(AUD_EXT_PA_ON_WITH_LCD)
#error "AUD_SPK_ON_WITH_LCD and AUD_EXT_PA_ON_WITH_LCD cannot be defined at the same time!"
#endif

// TODO: Must we couple HP detect with VMIC power on ???
#if defined(ABB_HP_DETECT) && !defined(VMIC_POWER_ON_WORKAROUND)
#error "VMIC_POWER_ON_WORKAROUND should be enabled when ABB_HP_DETECT is enabled."
#endif

#ifdef ABB_HP_DETECT
// Whether to detect headphone by ABB on idle time
#define ABB_HP_DETECT_ON_IDLE_TIME 0
#endif

// Whether to enable DSSI to improve sound quality during silence
// 0: disable DSSI all the time
// 1: only enable DSSI in music mode
// 2: enable DSSI both voice and music mode
#define AUD_SPK_DSSI_DETECT 0

extern UINT16 g_halAbbCodecPowerCtrl;
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) // 8808 or later
extern CONST UINT16 gc_halAbbCodecMiscSetting;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) // 8808
extern CONST UINT16 gc_halAbbCodecPllSetting1;
#else // 8809 or later
extern CONST UINT16 gc_halAbbCodecMicSetting;
extern UINT16 gc_halAbbCodecClockCodec;
#endif // 8809 or later
#endif // 8808 or later


#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
extern VOID pmd_SpeakerPaCpLowOutput(BOOL low);     
#ifdef ABB_HP_DETECT
extern VOID pmd_StartHpOutDetect(BOOL start);
#endif
#endif // 8809 or later


PRIVATE HAL_AUD_USER_T g_halAudUser = HAL_AUD_USER_NULL;


typedef enum
{
    HAL_AUD_FREQ_REQ_SRC_AUD_OPEN,
    HAL_AUD_FREQ_REQ_SRC_NORMAL,
    HAL_AUD_FREQ_REQ_SRC_LCD_OFF,
} HAL_AUD_FREQ_REQ_SRC_T;
PRIVATE UINT32 g_halAudFreqReqSrcBitmap = 0;


#ifdef VMIC_POWER_ON_WORKAROUND
typedef enum
{
    HAL_AUD_EP_DETECT_OFF,
    HAL_AUD_EP_DETECT_ON,
} HAL_AUD_EP_DETECT_T;
PRIVATE HAL_AUD_EP_DETECT_T g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_OFF;

PRIVATE BOOL g_halAudVMicValid = FALSE;
#endif // VMIC_POWER_ON_WORKAROUND


#ifdef AUD_MUTE_FOR_BT_WORKAROUND
PRIVATE BOOL g_halAudMuteOutput = FALSE;
#endif // AUD_MUTE_FOR_BT_WORKAROUND


#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
typedef enum
{
    HAL_AUD_DAC_PREV_PWR_OFF,
    HAL_AUD_DAC_PREV_PWR_ON,
    HAL_AUD_DAC_PREV_PWR_RESTORED,
} HAL_AUD_DAC_PREV_PWR_T;
PRIVATE HAL_AUD_DAC_PREV_PWR_T g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_OFF;

#ifdef RECV_USE_SPK_LINE
PUBLIC BOOL g_halAudRecvUseSpkLineRight = FALSE;
#endif
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD


#if defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
UINT32 g_halAudIntSpkPaStableTime = (20 MS_WAITING);
UINT32 g_halAudExtPaStableTime = (350 MS_WAITING);

#ifdef AUD_SPK_ON_WITH_LCD
BOOL g_halAudExtPaConfigNeeded = FALSE;
#endif

#ifdef AUD_EXT_PA_ON_WITH_LCD
typedef enum
{
    HAL_AUD_EXT_PA_PREV_PWR_OFF,
    HAL_AUD_EXT_PA_PREV_PWR_ON,
    HAL_AUD_EXT_PA_PREV_PWR_RESTORED,
} HAL_AUD_EXT_PA_PREV_PWR_T;
PRIVATE HAL_AUD_EXT_PA_PREV_PWR_T g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_OFF;
#endif // AUD_EXT_PA_ON_WITH_LCD
#endif // AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD


/// Whether to always select the MIC on phone (the regular one)
PRIVATE BOOL g_halAudForceReceiverMicSelection = FALSE;

PRIVATE UINT16 g_halAudCodecDigEn = 
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    0x0f04;
#else // 8808 or later
    ABB_DIG_S_DWA_EN|
    ABB_DIG_MASH_EN_ADC|
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    ABB_DIG_PADET_CLK_INV|
#endif
    ABB_DIG_S_CODEC_EN;
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
PUBLIC BOOL g_halAudLineInSpeakerInUse = FALSE;
#endif

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later

PRIVATE UINT16 g_halHpDetect_12h = 
    ABB_PD_SSI|
    ABB_DSSI_ANA_ENABLE|
    ABB_DSSI_STABLE_TIME_SEL(1)|
    ABB_DSSI_LG_EN_TIME_SEL(2)|
    ABB_RESET_PADET_TIME_SEL(2);

PRIVATE CONST UINT16 gc_halAudCodecFmMode = 
#if !defined(SOUND_RECORDER_SUPPORT_16K)	
    ABB_CNT_ADC_CLK(24)|
#endif
    ABB_CLK_FM_INV;

// DAC mode, FIXME
PUBLIC UINT16 g_halAbbCodecDacModeSetting = 0;

// Class K PA clock setting
PRIVATE UINT16 g_halAudCodecClockClassKPa = 
    ABB_DCDC4_RESETN_TIME_SEL(2)|
    ABB_DCDC4_CLKGEN4_DIV_BASE(160);

PRIVATE UINT16 g_halAudHpDetect_3dh = 
#ifndef AUD_3_IN_1_SPK
    ABB_NOTCH20_BYPASS|
#endif
    ABB_EARPHONE_DET_BYPASS|
    ABB_EARPHONE_DET_TH(20)|
    ABB_EARPHONE_DET_COEF_H(7);

#ifdef ABB_HP_DETECT
// By default audio disallows headphone detection, but others allow.
PRIVATE UINT16 g_halHpDetectOffBitmap = (1<<HP_DETECT_REQ_SRC_AUD);
PRIVATE BOOL g_halAudHeadphonePrevState = FALSE;

#if (ABB_HP_DETECT_ON_IDLE_TIME)
PRIVATE BOOL g_halAudHeadphoneDacMuted = FALSE;
#endif // ABB_HP_DETECT_ON_IDLE_TIME
#endif // ABB_HP_DETECT

// AUD_3_IN_1_SPK mode has the higher priority to control the notch filter
#ifdef AUD_3_IN_1_SPK
PRIVATE BOOL g_halAudBypassNotchFilterApp = FALSE;
#elif defined(ABB_HP_DETECT)
PRIVATE BOOL g_halAudBypassNotchFilterFm = FALSE;
#endif // ABB_HP_DETECT

#endif // 8809 or later

#if defined(SOUND_RECORDER_SUPPORT_16K)	
HAL_AIF_FREQ_T g_DivsampleRate = HAL_AIF_FREQ_8000HZ;
#endif

// ===========================================================

PRIVATE VOID hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_T src, BOOL on)
{
    if (on)
    {
        if (g_halAudFreqReqSrcBitmap == 0)
        {
            hal_SysRequestFreq(HAL_SYS_FREQ_AUDIO, HAL_SYS_FREQ_26M, NULL);
        }
        g_halAudFreqReqSrcBitmap |= (1<<src);
    }
    else
    {
        g_halAudFreqReqSrcBitmap &= ~(1<<src);
        if (g_halAudFreqReqSrcBitmap == 0)
        {
            hal_SysRequestFreq(HAL_SYS_FREQ_AUDIO, HAL_SYS_FREQ_32K, NULL);
        }
    }
}

PRIVATE VOID hal_AudMuteAllChannels(VOID)
{
    // Mute all channels
    hal_AudSetRcvGain(0);
    hal_AudSetHeadGain(0);
    HAL_AUD_SPK_CFG_T spkCfg;
    spkCfg.reg = 0;
    hal_AudSetSpkCfg(spkCfg);
    HAL_AUD_MIC_CFG_T micCfg;
    micCfg.reg = 0;
    hal_AudSetMicCfg(micCfg);
}

#ifdef VMIC_POWER_ON_WORKAROUND
PUBLIC BOOL hal_AudEarpieceDetectInit(VOID)
{
    // Power on audio codec components
    HAL_AUD_PD_T pdStatus, orgPdStatus;
    orgPdStatus = hal_AudGetPowerStatus();
    if (!orgPdStatus.noDeepPd)
    {
        pdStatus.reg = 0;
        pdStatus.noDeepPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_DEEP);
    }
    if (!orgPdStatus.noRefPd)
    {
        pdStatus.reg = 0;
        pdStatus.noRefPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_REF);
    }

    // Power on both Mic and AuxMic, then no matter which one
    // is selected in audio_sel_cfg, v_mic will be turned on.
    if (!orgPdStatus.noMicPd)
    {
        pdStatus.reg = 0;
        pdStatus.noMicPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_OTHER);
    }
    if (!orgPdStatus.noAuxMicPd)
    {
        pdStatus.reg = 0;
        pdStatus.noAuxMicPd = TRUE;
        hal_AudPowerOn(pdStatus);
        hal_TimDelay(AUD_TIME_POWER_ON_OTHER);
    }
    if (!orgPdStatus.noAdcPd)
    {
        pdStatus.reg = 0;
        pdStatus.noAdcPd = TRUE;
        hal_AudPowerOn(pdStatus);
    }

#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
    if (!orgPdStatus.noDacPd)
    {
        pdStatus.reg = 0;
        pdStatus.noDacPd = TRUE;
        hal_AudPowerOn(pdStatus);
        sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
    }
    if (!orgPdStatus.noDacReset)
    {
        pdStatus.reg = 0;
        pdStatus.noDacReset = TRUE;
        hal_AudPowerOn(pdStatus);
    }
#endif

    // Set g_halAudEarpieceDetectionOn first, which will be checked
    // in hal_AudSetSelCfg
    g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_ON;

    // Proceed with some special earpiece handlings
    hal_AudSetSelCfg(hal_AudGetSelCfg());

#if ! ( (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) && defined(MIC_CAPLESS_MODE) )
    g_halAudVMicValid = TRUE;
#endif

#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
    // Start headphone plugging-out detection
    hal_AudEnableHpDetect(TRUE, HP_DETECT_REQ_SRC_AUD);
#endif

    // No need to request freq for earpiece

    return TRUE;
}

PUBLIC BOOL hal_AudVMicValid(VOID)
{
    return g_halAudVMicValid;
}
#endif // VMIC_POWER_ON_WORKAROUND


#ifdef AUD_MUTE_FOR_BT_WORKAROUND
// =============================================================================
// hal_AudMuteOutput
// -----------------------------------------------------------------------------
/// Power on/off the audio when it is playing.
/// The function is called by MCI only to redirect the audio output to the bluetooth device.
/// The function might be removed if MCI changes its way to switch between bluetooth and normal codec.
/// 
/// @param on TRUE if power on, FALSE otherwise.
/// @return None.
// =============================================================================
PUBLIC VOID hal_AudMuteOutput(BOOL mute)
{
    // Mutex control is done in aud_core.c
    g_halAudMuteOutput = mute;

    if (mute)
    {
        // MCI will mute audio after stopping current stream but before restarting it.
        // So the mute operation is acutally done in hal_AudOpen (also within
        // mutex control).
        hal_AudEnableDac(FALSE);
    }
    else
    {
        // MCI might unmute audio while playing current stream.
        hal_AudEnableDac(TRUE);
    }
}
#endif // AUD_MUTE_FOR_BT_WORKAROUND


PUBLIC BOOL hal_AudOpen(HAL_AUD_USER_T user, HAL_AUD_CALLBACK_T cbFunc)
{
#ifdef CODEC_CLOSE_DELAY    
    BOOL openAud = TRUE;
#else
    BOOL openAud = FALSE;
#endif

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
        {
            return TRUE;
        }
        else
        {
            openAud = TRUE;
        }
    }
    else if (g_halAudUser == HAL_AUD_USER_NULL)
    {
        openAud = TRUE;
    }
#else // ! VMIC_POWER_ON_WORKAROUND
    if (g_halAudUser == HAL_AUD_USER_NULL)
    {
        openAud = TRUE;
    }
#endif // ! VMIC_POWER_ON_WORKAROUND

    if (!openAud)
    {
        return FALSE;
    }

    if (user == HAL_AUD_USER_AIF_ONLY)
    {
        g_halAudUser = user;
        return TRUE;
    }

    pmd_EnablePower(PMD_POWER_AUDIO, TRUE);

    // Request freq resource (to avoid entering LP mode from now on,
    // including the following sleeps as well)
    hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_AUD_OPEN, TRUE);

#ifdef VMIC_POWER_ON_WORKAROUND
    // Earpiece user needs v_mic power only
    if (user != HAL_AUD_USER_EARPIECE)
#endif // VMIC_POWER_ON_WORKAROUND
    {
        // Mute all channels
        hal_AudMuteAllChannels();

        // Disable codec DSP to start DSP configuration
        hal_AudEnableCodecDsp(FALSE);

        // Set FM mode
        if (user == HAL_AUD_USER_FM)
        {
            hal_AudSetFmMode(TRUE);
        }

#if defined(SOUND_RECORDER_SUPPORT_16K)	
        else
        {
            hal_AudSetFmMode(FALSE);
        }
#endif

        // Invoke callback function before power on codec
        if (cbFunc != NULL)
        {
            (*cbFunc)();
        }

        hal_AudEnableCodecDsp(TRUE);
    }

    // Power on audio codec components
    HAL_AUD_PD_T pdStatus, orgPdStatus;
    orgPdStatus = hal_AudGetPowerStatus();
    if (!orgPdStatus.noDeepPd)
    {
        pdStatus.reg = 0;
        pdStatus.noDeepPd = TRUE;
        hal_AudPowerOn(pdStatus);
        sxr_Sleep(AUD_TIME_POWER_ON_DEEP);
    }
    if (!orgPdStatus.noRefPd)
    {
        pdStatus.reg = 0;
        pdStatus.noRefPd = TRUE;
        hal_AudPowerOn(pdStatus);
        sxr_Sleep(AUD_TIME_POWER_ON_REF);
    }
		//mic'
	if (!orgPdStatus.noMicPd)
	        {
	            pdStatus.reg = 0;
	            pdStatus.noMicPd = TRUE;
	            hal_AudPowerOn(pdStatus);
	            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
	        }
	/////
    if ( user == HAL_AUD_USER_CODEC
#ifdef VMIC_POWER_ON_WORKAROUND
        || user == HAL_AUD_USER_EARPIECE
#endif
#if defined(FM_LINEIN_RECORD) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
        || user == HAL_AUD_USER_LINEIN
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
        || user == HAL_AUD_USER_FM
#endif		
       )
    {
        // Power on both Mic and AuxMic, then no matter which one
        // is selected in audio_sel_cfg, v_mic will be turned on.
        if (!orgPdStatus.noMicPd)
        {
            pdStatus.reg = 0;
            pdStatus.noMicPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
        if (!orgPdStatus.noAuxMicPd)
        {
            pdStatus.reg = 0;
            pdStatus.noAuxMicPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
        if (!orgPdStatus.noAdcPd)
        {
            pdStatus.reg = 0;
            pdStatus.noAdcPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
    }

    // DAC should be powered on even if the user is earpiece, in
    // case that ABB HP detection is enabled
#ifdef VMIC_POWER_ON_WORKAROUND
#if !(defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME))
    if (user != HAL_AUD_USER_EARPIECE)
#endif
#endif // VMIC_POWER_ON_WORKAROUND
    {
        if (!orgPdStatus.noDacPd)
        {
            pdStatus.reg = 0;
            pdStatus.noDacPd = TRUE;
            hal_AudPowerOn(pdStatus);
            sxr_Sleep(AUD_TIME_POWER_ON_OTHER);
        }
        if (!orgPdStatus.noDacReset)
        {
            pdStatus.reg = 0;
            pdStatus.noDacReset = TRUE;
            hal_AudPowerOn(pdStatus);
        }
    }

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        // Set g_halAudEarpieceDetectionOn first, which will be checked
        // in hal_AudSetSelCfg
        g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_ON;

        // Proceed with some special earpiece handlings
        hal_AudSetSelCfg(hal_AudGetSelCfg());

#if ! ( (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) && defined(MIC_CAPLESS_MODE) )
        g_halAudVMicValid = TRUE;
#endif

#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        // Start headphone plugging-out detection
        hal_AudEnableHpDetect(TRUE, HP_DETECT_REQ_SRC_AUD);
#endif

        // No need to request freq for earpiece
        hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_AUD_OPEN, FALSE);

        return TRUE;
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // Allow deep sleep if in FM/LINEIN mode
    // (except for 8809 LINEIN -- see hal_AudSetSelCfg)
    if (user != HAL_AUD_USER_FM && user != HAL_AUD_USER_LINEIN)
    {
        hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_NORMAL, TRUE);
    }
    hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_AUD_OPEN, FALSE);

    // Request audio module (no need to enter critical section)
    g_halAudUser = user;

    return TRUE;
}

PUBLIC BOOL hal_AudClose(HAL_AUD_USER_T user)
{
    BOOL closeAud = FALSE;

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_OFF)
        {
            return TRUE;
        }
        else
        {
            if (g_halAudUser == HAL_AUD_USER_NULL)
            {
                closeAud = TRUE;
            }
            else
            {
                // There is another audio user, so just set the flag and do not close the module
                g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_OFF;
                g_halAudVMicValid = FALSE;
#ifdef ABB_HP_DETECT
                // Codec must be reset if in FM mode
                if (g_halAudUser == HAL_AUD_USER_FM)
                {
                    hal_AudEnableCodecDsp(FALSE);
                }
                // Disable headphone detection
                hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
                if (g_halAudUser == HAL_AUD_USER_FM)
                {
                    hal_AudEnableCodecDsp(TRUE);
                }
                {
                    // If the headphone key detection disabling is triggered
                    // by LP mode, the headphone state must be restored; otherwise
                    // (triggered by plugging out) it can be safely reset to unselected.
                    // Among the audio users, only FM and LINEIN can enter
                    // LP mode.
                    HAL_AUD_SEL_T selCfg;
                    selCfg = hal_AudGetSelCfg();
                    if (g_halAudUser == HAL_AUD_USER_FM ||
                        g_halAudUser == HAL_AUD_USER_LINEIN)
                    {
                        selCfg.headSel = g_halAudHeadphonePrevState;
                    }
                    else
                    {
                        selCfg.headSel = FALSE;
                    }
                    hal_AudSetSelCfg(selCfg);
                }
#endif
#ifdef USE_EP_MIC_WITH_SPK
                {
                    // Not to select aux MIC any longer (after resetting 
                    // g_halAudEarpieceDetectionOn)
                    HAL_AUD_SEL_T selCfg;
                    selCfg = hal_AudGetSelCfg();
                    selCfg.auxMicSel = FALSE;
                    hal_AudSetSelCfg(selCfg);
                }
#endif
                return TRUE;
            }
            // Else audio driver is being configured by a user other than earpiece, and we need to wait
        }
        // Else audio driver is being configured by earpiece, and we need to wait
    }
    else if (g_halAudUser == user)
    {
        closeAud = TRUE;
    }
    else
    {
        // 1. Audio driver is already closed
        // 2. Audio driver has been opened by another user (not earpiece either)
        return TRUE;
    }
#else // ! VMIC_POWER_ON_WORKAROUND
    if (g_halAudUser == user)
    {
        closeAud = TRUE;
    }
    else
    {
        // 1. Audio driver is already closed
        // 2. Audio driver has been opened by another user
        return TRUE;
    }
#endif // ! VMIC_POWER_ON_WORKAROUND

    if (!closeAud)
    {
        return FALSE;
    }

#ifdef ABB_HP_DETECT
    g_halAudHeadphonePrevState = FALSE;
#endif

    if (user == HAL_AUD_USER_AIF_ONLY)
    {
        g_halAudUser = HAL_AUD_USER_NULL;
        return TRUE;
    }

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        // Reset earpiece flag before calling hal_AudSetSelCfg and hal_AudPowerDown
        g_halAudEarpieceDetectionOn = HAL_AUD_EP_DETECT_OFF;
        g_halAudVMicValid = FALSE;
#ifdef ABB_HP_DETECT
        // Disable headphone detection before powering down DAC
        hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
#endif
    }
    else
#endif // VMIC_POWER_ON_WORKAROUND
    {
        // Let the sound fade out naturally
        sxr_Sleep(6 MS_WAITING);
    }

    // Mute all channels
    hal_AudMuteAllChannels();

    // Deselect all modules
    HAL_AUD_SEL_T selCfg;
    selCfg.reg = 0;
    hal_AudSetSelCfg(selCfg);

    // Power down finally
    HAL_AUD_PD_T pdStatus;
    pdStatus.reg = HAL_AUD_PD_MASK;
    hal_AudPowerDown(pdStatus);

#ifdef VMIC_POWER_ON_WORKAROUND
    if (user == HAL_AUD_USER_EARPIECE)
    {
        return TRUE;
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // Disable codec DSP to start DSP configuration
    hal_AudEnableCodecDsp(FALSE);

    // Clear FM mode
    if (user == HAL_AUD_USER_FM)
    {
        // Must wait for AUD_TIME_SET_FM_MODE if setting to non-FM mode
        hal_AudSetFmMode(FALSE);
        sxr_Sleep(AUD_TIME_SET_FM_MODE);
    }

    // Set default voice mode
    hal_AudSetVoiceMode(FALSE);

    hal_AudEnableCodecDsp(TRUE);

    // Release freq resource
    hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_NORMAL, FALSE);

    // Release audio module (no need to enter critical section)
    g_halAudUser = HAL_AUD_USER_NULL;

    return TRUE;
}

PUBLIC VOID hal_AudPowerOn(HAL_AUD_PD_T pdStatus)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    if (pdStatus.noDacPd)
    {
        if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
        {
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_ON;
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

    hwp_configRegs->audio_pd_set = pdStatus.reg;

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pdStatus.noDacPd)
    {
        if (pmd_AudExtPaExists())
        {
            if (g_halAudExtPaPrevPwr != HAL_AUD_EXT_PA_PREV_PWR_RESTORED)
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_ON;
            }
            // Do we need any stable time here?
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, TRUE);
        }
    }
#endif // AUD_EXT_PA_ON_WITH_LCD
}

PUBLIC VOID hal_AudPowerDown(HAL_AUD_PD_T pdStatus)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    if (pdStatus.noDacPd)
    {
        // Audio might be stopped after LCD is turned off. In that case,
        // audio codec DAC can be powered off if requested.
        if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
        {
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_OFF;
            pdStatus.noDacPd = FALSE;
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

    // Cannot go into deep sleep here
    pdStatus.noDeepPd = FALSE;
    pdStatus.noRefPd = FALSE;
	pdStatus.noMicPd = FALSE;
	
#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
        // Power on both Mic and AuxMic, then no matter which one
        // is selected in audio_sel_cfg, v_mic will be turned on.
        pdStatus.noMicPd = FALSE;
        pdStatus.noAuxMicPd = FALSE;
        pdStatus.noAdcPd = FALSE;
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        // Power on DAC to detect headphone plugging-out
        pdStatus.noDacPd = FALSE;
        pdStatus.noDacReset = FALSE;
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    hwp_configRegs->audio_pd_clr = pdStatus.reg;

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pdStatus.noDacPd)
    {
        if (pmd_AudExtPaExists())
        {
            // Audio might be stopped after LCD is turned off. In that case,
            // the external PA can be powered off if requested.
            if (g_halAudExtPaPrevPwr == HAL_AUD_EXT_PA_PREV_PWR_RESTORED)
            {
                // We will not go to deep sleep here
                sxr_Sleep(g_halAudIntSpkPaStableTime);
                pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, FALSE);
                sxr_Sleep(g_halAudExtPaStableTime);
            }
            else
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_OFF;
            }
        }
    }
#endif // AUD_EXT_PA_ON_WITH_LCD
}

#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
PUBLIC VOID hal_AudLcdPowerOnCallback(VOID)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    // Power on audio codec DAC once LCD is turned on
    if (g_halAudDacPrevPwr == HAL_AUD_DAC_PREV_PWR_RESTORED)
    {
        HAL_AUD_PD_T pdStatus;
        pdStatus = hal_AudGetPowerStatus();
        if (pdStatus.noDacPd)
        {
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_ON;
        }
        else
        {
            // Set g_halAudDacPrevPwr before calling hal_AudSetSelCfg(),
            // as hal_AudSetSelCfg() needs to check g_halAudDacPrevPwr
            g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_OFF;

            HAL_AUD_SEL_T selCfg = hal_AudGetSelCfg();
            // Useless? selCfg.spkSel should have been set to zero
            selCfg.spkSel = FALSE;
            hal_AudSetSelCfg(selCfg);

#ifdef VMIC_POWER_ON_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
            if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_OFF)
#endif
#endif // VMIC_POWER_ON_WORKAROUND
            {
                // Power on DAC (and its dependent power)
                pdStatus.reg = 0;
                pdStatus.noDeepPd = TRUE;
                pdStatus.noRefPd = TRUE;
                pdStatus.noDacPd = TRUE;
                hwp_configRegs->audio_pd_set = pdStatus.reg;
            }
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pmd_AudExtPaExists())
    {
        if (g_halAudExtPaPrevPwr == HAL_AUD_EXT_PA_PREV_PWR_RESTORED)
        {
            if (g_halAudUser != HAL_AUD_USER_CODEC &&
                g_halAudUser != HAL_AUD_USER_FM &&
                g_halAudUser != HAL_AUD_USER_LINEIN)
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_OFF;
            }
            else
            {
                g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_ON;
            }
        }
        // Do we need any stable time here?
    	pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, TRUE);
    }
#endif // AUD_EXT_PA_ON_WITH_LCD
}

PUBLIC VOID hal_AudLcdPowerOffCallback(VOID)
{
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    HAL_AUD_DAC_PREV_PWR_T orgDacPrevPwr = g_halAudDacPrevPwr;
    // Set g_halAudDacPrevPwr before calling hal_AudSetSelCfg(),
    // as hal_AudSetSelCfg() needs to check g_halAudDacPrevPwr
    g_halAudDacPrevPwr = HAL_AUD_DAC_PREV_PWR_RESTORED;

    // Restore the power state of audio codec DAC, or saying,
    // power off audio codec DAC if possible
    if (orgDacPrevPwr == HAL_AUD_DAC_PREV_PWR_OFF)
    {
        HAL_AUD_SEL_T selCfg = hal_AudGetSelCfg();
        selCfg.spkSel = FALSE;
        hal_AudSetSelCfg(selCfg);

#ifdef VMIC_POWER_ON_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_OFF)
#endif
#endif // VMIC_POWER_ON_WORKAROUND
        {
            // Power off DAC
            HAL_AUD_PD_T pdStatus;
            pdStatus.reg = 0;
            pdStatus.noDacPd = TRUE;
            hwp_configRegs->audio_pd_clr = pdStatus.reg;
        }
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

#ifdef AUD_EXT_PA_ON_WITH_LCD
    if (pmd_AudExtPaExists())
    {
        if (g_halAudExtPaPrevPwr == HAL_AUD_EXT_PA_PREV_PWR_OFF)
        {
            // Audio module has been requested to shutdown before
            // Forbid deep sleep
            hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_LCD_OFF, TRUE);
            sxr_Sleep(g_halAudIntSpkPaStableTime);
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, FALSE);
            sxr_Sleep(g_halAudExtPaStableTime);
            // Allow deep sleep from now on
            hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_LCD_OFF, FALSE);
        }
        g_halAudExtPaPrevPwr = HAL_AUD_EXT_PA_PREV_PWR_RESTORED;
    }
#endif // AUD_EXT_PA_ON_WITH_LCD
}
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD

PUBLIC VOID hal_AudDeepPowerDown(VOID)
{
    if (g_halAudUser != HAL_AUD_USER_NULL)
    {
        // Not to power down audio if there is an audio user
        // (e.g., in FM/LINEIN mode)
        return;
    }

#ifdef RECV_USE_SPK_LINE
    // pd control should have been released
#endif // RECV_USE_SPK_LINE

#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
        g_halAudVMicValid = FALSE;
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        // TODO: Should we put these codes into INT-SRAM?
        hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    hwp_configRegs->audio_pd_clr = HAL_AUD_PD_MASK;
}

PUBLIC VOID hal_AudForcePowerDown(VOID)
{
    // Mute all channels
    hal_AudMuteAllChannels();
    // Deselect all channels to disable PA modules
    hwp_configRegs->audio_sel_cfg = 0;
    // Power down all codec modules except for Deep and Ref
    // (Excluding Deep and Ref is to avoid the pop sound, which would
    // occur if all codec modules were down at the same time.)
    HAL_AUD_PD_T pdStatus;
    pdStatus.reg = HAL_AUD_PD_MASK;
    pdStatus.noDeepPd = FALSE;
    pdStatus.noRefPd = FALSE;
    hwp_configRegs->audio_pd_clr = pdStatus.reg;
}

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
#ifdef AUD_3_IN_1_SPK
PUBLIC VOID hal_AudBypassNotchFilterReq(BOOL bypass)
{
    // 3-in-1 speaker enables the notch filter to filter out 170 Hz signal
    // to avoid vibration, and disables it to vibrate.
    UINT32 status = hal_SysEnterCriticalSection();
    g_halAudBypassNotchFilterApp = bypass;

    if (g_halAudBypassNotchFilterApp)
    {
        g_halAudHpDetect_3dh |= ABB_NOTCH20_BYPASS;
    }
    else
    {
        g_halAudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
    }
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
    hal_SysExitCriticalSection(status); 
}
#endif // AUD_3_IN_1_SPK
#endif // 8809 or later

/// Must wait for AUD_TIME_SET_FM_MODE if setting to non-FM mode
PUBLIC VOID hal_AudSetFmMode(BOOL isFmMode)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) // 8805 or 8806
    // Set VCOM mode
    if (isFmMode)
    {
        // FM mode (default mode)
        hal_AbbRegWrite(CODEC_LDO_SETTING2, 0x8800);
    }
    else
    {
        // Normal mode
        hal_AbbRegWrite(CODEC_LDO_SETTING2, 0x6800);
    }
#elif (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    UINT32 status;
    UINT32 dacModeSetting = g_halAbbCodecDacModeSetting;
    UINT32 fmModeSetting = gc_halAudCodecFmMode;
    UINT32 codecClockSetting = gc_halAbbCodecClockCodec;

    if (isFmMode)
    {
        // FM mode
        // FM record begin
        // 1. enable muxin lef and right
        // this will be done in hal_AudGetSelCfg later
        // 2. set codec to dac_mode to 1, to connect dac to adc to record fm. 8809 u06
        dacModeSetting |= ABB_DAC_MODE_L_EN_DR|ABB_DAC_MODE_L_EN_REG|
            ABB_DAC_MODE_R_EN_DR|ABB_DAC_MODE_R_EN_REG;

        // 3.  fm_mode adc clk to 5, means ~8K ( 2 means ~21K) when osr is 0
        fmModeSetting &= ~(ABB_CNT_ADC_CLK_MASK);
        fmModeSetting |= ABB_CNT_ADC_CLK_FOR_FM_RECORD;
        //   A. fm_mode osr to 00
        fmModeSetting &= ~(ABB_S_ADC_OSR_SEL_MASK);
        fmModeSetting |= ABB_S_ADC_OSR_SEL_1M;		
        //   B. sample rate osr, this will be set every time when codec mode
        //   hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, xxxx);
        //   this will be done in aud_CodecCommonSetRateStg
        //   C.  codec clock to 23
        codecClockSetting &= ~(ABB_DCDC_CLKGEN3_DIV_BASE_MASK);
        codecClockSetting |= ABB_DCDC_CLKGEN3_DIV_BASE_FOR_FM_RECORD;

        hal_AbbRegWrite(CODEC_MODE_SEL, dacModeSetting);
        hal_AbbRegWrite(CODEC_CLOCK_CODEC, codecClockSetting);	
        hal_AbbRegWrite(CODEC_FM_MODE, fmModeSetting | ABB_FM_MODE);

        // Set DCDC4 clock divider for FM mode
        // Avoid conflicting with hal_AudForceDcdc4ClockOn
        status = hal_SysEnterCriticalSection();
        g_halAudCodecClockClassKPa &= ~ABB_DCDC4_CLKGEN4_DIV_BASE_MASK;
        g_halAudCodecClockClassKPa |= 
            ABB_DCDC4_CLKGEN4_CLK_SEL|ABB_DCDC4_CLKGEN4_DIV_BASE(100);
        hal_AbbRegWrite(CODEC_CLOCK_CLASS_K_PA, g_halAudCodecClockClassKPa);
        hal_SysExitCriticalSection(status);
    }
    else
    {
        // Codec mode
        hal_AbbRegWrite(CODEC_MODE_SEL, dacModeSetting);
        hal_AbbRegWrite(CODEC_CLOCK_CODEC, codecClockSetting);	
#if defined(SOUND_RECORDER_SUPPORT_16K)			
        hal_AbbRegWrite(CODEC_FM_MODE, fmModeSetting|ABB_CNT_ADC_CLK(24/(g_DivsampleRate/HAL_AIF_FREQ_8000HZ)));
#else
        hal_AbbRegWrite(CODEC_FM_MODE, fmModeSetting);
#endif

        // Set DCDC4 clock divider for codec mode
        // Avoid conflicting with hal_AudForceDcdc4ClockOn
        status = hal_SysEnterCriticalSection();
        g_halAudCodecClockClassKPa &= 
            ~(ABB_DCDC4_CLKGEN4_CLK_SEL|ABB_DCDC4_CLKGEN4_DIV_BASE_MASK);
        g_halAudCodecClockClassKPa |= ABB_DCDC4_CLKGEN4_DIV_BASE(160);
        hal_AbbRegWrite(CODEC_CLOCK_CLASS_K_PA, g_halAudCodecClockClassKPa);
        hal_SysExitCriticalSection(status);
    }

    // AUD_3_IN_1_SPK mode has the higher priority to control the notch filter
#if defined(AUD_3_IN_1_SPK) || !defined(ABB_HP_DETECT)
    // Just set the current value (or the initial value)
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
#else // !AUD_3_IN_1_SPK && ABB_HP_DETECT
    status = hal_SysEnterCriticalSection();
    g_halAudBypassNotchFilterFm = isFmMode;
    if ( (g_halAudHpDetect_3dh & ABB_EARPHONE_DET_BYPASS) == 0 &&
         !g_halAudBypassNotchFilterFm )
    {
        // HP DET is on and FM is off
        g_halAudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
    }
    else
    {
        g_halAudHpDetect_3dh |= ABB_NOTCH20_BYPASS;
    }
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
    hal_SysExitCriticalSection(status);
#endif // !AUD_3_IN_1_SPK && ABB_HP_DETECT
#endif // 8809 or later
}

PUBLIC VOID hal_AudSetVoiceMode(BOOL isVoiceMode)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    if (isVoiceMode)
    {
        g_halAudCodecDigEn |= (1<<3);
    }
    else
    {
        g_halAudCodecDigEn &= ~(1<<3);
    }
#else // 8808 or later
    if (isVoiceMode)
    {
        g_halAudCodecDigEn |= ABB_DIG_S_ISPHONE;
    }
    else
    {
        g_halAudCodecDigEn &= ~ABB_DIG_S_ISPHONE;
    }
#endif
    hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn);

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    pmd_SpeakerPaCpLowOutput(isVoiceMode);     
#if (AUD_SPK_DSSI_DETECT==2)
    hal_AudEnableDSSI(TRUE);
#elif (AUD_SPK_DSSI_DETECT==1)
    hal_AudEnableDSSI(!isVoiceMode);
#else
    //hal_AudEnableDSSI(FALSE);			//DSSI mode does not work by default 
#endif	
#endif
}

PUBLIC VOID hal_AudEnableDac(BOOL on)
{
    if (on)
    {
#ifdef AUD_MUTE_FOR_BT_WORKAROUND
        if (g_halAudMuteOutput)
        {
            on = FALSE;
        }
#endif // AUD_MUTE_FOR_BT_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        if (g_halAudHeadphoneDacMuted)
        {
            on = FALSE;
        }
#endif
    }

    if (on)
    {
        // Not to direct-reg codec DAC
        g_halAbbCodecPowerCtrl &= ~ABB_DAC_EN_CODEC_DR;
    }
    else
    {
        // Direct-reg to disable codec DAC
        g_halAbbCodecPowerCtrl |= ABB_DAC_EN_CODEC_DR;
    }
    hal_AbbRegWrite(CODEC_POWER_CTRL, g_halAbbCodecPowerCtrl);
}

PUBLIC VOID hal_AudEnableCodecDsp(BOOL on)
{
#ifdef ABB_HP_DETECT
    //HAL_TRACE(HAL_AUDIO_TRC, 0, "hal_AudEnableCodecDsp: %d, offMap=0x%04x",
    //    on, g_halHpDetectOffBitmap);
#endif

    if (!on)
    {
#ifdef VMIC_POWER_ON_WORKAROUND
#ifdef ABB_HP_DETECT
        // Stop headphone detection before resetting codec digital part
        if (g_halHpDetectOffBitmap == 0)
        {
            // Not respond to HP-OUT IRQ until codec is re-enabled
            pmd_StartHpOutDetect(FALSE);
        }
#endif
#endif
    }

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    // Control the power of codec digital part
    if (on)
    {
        g_halAbbCodecPowerCtrl |= ABB_DAC_EN_CODEC;
    }
    else
    {
        g_halAbbCodecPowerCtrl &= ~ABB_DAC_EN_CODEC;
    }
    hal_AbbRegWrite(CODEC_POWER_CTRL, g_halAbbCodecPowerCtrl);
#else // 8808 or later
    // Control the reset state of codec digital part
    if (on)
    {
        g_halAudCodecDigEn |= ABB_DIG_S_CODEC_EN;
    }
    else
    {
        g_halAudCodecDigEn &= ~ABB_DIG_S_CODEC_EN;
    }
    hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn);
#endif // 8808 or later

    if (on)
    {
#ifdef VMIC_POWER_ON_WORKAROUND
#ifdef ABB_HP_DETECT
        // Restart headphone detection after resetting codec digital part
        if (g_halHpDetectOffBitmap == 0)
        {
            pmd_StartHpOutDetect(TRUE);
        }
#endif
#endif
    }
}

#if defined(SOUND_RECORDER_SUPPORT_16K)			
PUBLIC VOID hal_AudSetSampleRateDiv(HAL_AIF_FREQ_T sampleRate)
{

    g_DivsampleRate = sampleRate;

}
#endif

PUBLIC VOID hal_AudSetSampleRate(UINT32 selReg, UINT32 divReg)
{
    hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, selReg);
    hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_DIV, divReg);
}

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
// Force to enable charge pump clock.
// Called by PMD when LCD backlight is on if LCD_BACKLIGHT_CHARGE_PUMP enabled.
PUBLIC VOID hal_AudForceDcdc4ClockOn(BOOL on)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (on)
    {
        g_halAudCodecClockClassKPa |= (ABB_DCDC4_CLKGEN4_RESETN_DR|
                                       ABB_DCDC4_CLKGEN4_RESETN_REG);
    }
    else
    {
        g_halAudCodecClockClassKPa &= ~(ABB_DCDC4_CLKGEN4_RESETN_DR|
                                        ABB_DCDC4_CLKGEN4_RESETN_REG);
    }
    hal_AbbRegWrite(CODEC_CLOCK_CLASS_K_PA, g_halAudCodecClockClassKPa);
    hal_SysExitCriticalSection(status);
}

PUBLIC VOID hal_AudEnableDSSI(BOOL on)
{
	UINT32 status = hal_SysEnterCriticalSection();
	if(on)
	{
		g_halHpDetect_12h &= ~ABB_PD_SSI;
	}
	else
	{
		g_halHpDetect_12h |= ABB_PD_SSI;
	}
	 hal_AbbRegWrite(CODEC_HP_DETECT_12H, g_halHpDetect_12h);
	 hal_SysExitCriticalSection(status); 
}


#ifdef ABB_HP_DETECT
PUBLIC VOID hal_AudEnableHpDetect(BOOL on, HP_DETECT_REQ_SRC_T src)
{
    //HAL_TRACE(HAL_AUDIO_TRC, 0, "hal_AudEnableHpDetect: %d, src=%d, offMap=0x%04x",
    //    on, src, g_halHpDetectOffBitmap);

    UINT16 prevBitmap = g_halHpDetectOffBitmap;
    if (on)
    {
        g_halHpDetectOffBitmap &= ~(1<<src);
    }
    else
    {
        g_halHpDetectOffBitmap |= (1<<src);
    }

    if (!on && prevBitmap != 0)
    {
        // Headphone detection has been already disabled
        return;
    }
    if (on)
    {
        if (g_halHpDetectOffBitmap != 0)
        {
            // Another src still disables headphone detection
            return;
        }
        if ((g_halAudHpDetect_3dh & ABB_EARPHONE_DET_BYPASS) == 0)
        {
            // Headphone detection is running, and we cannot enable it twice
            // (codec will be reset during the enabling process)
            return;
        }
    }

    if (!on)
    {
        pmd_StartHpOutDetect(FALSE);
    }

    hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn&~ABB_DIG_S_CODEC_EN);

    UINT32 status = hal_SysEnterCriticalSection();
    if (on)
    {
        g_halAudHpDetect_3dh &= ~ABB_EARPHONE_DET_BYPASS;
        g_halHpDetect_12h |= ABB_PADET_EN;
    }
    else
    {
        g_halAudHpDetect_3dh |= ABB_EARPHONE_DET_BYPASS;
        g_halHpDetect_12h &= ~ABB_PADET_EN;
    }
    // AUD_3_IN_1_SPK mode has the higher priority to control the notch filter
#ifndef AUD_3_IN_1_SPK
    if (on && !g_halAudBypassNotchFilterFm)
    {
        g_halAudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
    }
    else
    {
        g_halAudHpDetect_3dh |= ABB_NOTCH20_BYPASS;
    }
#endif
    // CODEC_HP_DETECT_3DH controls the HP DET digital part, and the
    // whole codec digital part should be reset if it changes
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
    // CODEC_HP_DETECT_12H controls the HP DET analog part, and it is
    // irrelevant to codec digital part
    hal_AbbRegWrite(CODEC_HP_DETECT_12H, g_halHpDetect_12h);	
    hal_SysExitCriticalSection(status); 

    // Keep disabling codec for FM to avoid pop sound when searching
    // FM stations
    if (on || src != HP_DETECT_REQ_SRC_FM)
    {
        hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn);
    }

    if (on)
    {
        pmd_StartHpOutDetect(TRUE);
    }
}

PUBLIC VOID hal_AudSaveHeadphoneState(BOOL on)
{
    // Save the headphone state
    g_halAudHeadphonePrevState = on;
}
#endif // ABB_HP_DETECT
#endif // 8809 or later

PUBLIC VOID hal_AudSetDacVolumn(UINT32 vol)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)


    // DacVolumn[6:3] = 0 ... 15 = -15dB ... 0dB
    hal_AbbRegWrite(0x2a, 0xf000|((vol&0xf)<<3));

#else // 8808 or later

    // DacVolumn[6:2] = 0 ... 31 = -26dB ... 5dB
    UINT32 data;
    data = ABB_DIG_SIDE_TONE_GAIN_L(15)|
           ABB_DIG_DAC_VOLUME(vol);
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    data |= ABB_DIG_DITHER_GAIN_H(2);
#else // 8809 or later
    data |= ABB_DIG_DITHER_GAIN_H(0)|
            ABB_DIG_SDM_GAIN(3);
#endif
    hal_AbbRegWrite(CODEC_DIG_DAC_GAIN, data);

#endif // 8808 or later
}

PUBLIC VOID hal_AudSetAdcVolumn(UINT32 vol)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

    // MIC AdcVolumn[12:9] = 0(-10dB), 1(-8dB), 2(-12dB), 3(-4dB)
    // 4(-2dB), 5(-6dB)
    hal_AbbRegWrite(0x2b, 0x0003 | (((vol)&0xf)<<9));

#else // 8808 or later

    // MIC AdcVolumn[12:9] = 0 ... 15 = MUTE -12dB -10dB ... 14dB 16dB
    UINT32 data;
    data = ABB_DIG_MIC_VOLUME(vol)|
           ABB_DIG_SIDE_TONE_GAIN_H(1);
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    data |= ABB_DIG_DITHER_GAIN_L(0);
#else // 8809 or later
    data |= ABB_DIG_DITHER_GAIN_L(4);
#endif
    hal_AbbRegWrite(CODEC_DIG_MIC_GAIN, data);

#endif // 8808 or later
}

PUBLIC VOID hal_AudForceReceiverMicSelection(BOOL on)
{
    g_halAudForceReceiverMicSelection = on;

    if (on)
    {
        HAL_AUD_SEL_T selCfg;
        selCfg = hal_AudGetSelCfg();;
        selCfg.auxMicSel = FALSE;
        hal_AudSetSelCfg(hal_AudGetSelCfg());
    }
}

PUBLIC VOID hal_AudSetSelCfg(HAL_AUD_SEL_T selCfg)
{
	UINT32 prev_cfg = 0;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    // Set CODEC mode
    UINT32 codecSel = 0;
#else // 8808 or later
#ifdef RECV_USE_SPK_LINE
    BOOL spkLeftChanSel = FALSE;
    BOOL spkRightChanSel = FALSE;
#endif
#endif // 8808 or later
    BOOL spkPaForceOn = FALSE;
    spkPaForceOn = spkPaForceOn; // Get rid of compiler warning
    // Save the orginal selection
    HAL_AUD_SEL_T orgSelCfg;
//    selCfg.headSel = TRUE;
    orgSelCfg.reg = selCfg.reg;

#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
#ifdef USE_EP_MIC_WITH_SPK
        // NOTE: This setting might be overwritten
        // if receiver mic is forced to select.
        selCfg.auxMicSel = TRUE;
#endif
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
        selCfg.headSel = TRUE;
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    if (g_halAudForceReceiverMicSelection)
    {
        selCfg.auxMicSel = FALSE;
    }

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (g_halAudLineInSpeakerInUse)
    {
        if ( ! (orgSelCfg.headSel || orgSelCfg.rcvSel ||
                orgSelCfg.spkSel || orgSelCfg.spkMonoSel) )
        {
            // Never power off speaker when speaker is in use
            // (including mute state) in line-in mode on 8808
            selCfg.spkSel = TRUE;
        }
    }
#endif

#ifdef VMIC_POWER_ON_WORKAROUND
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME)
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON &&
        ! (orgSelCfg.headSel || orgSelCfg.rcvSel ||
            orgSelCfg.spkSel || orgSelCfg.spkMonoSel) )
    {
        // Headphone should be muted

        // Set g_halAudHeadphoneDacMuted first, which will be checked
        // in hal_AudEnableDac
        g_halAudHeadphoneDacMuted = TRUE;
        hal_AudEnableDac(FALSE);
    }
    else
    {
        // Headphone is not forced to mute

        // Set g_halAudHeadphoneDacMuted first, which will be checked
        // in hal_AudEnableDac
        g_halAudHeadphoneDacMuted = FALSE;
        hal_AudEnableDac(TRUE);
    }
#endif
#endif

#if defined(AUD_SPK_ON_WITH_LCD) && !defined(RECV_USE_SPK_LINE)
    // If the LCD is off, there is no need to select speaker, for it
    // will be selected once LCD is turned on
    if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
    {
        if ( ! (orgSelCfg.headSel || orgSelCfg.rcvSel ||
                orgSelCfg.spkSel || orgSelCfg.spkMonoSel) )
        {
            // Select speaker output even in idle time, so that the capacitor conntected
            // to the speaker can be charged once LCD is turned on
            spkPaForceOn = TRUE;
            selCfg.spkSel = TRUE;
        }
    }
#elif defined(RECV_USE_SPK_LINE)
    if (orgSelCfg.headSel)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
        codecSel |= ABB_PA_EN_HP;
#endif
    }
    if (orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
    {
        selCfg.spkSel = TRUE;
        selCfg.spkMonoSel = FALSE;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
        if (g_halAudRecvUseSpkLineRight)
        {
            codecSel |= ABB_PA_EN_L_SPK;
        }
        else
        {
            codecSel |= ABB_PA_EN_R_SPK;
        }
#else // 8808 or later
        if (g_halAudRecvUseSpkLineRight)
        {
            spkLeftChanSel = TRUE;
        }
        else
        {
            spkRightChanSel = TRUE;
        }
#endif
    }
    if (orgSelCfg.rcvSel)
    {
        selCfg.spkSel = TRUE;
        selCfg.spkMonoSel = FALSE;
        selCfg.rcvSel = FALSE;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
        if (g_halAudRecvUseSpkLineRight)
        {
            codecSel |= ABB_PA_EN_R_SPK;
        }
        else
        {
            codecSel |= ABB_PA_EN_L_SPK;
        }
#else // 8808 or later
        if (g_halAudRecvUseSpkLineRight)
        {
            spkRightChanSel = TRUE;
        }
        else
        {
            spkLeftChanSel = TRUE;
        }
#endif
    }
    if ( ! (orgSelCfg.headSel || orgSelCfg.rcvSel ||
            orgSelCfg.spkSel || orgSelCfg.spkMonoSel) )
    {
        // If the LCD is off, there is no need to select speaker, for it
        // will be selected once LCD is turned on
        if (g_halAudDacPrevPwr != HAL_AUD_DAC_PREV_PWR_RESTORED)
        {
            // Select speaker output even in idle time, so that the capacitor conntected
            // to the receiver/speaker can be charged once LCD is turned on
            spkPaForceOn = TRUE;
            selCfg.spkSel = TRUE;
            selCfg.spkMonoSel = FALSE;
            selCfg.rcvSel = FALSE;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

#ifdef AUD_SPK_ON_WITH_LCD
            codecSel |= ABB_PA_EN_L_SPK|ABB_PA_EN_R_SPK;
#else // !AUD_SPK_ON_WITH_LCD
            // The CODEC_MODE_SEL still needs configuring, as this register
            // will not be changed when LCD is turned on
            if (g_halAudRecvUseSpkLineRight)
            {
                codecSel |= ABB_PA_EN_R_SPK;
            }
            else
            {
                codecSel |= ABB_PA_EN_L_SPK;
            }
#endif // !AUD_SPK_ON_WITH_LCD

#else // CHIP_ASIC_ID,  8808 or later

#ifdef AUD_SPK_ON_WITH_LCD
            spkLeftChanSel = TRUE;
            spkRightChanSel = TRUE;
#else // !AUD_SPK_ON_WITH_LCD
            if (g_halAudRecvUseSpkLineRight)
            {
                spkRightChanSel = TRUE;
            }
            else
            {
                spkLeftChanSel = TRUE;
            }
#endif // !AUD_SPK_ON_WITH_LCD

#endif // CHIP_ASIC_ID
        }
    }
#endif // RECV_USE_SPK_LINE

    // Speaker PA power
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    BOOL spkPaReq;
    if (orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
    {
        spkPaReq = TRUE;
    }
    else
    {
        spkPaReq = FALSE;
    }
    pmd_SpeakerPaPreEnable(spkPaReq);
#endif // 8809 or later

    // Check if vmic will be invalid
#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifdef MIC_CAPLESS_MODE
        if (!selCfg.auxMicSel)
        {
            g_halAudVMicValid = FALSE;
        }
#endif
#endif
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME == 0)
        if (!selCfg.headSel)
        {
            hal_AudEnableHpDetect(FALSE, HP_DETECT_REQ_SRC_AUD);
        }
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // ----------------------------------------
    // Setting audio_sel_cfg register
    // ----------------------------------------
    prev_cfg = selCfg.reg;
    hwp_configRegs->audio_sel_cfg = 0;

    // ----------------------------------------
    // End of setting audio_sel_cfg register
    // ----------------------------------------

    // Check if vmic is valid
#ifdef VMIC_POWER_ON_WORKAROUND
    if (g_halAudEarpieceDetectionOn == HAL_AUD_EP_DETECT_ON)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifdef MIC_CAPLESS_MODE
        {
            HAL_AUD_PD_T pdStatus;
            pdStatus = hal_AudGetPowerStatus();
            if (selCfg.auxMicSel && 
                pdStatus.noDeepPd && pdStatus.noRefPd &&
                pdStatus.noAuxMicPd && pdStatus.noAdcPd)
            {
                g_halAudVMicValid = TRUE;
            }
        }
#endif
#endif
#if defined(ABB_HP_DETECT) && (ABB_HP_DETECT_ON_IDLE_TIME == 0)
        {
            HAL_AUD_PD_T pdStatus;
            pdStatus = hal_AudGetPowerStatus();
            if (selCfg.headSel &&
                pdStatus.noDeepPd && pdStatus.noRefPd &&
                pdStatus.noDacPd && pdStatus.noDacReset)
            {
                hal_AudEnableHpDetect(TRUE, HP_DETECT_REQ_SRC_AUD);
            }
        }
#endif
    }
#endif // VMIC_POWER_ON_WORKAROUND

    // Speaker PA power
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE && \
     CHIP_ASIC_ID != CHIP_ASIC_ID_8808) // 8809 or later
    pmd_SpeakerPaPostEnable(spkPaReq);
    if (g_halAudUser == HAL_AUD_USER_LINEIN || g_halAudUser == HAL_AUD_USER_FM)
    {
        // Class D/K speaker needs 26M clock. Internal FM can supply 26M to
        // speaker, but the line-in channel cannot do that. So deep sleep
        // will be disallowed when speaker is on in line-in mode, and it
        // will be allowed when speaker is off in line-in mode.
        hal_AudRequestFreq(HAL_AUD_FREQ_REQ_SRC_NORMAL, spkPaReq);
    }
#endif // 8809 or later

    // Line-in & MIC config
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    UINT32 lineInSetting = 0x43;
    if (g_halAudUser == HAL_AUD_USER_LINEIN)
    {
        if (orgSelCfg.headSel)
        {
            lineInSetting |= ABB_CALIB_LINEIN_HP;
        }
        if (orgSelCfg.spkMonoSel || orgSelCfg.spkSel)
        {
            lineInSetting |= ABB_CALIB_LINEIN_SPK;
        }
    }
    hal_AbbRegWrite(CODEC_CALIB_SETTING, lineInSetting);
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    UINT32 miscSetting = gc_halAbbCodecMiscSetting;
    UINT32 modeSel = 0;
    UINT32 pllSetting1 = gc_halAbbCodecPllSetting1;
    g_halAbbCodecPowerCtrl &= ~(ABB_RESET_MIC_DR|ABB_RESET_MIC_REG);
    if (g_halAudUser == HAL_AUD_USER_LINEIN)
    {
        if (orgSelCfg.headSel || orgSelCfg.rcvSel ||
            orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
        {
            // Select line-in mode
            modeSel |= ABB_LINEIN_MODE_EN;

            if (orgSelCfg.headSel)
            {
                miscSetting &= ~ABB_MIC_MODE_EN;
                miscSetting |= ABB_MIC_FM_L_EN|ABB_MIC_FM_R_EN;
            }
            else if (orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
            {
                // Unmute speaker
                HAL_AUD_SPK_CFG_T spkCfg = hal_AudGetSpkCfg();
                spkCfg.noSpkMute = TRUE;
                hal_AudSetSpkCfg(spkCfg);
                // Wait until speaker is ready
                sxr_Sleep(50 MS_WAITING);

                // Enable speaker test mode to make it stable
                miscSetting |= ABB_TEST_EN_SPK;
                // Select MIC mode to make speaker stable
                // (but keep MIC in reset state)
                g_halAbbCodecPowerCtrl |= ABB_RESET_MIC_DR|ABB_RESET_MIC_REG;
                pllSetting1 |= ABB_MIC_STD_EN_DR|ABB_MIC_AUX_EN_DR;
                //modeSel |= ABB_SPK_MIC_EN;
            }
        }
        else // line-in mute
        {
            // Switch off the line-in mode, and mute the speaker DAC line
            // (to avoid noises when AUD_SPK_ON_WITH_LCD is enabled)
            modeSel |= ABB_SPK_DAC_EN_DR;
        }
    }
    hal_AbbRegWrite(CODEC_POWER_CTRL, g_halAbbCodecPowerCtrl);
    hal_AbbRegWrite(CODEC_PLL_SETTING1, pllSetting1);
    hal_AbbRegWrite(CODEC_MISC_SETTING, miscSetting);
    if (modeSel & ABB_SPK_MIC_EN)
    {
        // First enable MIC mode only
        hal_AbbRegWrite(CODEC_MODE_SEL, modeSel&~ABB_LINEIN_MODE_EN);
        // Second enable line-in mode as well
    }
    hal_AbbRegWrite(CODEC_MODE_SEL, modeSel);
#else // 8809 or later
    UINT32 lineInSetting = 0;
    UINT32 micSetting = gc_halAbbCodecMicSetting;
    if (g_halAudUser == HAL_AUD_USER_LINEIN || g_halAudUser == HAL_AUD_USER_FM)
    {
        if (orgSelCfg.headSel || orgSelCfg.rcvSel ||
            orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
        {
            if(g_halAudUser == HAL_AUD_USER_LINEIN)
                lineInSetting |= ABB_LINEIN_TO_PA_MODE;

            // A. muxin enable
            micSetting |= ABB_MUXIN_LEFT_EN_DR|ABB_MUXIN_LEFT_EN_REG|
                          ABB_MUXIN_RIGHT_EN_DR|ABB_MUXIN_RIGHT_EN_REG;

            // B. normal/mux mic disable
            micSetting |= ABB_MICIN_IBPGA_EN_DR;
            micSetting &= ~ABB_MICIN_IBPGA_EN_REG;
        }
        else // line-in mute
        {
            // TODO: Need to mute the speaker DAC line too?
        }
    }
		
    hal_AbbRegWrite(CODEC_LINEIN_SETTING, lineInSetting);
    hal_AbbRegWrite(CODEC_MIC_SETTING, micSetting);
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    // Select codec mode (Gallite only)
    if (selCfg.rcvSel || selCfg.spkSel || selCfg.spkMonoSel)
    {
        // If audio PA is in forced-on status, there is no need to
        // switch on the audio source (speaker mode)
        if (!spkPaForceOn)
        {
#ifdef RECV_USE_SPK_LINE
            if (codecSel & ABB_PA_EN_R_SPK)
            {
                codecSel |= ABB_SPK_MODE1_R_SEL;
            }
            else
            {
                codecSel |= ABB_SPK_MODE1_L_SEL;
            }
#else // !RECV_USE_SPK_LINE
            codecSel |= ABB_SPK_MODE1_L_SEL|ABB_SPK_MODE1_R_SEL;
#endif // !RECV_USE_SPK_LINE
        }
        if (selCfg.fmSel)
        {
            codecSel |= ABB_SPK_FM_SEL;
        }
    }
    if (selCfg.headSel)
    {
        codecSel |= ABB_HP_MODE1_L_SEL|ABB_HP_MODE1_R_SEL;
        if (selCfg.fmSel)
        {
            codecSel |= ABB_HP_FM_SEL;
        }
        else
        {
            codecSel |= ABB_HP_DAC_SEL;
        }
    }
    hal_AbbRegWrite(CODEC_MODE_SEL, codecSel);
#endif // (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)

#ifdef RECV_USE_SPK_LINE
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    if (selCfg.spkMonoSel || selCfg.spkSel)
    {
        // Direct-reg to power on codec
        g_halAbbCodecPowerCtrl |= ABB_PD_CODEC_DR;
    }
    else
    {
        // Not to direct-reg codec power
        g_halAbbCodecPowerCtrl &= ~ABB_PD_CODEC_DR;
    }
    hal_AbbRegWrite(CODEC_POWER_CTRL, g_halAbbCodecPowerCtrl);
#else // 8808 or later
    UINT32 ldoSetting1 = 0;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    UINT32 ldoSetting2 = ABB_VCOM_MODE(6)|
                         ABB_VREF_MODE(0)|
                         ABB_ISEL_BIT_B(2);
#endif
    if (selCfg.spkMonoSel || selCfg.spkSel)
    {
        // Direct-reg codec dac control
        ldoSetting1 |= ABB_PD_DAC_L_DR|ABB_PD_DAC_R_DR;
        if (spkPaForceOn)
        {
            ldoSetting1 |= ABB_PD_DAC_L_REG|ABB_PD_DAC_R_REG;
        }
        else
        {
            if (!spkRightChanSel)
            {
                ldoSetting1 |= ABB_PD_DAC_R_REG;
            }
            if (!spkLeftChanSel)
            {
                ldoSetting1 |= ABB_PD_DAC_L_REG;
            }
        }
        // Direct-reg codec spk pa control
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
        ldoSetting2 |= ABB_PA_EN_L_SPK_DR|ABB_PA_EN_R_SPK_DR;
        if (spkLeftChanSel)
        {
            ldoSetting2 |= ABB_PA_EN_L_SPK_REG;
        }
        if (spkRightChanSel)
        {
            ldoSetting2 |= ABB_PA_EN_R_SPK_REG;
        }
#endif
    }
    // DAC control
    hal_AbbRegWrite(CODEC_LDO_SETTING1, ldoSetting1);
    // Speaker PA control
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    hal_AbbRegWrite(CODEC_LDO_SETTING2, ldoSetting2);
#else // 8809 or later
    if (selCfg.spkMonoSel || selCfg.spkSel)
    {
        pmd_SpeakerPaSelectChannel(spkLeftChanSel, spkRightChanSel);
    }
    else
    {
        pmd_SpeakerPaSelectChannel(FALSE, FALSE);
    }
#endif // 8809 or later
#endif // CHIP_ASIC_ID, 8808 or later
#endif // RECV_USE_SPK_LINE

#ifdef AUD_SPK_ON_WITH_LCD
    if (pmd_AudExtPaExists())
    {
        if (orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
        {
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, TRUE);
            g_halAudExtPaConfigNeeded = TRUE;
        }
        else if (g_halAudExtPaConfigNeeded)
        {
            if ( ! (selCfg.spkSel || selCfg.spkMonoSel) )
            {
                // Internal PA has just been turned off
                // aud_Setup() in interrupts will never run into here?
                sxr_Sleep(g_halAudIntSpkPaStableTime);
            }
            pmd_EnablePower(PMD_POWER_LOUD_SPEAKER, FALSE);
            g_halAudExtPaConfigNeeded = FALSE;
        }
    }
#endif // AUD_SPK_ON_WITH_LCD
    hwp_configRegs->audio_sel_cfg = prev_cfg;
}

PUBLIC VOID hal_AudSetMicCfg(HAL_AUD_MIC_CFG_T micCfg)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (micCfg.micGain >= 8)
        micCfg.micGain -= 8;
    else
        micCfg.micGain += 8;
#endif

    hwp_configRegs->audio_mic_cfg = micCfg.reg;
}

PUBLIC VOID hal_AudSetSpkCfg(HAL_AUD_SPK_CFG_T spkCfg)
{

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if ( (g_halAudLineInSpeakerInUse || g_halAudUser == HAL_AUD_USER_LINEIN) &&
         !spkCfg.noSpkMute )
    {
        // Never mute speaker when in line-in mode on 8808
        spkCfg.spkGain = 0;
        spkCfg.noSpkMute = TRUE;
    }
HAL_AUD_SEL_T selCfg = hal_AudGetSelCfg();
if (g_halAudUser == HAL_AUD_USER_LINEIN)
{
   if (selCfg.spkSel || selCfg.spkMonoSel)
   {
        HAL_AUD_SPK_CFG_T oldSpkCfg = hal_AudGetSpkCfg();
        if (!oldSpkCfg.noSpkMute && spkCfg.noSpkMute)
        {
            oldSpkCfg = spkCfg;
            for (oldSpkCfg.spkGain = 10; oldSpkCfg.spkGain > spkCfg.spkGain; oldSpkCfg.spkGain -= 2)
            {
                hwp_configRegs->audio_spk_cfg = oldSpkCfg.reg;
                hal_TimDelay(10);
            }
        }
   }
}	
#endif

    hwp_configRegs->audio_spk_cfg = spkCfg.reg;
}

PUBLIC VOID hal_AudSetRcvGain(UINT32 gain)
{


#ifdef RECV_USE_SPK_LINE
    HAL_AUD_SPK_CFG_T spkCfg;
    spkCfg.noSpkMute = TRUE;
    spkCfg.spkGain = gain;
    hwp_configRegs->audio_spk_cfg = spkCfg.reg;
#else // !RECV_USE_SPK_LINE
    hwp_configRegs->audio_rcv_gain = CFG_REGS_AU_RCV_GAIN(gain);
#endif // !RECV_USE_SPK_LINE
}

PUBLIC VOID hal_AudSetHeadGain(UINT32 gain)
{


    hwp_configRegs->audio_head_gain = CFG_REGS_AU_HEAD_GAIN(gain);
}

PUBLIC HAL_AUD_PD_T hal_AudGetPowerStatus(VOID)
{
    HAL_AUD_PD_T pdStatus;
    pdStatus.reg = hwp_configRegs->audio_pd_set;

#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD)
    if (g_halAudDacPrevPwr == HAL_AUD_DAC_PREV_PWR_OFF)
    {
        pdStatus.noDacPd = FALSE;
    }
    else if (g_halAudDacPrevPwr == HAL_AUD_DAC_PREV_PWR_ON)
    {
        pdStatus.noDacPd = TRUE;
    }
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD

    return pdStatus;
}

PUBLIC HAL_AUD_SEL_T hal_AudGetSelCfg(VOID)
{
    HAL_AUD_SEL_T selCfg;
    selCfg.reg = hwp_configRegs->audio_sel_cfg;
    return selCfg;
}

PUBLIC HAL_AUD_MIC_CFG_T hal_AudGetMicCfg(VOID)
{
    HAL_AUD_MIC_CFG_T micCfg;
    micCfg.reg = hwp_configRegs->audio_mic_cfg;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (micCfg.micGain >= 8)
        micCfg.micGain -= 8;
    else
        micCfg.micGain += 8;
#endif

    return micCfg;
}

PUBLIC HAL_AUD_SPK_CFG_T hal_AudGetSpkCfg(VOID)
{
    HAL_AUD_SPK_CFG_T spkCfg;
    spkCfg.reg = hwp_configRegs->audio_spk_cfg;
    return spkCfg;
}

PUBLIC UINT32 hal_AudGetRcvGain(VOID)
{
    return hwp_configRegs->audio_rcv_gain & CFG_REGS_AU_RCV_GAIN_MASK;
}

PUBLIC UINT32 hal_AudGetHeadGain(VOID)
{
    return hwp_configRegs->audio_head_gain & CFG_REGS_AU_HEAD_GAIN_MASK;
}

PUBLIC UINT32 hal_AudRcvGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
    return db/3*2;
}

PUBLIC UINT32 hal_AudHeadGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
    return db/3*2;
}

PUBLIC UINT32 hal_AudSpkGainDb2Val(INT32 db)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
    return db/3*2;
#else // 8809 or later
    // 0:-2dB, 2:10dB, 4:16dB, 6:19db, 8:20.5dB,
    // 10:22dB, 12:23dB, 14:24dB
    UINT32 reg;
    if (db <= 4)
        reg = 0;
    else if (db <= 13)
        reg = 2;
    else if (db <= 17)
        reg = 4;
    else if (db <= 19)
        reg = 6;
    else if (db <= 21)
        reg = 8;
    else if (db <= 22)
        reg = 10;
    else if (db <= 23)
        reg = 12;
    else // 24 or above
        reg = 14;
    return reg;
#endif
}

PUBLIC UINT32 hal_AudMicGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 21)
        db = 21;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    return db/3*2;
#else // 8808 or later
    return 8+db/3;
#endif
}

PUBLIC UINT32 hal_AudDacGainDb2Val(INT32 db)
{
    UINT32 value;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    // DacVolumn[6:3] = 0 ... 15 = -15dB ... 0dB
    if (db < -15)
        db = -15;
    else if (db > 0)
        db = 0;
    value = db + 15;
#else // 8808 or later
    // DacVolumn[6:2] = 0 ... 31 = -26dB ... 5dB
    if (db < -26)
        db = -26;
    else if (db > 5)
        db = 5;
    value = db + 26;
#endif
    return value;
}

PUBLIC UINT32 hal_AudAdcGainDb2Val(INT32 db)
{
    UINT32 value;
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    // MIC AdcVolumn[12:9] = 0(-10dB), 1(-8dB), 2(-12dB), 3(-4dB)
    // 4(-2dB), 5(-6dB)
    CONST INT8 availDbs[] = { -10, -8, -12, -4, -2, -6 };
    for (value = 0; value < sizeof(availDbs); value++)
    {
        if (availDbs[value] == db)
            break;
    }
    if (value == sizeof(availDbs))
        value = 5;
#else // 8808 or later
    // MIC AdcVolumn[12:9] = 0 ... 15 = MUTE -12dB -10dB ... 14dB 16dB
    if (db > 16)
        db = 16;
    if (db < -12)
        value = 0; // MIC ADC MUTE
    else
        value = (db + 12) / 2 + 1;
#endif
    return value;
}

void SetMicVal(UINT32 val)
{
    HAL_AUD_MIC_CFG_T micCfg;
    micCfg.noMicMute = TRUE;
    micCfg.micGain = val;
    hal_AudSetMicCfg(micCfg);
    hal_AudSetAdcVolumn(val);
}


