////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audiomp3stream.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MMC_AUDIO_STREAM_H
#define MMC_AUDIO_STREAM_H
//#include "lily_lcd_api.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"

typedef enum 
{
	MSG_MMC_MP3STREAMDEC_VOC,
	MSG_MMC_MP3STREAMDEC_PCM,
}MP3STREAMDEC_USER_MSG;

typedef struct MP3_DECODER_INPUT 
{
	uint8 *data; 
	uint8 *bof;
	uint32 length;
	uint8 is_first_fill_pcm;
	uint8 is_open_intr;
} MP3_STREAM_INPUT;

typedef struct MP3_DECODER_OUTPUT 
{
	int16 *data;
	uint32 length;
	uint8 channels;
	uint8 rPingpFlag;
	uint8 wPingpFlag;
	uint8 outFlag;
	PCMSETTING pcm;
	int32 OutputPath;
} MP3_STREAM_OUTPUT;



typedef struct MP3_STREAM_DECODER 
{
	MP3_STREAM_INPUT  Mp3StreamInput;
	MP3_STREAM_OUTPUT  Mp3StreamOutput;
	Codecmode mode;
	uint8 loop;
	vpp_AudioJpeg_DEC_OUT_T Voc_Mp3StreamDecStatus;	
	vpp_AudioJpeg_DEC_IN_T Voc_Mp3StreamDecIN;
} MP3_STREAM_PLAY;

int32 AudioMp3StartStream(uint32 *pBuffer, uint32 len, uint8 loop);
int32 AudioMp3StopStream(void);
int32 Audio_Mp3StreamUserMsg(int32 nMsg);

#endif

