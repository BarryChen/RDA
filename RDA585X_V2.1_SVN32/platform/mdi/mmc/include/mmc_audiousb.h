////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audioUSB.h
//
// DESCRIPTION:
//   TODO...
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   jiashuo
//
////////////////////////////////////////////////////////////////////////////////
#include "mci.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc_adpcm.h"

#ifndef MMC_AUDIOUSB_H
#define MMC_AUDIOUSB_H

#define MSG_MMC_USB_PCM_INT 10
#define MSG_MMC_USB_PCM_END 20
#define MSG_MMC_USB_MIC_INT 30
#define MSG_MMC_USB_DATA_IND MSG_MMC_AUDIODEC_USB

typedef struct USB_INPUT  // struct for record data to send by sco
{
    uint16 sco_handle;
    uint16 *data;          // pcm record buff
	uint16 length;       // valid data in pcm buff, in pcm sample
    uint16 size;          // size of data, in pcm sample
} USB_INPUT;

typedef struct USB_OUTPUT  // struct for play received sco data
{
	uint16 *Buffer;		//Pointer for PCM buffer
	uint16 length;       // valid data in pcm buff, in pcm sample
	uint16 Size;		//Size of PCM buffer, in pcm sample
} USB_OUTPUT;

typedef struct USB_PLAY 
{
	uint8 in_flag;
    uint8 packet_size;
	USB_INPUT  USBInput;
	USB_OUTPUT  USBOutput;
//	vpp_AudioJpeg_DEC_OUT_T Voc_AudioDecStatus;	
//	vpp_AudioJpeg_DEC_IN_T Voc_AudioDecIN;
} USB_PLAY;

int32 Audio_USBPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_USBStop (void);

int32 Audio_USBPause (void);	

int32 Audio_USBResume (HANDLE fhd); 

int32 Audio_USBGetID3 (char * pFileName);

int32 Audio_USBUserMsg(int32 nMsg);

int32 Audio_USBGetPlayInformation (MCI_PlayInf * MCI_PlayInfUSB);


#endif


