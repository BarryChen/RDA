#ifndef __RECORDMP3_H__
#define __RECORDMP3_H__

#include "cs_types.h"
#include "vpp_audiojpegenc.h"

#define FRAME_NUM 11
#define MP3ENC_INPUT_PCMSIZE        2*288*FRAME_NUM//6336 byte
#define MP3ENC_INPUT_BUFSIZE		2*1152*FRAME_NUM //25344 byte
#define MP3ENC_INPUT_FRAMESIZE		2*1152

#define MP3ENC_INPUT_ONEFRAME_SIZE  2*1152//2304 BYTE
#define MP3ENC_SAMPLE_RATE_N 32000
#define ENCODER_OUTPUT_FRAME_BUF_MAX_SIZE 1000//byte

//define local msg
#define MSG_MMC_AUDIOENC_VOC_INT 1

extern int32 RecordStart(int32 volume, int32 play_style, char * filename);

extern int32 RecordStop (void) ;

extern int32 RecordPause (void)	;

extern int32 RecordResume (void) ;

extern int32 RecordSample (char *pSample, int32 nSample);

extern int32 RecordUserMsg(int32 nMsg);

#endif//__RECORDMP3_H__


