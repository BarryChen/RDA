/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/

#ifndef _AP_FM_H_
#define _AP_FM_H_

#include "ap_common.h"
#include "fmd_m.h"
#include "ap_gui.h"
#include "ap_message.h"
#include "gui_const.h"
#include "globalconstants.h"

#define FM_TAB_NUMS		20

typedef struct
{
	UINT16 *param1;              //电台表地址指针，20个（US模式）/20个（Japan模式）
	UINT16 *param2;              //当前电台信息的地址指针
} FM_param_t;

typedef enum
{
	DOWN = 1,            //[0]下降
	UP,                //[1]上升
	STOP,
} FM_direct_t;

typedef enum
{
	STERE,               //[0]立体声
	MONO                 //[1]单声道
} FM_Audio_t;

typedef enum
{
	releaseMUTE,        //解除静音
	SetMUTE             //静音
} FM_MUTE_t;

typedef struct
{
	FM_Audio_t  FM_Stereo_Status;	//0，立体声；非0，单声道
	UINT16        FM_CurrentFreq;		//当前频率
} FMStatus_Tab_t;

typedef enum					//波段模式
{
	Bank_US_Europe,
	Bank_Japan
} FM_Station_bank_t;


typedef struct
{
	UINT8 status;           // (1:playing ; 2:pause ; 3(searching).
	UINT8 station;          //当前电台对应的电台表项号。
	UINT32  freq;             //当前电台的频率。单位为1k (最高位恒为1)
	FMD_BAND_T mode;             //0 (US-Europe) /1 (Japan) ;
	FMD_ANA_LEVEL_T volume;           //音量。
	BOOL stereo_status;
	UINT8 rssi;
	UINT8 tab_num;
	UINT16 tabs[FM_TAB_NUMS];
	UINT16 magic;
} FM_play_status_t;

//FM 搜索接口函数
INT32  FM_SearchHandle(UINT8 Direct);                //自动搜索处理，Direct=10，NEXT，=20，LAST
INT32  FM_AutoSearchHandle(UINT8 Direct);
INT32  FM_MainFunc(INT32 test);                      //FM应用的处理
void FM_ShowFreqIn(void);                         //FM主界面显示
void FM_ShowFreqOut(void);                  //FM output 界面显示
BOOL FM_SearchDisplay(FM_direct_t Direct);          //自动搜台时的显示
INT32  FM_PlayingHandle(void);                      //播放处理函数
BOOL   FM_SendCommand(UINT8 cmd, INT32 param);     //发送消息给其他task
INT32  FM_Entry(INT32 param);/*进入FM 设置界面*/
INT32  FM_OutPut(INT32 param, UINT32  *pFreq, BOOL *fm_ouput_on);/*进入FM Output 设置界面*/

#endif/*_AP_FM_H*/





