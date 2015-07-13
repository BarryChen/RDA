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
//  $HeadURL: http://svn.coolsand-tech.com/svn/developing1/Workspaces/aint/bv5_dev/platform/edrv/aud/fm/src/aud_fm.c $ //
//	$Author: admin $                                                        // 
//	$Date: 2012-07-26 17:12:22 +0800 (星期四, 26 七月 2012) $                     //   
//	$Revision: 16490 $                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file aud_maxime.c                                                        //
/// That file provides the implementation of the AUD interface for the maxime //
//  chip.                                                                     //
////////////////////////////////////////////////////////////////////////////////


// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================

#include "cs_types.h"

#include "audp_debug.h"
#include "aud_codec_common.h"
#include "aud_fm.h"
#include "sxr_tls.h"


#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef FM_USING_I2S
extern BOOL g_halAudLineInSpeakerInUse;
#endif
#endif


// =============================================================================
//  MACROS
// =============================================================================

#define MAX_FM_RETRY_NUM 5

#define AUD_FM_START_WAIT_TIME_8809 (90 MS_WAITING)
// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE HAL_AIF_SERIAL_CFG_T POSSIBLY_UNUSED g_audFmAifSerialCfg;
PRIVATE HAL_AIF_CONFIG_T     g_audFmAifCfg;

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// aud_FmPopulateAifCfg
// -----------------------------------------------------------------------------
/// Populate the AIF configuration structure.
/// @param stream Stream to play or record.
/// @return
// =============================================================================
PRIVATE VOID aud_FmPopulateAifCfg(CONST HAL_AIF_STREAM_T* stream)
{
#ifdef FM_LINEIN_RECORD

    AUD_ASSERT(stream->sampleRate==HAL_AIF_FREQ_8000HZ,
        "FM line-in record only supports 8K sample rate");

    g_audFmAifCfg.interface=HAL_AIF_IF_PARALLEL_STEREO;
    g_audFmAifCfg.sampleRate=stream->sampleRate;
    g_audFmAifCfg.channelNb=stream->channelNb;
    g_audFmAifCfg.serialCfg=NULL;

#else // !FM_LINEIN_RECORD

    // AIF Serial Interface configuration
    g_audFmAifSerialCfg.mode=HAL_SERIAL_MODE_I2S;
    g_audFmAifSerialCfg.aifIsMaster=TRUE;
    g_audFmAifSerialCfg.lsbFirst=FALSE;
    g_audFmAifSerialCfg.polarity=FALSE;
#ifdef INTERNAL_FM
    g_audFmAifSerialCfg.rxDelay=HAL_AIF_RX_DELAY_3; // rxDelay for I2S input 2
#else
    g_audFmAifSerialCfg.rxDelay=HAL_AIF_RX_DELAY_2;
#endif
    g_audFmAifSerialCfg.txDelay=HAL_AIF_TX_DELAY_ALIGN;
    // FM I2S will output the same data on both channels if it is configured in mono mode.
    // AIF I2S has no idea of the actual mono/stereo mode.
    g_audFmAifSerialCfg.rxMode=(stream->channelNb==HAL_AIF_MONO) ?
                                HAL_AIF_RX_MODE_STEREO_MONO_FROM_L :
                                HAL_AIF_RX_MODE_STEREO_STEREO;
    g_audFmAifSerialCfg.txMode=HAL_AIF_TX_MODE_STEREO_STEREO;
    g_audFmAifSerialCfg.fs=stream->sampleRate;
    g_audFmAifSerialCfg.bckLrckRatio=32;
    g_audFmAifSerialCfg.invertBck=TRUE;
    g_audFmAifSerialCfg.outputHalfCycleDelay=FALSE;
    g_audFmAifSerialCfg.inputHalfCycleDelay=FALSE;
    g_audFmAifSerialCfg.enableBckOutGating=FALSE;

    // AIF configuration
#ifdef INTERNAL_FM
    g_audFmAifCfg.interface=HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT; // I2S input 2  
#else
    g_audFmAifCfg.interface=HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT; // I2S input 1  
#endif
    g_audFmAifCfg.sampleRate=stream->sampleRate;
    g_audFmAifCfg.channelNb=stream->channelNb;
    g_audFmAifCfg.serialCfg=&g_audFmAifSerialCfg;

#endif // !FM_LINEIN_RECORD
}


// =============================================================================
// aud_FmSetup
// -----------------------------------------------------------------------------
/// Configure the audio. 
/// 
/// This functions configures an audio interface: gain for the side tone and  
/// the speaker, input selection for the microphone and output selection for 
/// the speaker.
/// 
/// @param cfg The configuration set applied to the audio interface. See 
/// #AUD_DEVICE_CFG_T for more details.
/// @return #AUD_ERR_NO if it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_FmSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef FM_USING_I2S
    if (aud_GetCurOutputDevice() == SND_ITF_LOUD_SPEAKER)
    {
        g_halAudLineInSpeakerInUse = TRUE;
    }
    else
    {
        g_halAudLineInSpeakerInUse = FALSE;
    }
#endif
#endif

    return aud_CodecCommonSetup(itf, cfg);
}


// =============================================================================
// aud_FmStreamStart
// -----------------------------------------------------------------------------
/// Start the playing of a stream. 
/// 
/// This function  outputs the 
/// audio on the audio interface specified as a parameter, on the output selected in 
/// the #aud_Setup function. \n
/// In normal operation, when 
/// the buffer runs out, the playing will wrap at the beginning. Here, there are two ways 
/// you can handle your buffer: HAL can call a user function at the middle or 
/// at the end of the playback or, depending 
/// on your application, you can simply poll the playing state using the "reached half"
/// and "reached end functions" TODO Ask if needed, and then implement !
/// 
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio 
/// command,
///         #AUD_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_FmStreamStart(
                        SND_ITF_T itf,
                        CONST HAL_AIF_STREAM_T* stream,
                        CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_ERR_T status;
    BOOL ret;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef FM_USING_I2S
    if (aud_GetCurOutputDevice() == SND_ITF_LOUD_SPEAKER)
    {
        g_halAudLineInSpeakerInUse = TRUE;
    }
    else
    {
        g_halAudLineInSpeakerInUse = FALSE;
    }
#endif
#endif

#ifdef FM_USING_I2S
    aud_FmPopulateAifCfg(stream);
    ret = aud_CodecCommonSetConfig(FALSE, HAL_AUD_USER_CODEC, &g_audFmAifCfg);
#else // !FM_USING_I2S
#ifdef INTERNAL_FM
    ret = aud_CodecCommonSetConfig(FALSE, HAL_AUD_USER_FM, NULL);
#else
    ret = aud_CodecCommonSetConfig(FALSE, HAL_AUD_USER_LINEIN, NULL);
#endif
#endif // !FM_USING_I2S

    if (!ret)
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
            "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    status = aud_CodecCommonStreamStart(itf, stream, cfg);
#ifdef FM_USING_I2S
    if (status == AUD_ERR_NO)
    {
        // Buffer loop back is needed for I2S playing
        status = aud_CodecCommonStreamRecord(itf, stream, cfg);
    }
#endif

#if 0 // FM to BT
    extern AUD_ERR_T aud_BtStreamStart(
                            SND_ITF_T itf,
                            CONST HAL_AIF_STREAM_T* stream,
                            CONST AUD_DEVICE_CFG_T *       cfg);

    if (stream->channelNb == HAL_AIF_MONO)
    {
        g_audFmBTOpened=TRUE;

        aud_FmAifConfig(stream->sampleRate, TRUE);

        // Send the stream through the IFC
        if (hal_AifRecordStream(stream) != HAL_ERR_NO)
        {
            AUD_TRACE(AUD_INFO_TRC, 0,
            "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
            errStatus = AUD_ERR_RESOURCE_BUSY;
            aud_FmClose();
            return errStatus; 
        }
        else
        {
            if (aud_BtStreamStart(AUD_ITF_BLUETOOTH,stream,cfg) != HAL_ERR_NO)
            {  
                AUD_TRACE(AUD_INFO_TRC, 0,
                "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
                errStatus = AUD_ERR_RESOURCE_BUSY;
                aud_FmClose();
            }
            aud_FmAifConfig(stream->sampleRate, TRUE); 
        }

    }
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef FM_USING_I2S
    if (status != AUD_ERR_NO)
    {
        g_halAudLineInSpeakerInUse = FALSE;
    }
#endif
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#ifdef INTERNAL_FM
    if (status == AUD_ERR_NO)
    {
        sxr_Sleep(AUD_FM_START_WAIT_TIME_8809);
    }
#endif
#endif
    return status;
}



// =============================================================================
// aud_FmStreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns 
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another 
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_FmStreamStop(SND_ITF_T itf)
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef FM_USING_I2S
    g_halAudLineInSpeakerInUse = FALSE;
#endif
#endif

    return aud_CodecCommonStreamStop(itf);

#if 0
    extern AUD_ERR_T aud_BtStreamStop(SND_ITF_T itf);
    if(g_audFmBTOpened==TRUE)
    {
        hal_AifStopRecord();
        aud_BtStreamStop(SND_ITF_BLUETOOTH);
    }
#endif
}


// =============================================================================
// aud_FmStreamPause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns 
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another 
/// audio command.
///
/// @param pause If \c TRUE, pauses the stream. If \c FALSE, resumes a paused
/// stream.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_FmStreamPause(SND_ITF_T itf, BOOL pause)
{
    return aud_CodecCommonStreamPause(itf, pause);
}


// =============================================================================
// aud_FmStreamRecord
// -----------------------------------------------------------------------------
/// Manage the recording of a stream. 
/// 
///
/// The #aud_StreamRecord send a message to the driver which begins the 
/// dataflow from the audio and the compression. If the function returns \c 
/// FALSE, it means that the driver cannot handle the record. The value \c len 
/// should be the maximum size of the recording buffer and when the buffer is 
/// full, the recording will automatically wrap. A 
/// user function can be called, either when the middle or the end of the 
/// buffer is reached. (TODO add iatus about polling if available)
/// 
/// @param stream Stream recorded (describe the buffer and the possible interrupt
/// of middle-end buffer handlers.
/// @param cfg The configuration set applied to the audio interface.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver cannot handle the stream,
///         #AUD_ERR_NO if the stream ca be recorded.
// =============================================================================
PUBLIC AUD_ERR_T aud_FmStreamRecord(
                        SND_ITF_T itf,
                        CONST HAL_AIF_STREAM_T* stream,
                        CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_ERR_T status;
    HAL_AUD_USER_T user;
    SND_ITF_T curOutputDevice;

#ifdef FM_USING_I2S
    user = HAL_AUD_USER_CODEC;
#else // !FM_USING_I2S
#ifdef INTERNAL_FM
    user = HAL_AUD_USER_FM;
#else
    user = HAL_AUD_USER_LINEIN;
#endif
#endif // !FM_USING_I2S

    // Save current output device
    curOutputDevice = aud_GetCurOutputDevice();
    // Stop playing
    for (int i=0; i<MAX_FM_RETRY_NUM; i++)
    {
        status = aud_FmStreamStop(itf);
        if (status == AUD_ERR_NO)
        {
            break;
        }
        sxr_Sleep(AUD_TIME_RETRY);
    }
    if (status != AUD_ERR_NO)
    {
        return status;
    }
    // Restore the output device
    aud_SetCurOutputDevice(curOutputDevice);
    // Start to configure recording stream
    aud_FmPopulateAifCfg(stream);

    if (!aud_CodecCommonSetConfig(TRUE, user, &g_audFmAifCfg))
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
            "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    HAL_AIF_STREAM_T playStream = *stream;
    playStream.halfHandler = NULL;
    playStream.endHandler = NULL;
    status = aud_CodecCommonStreamStart(itf, &playStream, cfg);
    if (status == AUD_ERR_NO)
    {
        status = aud_CodecCommonStreamRecord(itf, stream, cfg);
    }

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#ifdef INTERNAL_FM
    if (status == AUD_ERR_NO)
    {
        sxr_Sleep(AUD_FM_START_WAIT_TIME_8809);
    }
#endif
#endif
    return status;
}



// =============================================================================
// aud_FmTone
// -----------------------------------------------------------------------------
//  Manage the playing of a tone: DTMF or Comfort Tone. 
/// 
/// Outputs a DTMF or comfort tone
///
/// When the audio output is enabled, a DTMF or a comfort tones can be output 
/// as well. This function starts the output of a tone generated in the audio 
/// module. \n
/// You can call this function several times without calling the 
/// #aud_ToneStop function or the #aud_TonePause function in 
/// between, if you just need to change the attenuation or the tone type. \n
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver is 
/// busy with an other audio command.
///
/// @param tone The tone to generate
/// @param attenuation The attenuation level of the tone generator
/// @param cfg The configuration set applied to the audio interface
/// @param start If \c TRUE, starts the playing of the tone.
///              If \c FALSE, stops the tone. 
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_FmTone(
                SND_ITF_T itf, 
                CONST SND_TONE_TYPE_T        tone,
                CONST AUD_DEVICE_CFG_T*             cfg,
                CONST BOOL                   start)
{
    return aud_CodecCommonTone(itf, tone, cfg, start);
}



// =============================================================================
// aud_FmTonePause
// -----------------------------------------------------------------------------
/// This function pauses the audio tone.
/// But it doesn't free the audio out resource. If the function returns 
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another 
/// audio command.
///
/// @param pause If \c TRUE, pauses the tone. If \c FALSE, resumes a paused
/// tone.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_FmTonePause(SND_ITF_T itf, BOOL pause)
{
    return aud_CodecCommonTonePause(itf, pause);
}


// =============================================================================
// aud_FmCalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
/// 
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_FmCalibUpdateValues(SND_ITF_T itf)
{
    return aud_CodecCommonCalibUpdateValues(itf);
}



