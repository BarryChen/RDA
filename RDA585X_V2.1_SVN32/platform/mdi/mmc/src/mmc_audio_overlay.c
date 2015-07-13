////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2007, Coolsand Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: mmc_audio_overlay.c
//
// DESCRIPTION:
//  pcm overlay for audio play. 
//
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   shenh
//
////////////////////////////////////////////////////////////////////////////////
#include "cswtype.h"
#include "mci.h"

//uint32 pcmbuf_overlay[1];//18*1024 bytes
#define MCI_PCM_FIfO_BUF_SIZE        (512)  // 2KB
#define MCI_PCM_Voc_BUF_SIZE         (640)  // 2.5KB
#define MCI_PCM_NS_BUF_SIZE          (2560) // 10KB
//#define MCI_PCM_Pitch_BUF_SIZE          (2560) // 10KB


uint32 bigbuf[0x1000]  __attribute__((section(".pcm_big_datamem")));

uint32 *pcmbuf_overlay=bigbuf;
uint32 *inputbuf_overlay= bigbuf+MCI_PCM_BUF_SIZE;


/////////////////////////////////////////////////////////////////
//For NS and PotchFilter
/////////////////////////////////////////////////////////////////
uint32 *pBtSpeechFifoOverlay  = bigbuf;
uint32 *pBtSpeechVocOverlay  = bigbuf + MCI_PCM_FIfO_BUF_SIZE;
uint32 *pBtSpeechNSPcmOverlay = bigbuf + MCI_PCM_FIfO_BUF_SIZE + MCI_PCM_Voc_BUF_SIZE;

//pcmover is USED 16K   

