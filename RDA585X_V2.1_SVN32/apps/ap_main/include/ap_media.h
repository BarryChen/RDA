/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_MEDIA_H_
#define _AP_MEDIA_H_


#define MEDIA_PLAY			1
#define MEDIA_RECORD		2
#define MEDIA_PLAYREC		3

typedef enum
{
	Music_NON,
	Music_MP3,
	Music_WAV,
	Music_WMA,
	Music_ASF,
	Music_AAC,
	Music_OGG,
	Music_SBC,
	Music_USB,
	
} Music_type_t;

typedef enum
{
	normal,   /*eq=0*/
	bass,     /*eq=1*/
	dance,    /*eq=2*/
	classical,/*eq=3*/
	treble,   /*eq=4*/
	party,    /*eq=5*/
	pop,      /*eq=6*/
	rock,     /*eq=7*/
	max_eq
} eq_t;

typedef enum
{
	no_fade,
	fast_fade,
	slow_fade,
} fade_t;

//把一些地址参数集中在MC_OPEN命令中传送
typedef struct
{
	UINT32  file_entry;//文件名地址
	UINT8   mode;
	UINT8   type;
	INT8 *file_name;
} Open_param_t;

/*for compiler compatible */
typedef struct
{
	UINT8 status;
	UINT8 signal;
	UINT8 volume;
	UINT32 progress;    // 0~10000
	UINT32 total_time;  // in millisecond
	eq_t eq;
	INT8 speed;
} media_status_t;


#define MAX_AUD_DESCRIPTION     30
typedef struct tAppAudInfoDescription
{
	UINT32 time; // duration in sec
	UINT32 bitRate;
	UINT32 sampleRate;
	UINT32 createDate;
	UINT32 createTime;
	BOOL stereo;
	INT8 title[MAX_AUD_DESCRIPTION + 1];
	INT8 artist[MAX_AUD_DESCRIPTION + 1];
	INT8 album[MAX_AUD_DESCRIPTION + 1];
	INT8 author[MAX_AUD_DESCRIPTION + 1];
	//   INT16 copyright[MAX_AUD_DESCRIPTION+1];
	//   INT16 date[MAX_AUD_DESCRIPTION+1];
} audInfoStruct;


BOOL mediaSendCommand(UINT8 cmd, INT32 param);

BOOL AudioPlayTimeTransfer(UINT32  duration, ap_time_t *time);

void  AudioGetDuration(ap_time_t *t, UINT32 *duration);

UINT32 media_PlayInternalAudio(UINT8 audio_id, UINT8 times, BOOL waitKey); //times: loop times, 0 for continuous loop
void media_StopInternalAudio(void);


#endif  /* _AP_MEDIA_H_ */

