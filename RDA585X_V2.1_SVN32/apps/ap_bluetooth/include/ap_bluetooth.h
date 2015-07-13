/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#ifndef _AP_BLUETOOTH_H_
#define _AP_BLUETOOTH_H_

#if APP_SUPPORT_BLUETOOTH==1

#ifdef APP_SUPPORT_PBAP==1
/*******************Warning!!!!!********************/
//Add by WuXiang
//Don't define this two macro definition at the same time!!
#define BT_PBAP_A2DP_PARALLEL

//#define BT_PBAP_DATA_SAVE

/*******************************************************/
/*******************************************************/
//Add by WuXiang
//If you want use AT command to get phonebook of remote phone, use this macro definition.
//#define BT_PBAP_USE_AT
/*******************************************************/
#ifdef BT_PBAP_A2DP_PARALLEL
#define PBAP_GET_SIZE		(1) 
#define PBAP_BUF_SIZE		(700)
#else
#define PBAP_GET_SIZE		(10)
#define PBAP_BUF_SIZE 		(2048)
#endif

#endif

/*********************Add by WuXiang*********************/
#define BT_OPS_MUSIC_PLAY					(1 << 0) 		  			
#define BT_OPS_MUSIC_PAUSE				(1 << 1)
#define BT_OPS_MUSIC_PLAY_FAILED			(1 << 2)
#define BT_OPS_FILE_TRANS_ABORT 			(1 << 3)
#define BT_OPS_FILE_TRANS_PAUSE			(1 << 4)
#define BT_OPS_FILE_TRANS_COMPLETE		(1 << 5)
#define BT_OPS_PBAP_PHONEBOOK_REQ 		(1 << 6)
#define BT_OPS_PBAP_HISTORY_REQ 			(1 << 7)
#define BT_OPS_MAP_MSG_REQ 				(1 << 8)	
#define BT_OPS_IS_CONNECTED				(1 << 9)					
#define BT_OPS_STOP_RECV					(1 << 10)	
#define BT_OPS_RECV_PAUSE					(1 << 11)
#define BT_OPS_CLOSED					(1 << 12)
#define BT_OPS_CALL_STATUS_OUTGOING		(1 << 13)
#define BT_OPS_AVRCP_STATUS_ERROR		(1 << 14)
#define BT_OPS_BT_MODE_EXIT				(1 << 15)
#define BT_OPS_AVRCP_TEST				(1 << 16)
#define BT_OPS_RECALL					(1 << 17)
#define BT_OPS_MODULE_CHANGE			(1 << 18)
/********************************************************/

#define SIZE_OF_DHKEY   24

#ifndef RDABT_HOST_TYPES_H
#define SIZE_OF_BDADDR 6
typedef struct {
	UINT8 bytes[SIZE_OF_BDADDR];
} t_bdaddr;
#endif

#define OBEX_SHOW_PHONEBOOK           AP_MSG_USER+100
#define OBEX_SHOW_HISTORY         AP_MSG_USER+99
#define OBEX_SHOW_MSG_LIST             AP_MSG_USER+98
#define OBEX_GET_PHONEBOOK_USE_HF     AP_MSG_USER+97
#define OBEX_GET_HISTORY_USE_HF     AP_MSG_USER+96
#define OBEX_GET_MSG_USE_HF     AP_MSG_USER+95
#define PB_LENGTH                     20
#define MSG_BUFF_LEN  (PB_LENGTH*16)
//#define MSG_BUFF_LEN  (1)
#define PBAP_HFP  1
#define PBAP_OBEX 2

#define MAX_PINYIN_LEN (6)
#define MAX_PINYIN_COUNT (4)
#define PHONEBOOK_SEARCH_PINYIN_MAXLEN (1)
#define PHONEBOOK_NUMBER_COUNT      (3)

typedef struct
{
	char *msg;
	char msg_name[PB_LENGTH];
	char msg_tel[PB_LENGTH];
	char msg_time[PB_LENGTH];
}Message_info;

typedef struct 
{
	char name[PB_LENGTH];
	char time[PB_LENGTH];
	char tel[PB_LENGTH];
	unsigned char status;
}History_info;

typedef struct
{
	char pinyin[4];
	char name[PB_LENGTH];
	char tel[3][PB_LENGTH];
}Phonebook_info;

typedef struct
{
	char pin_yin[4];
	UINT16 index;
}Pb_Sort;

typedef struct
{
	char message_center[12];//短信中心号码
	char dst_number[20]; //目标号码
	char msg[160];//短信正文
}Send_Msg_Info;
//add by wuxiang
//导致文件传输终止的不同原因
#define    STOP_RECV_SD_OUT   0x01
#define    STOP_RECV_ABORT   0x02
//add end
typedef enum {
    BT_CLOSED = 0,
    BT_IDLE,
    BT_SCAN,
    BT_CONNECTED,
    BT_CALL,
    BT_PLAY,
    BT_FILE,
} bt_state_t;

typedef struct {
    t_bdaddr addr;
    UINT32   cod;
    UINT8 name[MAX_BT_DEVICE_NAME];
    UINT8 active;
    UINT8 key_type;
    UINT8 link_key[SIZE_OF_LINKKEY];
} bt_device_t;

typedef struct
{
    t_bdaddr    local_addr;
    INT8       device_count;
	INT8       default_device; /*Set As The Default Device*/
	file_location_t location;   /*Opp Support location*/	
    bt_device_t device_list[MAX_BT_DEVICE_LIST];
    INT8    volume;
    INT8    eq_type;
    INT32   local_key[SIZE_OF_DHKEY/4];
    INT32   public_key[2][SIZE_OF_DHKEY/4];
    UINT16      magic;		//用来判断vm里的数据是否有效
}bt_vars_t;


extern bt_state_t g_bt_state;
extern bt_vars_t *g_pBT_vars;

INT32 BT_Entry(INT32 param);
void BT_DisplayIcon(INT32 x, INT32 y);
void BT_UpdateLed();

#endif

#endif /*_AP_BLUETOOTH_H_*/




