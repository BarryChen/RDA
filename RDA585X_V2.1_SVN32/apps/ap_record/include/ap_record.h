/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_RECORD_H_
#define _AP_RECORD_H_

#include "gui_const.h"
#include "project_config.h"
#include "ap_common.h"
#include "ap_gui.h"
#include "ap_message.h"


#define RECORD_DIR              L"RECORD"
#define VOICE_DIR               L"VOICE"
#define MUSIC_DIR               L"MUSIC"

//磁盘的剩余量不足时，功能失败，以及电压低，提示的次数；
#define	SPACE_LOW		      (20L*512)	//剩余bytes
#define LOW_POWER_COUNT		4	//停止录音时低电压发生的次数

//message
#define RESULT_USER_STOP	    (RESULT_USER1+1)	//用户按键结束
#define RESULT_LOW_POWER_STOP	(RESULT_USER1+2)	//低电压结束
#define RESULT_DIR_CHANGED	    (RESULT_USER1+3)
#define RESULT_TRACKED          (RESULT_USER1+4)    //分曲退出结果
#define RESULT_NEW_FILE	    (RESULT_USER1+5)	//用户按键结束
#define RESULT_PLAY_RECORD	    (RESULT_USER1+6)	//用户按键结束

#define  MICREC_NOSTART      RESULT_RECORD_NOSTART                //非FM录音,此时录音源由g_mrecord_vars.rec_source决定
#define  MICREC_START   RESULT_RECORD_START         //非FM进程下,按REC键直接进入MIC录音.
#define  FMREC_START    RESULT_FMREC_START    //FM录音.FM进程收听电台时按热键REC进行的FM录音
#define  FMREC_NOSTART  RESULT_FMREC_NOSTART  //FM录音.FM进程收听电台时选择"电台录音"菜单进行的FM录音

//录音源常量定义(全局变量:g_rec_from会用到!)
#define  R_SPDIF          0x80
#define  R_FM             0x40
#define  R_LINEIN         0x20
#define  R_MIC            0x10


typedef struct
{
	UINT16 rec_sample_rate;	//采样率
	UINT16 wav_num;       //当前录音文件的编号
	UINT8 volume;
	file_location_t location;
	UINT16 maigc;		//用来判断vm里的数据是否有效
} record_vars_t;

typedef struct
{
	UINT16 maigc; //用来判断vm里的数据是否有效
	UINT8 track_mode;    //0：自动分曲，1：关闭分曲
	UINT8 rec_bitrate;   //0：320kbps，1：256kbps，…，9：8kbps.(通过BitRate_Tab[]转换成module层的接口参数)
	UINT8 rec_source;    //当前设置的录音源。0：Line IN，1：SPDIF，2：MIC
	UINT8 rec_num;       //当前录音文件的编号
	file_location_t location;
} mrecord_vars_t;       //mrecord:指MP3 录音.



/*关闭录音通道的接口函数*/
void RECORD_CloseRecRoute(void);/*共同函数，关闭硬件通道*/
BOOL RECORD_ResetDir(file_location_t *location, WCHAR *DirName);
void RECORD_TotalNum(void);/*计算录音的文件总数目*/

/*不同录音模式的入口函数*/
INT32 RECORD_Entry(INT32 param);//外部接口函数



#endif  /* _AP_RECORD_H_*/



