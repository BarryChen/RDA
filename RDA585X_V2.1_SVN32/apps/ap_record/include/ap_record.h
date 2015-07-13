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

//���̵�ʣ��������ʱ������ʧ�ܣ��Լ���ѹ�ͣ���ʾ�Ĵ�����
#define	SPACE_LOW		      (20L*512)	//ʣ��bytes
#define LOW_POWER_COUNT		4	//ֹͣ¼��ʱ�͵�ѹ�����Ĵ���

//message
#define RESULT_USER_STOP	    (RESULT_USER1+1)	//�û���������
#define RESULT_LOW_POWER_STOP	(RESULT_USER1+2)	//�͵�ѹ����
#define RESULT_DIR_CHANGED	    (RESULT_USER1+3)
#define RESULT_TRACKED          (RESULT_USER1+4)    //�����˳����
#define RESULT_NEW_FILE	    (RESULT_USER1+5)	//�û���������
#define RESULT_PLAY_RECORD	    (RESULT_USER1+6)	//�û���������

#define  MICREC_NOSTART      RESULT_RECORD_NOSTART                //��FM¼��,��ʱ¼��Դ��g_mrecord_vars.rec_source����
#define  MICREC_START   RESULT_RECORD_START         //��FM������,��REC��ֱ�ӽ���MIC¼��.
#define  FMREC_START    RESULT_FMREC_START    //FM¼��.FM����������̨ʱ���ȼ�REC���е�FM¼��
#define  FMREC_NOSTART  RESULT_FMREC_NOSTART  //FM¼��.FM����������̨ʱѡ��"��̨¼��"�˵����е�FM¼��

//¼��Դ��������(ȫ�ֱ���:g_rec_from���õ�!)
#define  R_SPDIF          0x80
#define  R_FM             0x40
#define  R_LINEIN         0x20
#define  R_MIC            0x10


typedef struct
{
	UINT16 rec_sample_rate;	//������
	UINT16 wav_num;       //��ǰ¼���ļ��ı��
	UINT8 volume;
	file_location_t location;
	UINT16 maigc;		//�����ж�vm��������Ƿ���Ч
} record_vars_t;

typedef struct
{
	UINT16 maigc; //�����ж�vm��������Ƿ���Ч
	UINT8 track_mode;    //0���Զ�������1���رշ���
	UINT8 rec_bitrate;   //0��320kbps��1��256kbps������9��8kbps.(ͨ��BitRate_Tab[]ת����module��Ľӿڲ���)
	UINT8 rec_source;    //��ǰ���õ�¼��Դ��0��Line IN��1��SPDIF��2��MIC
	UINT8 rec_num;       //��ǰ¼���ļ��ı��
	file_location_t location;
} mrecord_vars_t;       //mrecord:ָMP3 ¼��.



/*�ر�¼��ͨ���Ľӿں���*/
void RECORD_CloseRecRoute(void);/*��ͬ�������ر�Ӳ��ͨ��*/
BOOL RECORD_ResetDir(file_location_t *location, WCHAR *DirName);
void RECORD_TotalNum(void);/*����¼�����ļ�����Ŀ*/

/*��ͬ¼��ģʽ����ں���*/
INT32 RECORD_Entry(INT32 param);//�ⲿ�ӿں���



#endif  /* _AP_RECORD_H_*/



