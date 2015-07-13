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
	UINT16 *param1;              //��̨���ַָ�룬20����USģʽ��/20����Japanģʽ��
	UINT16 *param2;              //��ǰ��̨��Ϣ�ĵ�ַָ��
} FM_param_t;

typedef enum
{
	DOWN = 1,            //[0]�½�
	UP,                //[1]����
	STOP,
} FM_direct_t;

typedef enum
{
	STERE,               //[0]������
	MONO                 //[1]������
} FM_Audio_t;

typedef enum
{
	releaseMUTE,        //�������
	SetMUTE             //����
} FM_MUTE_t;

typedef struct
{
	FM_Audio_t  FM_Stereo_Status;	//0������������0��������
	UINT16        FM_CurrentFreq;		//��ǰƵ��
} FMStatus_Tab_t;

typedef enum					//����ģʽ
{
	Bank_US_Europe,
	Bank_Japan
} FM_Station_bank_t;


typedef struct
{
	UINT8 status;           // (1:playing ; 2:pause ; 3(searching).
	UINT8 station;          //��ǰ��̨��Ӧ�ĵ�̨����š�
	UINT32  freq;             //��ǰ��̨��Ƶ�ʡ���λΪ1k (���λ��Ϊ1)
	FMD_BAND_T mode;             //0 (US-Europe) /1 (Japan) ;
	FMD_ANA_LEVEL_T volume;           //������
	BOOL stereo_status;
	UINT8 rssi;
	UINT8 tab_num;
	UINT16 tabs[FM_TAB_NUMS];
	UINT16 magic;
} FM_play_status_t;

//FM �����ӿں���
INT32  FM_SearchHandle(UINT8 Direct);                //�Զ���������Direct=10��NEXT��=20��LAST
INT32  FM_AutoSearchHandle(UINT8 Direct);
INT32  FM_MainFunc(INT32 test);                      //FMӦ�õĴ���
void FM_ShowFreqIn(void);                         //FM��������ʾ
void FM_ShowFreqOut(void);                  //FM output ������ʾ
BOOL FM_SearchDisplay(FM_direct_t Direct);          //�Զ���̨ʱ����ʾ
INT32  FM_PlayingHandle(void);                      //���Ŵ�����
BOOL   FM_SendCommand(UINT8 cmd, INT32 param);     //������Ϣ������task
INT32  FM_Entry(INT32 param);/*����FM ���ý���*/
INT32  FM_OutPut(INT32 param, UINT32  *pFreq, BOOL *fm_ouput_on);/*����FM Output ���ý���*/

#endif/*_AP_FM_H*/





