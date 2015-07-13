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
//  $HeadURL: http://10.10.100.14/svn/developing/Sources/svc/branches/Total_BJ/vois/src/vois.c $ //
//	$Author: xuml $                                                        // 
//	$Date: 2010-06-17 14:53:21 +0800 (Thu, 17 Jun 2010) $                     //   
//	$Revision: 36750 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file vois.c                                                               //
/// That file implementes the VOIS service.                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"
#include "string.h"

#include "hal_timers.h"
#include "hal_aif.h"
#include "hal_sys.h"
#include "hal_speech_bt.h"
#include "hal_overlay.h"

#include "vois_m.h"
//#include "vois_map.h"
//#include "voisp_map.h"
//#include "voisp_cfg.h"

#include "aud_m.h"
#include "vpp_speech_bt.h"
#include "tgt_calib_m.h"

#include "hal_map_engine.h"

#include "sxr_sbx.h"
#include "mcip_debug.h"
#include "mci.h"
#include "cos.h"
#include "event.h"
#include "dm.h"
#include "speex/speex_preprocess.h"

// =============================================================================
//  MACROS
// =============================================================================
/// Number of trames before unmuting VPP
#define BT_VPP_UNMUTE_TRAME_NB 2

#define VOC_INTR_STEP


// =============================================================================
//  DEBUGE
// =============================================================================
//#define DEBUG_TICKS
#ifdef DEBUG_TICKS
	INT32 g_VoCHandleTicks = 0;
	INT32 g_ScoTicks = 0;
#endif
// =============================================================================
//  CvsdEnc
// =============================================================================
INT16 PcmOutCvsdBuf[80];



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

#define MMC_SCO_CVSD_SIZE           30
#define MMC_SCO_SEND_BUFF_SIZE      (160*2)
#define MMC_SCO_BUFF_SIZE           (160*2)
#define MMC_SCO_MIC_BUFF_SIZE		(640)
#define CVSD_ZERO_DATA_COUNT        (10)

UINT16 ReceivedScoDataCache[MMC_SCO_BUFF_SIZE];
static UINT32  MicData[MMC_SCO_MIC_BUFF_SIZE/4];
static UINT32  ScoDataToVoc[MMC_SCO_BUFF_SIZE/4];

UINT16 SendDataLen = 0;
UINT16 ReceiveDataLen = 0;
UINT16 ScoDataToVocDataLen = 0;
BOOL  MMC_ScoMicDataReadySend = FALSE;
BOOL  MMC_ScoDataReadyPlay = FALSE;
BOOL  MMC_ScoDataReadySend= FALSE;

extern int32 MMC_AudioSCO_AsynSendReq(int16* pdu,uint16 handle, uint16 length);


// =============================================================================
// g_BtCtx
// -----------------------------------------------------------------------------
/// VoiS context, holding global variables, to be watched through CoolWatcher.
// =============================================================================
PROTECTED VOIS_CONTEXT_T g_BtCtx =
{
            .voisStarted = FALSE,
            .voisEncOutput = NULL,
            .voisDecInput = NULL, 
            .voisTramePlayedNb = 0,
            .voisAudioCfg = {0,0,0,0},
            .voisVppCfg = {0,0,0,0,0,0,0,0,NULL, NULL,0},
    .voisItf = AUD_ITF_NONE,
    .voisLatestCfg =
    {
                                .spkSel     = AUD_SPK_RECEIVER,
                                .micSel     = AUD_MIC_RECEIVER,
                                .spkLevel   = AUD_SPK_MUTE,
                                .micLevel   = AUD_MIC_MUTE,
                                .sideLevel  = AUD_SIDE_MUTE,
                                .toneLevel  = AUD_TONE_0dB,
                                .encMute    = VPP_SPEECH_MUTE, 
        .decMute    = VPP_SPEECH_MUTE
    },
            .mode = 0,
            .bufferSize = MMC_SCO_BUFF_SIZE,
            .fileHandle = -1,
};

///  Audio params,MDF and SDF profile.
const UINT16 default_sdfFilter[] = {
                0x4000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000, \
				0x0000, 0x0000, 0x0000, 0x0000};
                
/// Stream to encode.
PUBLIC HAL_AIF_STREAM_T micStream;

/// Decoded stream.
extern HAL_AIF_STREAM_T audioStream;

/// User handler to call with play status is stored here.
PRIVATE VOIS_USER_HANDLER_T g_BtUserHandler= NULL;

/// SRAM overlay usage flag
PRIVATE BOOL g_BtOverlayLoaded = FALSE;

/// Record flag
PRIVATE VOLATILE BOOL g_BtRecordFlag = FALSE;
       
/// End address of the stream buffer    
PRIVATE UINT32 g_BtBufferLength= 0;

/// End address of the stream buffer    
PRIVATE UINT32* g_BtBufferTop = NULL;

/// PCM buffer address
UINT32* g_BtRecordMicPcmBuffer = NULL;

PRIVATE  UINT8 *g_BtAgcPcmBufferReceiver=NULL;

// =============================================================================
//  FUNCTIONS
// =============================================================================

VOLATILE AUD_ITF_T BT_AudInterface = 0;

VOLATILE INT16 BT_VoCRunFlag = 0;

VOLATILE INT8 BT_AudPath = 0; // 0 for ear-piece, 1 for louder speaker


INT8* Get_BtvoisEncOutput(void)
{
    return (INT8*)g_BtCtx.voisEncOutput;
}

INT8* Get_BtAgcPcmBufferReceiver(void)
{
    return (INT8*)g_BtAgcPcmBufferReceiver;
}

// =============================================================================
// BTSco_SetCalibratedVppCfg
// -----------------------------------------------------------------------------
/// Using a VOIS configuration structure, this function gets the calibrated data
/// from the calibration structure to configure properly a VPP config
/// structure.
/// @param itf Audio Interface used.
/// @param vppCfg VPP configuration structure to set with calibrated data.
// =============================================================================
PRIVATE VOID BTSco_SetCalibratedVppCfg(AUD_ITF_T itf,
                                      VPP_SPEECH_AUDIO_CFG_T* vppCfg)
{
    // FIXME : All calibration to be checked
    //CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    CALIB_BUFFER_T * calibPtr = tgt_GetCalibConfig();

    // Set the configuration (in the global variables)
    // Get VPP config From Calib
    vppCfg->echoEsOn     = calibPtr->aud.echo[BT_AudPath].vocEc.esOn;
    vppCfg->echoEsVad    = calibPtr->aud.echo[BT_AudPath].vocEc.esVad;
    vppCfg->echoEsDtd    = calibPtr->aud.echo[BT_AudPath].vocEc.esDtd;
    vppCfg->echoExpRel   = calibPtr->aud.echo[BT_AudPath].vocEc.ecRel;
    vppCfg->echoExpMu    = ((calibPtr->aud.echo[BT_AudPath].vocEc.ecMu == 0) || (g_BtCtx.voisAudioCfg.spkLevel < calibPtr->aud.echo[BT_AudPath].vocEc.ecMu)) ? 0 : 1;
    vppCfg->echoExpMin   = calibPtr->aud.echo[BT_AudPath].vocEc.ecMin;
    vppCfg->sdf          = default_sdfFilter;// = (INT32*)calib_bb_audio[itf].vocFilters.sdfFilter;
    vppCfg->mdf          = default_sdfFilter;// = (INT32*)calib_bb_audio[itf].vocFilters.mdfFilter;    
}



// =============================================================================
// BTSco_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams. 
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called 
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID BTSco_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* status)
{
	      // FIXME : All calibration to be checked
  //  CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    CALIB_BUFFER_T * calibPtr = tgt_GetCalibConfig();
    COS_EVENT ev;
    ev.nEventId = EV_BT_SCO_REC;
   //MCI_TRACE(MCI_AUDIO_TRC, 0, "BTSco_VppSpeechHandler");
   UINT32 voc_ticks;
   voc_ticks = csw_TMGetTick();
   
 #ifdef DEBUG_TICKS  
   hal_HstSendEvent(SYS_EVENT,0x77700000 + voc_ticks - g_VoCHandleTicks);
   g_VoCHandleTicks = voc_ticks;
 #endif
 
    BT_VoCRunFlag=0;
    if (g_BtCtx.voisTramePlayedNb == BT_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        vpp_SpeechBTAudioCfg((VPP_SPEECH_AUDIO_CFG_T*)&g_BtCtx.voisVppCfg);
    }

    // FIXME: trick to only copy data after a encode finished interrupt
    {    
        // Copy the buffer to decode (Rx) and fetch from VPP's memory
        // the freshly encoded buffer (Tx).
        


		if(g_BtCtx.voisTramePlayedNb%2==0)
		{				
		        hal_Speech_BtPopRxPushTx(g_BtCtx.voisDecInput, g_BtCtx.voisEncOutput);
#if APP_SUPPORT_BT_RECORD == 1
	            if(g_BtCtx.fileHandle>=0)
	            {
	              ev.nParam1 = 0;
			      COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		        }    
#endif					
		}
		else
		{
				
		        hal_Speech_BtPopRxPushTx((UINT8*)g_BtCtx.voisDecInput+320, (UINT8*)g_BtCtx.voisEncOutput+320);		
#if APP_SUPPORT_BT_RECORD == 1
	            if(g_BtCtx.fileHandle>=0)
	            {
	                ev.nParam1 = 1;
			        COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		        }
#endif
		}

   //     memset(g_BtAgcPcmBufferReceiver, 0x91, 640);

			

			vpp_SpeechBTSetAecEnableFlag(calibPtr->aud.echo[BT_AudPath].audioParams.AecEnbleFlag,
	   						calibPtr->aud.echo[BT_AudPath].audioParams.AgcEnbleFlag);		


			vpp_SpeechBTSetAecPara(calibPtr->aud.echo[BT_AudPath].audioParams.NoiseGainLimit,
	   						calibPtr->aud.echo[BT_AudPath].audioParams.NoiseMin,
	   						calibPtr->aud.echo[BT_AudPath].audioParams.NoiseGainLimitStep,
	   						calibPtr->aud.echo[BT_AudPath].audioParams.AmpThr,
	   						calibPtr->aud.echo[BT_AudPath].audioParams.StrongEchoFlag);	


			vpp_SpeechBTSetFilterPara(calibPtr->aud.echo[BT_AudPath].audioParams.Flags);


	        g_BtCtx.voisTramePlayedNb++;
	}
    
}
// =============================================================================
// BTSco_Setup
// -----------------------------------------------------------------------------
/// Configure the vois service.. 
/// 
/// This functions configures the audio interface for the speech stream:
/// gain for the side tone and the speaker, input selection for the microphone
/// and output selection for the speaker... It also configures the decoding
/// parameters like audio cancellation. (cf #VOIS_AUDIO_CFG_T for exhaustive
/// details).
/// 
/// DO NOT CALL THAT FUNCTION BEFORE #vois_start.
/// 
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See 
/// #VOIS_AUDIO_CFG_T for more details.
/// @return #VOIS_ERR_NO it can execute the configuration.
// =============================================================================

PUBLIC VOIS_ERR_T BTSco_Setup(AUD_ITF_T itf, CONST VOIS_AUDIO_CFG_T* cfg)
{
    AUD_ERR_T audErr = AUD_ERR_NO;
    AUD_LEVEL_T audioCfg;
    VPP_SPEECH_AUDIO_CFG_T vppCfg;

    MCI_TRACE(MCI_AUDIO_TRC, 0, "BTSco_Setup itf=%d", itf);

      // FIXME : All calibration to be checked
 //   CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    CALIB_BUFFER_T * calibPtr = tgt_GetCalibConfig();
    //dbg_TraceOutputText(0 , "BTSco_Setup");

    // Check that VOIS is running
    if (g_BtCtx.voisItf == AUD_ITF_NONE || !g_BtCtx.voisStarted)
    {
        // Ignore this call
	//dbg_TraceOutputText(0 , "Ignore this call");
        return VOIS_ERR_NO;
    }

    while (BT_VoCRunFlag==0);
    while (BT_VoCRunFlag==1 && g_BtCtx.voisStarted==TRUE)
    {
        // //dbg_TraceOutputText(0 , "BTSco_Setup waiting");   
    }

  //  BT_SpkLevel=cfg->spkLevel;
	
    BT_AudInterface=itf;

    BT_AudPath = itf - AUD_ITF_BT_EP;
    
    // Record the last applied configuration
    *(VOIS_MAP_AUDIO_CFG_T*)&g_BtCtx.voisLatestCfg =*(VOIS_MAP_AUDIO_CFG_T*)cfg;
    
    // Those parameters are not calibration dependent.
    g_BtCtx.voisVppCfg.encMute = cfg->encMute;
    g_BtCtx.voisVppCfg.decMute = cfg->decMute;

    // disable push-to-talk
    g_BtCtx.voisVppCfg.if1 = 0; 

    // Set audio interface 
    g_BtCtx.voisAudioCfg.spkLevel = cfg->spkLevel;
    g_BtCtx.voisAudioCfg.micLevel = cfg->micLevel;
    g_BtCtx.voisAudioCfg.sideLevel = cfg->sideLevel;
    g_BtCtx.voisAudioCfg.toneLevel = cfg->toneLevel;
   
    // get calibration params for Vpp Speech. Need to be called after setting g_BtCtx 
    BTSco_SetCalibratedVppCfg(itf, (VPP_SPEECH_AUDIO_CFG_T*)&g_BtCtx.voisVppCfg);

    // If the interface is different, we stop and resume the streams
    // to apply the interface change.
    if (itf != g_BtCtx.voisItf)
    {
        // Stop the stream.
        audErr = aud_StreamStop(g_BtCtx.voisItf);
        g_BtCtx.voisItf = itf;
		

        // Set the DAC to use 
        audioStream.voiceQuality = 
#ifdef VOIS_USE_STEREO_DAC_FOR_LOUDSPEAKER
                        // use stereo DAC if the mode is loudspeaker 
                        (itf == AUD_ITF_LOUD_SPEAKER) ? FALSE :
#endif
#ifdef VOIS_USE_STEREO_DAC_FOR_EARPIECE
                        // use stereo DAC if the mode is earpiece 
                        (itf == AUD_ITF_EAR_PIECE) ?   FALSE :
#endif
                        // default 
                        TRUE;

        // Enforce temporary muting and progressive level resume by the 
        // speech handler.
        g_BtCtx.voisTramePlayedNb = 1;
        
        // Set audio interface, initially muted.
        *(AUD_LEVEL_T*)&audioCfg = *(AUD_LEVEL_T*)&g_BtCtx.voisAudioCfg;
        audioCfg.spkLevel   = cfg->spkLevel;
        audioCfg.micLevel   = cfg->micLevel;
        audioCfg.toneLevel  = cfg->sideLevel;
        audioCfg.sideLevel  = cfg->toneLevel;




        // Set the VPP config, initially muted.
        *(VPP_SPEECH_AUDIO_CFG_T*)&vppCfg = *(VPP_SPEECH_AUDIO_CFG_T*)&g_BtCtx.voisVppCfg;
        vppCfg.encMute = VPP_SPEECH_MUTE;
        vppCfg.decMute = VPP_SPEECH_MUTE;
	
        vpp_SpeechBTAudioCfg(&vppCfg);

	vpp_SpeechBTReset();


        if (audErr == AUD_ERR_NO)
        {
            // configure AUD and VPP buffers
            audErr = aud_StreamStart(itf, &audioStream, &audioCfg);
        }
        
        if (audErr == AUD_ERR_NO)
        {
            // Start the other flux only if the Rx one is started
            audErr = aud_StreamRecord(itf, &micStream, &audioCfg);
        }
    }
    else
    {
        // Just apply the configuration change on the same
        // interface.
        vpp_SpeechBTAudioCfg((VPP_SPEECH_AUDIO_CFG_T*)&g_BtCtx.voisVppCfg);
        audErr = aud_Setup(itf, (AUD_LEVEL_T*)&g_BtCtx.voisAudioCfg);
    }
	//Set Mic Gain in VoC

    if (cfg->encMute == VPP_SPEECH_MUTE)
        vpp_SpeechBTSetInAlgGain(CALIB_AUDIO_GAIN_VALUE_MUTE);
    else
        vpp_SpeechBTSetInAlgGain(calibPtr->aud.audioGains[calibPtr->aud.gainItfMap[itf]].inGains.alg);

    vpp_SpeechBTSetAecEnableFlag(calibPtr->aud.echo[BT_AudPath].audioParams.AecEnbleFlag,
   						calibPtr->aud.echo[BT_AudPath].audioParams.AgcEnbleFlag);	
   
    vpp_SpeechBTSetAecPara(calibPtr->aud.echo[BT_AudPath].audioParams.NoiseGainLimit,
   						calibPtr->aud.echo[BT_AudPath].audioParams.NoiseMin,
   						calibPtr->aud.echo[BT_AudPath].audioParams.NoiseGainLimitStep,
   						calibPtr->aud.echo[BT_AudPath].audioParams.AmpThr,
   						calibPtr->aud.echo[BT_AudPath].audioParams.StrongEchoFlag);	

    vpp_SpeechBTSetFilterPara(calibPtr->aud.echo[BT_AudPath].audioParams.Flags);

    CALIB_AUDIO_OUT_GAINS_T *pOutGains;
    pOutGains = &calibPtr->aud.audioGains[calibPtr->aud.gainItfMap[itf]].outGains[cfg->spkLevel];
    vpp_SpeechBTSetOutAlgGain(pOutGains->voiceOrEpAlg);

    //dbg_TraceOutputText(0 , "SETUP Gain=%d", pOutGains->voiceOrEpAlg);

//    //dbg_TraceOutputText(0 , "BT_VoCRunFlag=%d", BT_VoCRunFlag);

#ifndef VOC_INTR_STEP    
    hal_VocIntrOpen(Wakeup_Mask_Status);
#endif
	//dbg_TraceOutputText(0 , "BTSco_Setup RET=%dl" ,audErr);

    switch (audErr)
    {
        // TODO Add error as they come

        case AUD_ERR_NO:

            return VOIS_ERR_NO;
            break;

        default:

            return audErr; // ...unknown error ...
            break;
    }
}





VOID BTSco_txhandle_half(VOID)
{

#ifdef DEBUG_TICKS
	g_VoCHandleTicks= csw_TMGetTick();
#endif

	vpp_SpeechScheduleOneFrame();
	BT_VoCRunFlag=1;
	return;
}

VOID BTSco_txhandle_end(VOID)
{

#ifdef DEBUG_TICKS
	g_VoCHandleTicks= csw_TMGetTick();
#endif

   vpp_SpeechScheduleOneFrame();
   BT_VoCRunFlag=1;
   return;
}



// =============================================================================
// BTSco_Start
// -----------------------------------------------------------------------------
/// Start the VOIS service. 
/// 
/// This function records from the mic and outputs sound on the speaker
/// on the audio interface specified as a parameter, using the input and output
/// set by the \c parameter. \n
///
/// @param itf Interface providing the audion input and output.
/// @param cfg The configuration set applied to the audio interface
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio 
/// command, or if VPP is unavailable.
///         #VOIS_ERR_NO it can execute the command.
// =============================================================================


PUBLIC VOIS_ERR_T BTSco_Start(
                        CONST AUD_ITF_T      itf,
                        CONST VOIS_AUDIO_CFG_T* cfg)
{
    //VOIS_PROFILE_FUNCTION_ENTRY(BTSco_Start);
    //dbg_TraceOutputText(0, "BTSco_Start");
    MCI_TRACE(MCI_AUDIO_TRC, 0, "BTSco_Start itf=%d", itf);
    hal_HstSendEvent(SYS_EVENT, 0x19820103);
    // Initial audio confguration
    AUD_LEVEL_T audioCfg;
    VPP_SPEECH_AUDIO_CFG_T vppCfg;
    AUD_ERR_T audErr;

	
	//set cpu freqence. use HAL_SYS_FREQ_AVPS temporary
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_104M);
 //   CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    CALIB_BUFFER_T * calibPtr = tgt_GetCalibConfig();
    
    //ReceivedScoDataCache = (UINT16*)pcmbuf_overlay;//[MMC_SCO_BUFF_SIZE/2];
    //MicData = (UINT16*)(pcmbuf_overlay+MMC_SCO_BUFF_SIZE/4);//[MMC_SCO_BUFF_SIZE/2];
#if HAL_OVERLAY
    if (calib_bb_audio[itf].audioParams.AecEnbleFlag)
    {
        if (hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_5) != HAL_ERR_NO)
        {
            //VOIS_PROFILE_FUNCTION_EXIT(BTSco_Start);
          //  dbg_TraceOutputText(0, "VOIS_ERR_RESOURCE_BUSY");

            return VOIS_ERR_RESOURCE_BUSY;
        }
        g_BtOverlayLoaded = TRUE;
    }
#endif	

  //  BT_AGCEnbleFlag = FALSE;
   // BT_SpkLevel=cfg->spkLevel;
    BT_AudInterface=itf;
    BT_AudPath = itf - AUD_ITF_BT_EP;

    // Record the last applied configuration
    *(VOIS_MAP_AUDIO_CFG_T*)&g_BtCtx.voisLatestCfg =*(VOIS_MAP_AUDIO_CFG_T*)cfg;
    hal_HstSendEvent(SYS_EVENT, 0x19820107);
    // Configure the Speech FIFO
    //hal_Speech_BtFifoReset(pcmbuf_overlay+MMC_SCO_BUFF_SIZE/2);
    hal_Speech_BtFifoReset(pBtSpeechFifoOverlay);
    // initialise the vpp buffer
    vpp_SpeechInitBuffer();
    
    g_BtCtx.voisVppCfg.encMute = cfg->encMute;
    g_BtCtx.voisVppCfg.decMute = cfg->decMute;
    g_BtCtx.fileHandle = -1;
    
    // disable push-to-talk
    g_BtCtx.voisVppCfg.if1 = 0; 

    // Set audio interface 
    g_BtCtx.voisAudioCfg.spkLevel = cfg->spkLevel;
    g_BtCtx.voisAudioCfg.micLevel = cfg->micLevel;
    g_BtCtx.voisAudioCfg.sideLevel = cfg->sideLevel;
    g_BtCtx.voisAudioCfg.toneLevel = cfg->toneLevel;
    hal_HstSendEvent(SYS_EVENT, 0x19820108);
    // get calibration params for Vpp Speech. Need to be called after setting g_BtCtx 
    BTSco_SetCalibratedVppCfg(itf, (VPP_SPEECH_AUDIO_CFG_T*)&g_BtCtx.voisVppCfg);
    // Register global var
    g_BtCtx.voisItf = itf;

    g_BtCtx.voisTramePlayedNb = 0;
    
    // Set audio interface, initially muted
    *(AUD_LEVEL_T*)&audioCfg = *(AUD_LEVEL_T*)&g_BtCtx.voisAudioCfg;
    audioCfg.spkLevel   =  cfg->spkLevel;
    audioCfg.micLevel   =  cfg->micLevel;
    audioCfg.toneLevel  =  cfg->sideLevel;
    audioCfg.sideLevel  =  cfg->toneLevel;

    // Set the VPP config
    *(VPP_SPEECH_AUDIO_CFG_T*)&vppCfg = *(VPP_SPEECH_AUDIO_CFG_T*)&g_BtCtx.voisVppCfg;
    vppCfg.encMute = VPP_SPEECH_MUTE;
    vppCfg.decMute = VPP_SPEECH_MUTE;
    hal_HstSendEvent(SYS_EVENT, 0x19820109);

    g_BtAgcPcmBufferReceiver=(UINT8 *)((UINT32)( vpp_SpeechBTGetRxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechBTGetRxPcmBuffer();
    g_BtRecordMicPcmBuffer=(UINT32 *)((UINT32)(vpp_SpeechBTGetTxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechBTGetTxPcmBuffer();



    //g_BtBufferStart=(UINT32 *)COS_SHMEMMALLOC(50*160*2);
    //BTSco_RecordStart(g_BtBufferStart,50*160*2,Vois_test);

    // Typical global vars
    // Those buffer are written by VoC, thus the need to access
    // them the uncached way.
    g_BtCtx.voisEncOutput = (HAL_SPEECH_BT_ENC_OUT_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)vpp_SpeechBTGetBluetoothRxPcmBuffer());
    g_BtCtx.voisDecInput  = (HAL_SPEECH_BT_DEC_IN_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)vpp_SpeechBTGetBluetoothTxPcmBuffer());
    hal_HstSendEvent(SYS_EVENT, 0x1982010a);

    // PCM Audio Tx stream, output of VPP
    audioStream.startAddress = (UINT32*)g_BtAgcPcmBufferReceiver;
    audioStream.length = MMC_SCO_BUFF_SIZE*2;
    audioStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    audioStream.channelNb = HAL_AIF_MONO;
    audioStream.voiceQuality = TRUE;
    audioStream.playSyncWithRecord = TRUE;

    audioStream.halfHandler =  NULL ; // Mechanical interaction with VPP's VOC
    audioStream.endHandler =  NULL ; // Mechanical interaction with VPP's VOC


    micStream.startAddress = (UINT32*)g_BtRecordMicPcmBuffer;
    micStream.length = MMC_SCO_BUFF_SIZE*2;
    micStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    micStream.channelNb = HAL_AIF_MONO;
    micStream.voiceQuality = TRUE;
    micStream.playSyncWithRecord = TRUE;

    micStream.halfHandler =  BTSco_txhandle_half ; // Mechanical interaction with VPP's VOC
    micStream.endHandler =  BTSco_txhandle_end ; // Mechanical interaction with VPP's VOC_AHB_BCPU_DEBUG_IRQ_CAUSE
    hal_HstSendEvent(SYS_EVENT, 0x1982010b);

    // set vpp
    UINT32 spStatus =    vpp_SpeechBTOpen(BTSco_VppSpeechHandler, ( VPP_SPEECH_WAKEUP_SW_DECENC ));

    hal_HstSendEvent(SYS_EVENT, 0x19820104);

    //dbg_TraceOutputText(0 , "VPP_SPEECH_OPEN STATUS = %08X", spStatus);
#if HAL_OVERLAY
    if (spStatus == HAL_ERR_RESOURCE_BUSY)
    {
        if (g_BtOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_BtOverlayLoaded = FALSE;
        }
        //VOIS_PROFILE_FUNCTION_EXIT(BTSco_Start);
        return VOIS_ERR_RESOURCE_BUSY;
    }
#endif	
    vpp_SpeechBTAudioCfg(&vppCfg);

    memset(audioStream.startAddress,0,audioStream.length);

    vpp_SpeechBTSetInAlgGain(calibPtr->aud.audioGains[calibPtr->aud.gainItfMap[itf]].inGains.alg);

    vpp_SpeechBTSetAecEnableFlag(calibPtr->aud.echo[BT_AudPath].audioParams.AecEnbleFlag,
                               calibPtr->aud.echo[BT_AudPath].audioParams.AgcEnbleFlag); 

    vpp_SpeechBTSetAecPara(calibPtr->aud.echo[BT_AudPath].audioParams.NoiseGainLimit,
                         calibPtr->aud.echo[BT_AudPath].audioParams.NoiseMin,
                         calibPtr->aud.echo[BT_AudPath].audioParams.NoiseGainLimitStep,
                         calibPtr->aud.echo[BT_AudPath].audioParams.AmpThr,
                         calibPtr->aud.echo[BT_AudPath].audioParams.StrongEchoFlag);		

    vpp_SpeechBTSetFilterPara(calibPtr->aud.echo[BT_AudPath].audioParams.Flags);

    CALIB_AUDIO_OUT_GAINS_T *pOutGains;
    pOutGains = &calibPtr->aud.audioGains[calibPtr->aud.gainItfMap[itf]].outGains[cfg->spkLevel];
    vpp_SpeechBTSetOutAlgGain(pOutGains->voiceOrEpAlg);
    hal_HstSendEvent(SYS_EVENT, 0x19820105);

    //dbg_TraceOutputText(0 , "Gain=%d", pOutGains->voiceOrEpAlg);

  //  vpp_SpeechBTSetMorphVoice(g_MorphVoiceEnableFlag,g_MorphVoicePara);

    // configure AUD and VPP buffers
    audErr = aud_StreamStart(itf, &audioStream, &audioCfg);

    if (audErr == AUD_ERR_NO)
    {
        // Start the other flux only if the Rx one is started
        audErr = aud_StreamRecord(itf, &micStream, &audioCfg);
    }
            //dbg_TraceOutputText(0, "BTSco_Start ret=%d",audErr);

    switch (audErr)
    {
        case AUD_ERR_NO:
            break; 

        default:
            vpp_SpeechBTClose();
#if HAL_OVERLAY
            if (g_BtOverlayLoaded)
            {
                hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
                g_BtOverlayLoaded = FALSE;
            }
#endif
            //VOIS_PROFILE_FUNCTION_EXIT(BTSco_Start);
            //dbg_TraceOutputText(0 , "VOIS Start Failed !!!");
            return VOIS_ERR_UNKNOWN;
            break; // :)
    }
    hal_HstSendEvent(SYS_EVENT, 0x19820106);

    //VOIS_PROFILE_FUNCTION_EXIT(BTSco_Start);
    g_BtCtx.voisStarted = TRUE;
    return VOIS_ERR_NO;
}

BOOL BTSco_GetStatus(void)
{
	return g_BtCtx.voisStarted;
}

// =============================================================================
// BTSco_Stop
// -----------------------------------------------------------------------------
/// This function stops the VOIS service.
/// If the function returns 
/// #VOIS_ERR_RESOURCE_BUSY, it means that the driver is busy with another 
/// audio command.
///
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #VOIS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T BTSco_Stop(VOID)
{
    //VOIS_PROFILE_FUNCTION_ENTRY(BTSco_Stop);
    MCI_TRACE(MCI_AUDIO_TRC, 0, "BTSco_Stop");
    //csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_78M);
    if (g_BtCtx.voisStarted)
    {
        // stop stream ...
        aud_StreamStop(g_BtCtx.voisItf);
        vpp_SpeechBTClose();
#if HAL_OVERLAY
        if (g_BtOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_BtOverlayLoaded = FALSE;
        }
#endif
    	BT_VoCRunFlag=0;

        // and reset global state and cl
        g_BtCtx.voisItf = AUD_ITF_NONE;

        g_BtCtx.voisTramePlayedNb = 0;

        g_BtCtx.voisAudioCfg.spkLevel = AUD_SPK_MUTE;
        g_BtCtx.voisAudioCfg.micLevel = AUD_MIC_MUTE;
        g_BtCtx.voisAudioCfg.sideLevel = 0;
        g_BtCtx.voisAudioCfg.toneLevel = 0;

        g_BtCtx.voisVppCfg.echoEsOn = 0;
        g_BtCtx.voisVppCfg.echoEsVad = 0;
        g_BtCtx.voisVppCfg.echoEsDtd = 0;
        g_BtCtx.voisVppCfg.echoExpRel = 0;
        g_BtCtx.voisVppCfg.echoExpMu = 0;
        g_BtCtx.voisVppCfg.echoExpMin = 0;
        g_BtCtx.voisVppCfg.encMute = VPP_SPEECH_MUTE;
        g_BtCtx.voisVppCfg.decMute = VPP_SPEECH_MUTE;
        g_BtCtx.voisVppCfg.sdf = NULL;
        g_BtCtx.voisVppCfg.mdf = NULL;
        g_BtCtx.voisVppCfg.if1 = 0;
        hal_Speech_BtFifoReset(pBtSpeechFifoOverlay);

        g_BtCtx.voisStarted = FALSE;

        // PCM Audio Tx stream, output of VPP
        audioStream.startAddress = NULL;
        audioStream.length = 0;
        audioStream.sampleRate = HAL_AIF_FREQ_8000HZ;
        audioStream.channelNb = HAL_AIF_MONO;
        audioStream.voiceQuality = TRUE;
        audioStream.playSyncWithRecord = FALSE;//TRUE;
        audioStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
        audioStream.endHandler = NULL; // Mechanical interaction with VPP's VOC

        // PCM Audio Rx stream, input of VPP
        micStream.startAddress = NULL;
        micStream.length = 0;
        micStream.sampleRate = HAL_AIF_FREQ_8000HZ;
        micStream.channelNb = HAL_AIF_MONO;
        micStream.voiceQuality = TRUE;
        micStream.playSyncWithRecord = FALSE;//TRUE;
        micStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
        micStream.endHandler = NULL; // Mechanical interaction with VPP's VOC
    }
	
	MMC_ScoDataReadyPlay = FALSE;
	ReceiveDataLen=0;

    if(g_BtRecordFlag == TRUE)
    {
        if (g_BtUserHandler)
        {
            g_BtUserHandler(VOIS_STATUS_NO_MORE_DATA);
        }               
    }

  //  COS_SHMEMFREE(g_BtBufferStart);
  //  BTSco_RecordStop();
	ReceiveDataLen = 0;
	SendDataLen = 0;
	MMC_ScoDataReadyPlay = FALSE;
	MMC_ScoDataReadySend = FALSE;
    //VOIS_PROFILE_FUNCTION_EXIT(BTSco_Stop);
    return VOIS_ERR_NO;
}



// =============================================================================
// BTSco_CalibUpdateValues
// -----------------------------------------------------------------------------
/// Update Vois related values depending on calibration parameters.
/// 
/// @return #VOIS_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #VOIS_ERR_NO otherwise
// =============================================================================
PUBLIC VOIS_ERR_T BTSco_CalibUpdateValues(VOID)
{
    // Only relevant would VoiS be started
        //dbg_TraceOutputText(0 , "BTSco_CalibUpdateValues g_BtCtx.voisStarted = %d",g_BtCtx.voisStarted);

    if (g_BtCtx.voisStarted)
    {
        return BTSco_Setup(g_BtCtx.voisItf, (VOIS_AUDIO_CFG_T*)&g_BtCtx.voisLatestCfg);
        // As BTSco_Setup does a aud_Setup, all settings 
        // will be updated from the possibly new calibration
        // values.
    }
    else
    {
        return VOIS_ERR_NO;
    }
}

// =============================================================================
// BTSco_RecordStart
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOIS_ERR_T BTSco_RecordStart( INT32 file)
{
    MCI_TRACE(MCI_AUDIO_TRC, 0, "BTSco_RecordStart, file=%d", file);

    g_BtCtx.fileHandle = file;
  
    return VOIS_ERR_NO;
}

// =============================================================================
// BTSco_RecordStop
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOIS_ERR_T BTSco_RecordStop( VOID)
{
    MCI_TRACE(MCI_AUDIO_TRC, 0, "BTSco_RecordStop");

    //dbg_TraceOutputText(0 , "BTSco_RecordStop");
    return VOIS_ERR_NO;
}

VOID BTSco_WriteData(INT16 *pMicData, INT16 *pSpeakerData)
{
    UINT8 *pcm_data, *mic_data;
	UINT8 rec_data[320];
    UINT i;

    pcm_data = (INT8*)pSpeakerData;
    mic_data = (INT8*)pMicData;

    for(i=0;i<160;i++)
    {
        rec_data[2*i]=mic_data[2*i+1]+0x80;
        rec_data[2*i+1]=pcm_data[2*i+1]+0x80;
    }

    hal_HstSendEvent(SYS_EVENT, g_BtCtx.fileHandle);
    FS_Write(g_BtCtx.fileHandle, rec_data, 320);

    return MCI_ERR_NO;
}

INT16 *MMC_SCOGetRXBuffer(VOID)
{
	
    //return (int16 *)(ReceivedScoDataCache);
     return (int16 *)(ReceivedScoDataCache+ReceiveDataLen/2);
}

VOID MMC_SCOReceiveData(INT32 length)
{
    uint32 sco_tick;
    if(!g_BtCtx.voisStarted)
        return;
	if (length>120) hal_DbgAssert("BTSco_ReceiveData length overflow");
	
#ifdef DEBUG_TICKS
	sco_tick = csw_TMGetTick();
    hal_HstSendEvent(SYS_EVENT,0x99900000 + sco_tick - g_ScoTicks);
	g_ScoTicks = sco_tick;
#endif

#if 1
	if (length!=30 && length!=60 && length!=120)
	{
		{
			dbg_TraceOutputText(0,"BTSco_ReceiveData	ready?=%d,len?%d",MMC_ScoDataReadyPlay,ReceiveDataLen);
			memset(ReceivedScoDataCache,0x0,sizeof(ReceivedScoDataCache));
#ifdef USE_VOC_CVSD		
			memset((UINT8*)ScoDataToVoc,0x55,sizeof(ScoDataToVoc));
			ScoDataToVocDataLen=0;
#endif
			return;
		}
	}
#endif

#ifdef USE_VOC_CVSD
//process lost data;
	{
		int i = 0;
		for(i = 0;i<length/2;i++)
		{
			if(0x00 != *((ReceivedScoDataCache+ReceiveDataLen/2)+i))
			{
				break;
			}
		}
		if(i == length/2)
		{
			memset((ReceivedScoDataCache+ReceiveDataLen/2),0x55,length);
			hal_HstSendEvent(SYS_EVENT,0x89000001);
		}
	}
    ReceiveDataLen+=length;//yh
    
    while(ReceiveDataLen>=MMC_SCO_CVSD_SIZE)
	{		
		memcpy(((UINT8*)ScoDataToVoc)+ScoDataToVocDataLen,(UINT8*)ReceivedScoDataCache,MMC_SCO_CVSD_SIZE);
		ReceiveDataLen-=MMC_SCO_CVSD_SIZE;
		memcpy((UINT8*)ReceivedScoDataCache,((UINT8*)ReceivedScoDataCache)+MMC_SCO_CVSD_SIZE,ReceiveDataLen);
		ScoDataToVocDataLen+=MMC_SCO_CVSD_SIZE;

	}

	if(ScoDataToVocDataLen >=(MMC_SCO_SEND_BUFF_SIZE/2))
	{
		hal_Speech_BtPushRxPopTx((CONST UINT8* )ScoDataToVoc,((UINT8*)MicData)+SendDataLen, MMC_SCO_SEND_BUFF_SIZE);
		SendDataLen+=MMC_SCO_SEND_BUFF_SIZE/2;
		ScoDataToVocDataLen-=MMC_SCO_SEND_BUFF_SIZE/2;
		memset((UINT8*)ScoDataToVoc,0x55,MMC_SCO_SEND_BUFF_SIZE/2);//CVSD only used MMC_SCO_SEND_BUFF_SIZE/2
		memcpy((UINT8*)ScoDataToVoc,((UINT8*)ScoDataToVoc)+MMC_SCO_SEND_BUFF_SIZE/2,ScoDataToVocDataLen);
		//  MMC_AudioSCO_AsynSendReq((int16*)MicData, BTSco_AudFileHandle(), MMC_SCO_BUFF_SIZE);

	}
	//hal_Speech_BtPushRxPopTx((CONST UINT8* )ReceivedScoDataCache,(UINT8*)MicData, length);

	if(MMC_ScoMicDataReadySend == FALSE)
	{
		if(SendDataLen>=MMC_SCO_SEND_BUFF_SIZE)
		{
			int i;
			for(i=0;i<3;i++)
			{
				rdabt_uart_tx_sco_data((int16*)MicData, BTSco_AudFileHandle(), length);
				memcpy((UINT8*)MicData,((UINT8*)MicData)+length,SendDataLen-length);
				memset(((UINT8*)MicData)+SendDataLen-length,0x55,length);
				SendDataLen-=length;
			}
			MMC_ScoMicDataReadySend = TRUE;
		}
	}else if(SendDataLen>length){

		rdabt_uart_tx_sco_data((int16*)MicData, BTSco_AudFileHandle(), length);
		memcpy((UINT8*)MicData,((UINT8*)MicData)+length,SendDataLen-length);
		memset(((UINT8*)MicData)+SendDataLen-length,0x55,length);
		SendDataLen-=length;


	}


	if(SendDataLen>= MMC_SCO_MIC_BUFF_SIZE)
	{
		//dbg_TraceOutputText(0,"    SendDataLen  error unknow");
		memset(MicData,0x55,MMC_SCO_MIC_BUFF_SIZE);
		SendDataLen=0;
	}

	if(!MMC_ScoDataReadyPlay )
	{
		//dbg_TraceOutputText(0,"BTSco_ReceiveData    ready?=%d,len?%d",MMC_ScoDataReadyPlay,ReceiveDataLen);
		memset(ReceivedScoDataCache,0x55,MMC_SCO_BUFF_SIZE*2);
		MMC_ScoDataReadyPlay = TRUE;
	}




#else
    ReceiveDataLen+=length;//yh


	if(ReceiveDataLen >=MMC_SCO_BUFF_SIZE)
	{
		hal_Speech_BtPushRxPopTx((CONST UINT8* )ReceivedScoDataCache,(UINT8*)MicData+SendDataLen, MMC_SCO_BUFF_SIZE);
		SendDataLen+=MMC_SCO_BUFF_SIZE;


		memset(ReceivedScoDataCache,0x0,MMC_SCO_BUFF_SIZE);
		memcpy(ReceivedScoDataCache,ReceivedScoDataCache+MMC_SCO_BUFF_SIZE/2,MMC_SCO_BUFF_SIZE);
		memset(ReceivedScoDataCache+MMC_SCO_BUFF_SIZE/2,0x0,MMC_SCO_BUFF_SIZE);
		ReceiveDataLen-=MMC_SCO_BUFF_SIZE;

		//  MMC_AudioSCO_AsynSendReq((int16*)MicData, BTSco_AudFileHandle(), MMC_SCO_BUFF_SIZE);

	}
	//hal_Speech_BtPushRxPopTx((CONST UINT8* )ReceivedScoDataCache,(UINT8*)MicData, length);

    if(SendDataLen >= length )
	{
	   //need wait for micdata buff	  &move data forward 
	   //MMC_AudioSCO_AsynSendReq((int16*)MicDataToBT, BTSco_AudFileHandle(), length);
	   rdabt_uart_tx_sco_data((int16*)MicData, BTSco_AudFileHandle(), length);
	   memset(MicData,0x0,length);
	   memcpy(MicData,MicData+length/4,SendDataLen-length);
	   memset(MicData+SendDataLen/4-length/4,0x0,length);
	   SendDataLen-=length;
	   
	}


	if(SendDataLen>= MMC_SCO_MIC_BUFF_SIZE)
	{
		//dbg_TraceOutputText(0,"    SendDataLen  error unknow");
		memset(MicData,0x0,MMC_SCO_MIC_BUFF_SIZE);
		SendDataLen=0;
	}

	if(!MMC_ScoDataReadyPlay )
	{
		//dbg_TraceOutputText(0,"BTSco_ReceiveData    ready?=%d,len?%d",MMC_ScoDataReadyPlay,ReceiveDataLen);
		memset(ReceivedScoDataCache,0x0,MMC_SCO_BUFF_SIZE*2);
		MMC_ScoDataReadyPlay = TRUE;
	}
#endif
}




