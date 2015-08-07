/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#ifndef _AP_MESSAGE_H_
#define _AP_MESSAGE_H_

#include "globalconstants.h"
#include "event.h"

#define Msg_KeyNull		          0x00

#define Msg_KeyShortUp          0x30    //�̰���̧��
#define Msg_KeyLongUp           0x31    //������̧��
#define Msg_KeyHold             0x22
#define Msg_KeyUnHold           0x24

#define MSG_USB_STICK           0x91
#define MSG_USB_UNSTICK         0x92
#define MSG_RTC2HZ              0x80
#define MSG_BAT_V_CHG           0x81
#define MSG_LOW_POWER	        0x82
#define MSG_SD_IN		        0xb0   /*�忨״̬*/
#define MSG_SD_OUT		        0xb1   /*����״̬*/


#define	AP_KEY_DOUBLE			(EV_KEY_DOUBLE_PRESS)  // add by gary
#define AP_KEY_LONG             (EV_KEY_LONG_PRESS)
#define AP_KEY_HOLD             (EV_KEY_LONG_PRESS) //  0x40
#define AP_KEY_DOWN             (EV_KEY_DOWN)
#define AP_KEY_UP               (EV_KEY_UP)
#define AP_KEY_PRESS            (EV_KEY_PRESS)

#define AP_KEY_NUM0             (KEY_0<<16)
#define AP_KEY_NUM1             (KEY_1<<16)
#define AP_KEY_NUM2             (KEY_2<<16)
#define AP_KEY_NUM3             (KEY_3<<16)
#define AP_KEY_NUM4             (KEY_4<<16)
#define AP_KEY_NUM5             (KEY_5<<16)
#define AP_KEY_NUM6             (KEY_6<<16)
#define AP_KEY_NUM7             (KEY_7<<16)
#define AP_KEY_NUM8             (KEY_8<<16)
#define AP_KEY_NUM9             (KEY_9<<16)

#define AP_KEY_NULL             (Msg_KeyNull<<16)
#define AP_KEY_MODE             (KEY_MODE<<16)
#define AP_KEY_PLAY             (KEY_PLAY<<16)
#define AP_KEY_NEXT             (KEY_NEXT<<16)
#define AP_KEY_PREV             (KEY_PREV<<16)
#define AP_KEY_VOLADD           (KEY_VOL_UP<<16)
#define AP_KEY_VOLSUB           (KEY_VOL_DOWN<<16)
#define AP_KEY_REC              (KEY_REC<<16)
#define AP_KEY_EQ               (KEY_EQ<<16)
#define AP_KEY_LOOP             (KEY_LOOP<<16)
#define AP_KEY_POWER            (KEY_POWER<<16)
#define AP_KEY_MUTE             (KEY_MUTE<<16)
#define AP_KEY_EXTRA1           (KEY_EXTRA_1<<16)
#define AP_KEY_EXTRA2           (KEY_EXTRA_2<<16)
#define AP_KEY_STAR             (KEY_EQ<<16)            // * 
#define AP_KEY_HASH             (KEY_LOOP<<16)          // #
#define AP_KEY_CALL             (KEY_CALL<<16)          // #

//msg
#define AP_MSG_LOCK             25
#define AP_MSG_USB_PLUGIN       (EV_FS_UDISK_READY)
#define AP_MSG_USB_PULLOUT      (EV_FS_UDISK_REMOVE)
#define AP_MSG_SD_OUT           (EV_FS_TFLASH_DETECT)
#define AP_MSG_SD_IN            (EV_FS_TFLASH_DETECT|(1<<16))
#define AP_MSG_USB_CONNECT   	(EV_FS_UMSS_DETECT)
#define AP_MSG_USB_DISCONNECT   (EV_FS_UMSS_PLUG_OUT)
#define AP_MSG_BT_ACTIVE        (EV_BT_NOTIFY_IND)
#define AP_MSG_ALARM            (EV_ALARM)
#define AP_MSG_LINE_IN          (EV_LINE_IN|(1<<16))
#define AP_MSG_REMOVE_LINE_IN   (EV_LINE_IN)

//AP_MSG_CHARGING ����ʱ��ʾ AP_MSG_RTC Ҳ����
#define AP_MSG_CHARGING         27
#define AP_MSG_RTC              28
#define AP_MSG_STANDBY          29
#define AP_MSG_WAIT_TIMEOUT     30
#define AP_MSG_FORCE_STANDBY    31    //ǿ�ƹػ�
#define AP_MSG_SLEEP            AP_MSG_FORCE_STANDBY
#define AP_MSG_LOW_POWER        AP_MSG_FORCE_STANDBY
#define AP_MSG_KEY_CALIB        32

#define AP_MSG_USER              100

/* ���ܲ�����*/
#define     MC_OPEN             0x01
#define     MC_CLOSE            0x02
#define     MC_LOAD             0x03
#define     MC_PLAY             0x04
#define     MC_PAUSE            0x05
#define     MC_STOP             0x06
#define     MC_STEP             0x07
#define     MC_RECORD           0x08
#define     MC_STEPGRID         0x09
#define     MC_SEARCH           0x0a
#define     MC_SAVESTATION      0x0b
#define     MC_MUTE             0x0c
#define     MC_FADEINOUT        0x0d
#define     MC_FADEIN           0x0e
#define     MC_FADEOUT          0x0f
#define     MC_GETINF           0x10

#define     MC_GETFREQ          0x11
#define     MC_SETFREQ          0x12
#define     MC_CALTIME          0x13

#define     MC_PLAY_SCO         0x14
#define     MC_PLAY_A2DP        0x15
#define     MC_PLAY_USB         0x16

/* ��Ϣ�趨��*/
#define     MC_SETVOLUME        0x40
#define     MC_SETA_B           0x41
#define     MC_SETUSEREQ        0x42
#define     MC_SETEQ            0x43
#define     MC_BACKUP           0x44
#define     MC_RESTORE          0x45
#define     MC_SETSPEED         0x46
#define     MC_SETSTEP          0x47
#define     MC_SETCALLBACK      0x48
#define     MC_SETGAIN          0x49
#define     MC_SETVOR           0x50
#define     MC_SETAUDIOFMT      0x51
#define     MC_SETSOURCE        0x52
#define     MC_SETMODE          0x53
#define     MC_SETNOISE         0x54
#define     MC_DELONE           0x55
#define     MC_DELALL           0X56
#define     MC_RECODEMODE       0x57
#define     MC_SETVOX           0x58
#define	    MC_SETPOWERCOUNT	0x59	//added 2005-7-18 9:44
#define     MC_SETSTATION       0x5a

/* ��Ϣ��ȡ��*/
#define     MC_GETTIME          0x80
#define     MC_GETSAPLE         0x81
#define     MC_GETAUDIOFMT      0x82
#define     MC_GETSTATUS        0x83
#define     MC_GETTABLE         0x84
#define	  MC_GETPOWERVALUE	0x85
#define     MC_GETFILEINFO      0x86
#define     MC_GETTOTALTIME     0x87
#define     MC_GETSTARTPOS        0x88


void   MESSAGE_Initial(const comval_t *comval);
UINT32 MESSAGE_Wait(void);
UINT32 MESSAGE_HandleHotkey(UINT32 key);
void   MESSAGE_SetEvent(UINT32 event);

UINT32 MESSAGE_IsNumberKey(UINT32 key);
UINT32 MESSAGE_IsHold(void);


#endif/*_MESSAGE_H_*/




