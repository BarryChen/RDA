////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audiompeg.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MMC_AUDIO_H
#define MMC_AUDIO_H

//#include "lily_lcd_api.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"
#include "mci.h"




typedef struct ID3INFO 
{

	
} ID3INFO;

typedef struct MPEG_DECODER_INPUT 
{
	int32 fileHandle;
	uint32 fileOffset;
	uint8 *data; 
	uint8 *bof;
	uint8 *eof;
	uint32 length;
	uint32 readOffset;
	uint8 inFlag;
	uint8 is_first_frame;
	SSHdl *ss_handle; // for A2dp
	bt_a2dp_audio_cap_struct *audio_config_handle;
       bool  bt_sco;      //for audio over bt sco
       bool  pcmflag; 
} MPEG_INPUT;

typedef struct MPEG_DECODER_OUTPUT 
{
	int16 *data;
	uint32 length;
	uint8 channels;
	uint8 rPingpFlag;
	uint8 wPingpFlag;
	uint8 outFlag;
	PCMSETTING pcm;
	int32 OutputPath;
} MPEG_OUTPUT;

typedef struct MPEG_DECODER 
{
	MPEG_INPUT  MpegInput;
	MPEG_OUTPUT  MpegOutput;
	Codecmode mode;
	vpp_AudioJpeg_DEC_OUT_T Voc_AudioDecStatus;	
	vpp_AudioJpeg_DEC_IN_T Voc_AudioDecIN;
} MPEG_PLAY;

//for sbc encoder  zouying
#define SBC_FREQ_16000		0x00
#define SBC_FREQ_32000		0x01
#define SBC_FREQ_44100		0x02
#define SBC_FREQ_48000		0x03

/* channel mode */
#define SBC_MODE_MONO		0x00
#define SBC_MODE_DUAL_CHANNEL	0x01
#define SBC_MODE_STEREO		0x02
#define SBC_MODE_JOINT_STEREO	0x03

/* allocation method */
#define SBC_AM_LOUDNESS		0x00
#define SBC_AM_SNR		    0x01

#define SBC_BIT_POOL        32
///////////////////////////////////////
int32 Audio_MpegPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_MpegStop (void);

int32 Audio_MpegPause (void);	

int32 Audio_MpegResume (HANDLE fhd); 

int32 Audio_MpegGetID3 (char * pFileName);

int32 Audio_MpegUserMsg(int32 nMsg);

int32 Audio_MpegGetPlayInformation (MCI_PlayInf * MCI_PlayInfMPEG) ;
#endif

