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
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file tgt_calib_m.h                                                      //
/// That file provides the TGT Data Defines for calibration.             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _TGT_CALIB_M_H_
#define _TGT_CALIB_M_H_

#include "cs_types.h"
#include "tgt_calib_cfg.h"

/// Number of coefficiens in the MDF FIR filter.
#define CALIB_VOC_MDF_QTY                        (64)
/// Number of coefficiens in the SDF FIR filter.
#define CALIB_VOC_SDF_QTY                        (64)
/// Number of audio gain steps.
#define CALIB_AUDIO_GAIN_QTY                     (32)
/// The audio gain value standing for mute.
#define CALIB_AUDIO_GAIN_VALUE_MUTE              (-128)
/// Maximum number of different Audio Interfaces supported by this calibration structure.
/// The value of CALIB_AUDIO_TOTAL_ITF_QTY must be the equal as AUD_ITF_QTY!
#define CALIB_AUDIO_TOTAL_ITF_QTY                (8)
/// The echo param number, one for speaker, on for ear-piece
#define CALIB_AUDIO_ECHO_QTY                     (2)

// ============================================================================
// CALIB_AUDIO_IN_GAINS_T
// -----------------------------------------------------------------------------
/// Input (MIC) gains.
// =============================================================================
typedef struct
{
    /// Input analog gain.
    INT8                           ana;                          //0x00000000
    /// Input ADC gain.
    INT8                           adc;                          //0x00000001
    /// Input algorithm gain.
    INT8                           alg;                          //0x00000002
    /// Reserved.
    INT8                           reserv;                       //0x00000003
} CALIB_AUDIO_IN_GAINS_T; //Size : 0x4



// ============================================================================
// CALIB_AUDIO_OUT_GAINS_T
// -----------------------------------------------------------------------------
/// Output gains.
// =============================================================================
typedef struct
{
    /// Output voice gains for physical interfaces, or earpiece gains for application
    /// interfaces. Output analog gain.
    INT8                           voiceOrEpAna;                 //0x00000000
    /// Output DAC gain.
    INT8                           voiceOrEpDac;                 //0x00000001
    /// Output algorithm gain.
    INT8                           voiceOrEpAlg;                 //0x00000002
    /// Reserved.
    INT8                           reserv1;                      //0x00000003
    /// Output music gains for physical interfaces, or loudspeaker gains for application
    /// interfaces. Output analog gain.
    INT8                           musicOrLsAna;                 //0x00000004
    /// Output DAC gain.
    INT8                           musicOrLsDac;                 //0x00000005
    /// Output algorithm gain.
    INT8                           musicOrLsAlg;                 //0x00000006
    /// Reserved.
    INT8                           reserv2;                      //0x00000007
} CALIB_AUDIO_OUT_GAINS_T; //Size : 0x8

// ============================================================================
// CALIB_AUDIO_VOC_FILTERS_T
// -----------------------------------------------------------------------------
/// VoC anti-distortion filters.
// =============================================================================
typedef struct
{
    /// VoC needs the MDF coeffs to be 32-bit aligned.
    //UINT16                         mdfFilter[CALIB_VOC_MDF_QTY]; //0x00000000
    /// SDF coeffs must stay right after MDF.
    UINT16                         sdfFilter[CALIB_VOC_SDF_QTY]; //0x00000000
} CALIB_AUDIO_VOC_FILTERS_T; //Size : 0x80


// ============================================================================
// CALIB_AUDIO_GAINS_T
// -----------------------------------------------------------------------------
/// Calib audio gain types.
// =============================================================================
typedef struct
{
    /// Params accessible by the API.
    CALIB_AUDIO_IN_GAINS_T         inGains;                      //0x00000000
    /// Output gains.
    CALIB_AUDIO_OUT_GAINS_T        outGains[CALIB_AUDIO_GAIN_QTY]; //0x00000004
    INT8                           sideTone[CALIB_AUDIO_GAIN_QTY]; //0x00000104
    //INT8                           reserv[CALIB_AUDIO_GAIN_QTY*3]; //0x00000000
} CALIB_AUDIO_GAINS_T; //Size : 0x124

// ============================================================================
// CALIB_AUDIO_PARAMS_T
// -----------------------------------------------------------------------------
/// Audio calibration parameters.
// Flags = HighPassFilterFlag | (NotchFilterFlag<<1)|(NoiseSuppresserFlag<<2)|(NoiseSuppresserWithoutSpeechFlag<<3)|(NonLinearTEF<<4)
// =============================================================================
typedef struct
{
    INT8                           AecEnbleFlag;                 //0x00000000
    INT8                           AgcEnbleFlag;                 //0x00000001
    INT8                           StrongEchoFlag;               //0x00000002
    INT8                           NoiseGainLimit;               //0x00000003
    INT8                           NoiseMin;                     //0x00000004
    INT8                           NoiseGainLimitStep;           //0x00000005
    INT8                           AmpThr;                       //0x00000006
    UINT8                          Flags;                        //0x00000007
} CALIB_AUDIO_PARAMS_T; //Size : 0x08

// ============================================================================
// CALIB_AUDIO_VOC_EC_T
// -----------------------------------------------------------------------------
/// Echo Cancelling for VoC.
// =============================================================================
typedef struct
{
    /// Enables Echo Canceller algorithm when >0.
    UINT8                          ecMu;                         //0x00000000
    /// Echo Canceller REL parameter (0,+16).
    UINT8                          ecRel;                        //0x00000001
    /// Echo Canceller MIN parameter (0, 32).
    UINT8                          ecMin;                        //0x00000002
    /// Enable Echo Suppressor algorithm (0,1).
    UINT8                          esOn;                         //0x00000003
    /// Double talk threshold for Echo Suppressor algo (0,32).
    UINT8                          esDtd;                        //0x00000004
    /// Donwlink vad threshold for Echo Suppressor algo (0,32).
    UINT8                          esVad;                        //0x00000005
    /// Enable for echo cancelling.
    UINT16                         enableField;                  //0x00000006
} CALIB_AUDIO_VOC_EC_T; //Size : 0x8

typedef struct
{
    CALIB_AUDIO_PARAMS_T audioParams;                            //0x00000000
    CALIB_AUDIO_VOC_EC_T vocEc;                                  //0x00000008
} CALIB_AUDIO_ECHO_T; //Size : 0x10


typedef struct 
{
    /// enable for sbc filter
    UINT8                           enable;                      //0x00000000
    /// filter band: 0 : 0-600HZ; 1:0-500HZ; 2:0-400HZ; 3:0-300HZ; 4:0-200HZ;
    UINT8                           band;                        //0x00000001
    /// gain of filter
    INT16                           gain;                        //0x00000002
} CALIB_SBC_FILTER_T; // Size: 0x04

typedef struct
{
    ///mode(earpiece or speaker) to config map
    //-1 = no config; 0 = config1; ... n = config(n+1)
    INT8                        modeConfigMap[CALIB_AUDIO_TOTAL_ITF_QTY];
    ///sbc db value(-15 - 15)
    INT8                        sbcDB[2][10];
}CALIB_SBC_EQ_T;

// ============================================================================
// CALIB_AUDIO_ITF_T
// -----------------------------------------------------------------------------
/// Calibration of an audio interface. It gathers the audio gains and VoC calibrations
/// data
// =============================================================================
typedef struct
{
    // value of CALIB_AUDIO_VOC_ITF_QTY
    INT16                          vocItfNums;                               //0x00000000
    // value of CALIB_AUDIO_GAIN_ITF_QTY
    INT16                          gainItfNums;                              //0x00000002
    /// index inf vocFilters for each itf
    INT8                           vocItfMap[CALIB_AUDIO_TOTAL_ITF_QTY];     //0x00000004
    /// index inf audioGains for each itf
    INT8                           gainItfMap[CALIB_AUDIO_TOTAL_ITF_QTY];    //0x0000000c
    /// VoC anti-distortion filters.
    CALIB_AUDIO_VOC_FILTERS_T      vocFilters[CALIB_AUDIO_VOC_ITF_QTY];      //0x00000014
    /// Calib audio gain types.
    CALIB_AUDIO_GAINS_T            audioGains[CALIB_AUDIO_GAIN_ITF_QTY];     //0x00000114
    /// audio aec param
    CALIB_AUDIO_ECHO_T             echo[CALIB_AUDIO_ECHO_QTY];               //0x000005a4 
    /// SBC Fileter
    CALIB_SBC_FILTER_T             sbcFilter;                                //0x000005c4
    ///SBC EQ
    CALIB_SBC_EQ_T              sbcEQ;                              //0x000005c8
} CALIB_AUDIO_ITF_T; //Size : 0x5e4


typedef struct
{
    /// Audio calibration.
    CALIB_AUDIO_ITF_T              aud;                                      //0x00000000
} CALIB_BUFFER_T; //Size : 0x5C4

//=============================================================================
// tgt_GetCalibConfig
//-----------------------------------------------------------------------------
/// Access to the calibration parameters.
///
/// @return The global pointers used by the code to access the calibrated
/// parameters. When the target's flash does not contain a flash sector,
/// the calib parameters are initialized with default parameters during
/// the calibration init. Also returns the pointers used by the remote
/// calibration tools to get the version number of the calibration and to 
/// find the calibration buffers.
//=============================================================================
PUBLIC CALIB_BUFFER_T* tgt_GetCalibConfig(VOID);


#endif // _TGT_CALIB_M_H_

