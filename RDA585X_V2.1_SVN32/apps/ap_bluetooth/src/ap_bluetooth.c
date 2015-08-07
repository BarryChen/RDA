
/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"
#include "ap_gui.h"
#include "event.h"
#if APP_SUPPORT_BLUETOOTH==1

#include "bt.h"
#include "manager.h"
#include "hfp.h"
#include "obex.h"
#include "avrcp.h"
#include "ap_headset.h"
#include "ap_bluetooth.h"
#include "ap_media.h"
#include "MainTask.h"
#include "mci.h"

#if XDL_APP_SUPPORT_TONE_TIP==1
extern UINT8 lowpower_flag_for_tone;
#endif
extern region_t scrollregion;
extern UINT8 g_a2dp_play;
extern u_int8 stop_recv;
extern UINT8 g_avrcp_status;
extern UINT8 g_hfp_connect;
extern UINT8 g_avdtp_connect;
extern UINT8 g_avrcp_connect;
extern BOOL support_avrcp_status;
extern BOOL obex_disconnected;
extern BOOL short_play_status;
extern UINT32 a2dp_con_timer;
extern UINT32 hfp_con_timer;
extern UINT8 a2dp_con_count;
extern UINT8 hfp_con_count;
extern BOOL ui_timeout_exit;
extern UINT8 g_prev_module;//warkey 2.1
extern UINT8 bt_connect_timer;//warkey 2.1

UINT32 g_bt_ops_state = 0;
BOOL is_connected = FALSE;
INT8 	   inquiry_count = 0;
bt_device_t *inquiry_device = NULL;//[MAX_BT_DEVICE_LIST];
file_location_t g_bt_record_file = {0};
INT8 g_bt_record_flag = 0;
UINT8     g_bt_local_name[MAX_BT_DEVICE_NAME] = {0};
BOOL music_playing=0;

static BOOL page_up = FALSE;
static BOOL page_down = FALSE;
static BOOL last_page = FALSE;
static BOOL first_page = FALSE;
static u_int8 rssi_timer_id;

#if APP_SUPPORT_MAP==1
extern u_int32 mas_cid;
extern u_int32 mas_tid;
extern u_int32 mns_cid;
extern u_int32 mns_tid;
extern Message_info *message_curr[4];
extern Message_info *message_prev[4];
extern Message_info *message_next[4];

int mas_step = 0;
int send_msg_step = 0;
int mns_step = 0;
BOOL first_show_msg = TRUE;
char msg_tel_show[4][PB_LENGTH] = {0};
char *msg_show[4] = {0};
#endif

#if APP_SUPPORT_PBAP==1
extern History_info *history_curr[4];
extern History_info *history_prev[4];
extern History_info *history_next[4];
//char name_show[4][PB_LENGTH*40] = {0};
extern u_int32 pb_cid;
extern u_int32 pb_tid;
extern int phonebook_size;
extern UINT16 pb_get_pos;
extern UINT16 g_list_active;
extern u_int16 connect_type_obex_ftp;
extern void get_phonebook(void);
extern u_int32 *inputbuf_overlay;
extern int history_size;
extern Phonebook_info *phonebook_info;
extern UINT8 *pb_addr;
extern Pb_Sort *pb_sort;
extern int pb_length;
extern UINT16 pb_index;
extern int current_pn_index;

History_info *history_show[4] = {0};
int current_get_pos = 0;
BOOL first_show_history = TRUE;
int last_active = 0;
int select_name = 0;
int phonebook_step = 0;
int history_step = 0;

static int current_page = 0;
#endif

extern slider_t              slider;
extern BOOL ui_auto_update;
extern BOOL ui_auto_select;
extern btconfig_t       g_btconfig;

bt_vars_t *g_pBT_vars = NULL;
INT8 g_bt_cur_device = 0;

#if APP_SUPPORT_LCD==1
UINT8 g_bt_msg[32] = {0};
UINT8 g_bt_currdevice[48] = {0};
#endif

extern u_int32 recv_file_size;
extern u_int32 have_recv_file_size;
extern char bt_file_name[256];
extern UINT16 call_status;
extern UINT8 g_hfp_connect, g_avdtp_connect, g_sco_play;
extern INT32           g_current_module;
extern UINT8 g_hid_connect;

UINT8 g_bt_device_change = 0;
UINT16 g_bt_timer = 0;
UINT32 g_bt_powerof_timer = 0;

UINT8 g_bt_connect_pending = 0;
UINT8 g_bt_ecc_flag = 0; // 0: idle, 1: calc local key, 2: calc dk key
UINT16 g_bt_sco_handle = 0;
bt_state_t g_bt_state = BT_CLOSED;
u_int8 pbap_way = PBAP_OBEX;
//add by wuxiang
UINT32 g_bt_call_time = 0;
//add end
UINT8 g_bt_need_draw = 0;
BOOL downLoading = FALSE;
UINT8 pb_timer = 0;
UINT8 msg_timer = 0;
//add end

#define BT_ACTIVE                   AP_MSG_USER+1
#define BT_DEACTIVE                 AP_MSG_USER+2
#define BT_VISIBLE                  AP_MSG_USER+3
#define BT_INVISIBLE                AP_MSG_USER+4
#define BT_DEVICE_LIST              AP_MSG_USER+5
#define BT_SET_PATH                 AP_MSG_USER+6
#define BT_INQUIRY_DEVICE           AP_MSG_USER+19

/*Device Option*/
#define BT_CONNECT_HFP              AP_MSG_USER+7
#define BT_CONNECT_A2DP             AP_MSG_USER+8
#define BT_SWITCH_AUDIO             AP_MSG_USER+9
#define BT_DISCONNECT               AP_MSG_USER+10
#define BT_SETDEFAULT_DEVICE        AP_MSG_USER+11
#define BT_DELETE                   AP_MSG_USER+12
#define BT_DELETEALL                AP_MSG_USER+13
#if APP_SUPPORT_PBAP==1
#define BT_PHONE_BOOK               AP_MSG_USER+14
#define BT_CALL_HISTORY             AP_MSG_USER+16
#endif
#if APP_SUPPORT_MAP==1
#define BT_MAP                      AP_MSG_USER+18
#endif
#define BT_DEVICEOPTION             AP_MSG_USER+20
#define BT_CONNECT_TIMEOUT          AP_MSG_USER+15
#define BT_CALL_RECORD              AP_MSG_USER+21
#define BT_REMOTE_CONTROL           AP_MSG_USER+22
#define BT_PB_SYNC                  AP_MSG_USER+23
#define BT_PB_SEARCH                AP_MSG_USER+24
#if APP_SUPPORT_MSG_SEND==1
#define BT_SEND_MSG                 AP_MSG_USER+25
#endif
#define BT_VOLUME_MAX               AUD_MAX_LEVEL

#define BT_RECEIVED_DIR              L"RECEIVED"
#define BT_RECORD_DIR                L"CALL"

extern BOOL is_prev;
extern int end_pn_index;
extern int obex_start;
extern int obex_size;
extern BOOL obex_number_data;
extern UINT16 g_rtc_counter;
extern UINT8 g_usb_active_after_bt;
extern UINT8 media_GetInternalStatus();
INT32 BT_Set_Visiable(INT8 visiable);


/*******************Add By Wuxiang***********************/
void set_bt_ops_state(UINT32 state)
{
	g_bt_ops_state |= state;
}


void clean_bt_ops_state(UINT32 state)
{
	g_bt_ops_state &= ~state;
}
/********************************************************/


UINT32 UnicodeToUTF8(UINT8 *utf8, UINT16 utf8_length, const UINT16 *unicode, UINT16 unicode_length)
{
	UINT32 i = 0;
	UINT32 j = 0;
	
	/* If unicode_length is 0, unicode is terminated with 0 */
	while (((0 == unicode_length) || (j < unicode_length)) && (0 != unicode[j]))
	{
		if (unicode[j] < 0x0080)
		{
			/* single byte */
			if (i + 1 > utf8_length - 1)
			{
				break;
			}
			utf8[i] = (UINT8)(unicode[j] & 0x00ff);
			i++;
			j++;
		}
		else if (unicode[j] < 0x0800)
		{
			/* 2-bytes */
			if (i + 2 > utf8_length - 1)
			{
				break;
			}
			utf8[i] = (UINT8)(0xc0 | (unicode[j] >> 6));
			utf8[i + 1] = (UINT8)(0x80 | (unicode[j] & 0x3f));
			i += 2;
			j++;
		}
		else
		{
			/* 3-bytes */
			if (i + 3 > utf8_length - 1)
			{
				break;
			}
			utf8[i]     = (UINT8)(0xe0 | (unicode[j] >> 12));
			utf8[i + 1] = (UINT8)(0x80 | ((unicode[j] >> 6) & 0x3f));
			utf8[i + 2] = (UINT8)(0x80 | (unicode[j] & 0x3f));
			i += 3;
			j++;
		}
	}
	utf8[i] = 0;
	return i;
}


void BT_Send_Msg_Up(u_int32 msg_id, UINT32 param)
{
	COS_EVENT ev = {0};
	hal_HstSendEvent(SYS_EVENT, 0x1988ca00);
	hal_HstSendEvent(SYS_EVENT, 0xb7000000 + msg_id);
	
	if(msg_id == EV_BT_MESSAGE_IND && !AP_Support_LCD())
	{
		return;
	}
	
#if defined(BT_DEVICE_SAVE)
	if((msg_id != EV_BT_MESSAGE_IND && EV_BT_DISCONNECT_IND != msg_id && EV_BT_CONNECT_IND != msg_id) || g_current_module != FUN_BT)
#else
	if(msg_id != EV_BT_MESSAGE_IND || g_current_module != FUN_BT)
#endif
	{
		ev.nEventId = EV_BT_NOTIFY_IND;
		COS_SendEvent(MOD_APP,  &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		hal_HstSendEvent(SYS_EVENT, 0x13062501);
	}
	
	ev.nEventId = msg_id;
	ev.nParam1 = param;
	COS_SendEvent(MOD_APP,  &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	hal_HstSendEvent(SYS_EVENT, 0x13062502);
}


VOID BT_Execute(void)
{
	COS_EVENT ev = {0};
	
	ev.nEventId = EV_BT_NOTIFY_IND;
	COS_SendEvent(MOD_BT,  &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}


void BT_Update_Device_Active(INT32 active)
{
	INT32 i;
	
	for(i = 0; i < g_pBT_vars->device_count; i++)
	{
		if(g_pBT_vars->device_list[i].active > 1)
		{
			g_pBT_vars->device_list[i].active --;
		}
	}
	
	g_pBT_vars->device_list[active].active = MAX_BT_DEVICE_LIST;
}


INT32 BT_Find_Device(t_bdaddr bdaddr)
{
	INT32 i;
	
	for(i = 0; i < g_pBT_vars->device_count; i++)
	{
		if(memcmp(bdaddr.bytes, g_pBT_vars->device_list[i].addr.bytes, SIZE_OF_BDADDR) == 0)
		{
			return i;
		}
	}
	
	return -1;
}


INT32 btoption_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		return 0;
	}
	
	switch(value)
	{
	case BT_CONNECT_HFP:
		return g_bt_state != BT_CLOSED && g_hfp_connect == 0 && g_bt_state <= BT_CONNECTED;
		
	case BT_CONNECT_A2DP:
		return g_bt_state != BT_CLOSED && g_avdtp_connect == 0 && g_bt_state <= BT_CONNECTED;
		
	case BT_SWITCH_AUDIO:
		return (call_status != 0 && (HF_Get_Current_Profile() == ACTIVE_PROFILE_HANDSFREE)); //g_hfp_connect==1 ||
		
#if APP_SUPPORT_BT_RECORD==1
	case BT_CALL_RECORD:
		return g_sco_play == 1 && g_bt_record_flag == 0;
#endif
		
#if APP_SUPPORT_BTHID==1
	case BT_REMOTE_CONTROL:
		return g_hid_connect == 1;
#endif
		
	case BT_DISCONNECT:
		return g_bt_state >= BT_CONNECTED;
		
#if APP_SUPPORT_PBAP==1
	case BT_PHONE_BOOK:
	case BT_CALL_HISTORY:
	case BT_MAP:
#if APP_SUPPORT_MSG_SEND==1
	case BT_SEND_MSG:
#endif
	case BT_PB_SYNC:
		return g_hfp_connect && obex_disconnected && (call_status == BT_CALL_STATUS_NONE);// && (g_a2dp_play == 0)
		
	case BT_PB_SEARCH:
		return g_hfp_connect && (g_a2dp_play == 0);
#endif
		
	case RESULT_MAIN:
		return g_bt_state >= BT_CONNECTED;
		
	default:
		break;
	}
	
	return 0;
}


#if APP_SUPPORT_PBAP==1
void history_info_cpy(History_info *dst[4], const History_info *src[4])
{
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		if(dst[i]->name != NULL && src[i]->name != NULL)
		{
			strcpy(dst[i]->name, src[i]->name);
		}
		
		if(dst[i]->tel != NULL && src[i]->tel != NULL)
		{
			strcpy(dst[i]->tel, src[i]->tel);
		}
		
		if(dst[i]->time != NULL && src[i]->time != NULL)
		{
			strcpy(dst[i]->time, src[i]->time);
		}
		
		dst[i]->status = src[i]->status;
	}
	
	return;
}


void history_info_clean(History_info *dst[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		if(dst[i] != NULL)
		{
			memset(dst[i]->name, 0, PB_LENGTH);
			memset(dst[i]->tel, 0, PB_LENGTH);
			memset(dst[i]->time, 0, PB_LENGTH);
			dst[i]->status = 0xff;
		}
	}
	
	return;
}


void message_info_clean(Message_info *dst[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		if(dst[i]->msg != NULL)
		{
			memset(dst[i]->msg, 0, MSG_BUFF_LEN);
		}
		
		if(dst[i]->msg_name != NULL)
		{
			memset(dst[i]->msg_name, 0, PB_LENGTH);
		}
		
		if(dst[i]->msg_tel != NULL)
		{
			memset(dst[i]->msg_tel, 0, PB_LENGTH);
		}
		
		if(dst[i]->msg_time != NULL)
		{
			memset(dst[i]->msg_time, 0, PB_LENGTH);
		}
	}
	
	return;
}


void msg_tel_get(char dst[4][PB_LENGTH], Message_info *msg_info[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(dst[i], msg_info[i]->msg_tel);
	}
	
	return;
}


void msg_tel_put(Message_info *msg_info[4], char src[4][PB_LENGTH])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(msg_info[i]->msg_tel, src[i]);
	}
	
	return;
}


void msg_tel_show_clean(char src[4][PB_LENGTH])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		memset(src[i], 0, PB_LENGTH);
	}
	
	return;
}


void msg_get(char **dst, Message_info *msg_info[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(dst[i], msg_info[i]->msg);
	}
	
	return;
}


void msg_put(Message_info *msg_info[4], char **src)
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(msg_info[i]->msg, src[i]);
	}
	
	return;
}


void msg_show_clean(char **src)
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		memset(src[i], 0, MSG_BUFF_LEN);
	}
	
	return;
}


Phonebook_info *phonebook_show[4] = {0};
INT32 phonenumber_list_callback(INT32 type, INT32 value, INT8 **string)
{
	if(LIST_CALLBACK_CANCEL == type)
	{
		return 0;
	}
	
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = phonebook_show[select_name % 4]->tel[value];
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		//HF_Set_Number(number_show[select_name%4][value]);
		//HF_Set_Number(phonebook_info.tel[select_name%4]);
		//HF_Call_Request(HF_CALL_NUM, 0);
	}
	
	return 1;
}


void mem_free(void)
{
	int i = 0;
	int j = 0;
#if APP_SUPPORT_MAP==1
	for(i = 0; i < 4; i++)
	{
		if(message_curr[i] != NULL)
		{
			if(message_curr[i]->msg != NULL)
			{
				COS_Free(message_curr[i]->msg);
				message_curr[i]->msg = NULL;
			}
			COS_Free(message_curr[i]);
			message_curr[i] = NULL;
		}
		
		if(message_prev[i] != NULL)
		{
			if(message_prev[i]->msg != NULL)
			{
				COS_Free(message_prev[i]->msg);
				message_prev[i]->msg = NULL;
			}
			COS_Free(message_prev[i]);
			message_prev[i] = NULL;
		}
		
		if(message_next[i] != NULL)
		{
			if(message_next[i]->msg != NULL)
			{
				COS_Free(message_next[i]->msg);
				message_next[i]->msg = NULL;
			}
			COS_Free(message_next[i]);
			message_next[i] = NULL;
		}
		
		if(msg_show[i] != NULL)
		{
			COS_Free(msg_show[i]);
			msg_show[i] = NULL;
		}
	}
#endif
	
#ifdef APP_SUPPORT_PBAP==1
	for(i = 0; i < 4; i++)
	{
		if(history_curr[i] != NULL)
		{
			COS_Free(history_curr[i]);
			history_curr[i] = NULL;
		}
		
		if(history_prev[i] != NULL)
		{
			COS_Free(history_prev[i]);
			history_prev[i] = NULL;
		}
		
		if(history_next[i] != NULL)
		{
			COS_Free(history_next[i]);
			history_next[i] = NULL;
		}
		
		if(history_show[i] != NULL)
		{
			COS_Free(history_show[i]);
			history_show[i] = NULL;
		}
	}
	
	if(phonebook_info != NULL)
	{
		COS_Free(phonebook_info);
		phonebook_info = NULL;
	}
	
	for(i = 0; i < 4; i++)
	{
		if(phonebook_show[i] != NULL)
		{
			COS_Free(phonebook_show[i]);
			phonebook_show[i] = NULL;
		}
	}
#endif
	
	return;
}


void common_var_init(void)
{
	//if(pb_tid)
	{
		OBEX_Disconnect(pb_tid);
	}
	
	if(mas_tid)
	{
		OBEX_Disconnect(mas_tid);
	}
	
	pb_get_pos = 0;
	obex_number_data = FALSE;
	current_get_pos = 0;
	current_pn_index = 0;
	end_pn_index = 0;
	phonebook_step = 1;
	history_step = 1;
	current_page = 0;
	obex_start = 0;
	obex_size = 0;
	first_show_history = TRUE;
	first_show_msg = TRUE;
	last_active = 0;
	select_name = 0;
	page_up = FALSE;
	page_down = FALSE;
	last_page = FALSE;
	first_page = FALSE;
	is_prev = FALSE;
	pb_tid = 0;
	pb_cid = 0;
#if APP_SUPPORT_MAP == 1
	mas_tid = 0;
	mas_cid = 0;
	mas_step = 1;
#endif
#if APP_SUPPORT_MSG_SEND == 1
	send_msg_step = 1;
#endif
	pbap_way = PBAP_OBEX;
	downLoading = FALSE;
	ui_timeout_exit = TRUE;
	//	pb_index = 0;
	//	phonebook_size = 0;
	//	pb_addr = 0;
	//g_list_active = 0;
	//	msg_show_clean(msg_show);
	//	message_info_clean(message_curr);
	//	message_info_clean(message_prev);
	//	message_info_clean(message_next);
	//	msg_tel_show_clean(msg_tel_show);
	history_info_clean(history_show);
	history_info_clean(history_curr);
	history_info_clean(history_prev);
	history_info_clean(history_next);
	mem_free();
	obex_disconnected = TRUE;
	
	return;
}


INT32 history_list_callback(INT32 type, INT32 value, INT8 **string)
{
	int get_size = 0;
	int active = g_list_active;
	int ret = 0;
	
	if(pb_timer)
	{
		COS_KillTimer(pb_timer);
		pb_timer = 0;
	}
	
	if(LIST_CALLBACK_CANCEL == type)
	{
		//OBEX_Disconnect(pb_tid);
		common_var_init();
		return 0;
	}
	
	if(first_show_history)
	{
		first_show_history = FALSE;
		g_rtc_counter = 0;
		history_info_cpy(history_show, history_curr);
		history_info_clean(history_curr);
		if(history_size > 4)
		{
			current_pn_index = 4;
			get_size = (history_size > 8) ? 4 : (history_size - 4);
			current_get_pos = 2;
			history_step = 3;
			if(pbap_way == PBAP_OBEX)
			{
				OBEX_PullPhoneBook(pb_tid, pb_cid, PHONE_BOOK_COMBINED_HISTORY, 4, get_size + 1);
			}
			else if(pbap_way == PBAP_HFP)
			{
				HF_Get_PhoneBook(5, get_size + 4);
			}
		}
		else
		{
			history_step = 1;
		}
	}
	
	if(last_active == 0 && active > 4)
	{
		page_up = TRUE;
		last_page = TRUE;
		current_page = history_size / 4;
	}
	
	if(last_active == (history_size - 1) && active == 0)
	{
		page_down = TRUE;
		first_page = TRUE;
		current_page = 0;
	}
	
	if(last_page)
	{
		if(active == 0)
		{
			page_down = TRUE;
			last_page = FALSE;
		}
		else if(active < (current_page << 2))
		{
			page_up = TRUE;
			last_page = FALSE;
		}
	}
	else if(active < (current_page << 2))
	{
		page_up = TRUE;
		first_page = FALSE;
	}
	else if(active > (((current_page + 1) << 2) - 1))
	{
		page_down = TRUE;
		first_page = FALSE;
	}
	
	//翻页判断
	if(page_up)//上翻页
	{
		page_up = FALSE;
		current_get_pos = 1;//获取的值要存到name_prev中
		
		if(active < (current_page << 2))
		{
			get_size = 4;
			current_page--;
			ret = ((current_page - 1) << 2);
		}
		if(current_page == 0)
		{
			ret = (history_size / 4) << 2;
			if(history_size % 4)
			{
				get_size = history_size % 4;
			}
			else
			{
				ret -= 4;
				get_size = 4;
			}
		}
		if(last_page)
		{
			ret = ((current_page - 1) << 2);
			get_size = 4;
		}
		
		history_info_clean(history_next);
		history_info_cpy(history_next, history_show);
		history_info_clean(history_show);
		history_info_cpy(history_show, history_prev);
		history_info_clean(history_prev);
		history_info_clean(history_curr);
		
		history_step = 3;
		if(history_size - (ret + get_size + 1) < 0)
		{
			if(pbap_way == PBAP_OBEX)
			{
				OBEX_PullPhoneBook(pb_tid, pb_cid, PHONE_BOOK_COMBINED_HISTORY, ret, get_size);
			}
			else if(pbap_way == PBAP_HFP)
			{
				HF_Get_PhoneBook(ret + 1, history_size);
			}
		}
		else
		{
			if(pbap_way == PBAP_OBEX)
			{
				OBEX_PullPhoneBook(pb_tid, pb_cid, PHONE_BOOK_COMBINED_HISTORY, ret, get_size + 1);
			}
			else if(pbap_way == PBAP_HFP)
			{
				HF_Get_PhoneBook(ret + 1, get_size + ret);
			}
		}
		current_pn_index = ret;
	}
	else if(page_down)//下翻页
	{
		page_down = FALSE;
		current_get_pos = 2;//获取的值要存到name_next中
		
		history_info_clean(history_prev);
		history_info_cpy(history_prev, history_show);
		history_info_clean(history_show);
		history_info_cpy(history_show, history_next);
		history_info_clean(history_next);
		history_info_clean(history_curr);
		if(!first_page)
		{
			current_page++;
		}
		
		if(current_page > (int)(history_size / 4))
		{
			current_page = 0;
		}
		
		if(((current_page == (int)(history_size / 4)) && (history_size % 4)) || ((current_page + 1) == (int)(history_size / 4)) && !(history_size % 4))
		{
			ret = 0;
			get_size = 4;
		}
		else
		{
			//剩下的通讯记录个数大于4则拿4个，否则全拿
			if(history_size - active != 0)
			{
				get_size = (history_size - active > 4) ? 4 : (history_size - active);
			}
			
			ret = ((current_page + 1) << 2);
			if((current_page + 1) == (int)(history_size / 4))
			{
				if(history_size % 4)
				{
					get_size = history_size % 4;
				}
				else
				{
					get_size = 4;
				}
			}
		}
		
		history_step = 3;
		//OBEX_PullPhoneBook(pb_tid,pb_cid,PHONE_BOOK_COMBINED_HISTORY, ret, get_size);
		if(history_size - (ret + get_size + 1) < 0)
		{
			if(pbap_way == PBAP_OBEX)
			{
				OBEX_PullPhoneBook(pb_tid, pb_cid, PHONE_BOOK_COMBINED_HISTORY, ret, get_size);
			}
			else if(pbap_way == PBAP_HFP)
			{
				HF_Get_PhoneBook(ret + 1, history_size);
			}
		}
		else
		{
			if(pbap_way == PBAP_OBEX)
			{
				OBEX_PullPhoneBook(pb_tid, pb_cid, PHONE_BOOK_COMBINED_HISTORY, ret, get_size + 1);
			}
			else if(pbap_way == PBAP_HFP)
			{
				HF_Get_PhoneBook(ret + 1, get_size + ret);
			}
		}
		current_pn_index = ret;
	}
	
	last_active = active;
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		if((history_show[value % 4]->name[0] < 32 &&  history_show[value % 4]->name[0] >= 0) || history_show[value % 4]->name[0] > 126)
		{
			*string = history_show[value % 4]->tel;
		}
		else
		{
			*string = history_show[value % 4]->name;
		}
		
		return history_show[value % 4]->status;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		HF_Set_Number(history_show[value % 4]->tel);
		HF_Call_Request(HF_CALL_NUM, 0);
	}
	
	return 0;
}


void phonebook_info_clean(Phonebook_info *pi)
{
	memset(pi->name, 0, PB_LENGTH);
	memset(pi->pinyin, 0, 4);
	memset(pi->tel[0], 0, PB_LENGTH);
	memset(pi->tel[1], 0, PB_LENGTH);
	memset(pi->tel[2], 0, PB_LENGTH);
}

INT32 phonebook_list_callback(INT32 type, INT32 value, INT8 **string)
{
	int get_size = 0;
	int active = g_list_active;
	int ret = 0;
	int num = 0;
	int i = 0;
	
	app_trace(APP_BT_TRC, "phonebook_list_callback");
	if(LIST_CALLBACK_CANCEL == type)
	{
		if(pb_tid)
		{
			OBEX_Disconnect(pb_tid);
			pb_tid = 0;
		}
		
		return 0;
	}
	
	if(last_active == 0 && active > 4)
	{
		page_up = TRUE;
		last_page = TRUE;
		current_page = phonebook_size / 4;
	}
	if(last_active == (phonebook_size - 1) && active == 0)
	{
		page_down = TRUE;
		first_page = TRUE;
		current_page = 0;
	}
	
	if(last_page)
	{
		if(active == 0)
		{
			page_down = TRUE;
			last_page = FALSE;
		}
		else if(active < (current_page << 2))
		{
			page_up = TRUE;
			last_page = FALSE;
		}
	}
	else if(active < (current_page << 2))
	{
		page_up = TRUE;
		first_page = FALSE;
	}
	else if(active > (((current_page + 1) << 2) - 1))
	{
		page_down = TRUE;
		first_page = FALSE;
	}
	
	//翻页判断
	if(page_up)//上翻页
	{
		page_up = FALSE;
		current_get_pos = 1;//获取的值要存到name_prev中
		
		if(active < (current_page << 2))
		{
			get_size = 4;
			current_page--;
			ret = ((current_page - 1) << 2);
		}
		
		if(current_page == 0)
		{
			if(phonebook_size % 4)
			{
				get_size = phonebook_size % 4;
			}
			else
			{
				get_size = 4;
			}
			ret = (phonebook_size / 4) << 2;
		}
		
		if(last_page)
		{
			ret = ((current_page - 1) << 2);
			get_size = 4;
		}
		
		//phonebook_info = (Phonebook_info *)(pb_addr+ret);
		/*
		if(phonebook_size - (ret + get_size + 1) < 0)
		{
			if(pbap_way == PBAP_OBEX)
				OBEX_PullvCardList(pb_tid,pb_cid,PHONE_BOOK_PHONE_BOOK, ret, get_size);
			else if(pbap_way == PBAP_HFP)
				HF_Get_PhoneBook(ret+1, phonebook_size);
		}
		else
		{
			if(pbap_way == PBAP_OBEX)
				OBEX_PullvCardList(pb_tid,pb_cid,PHONE_BOOK_PHONE_BOOK, ret, get_size+1);
			else if(pbap_way == PBAP_HFP)
				HF_Get_PhoneBook(ret+1, get_size+ret);
		}*/
		current_pn_index = ret;
		for(i = 0; i < 4; i++)
		{
			phonebook_info_clean(phonebook_show[i]);
		}
	}
	else if(page_down)//下翻页
	{
		page_down = FALSE;
		current_get_pos = 2;//获取的值要存到name_next中
		
		if(!first_page)
		{
			current_page++;
		}
		
		if(current_page > (int)(phonebook_size / 4))
		{
			current_page = 0;
		}
		
		if(current_page == (int)(phonebook_size / 4))
		{
			ret = 0;
			get_size = 4;
		}
		else
		{
			//剩下的电话本个数大于4则拿4个，否则全拿
			if(phonebook_size - active != 0)
			{
				get_size = (phonebook_size - active > 4) ? 4 : (phonebook_size - active);
			}
			ret = ((current_page + 1) << 2);
		}
		//phonebook_info = (Phonebook_info *)(pb_addr+ret);
		/*
		if(phonebook_size - (ret + get_size + 1) < 0)
		{
			if(pbap_way == PBAP_OBEX)
				OBEX_PullvCardList(pb_tid,pb_cid,PHONE_BOOK_PHONE_BOOK, ret, get_size);
			else if(pbap_way == PBAP_HFP)
				HF_Get_PhoneBook(ret+1, phonebook_size);
		}
		else
		{
			if(pbap_way == PBAP_OBEX)
				OBEX_PullvCardList(pb_tid,pb_cid,PHONE_BOOK_PHONE_BOOK, ret, get_size+1);
			else if(pbap_way == PBAP_HFP)
				HF_Get_PhoneBook(ret+1, get_size+ret);
		}*/
		current_pn_index = ret;
		for(i = 0; i < 4; i++)
		{
			phonebook_info_clean(phonebook_show[i]);
		}
	}
	
	app_trace(APP_BT_TRC, "pb_addr:%x", pb_addr);
	memcpy(phonebook_show[value % 4], ((Phonebook_info *)(pb_addr) + value), sizeof(Phonebook_info));
	app_trace(APP_BT_TRC, "pb_addr:%x", pb_addr);
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		//*string = name_show[value % 4];
		if(phonebook_show[value % 4]->name[0] == 0)
		{
			*string = phonebook_show[value % 4]->tel[0];
		}
		else
		{
			*string = phonebook_show[value % 4]->name;
		}
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		select_name = active;
		//MESSAGE_SetEvent(OBEX_SHOW_PHONENUMBER);
		//GUI_ClearScreen(NULL);
		for(i = 0; i < 3; i++)
		{
			//if(number_show[select_name % 4][i][0] != 0)
			if(phonebook_show[value % 4]->tel[i][0] != 0)
			{
				num++;
			}
		}
		
		if(num > 0)
		{
			return GUI_Display_List(LIST_TYPE_SINGLE_SELECT, 0 , num, 0, phonenumber_list_callback, FALSE);
		}
	}
	
	last_active = active;
	//    phonebook_info_clean(&phonebook_info);
	return 1;
}


UINT16 *search_index = NULL;
UINT16 search_value;
INT32 phonebook_search(UINT8 Pinyin[MAX_PINYIN_COUNT][PHONEBOOK_SEARCH_PINYIN_MAXLEN + 1])
{
	int i = 0;
	UINT8 m, n;
	UINT8 vPinYin[MAX_PINYIN_COUNT][PHONEBOOK_SEARCH_PINYIN_MAXLEN + 1] = {0};
	char py_tmp = 0;
	search_value = 0;
#ifdef WIN32
	search_index = (Pb_Sort *)(inputbuf_overlay + 4096);
#else
	search_index = (Pb_Sort*)pcmbuf_overlay;
#endif
	
	if(Pinyin[0][0] == 0)
	{
		for(i = 0; i < phonebook_size; i++)
		{
			search_index[search_value] = pb_sort[i].index;
			search_value++;
		}
		
		return search_value;
	}
	
	for(m = 0; m < MAX_PINYIN_COUNT; m++)
	{
		n = 0;
		while(Pinyin[m][n] != 0)
		{
			if(Pinyin[m][n] >= 'A' && Pinyin[m][n] <= 'Z')
			{
				vPinYin[m][n] = Pinyin[m][n] + 'a' - 'A';
			}
			else
			{
				vPinYin[m][n] = Pinyin[m][n];
			}
			n++;
		}
	}
	
	for(i = 0; i < phonebook_size; i++)
	{
		for(n = 0; n < MAX_PINYIN_COUNT; n++)
		{
			py_tmp = pb_sort[i].pin_yin[n];
			//app_trace(APP_BT_TRC, "py_tmp:%c", py_tmp);
			//app_trace(APP_BT_TRC, "vPinYin[n]:%s", vPinYin[n]);
			if(vPinYin[n][0] == 0)
			{
				break;
			}
			
			if(strncmp(vPinYin[n], &py_tmp, 1) == 0)
			{
				search_index[search_value] = pb_sort[i].index;
				search_value++;
				break;
			}
			else
			{
				break;
			}
		}
	}
	
	return search_value;
}


static char *my_strncpy(char *dst, const char *src, int cpy_length)
{
	char *tmp = dst;
	int i = 0;
	
	while(i < cpy_length)
	{
		*dst++ = *src++;
		i++;
	}
	
	return tmp;
}


INT32 phonebook_searchlist_callback(INT32 type, INT32 value, INT8 **string, UINT16* count, INT8* text)
{
	Phonebook_info *pb = NULL;
	
	NVRAMGetPhoneBookParam(&pb_length, &pb_index, &phonebook_size);
	pb_addr = NVRAMGetPhoneBookAddress();
	pb_sort = (Pb_Sort *)pb_addr;
	pb_addr += DSM_GetPhoneBookOffset();
	pb = (Phonebook_info *)pb_addr;
#if 0
	app_trace(APP_BT_TRC, "----------------------------------");
	app_trace(APP_BT_TRC, "phonebook_searchlist_callback");
	app_trace(APP_BT_TRC, "pb:%x", pb);
	app_trace(APP_BT_TRC, "pb_addr:%x", pb_addr);
	app_trace(APP_BT_TRC, "pb_sort:%x", pb_sort);
	app_trace(APP_BT_TRC, "type:%d", type);
	app_trace(APP_BT_TRC, "value:%d", value);
	app_trace(APP_BT_TRC, "----------------------------------");
#endif
	
	if(type == SEARCHLIST_CALLBACK_GET_ITEM)
	{
		app_trace(APP_BT_TRC, "search_index:%x", search_index);
		app_trace(APP_BT_TRC, "search_index[value]:%d", search_index[value]);
		//my_strncpy(phonebook_show[value%4].name, pb[pb_sort[value].index].name, PB_LENGTH);
		my_strncpy(phonebook_show[value % 4]->name, &pb[search_index[value]].name, PB_LENGTH);
		//gui_unicode2local(name_show[value%4],PB_LENGTH);
		*string = phonebook_show[value % 4]->name;
	}
	else if(type == SEARCHLIST_CALLBACK_TEXT)
	{
		UINT8 i = 0;
		UINT8 vPinyin[MAX_PINYIN_COUNT][PHONEBOOK_SEARCH_PINYIN_MAXLEN + 1] = {0};
		app_trace(APP_BT_TRC, "SEARCHLIST_CALLBACK_TEXT");
		//        memset(vPinyin, 0, MAX_PINYIN_COUNT*(PHONEBOOK_SEARCH_PINYIN_MAXLEN+1));
		for(i = 0; i < MAX_PINYIN_COUNT; i++)
		{
			app_trace(APP_BT_TRC, "i:%d", i);
			app_trace(APP_BT_TRC, "text:%x", text);
			//        	app_trace(APP_BT_TRC, "text[i]:%c", text[i]);
			memcpy(vPinyin[i], &(text[i]), 1);
		}
		app_trace(APP_BT_TRC, "count:%x", count);
		*count = phonebook_search(vPinyin);
		app_trace(APP_BT_TRC, "count:%d", *count);
		
		if(*count == 0)
		{
			GUI_DisplayMessage(0, GUI_STR_NO_PB_DATA, NULL, GUI_MSG_FLAG_WAIT);
			return 0;
		}
		//PhoneBook_FreeList(&_gPhonebookSearchList);
		//PhoneBook_GetList(vPinyin,&_gPhonebookSearchList);
		//*count = _gPhonebookSearchList.Total;
	}
	else if(type == SEARCHLIST_CALLBACK_SELECT_ITEM)
	{
		UINT16 i = 0, num = 0;
		//tPhoneBookItem vItem;
		if(search_value == 0)
		{
			return 1;
		}
		select_name = search_index[value];
		phonebook_info = ((Phonebook_info *)pb_addr + select_name);
		//PhoneBook_GetEntry(_gPhonebookSearchList.pIndexs[value], &vItem);
		
		for(i = 0; i < PHONEBOOK_NUMBER_COUNT; i++, num++)
		{
			if(phonebook_info->tel[i][0] == 0)
			{
				break;
			}
		}
		
		if(num == 0)
		{
			return 1;
		}
		
		GUI_Display_List(LIST_TYPE_SINGLE_SELECT, 0 , num, 0, phonenumber_list_callback, FALSE);
		
		return 0;
	}
	else if(type == SEARCHLIST_CALLBACK_CANCEL)
	{
		//PhoneBook_FreeList(&_gPhonebookSearchList);
		return 0;
	}
	
	return 1;
}
#endif


#if APP_SUPPORT_MAP==1
extern u_int32 msg_size;
#if APP_SUPPORT_PBAP==0
void msg_tel_get(char dst[4][PB_LENGTH], Message_info *msg_info[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(dst[i], msg_info[i].msg_tel);
	}
	
	return;
}


void msg_tel_put(Message_info *msg_info[4], char src[4][PB_LENGTH])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(msg_info[i].msg_tel, src[i]);
	}
	
	return;
}


void msg_tel_show_clean(char src[4][PB_LENGTH])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		memset(src[i], 0, PB_LENGTH);
	}
	
	return;
}


void msg_get(char **dst, Message_info *msg_info[4])
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(dst[i], msg_info[i].msg);
	}
	
	return;
}

void msg_put(Message_info *msg_info[4], char **src)
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		strcpy(msg_info[i].msg, src[i]);
	}
	
	return;
}


void msg_show_clean(char **src)
{
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		memset(src[i], 0, MSG_BUFF_LEN);
	}
	
	return;
}

void common_var_init(void)
{
	obex_number_data = FALSE;
	current_get_pos = 0;
	current_pn_index = 0;
	end_pn_index = 0;
	current_page = 0;
	obex_start = 0;
	obex_size = 0;
	last_active = 0;
	select_name = 0;
	page_up = FALSE;
	page_down = FALSE;
	last_page = FALSE;
	first_page = FALSE;
	first_show_msg = TRUE;
	is_prev = FALSE;
	mas_tid = 0;
	mas_cid = 0;
	mas_step = 1;
	pbap_way = PBAP_OBEX;
	//g_list_active = 0;
	msg_show_clean(msg_show);
	message_info_clean(message_curr);
	message_info_clean(message_prev);
	message_info_clean(message_next);
	msg_tel_show_clean(msg_tel_show);
	
	return;
}
#endif


INT32 msg_list_callback(INT32 type, INT32 value, INT8 **string)
{
	int get_size = 0;
	int active = g_list_active;
	int ret = 0;
	int num = 0;
	int i = 0;
	
	if(msg_timer)
	{
		COS_KillTimer(msg_timer);
		msg_timer = 0;
	}
	
	if(LIST_CALLBACK_CANCEL == type)
	{
		//OBEX_Disconnect(mas_tid);
		common_var_init();
		return 0;
	}
	
	if(first_show_msg)
	{
		first_show_msg = FALSE;
		msg_get(msg_show, message_curr);
		msg_tel_get(msg_tel_show, message_curr);
		message_info_clean(message_curr);
		g_rtc_counter = 0;
		if(msg_size > 4)
		{
			current_pn_index = 4;
			get_size = (msg_size > 8) ? 4 : (msg_size - 4);
			current_get_pos = 2;
			OBEX_GetMsgList(mas_tid, mas_cid, MSG_INBOX, 4, get_size + 1);
		}
	}
	
	if(last_active == 0 && active > 4)
	{
		page_up = TRUE;
		last_page = TRUE;
		current_page = msg_size / 4;
	}
	if(last_active == (msg_size - 1) && active == 0)
	{
		page_down = TRUE;
		first_page = TRUE;
		current_page = 0;
	}
	
	if(last_page)
	{
		if(active == 0)
		{
			page_down = TRUE;
			last_page = FALSE;
		}
		else if(active < (current_page << 2))
		{
			page_up = TRUE;
			last_page = FALSE;
		}
	}
	else if(active < (current_page << 2))
	{
		page_up = TRUE;
		first_page = FALSE;
	}
	else if(active > (((current_page + 1) << 2) - 1))
	{
		page_down = TRUE;
		first_page = FALSE;
	}
	
	//翻页判断
	if(page_up)//上翻页
	{
		page_up = FALSE;
		current_get_pos = 1;//获取的值要存到name_prev中
		
		if(active < (current_page << 2))
		{
			get_size = 4;
			current_page--;
			ret = ((current_page - 1) << 2);
		}
		if(current_page == 0)
		{
			if(msg_size % 4)
			{
				get_size = msg_size % 4;
			}
			else
			{
				get_size = 4;
			}
			ret = (msg_size / 4) << 2;
		}
		if(last_page)
		{
			ret = ((current_page - 1) << 2);
			get_size = 4;
		}
		
		message_info_clean(message_next);
		msg_put(message_next, msg_show);
		msg_show_clean(msg_show);
		msg_get(msg_show, message_prev);
		message_info_clean(message_curr);
		msg_tel_put(message_next, msg_tel_show);
		msg_tel_show_clean(msg_tel_show);
		msg_tel_get(msg_tel_show, message_prev);
		message_info_clean(message_prev);
		if(msg_size - (ret + get_size + 1) < 0)
		{
			OBEX_GetMsgList(mas_tid, mas_cid, MSG_INBOX, ret, get_size);
		}
		else
		{
			OBEX_GetMsgList(mas_tid, mas_cid, MSG_INBOX, ret, get_size + 1);
		}
		current_pn_index = ret;
	}
	else if(page_down)//下翻页
	{
		page_down = FALSE;
		current_get_pos = 2;//获取的值要存到name_next中
		
		message_info_clean(message_prev);
		msg_put(message_prev, msg_show);
		msg_show_clean(msg_show);
		msg_get(msg_show, message_next);
		message_info_clean(message_curr);
		msg_tel_put(message_prev, msg_tel_show);
		msg_tel_show_clean(msg_tel_show);
		msg_tel_get(msg_tel_show, message_next);
		message_info_clean(message_next);
		if(!first_page)
		{
			current_page++;
		}
		if(current_page > (int)(msg_size / 4))
		{
			current_page = 0;
		}
		
		if(current_page == (int)(msg_size / 4))
		{
			ret = 0;
			get_size = 4;
		}
		else
		{
			//剩下的短信个数大于4则拿4个，否则全拿
			if(msg_size - active != 0)
			{
				get_size = (msg_size - active > 4) ? 4 : (msg_size - active);
			}
			ret = ((current_page + 1) << 2);
		}
		
		if(phonebook_size - (ret + get_size + 1) < 0)
		{
			OBEX_GetMsgList(mas_tid, mas_cid, MSG_INBOX, ret, get_size);
		}
		else
		{
			OBEX_GetMsgList(mas_tid, mas_cid, MSG_INBOX, ret, get_size + 1);
		}
		current_pn_index = ret;
	}
	
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		if(strlen(msg_tel_show[value % 4]))
		{
			*string = msg_tel_show[value % 4];
		}
		else
		{
			*string = msg_tel_show[value % 4];
		}
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		select_name = active;
		BT_Send_Msg_Up(EV_BT_SHOW_MSG, 0);
		last_active = active;
		return 0;
	}
	
	last_active = active;
	
	return 1;
}


void show_msg(void)
{
	GUI_DisplayMultiText(msg_show[select_name % 4]);
	//OBEX_Disconnect(mas_tid);
	common_var_init();
	
	return;
}
#endif


INT32 btdevice_list_callback(INT32 type, INT32 value, INT8 **string)
{
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = g_pBT_vars->device_list[value].name;
		switch((g_pBT_vars->device_list[value].cod >> 8) & 0x1f) // bit12 ~ bit8, device class
		{
		case 2: // phone
			return 1;
			
		case 1: // pc
			return 2;
			
		case 4: // headset
			return 3;
			
		default:
			return 0; // default display bt icon
		}
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		g_bt_cur_device = value;
		MESSAGE_SetEvent(BT_DEVICEOPTION);
		//GUI_Display_Menu(GUI_MENU_BTDEVICEOPTION, btoption_menu_callback);
		return 0;
	}
	
	return 0;
}


INT32 current_inquiry_device = 0;//当前搜索列表中选中的设备
INT32 inquiry_device_list_callback(INT32 type, INT32 value, INT8 **string)
{
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		*string = inquiry_device[value].name;
		switch((inquiry_device[value].cod >> 8) & 0x1f) // bit12 ~ bit8, device class
		{
		case 2: // phone
			return 1;
			
		case 1: // pc
			return 2;
			
		case 4: // headset
			return 3;
			
		default:
			return 0; // default display bt icon
		}
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		current_inquiry_device = value;
	}
	return 0;
}


UINT8 con_hf_timer = 0;
void BT_Connect_Timeout(void *param)
{
	if(con_hf_timer)
	{
		COS_KillTimer(con_hf_timer);
		con_hf_timer = 0;
	}
	MESSAGE_SetEvent(BT_CONNECT_TIMEOUT);
	
	return;
}

void BT_Disconnect_Device(void);
void BT_Connect_Handsfree(UINT8 index)
{
	t_api result;
	t_MGR_DeviceEntry *p_entry = NULL;
	u_int8 profile;
	
	if(GetBtCloseFlag())
	{
		hal_HstSendEvent(APP_EVENT, 0x13100901);
		return;
	}
	else
	{
		hal_HstSendEvent(APP_EVENT, 0x13100902);
	}
	
#if (APP_SUPPORT_PBAP==1)
	//	if(pb_tid)
	//	{
	//		OBEX_Disconnect(pb_tid);
	//	}
	common_var_init();
#elif (APP_SUPPORT_MAP==1)
	
	//	if(mas_tid)
	//	{
	//		OBEX_Disconnect(mas_tid);
	//	}
	common_var_init();
#endif
	
	if(con_hf_timer)
	{
		COS_KillTimer(con_hf_timer);
		con_hf_timer = 0;
	}
	
	if(g_pBT_vars->device_list[index].cod & (1 << 21)) // target device is headset
	{
		profile = ACTIVE_PROFILE_HANDSFREE_AG;//|ACTIVE_PROFILE_HEADSET_AG;
	}
	else
	{
		profile = ACTIVE_PROFILE_HANDSFREE;//|ACTIVE_PROFILE_HEADSET;
	}
	
	result = HF_Connect_SLC(0, g_pBT_vars->device_list[index].addr, profile);
	if(result == RDABT_PENDING || result == RDABT_NOERROR)
	{
		GUI_DisplayMessage(0, GUI_STR_BT_CONNECTING, NULL, GUI_MSG_FLAG_DISPLAY);
		con_hf_timer = COS_SetTimer(15000, BT_Connect_Timeout, NULL, COS_TIMER_MODE_SINGLE);
		g_bt_connect_pending = 1;
	}
	else
	{
		//BT_Disconnect_Device();
		GUI_DisplayMessage(0, GUI_STR_BT_CONNECT_FAIL, NULL, GUI_MSG_FLAG_DISPLAY);
	}
	
	return;
}


void BT_Connect_A2DP(UINT8 index)
{
	t_api result;
	
	if(GetBtCloseFlag())
	{
		hal_HstSendEvent(APP_EVENT, 0x13100903);
		return;
	}
	else
	{
		hal_HstSendEvent(APP_EVENT, 0x13100904);
	}
	
	result = AVDTP_Setup_Connect(g_pBT_vars->device_list[index].addr);
	if(result == RDABT_PENDING || result == RDABT_NOERROR)
	{
		GUI_DisplayMessage(0, GUI_STR_BT_CONNECTING, NULL, GUI_MSG_FLAG_DISPLAY);
		COS_SetTimer(10000, BT_Connect_Timeout, NULL, COS_TIMER_MODE_SINGLE);
		g_bt_connect_pending = 2;
	}
	else
	{
		GUI_DisplayMessage(0, GUI_STR_BT_CONNECT_FAIL, NULL, GUI_MSG_FLAG_WAIT);
	}
	
	return;
}

void set_connectable_timeout(void *param)
{
	MGR_SetConnectableMode(TRUE);
}
extern UINT8 bt_connect_timer;

void BT_Disconnect_Device(void)
{
	extern u_int32 opp_recv_tid;
    MGR_SetConnectableMode(FALSE);
    COS_SetTimer(3000, set_connectable_timeout, NULL, COS_TIMER_MODE_SINGLE);	
	if(g_bt_state == BT_FILE)
	{
		OBEX_ServerTransportDisconnect(opp_recv_tid);
	}
	else if(g_bt_state >= BT_CONNECTED)
	{
#if !defined(NOT_SUPPORT_HFP)
		HF_Release_SLC(g_pBT_vars->device_list[g_bt_cur_device].addr);
#endif
		Avrcp_Disconnect_Req();
		//if(g_a2dp_play)
		Avdtp_Send_Close();
		//else
		//    AVDTP_Disconnect();
#if APP_SUPPORT_BTHID==1
		HID_Disconnect();
#endif
	}
	
	//add by wuxiang
	//	GUI_DisplayMessage(0, GUI_STR_BTDISCONNECTING, 0, GUI_MSG_FLAG_DISPLAY);
	//    COS_Sleep(1000);
	g_bt_state = BT_IDLE;
//	if(!(g_bt_ops_state & BT_OPS_CLOSED))
//    		MGR_SetConnectableMode(TRUE);
	//add end
	
	g_bt_ops_state = 0;
#ifdef PROFILE_CONNECT	
	a2dp_con_count = 0;
	hfp_con_count = 0;
	if(hfp_con_timer)
	{
		COS_KillTimer(hfp_con_timer);
	}
	hfp_con_timer = 0;
	
	if(a2dp_con_timer)
	{
		COS_KillTimer(a2dp_con_timer);
	}
	a2dp_con_timer = 0;
#endif	
#if 1//warkey 2.1
	if(bt_connect_timer)
	{
		COS_KillTimer(bt_connect_timer);
	}
	bt_connect_timer = 0;
#endif
	
	return;
}


void bt_device_copy(bt_device_t *dst, const bt_device_t *src)
{
	int i = 0;
	
	dst->active = src->active;
	for(i = 0; i < SIZE_OF_BDADDR; i++)
	{
		dst->addr.bytes[i] = src->addr.bytes[i];
	}
	
	dst->cod = src->cod;
	dst->key_type = src->key_type;
	for(i = 0; i < SIZE_OF_LINKKEY; i++)
	{
		dst->link_key[i] = src->link_key[i];
	}
	
	strcpy(dst->name, src->name);
	
	return;
}


/*This Function For Delete Spec Device Or Delete All Device*/
void  BT_Delete_Device(UINT8 index)
{
	int i = 0;
	t_MGR_DeviceEntry *p_entry = NULL;
	
	if(g_bt_state >= BT_CONNECTED && (index > g_pBT_vars->device_count || index == g_bt_cur_device))
	{
		BT_Disconnect_Device();
	}
	if(index > g_pBT_vars->device_count) /*Delete All Device*/
	{
		for(i = 0; i < g_pBT_vars->device_count; i++)
		{
			MGR_FindDeviceRecord(g_pBT_vars->device_list[i].addr, &p_entry);
			MGR_RemoveDeviceRecord(p_entry);
		}
		memset(g_pBT_vars->device_list, 0, sizeof(bt_device_t)*MAX_BT_DEVICE_LIST);
		g_pBT_vars->device_count = 0;
		g_bt_cur_device = -1;
		g_pBT_vars->default_device = -1;
	}
	else    /*Delete Spec Device*/
	{
		MGR_FindDeviceRecord(g_pBT_vars->device_list[i].addr, &p_entry);
		MGR_RemoveDeviceRecord(p_entry);
		memset(&g_pBT_vars->device_list[index], 0, sizeof(bt_device_t));
		g_pBT_vars->device_count--;
		if(g_pBT_vars->device_count == 0)
		{
			g_bt_cur_device = -1;
			g_pBT_vars->default_device = -1;
		}
	}
	
	for(i = index; i < MAX_BT_DEVICE_LIST - 1; i++)
	{
		bt_device_copy(&g_pBT_vars->device_list[i], &g_pBT_vars->device_list[i + 1]);
	}
	
	NVRAMWriteData();
	
	return;
}


void BT_Convert_Device_Name(UINT8 *utf8_name, UINT8 *name)
{
	INT32 unicode_len = 0;
	char name_tmp[MAX_BT_DEVICE_NAME * 2] = {0};
	
	unicode_len = UTF8_to_Unicode(name_tmp, utf8_name);
	if(unicode_len > 0)
	{
		memset(&name_tmp[unicode_len], 0, MAX_BT_DEVICE_NAME * 2 - unicode_len);
		gui_unicode2local(name_tmp, unicode_len);
		memcpy(name, name_tmp, MAX_BT_DEVICE_NAME);
	}
	else
	{
		memcpy(name, utf8_name, MAX_BT_DEVICE_NAME);
	}
	name[MAX_BT_DEVICE_NAME - 1] = 0;
	
	return;
}


INT32 BT_Add_Device(t_MGR_DeviceEntry *dbEntry)
{
	INT32 index, i, min_active;
	index = BT_Find_Device(dbEntry->bdAddress);
	
	if(index < 0)
	{
		if(g_pBT_vars->device_count >= MAX_BT_DEVICE_LIST)
		{
			min_active = MAX_BT_DEVICE_LIST;
			for(i = 0; i < g_pBT_vars->device_count; i++)
			{
				if(g_pBT_vars->device_list[i].active < min_active)
				{
					min_active = g_pBT_vars->device_list[i].active;
					index = i;
				}
			}
		}
		else
		{
			index = g_pBT_vars->device_count;
			g_pBT_vars->device_count++;
		}
	}
	
	if(index < 0) //warkey //严重BUG，MAX_BT_DEVICE_LIST设置为1时index连接第二个设备时非法值，导致写入异常，再也进不了蓝牙
	{
		index = 0;
	}
	
	BT_Update_Device_Active(index);
	memcpy(g_pBT_vars->device_list[index].addr.bytes, dbEntry->bdAddress.bytes, SIZE_OF_BDADDR);
	g_pBT_vars->device_list[index].cod      = dbEntry->classOfDevice;
	g_pBT_vars->device_list[index].key_type = dbEntry->linkKeyType;
	BT_Convert_Device_Name(dbEntry->name, g_pBT_vars->device_list[index].name);
	memcpy(g_pBT_vars->device_list[index].link_key, dbEntry->linkKey, SIZE_OF_LINKKEY);
	
	return index;
}


extern Message_info *message_curr[4];
extern Message_info *message_prev[4];
extern Message_info *message_next[4];
extern char *msg_show[4];
extern INT32 connect_count;
extern UINT8 bt_connect_timer;
extern void bt_auto_con_timeout(void *param);
extern void nfcd_GetVersion();

void BT_Read_RSSI(VOID *param)
{
	if(g_bt_state >= BT_CONNECTED)
	{
		s_int8 rssi;
		BOOL result = rdabt_read_rssi(&rssi);
		if(!result)
		{
			COS_ChangeTimer(rssi_timer_id, 10);
		}
		else
		{
			app_trace(APP_BT_TRC, "BT_Read_RSSI, rssi=%d", rssi);
			COS_ChangeTimer(rssi_timer_id, 950);
		}
	}
	else
	{
		COS_ChangeTimer(rssi_timer_id, 1000);
	}
	
	return;
}


UINT8 g_tCard_active_after_bt;
UINT8 g_lineIn_active_after_bt;
INT32 BT_Active_Bluetooth(void)
{
	COS_EVENT ev = {0};
	UINT8 visible = 0;
	int i = 0;
	char *tmp = NULL;
	extern BOOL g_test_mode;
	media_StopInternalAudio();
		/*if(!g_test_mode)
#if XDL_APP_SUPPORT_TONE_TIP==1
		{
			media_PlayInternalAudio(GUI_AUDIO_BT_POWERON, 1, FALSE);
			while(GetToneStatus())
			{
				MESSAGE_Sleep_EXT(1);
			}
		}
#else
		{
			;
		}
#endif	*/
	ev.nEventId = EV_BT_POWERON_REQ;
	ev.nParam1 = &(g_pBT_vars->local_addr);
	COS_SendEvent(MOD_BT,  &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	GUI_DisplayMessage(0, GUI_STR_BTACTIVEING, 0, GUI_MSG_FLAG_DISPLAY);
	
	while(1)
	{
		COS_WaitEvent(MOD_APP, &ev, COS_WAIT_FOREVER);
		if(ev.nEventId == AP_KEY_HOLD && ev.nParam1 == KEY_PLAY)
		{
			visible = 1;
		}
		else if(ev.nEventId == EV_BT_READY_IND)
		{
			break;
		}
#if 1
		else if(ev.nEventId == EV_FS_UMSS_DETECT)//handled usb event
		{
			if(g_usb_active_after_bt == 0)
			{
				g_usb_active_after_bt = 1;
			}
		}
		else if(ev.nEventId == EV_FS_TFLASH_DETECT)//handle t-card event
		{
			g_tCard_active_after_bt = 1;
		}
		else if(ev.nEventId == EV_LINE_IN && ev.nParam1 == 1) //handle line-in event
		{
			g_lineIn_active_after_bt = 1;
		}
#endif
	}
	
#ifdef BT_SSP_VOC_SUPPORT
	if(g_bt_ecc_flag == 1) // wait local key
	{
		INT res;
		
		res = vpp_ECC_GetKeyResult(g_pBT_vars->public_key, SIZE_OF_DHKEY * 2 / 4);
		while(res != 0)
		{
			COS_Sleep(100);
			res = vpp_ECC_GetKeyResult(g_pBT_vars->public_key, SIZE_OF_DHKEY * 2 / 4);
		}
		MGR_Write_DHKey(TRUE, g_pBT_vars->public_key[0], g_pBT_vars->public_key[1]);
	}
#endif
	
	if(ev.nParam1 == 0) // active bt success
	{
		
		//if(rssi_timer_id == 0)
		{
			//rssi_timer_id = COS_SetTimer(1000, BT_Read_RSSI, NULL, COS_TIMER_MODE_PERIODIC);
		}
		//	while(1)
		//		{
		//nfcd_configration();
		//		nfcd_GetVersion();
		//		COS_Sleep(1000);
		///		nfcd_gate_init();
		//		app_trace(APP_BT_TRC, "pn544 clear!-------------------------------------\n");
		//		clear_mem();
		//		nfcd_init();
		//		app_trace(APP_BT_TRC, "pn544 reset!-------------------------------------\n");
		//		COS_Sleep(2000);
		//		app_trace(APP_BT_TRC, "pn544 clear!-------------------------------------\n");
		//		clear_mem();
		//	 	pn544_test();
		//		app_trace(APP_BT_TRC, "pn544 open pipe!-------------------------------------\n");
		//		}
		g_bt_state = BT_IDLE;
		g_bt_ops_state = 0;
#ifdef PROFILE_CONNECT
		a2dp_con_count = 0;
		hfp_con_count = 0;
		if(hfp_con_timer)
		{
			COS_KillTimer(hfp_con_timer);
		}
		hfp_con_timer = 0;
		
		if(a2dp_con_timer)
		{
			COS_KillTimer(a2dp_con_timer);
		}
		a2dp_con_timer = 0;
#endif
#ifdef TOOLS_BROKEN
		g_pBT_vars->local_addr.bytes[0] = 0x05;
		g_pBT_vars->local_addr.bytes[1] = 0x01;
		g_pBT_vars->local_addr.bytes[2] = 0x01;
		g_pBT_vars->local_addr.bytes[3] = 0x50;
		g_pBT_vars->local_addr.bytes[4] = 0x58;
		g_pBT_vars->local_addr.bytes[5] = 0x00;
		tmp = "RDA BT TEST5";
		strncpy(g_btconfig.local_name, tmp, 12);
#endif
		gpio_SetMute(TRUE);//warkey 2.0
		

		
#ifndef MMI_ON_WIN32
		MGR_WriteLocalDeviceAddress(g_pBT_vars->local_addr);
#endif
		MGR_ChangeLocalName(g_btconfig.local_name);
		mgr_write_pagescan_parameters(0x800, 36);
		GUI_DisplayMessage(0,  GUI_STR_BTACTIVESUCCESS, 0, GUI_MSG_FLAG_WAIT);
#if APP_SUPPORT_LCD == 1
	MGR_SetPairingMode(TRUE);
#endif
		if(g_bt_cur_device >= 0)
		{
			connect_count = 1;
			MGR_SetConnectableMode(FALSE);
			bt_connect_timer = COS_SetTimer(3000, bt_auto_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
			//BT_Connect_Handsfree(g_bt_cur_device);
		}
		else
#if APP_SUPPORT_BT_AUTOVISIBLE==0
			if(visible)
#endif
			{
				if(!(g_bt_ops_state & BT_OPS_CLOSED))
				{
					MGR_SetConnectableMode(TRUE);
				}
				BT_Set_Visiable(1);
			}
			
		return 0;
	}
	else
	{
		GUI_DisplayMessage(0, GUI_STR_BTACTIVEFAILED, 0, GUI_MSG_FLAG_WAIT);
		return RESULT_MAIN;
	}
	
	return 0;
}


#if APP_SUPPORT_BT_RECORD==1
void BT_GeneRecFile(BOOL reset);
#endif
INT32 BT_Close_Bluetooth(void)
{
	COS_EVENT ev = {0};
	
    if(g_bt_state == BT_CLOSED || (get_rdabt_state() == 0))
	{
		return 0;
	}
	
	COS_KillTimer(rssi_timer_id);
	rssi_timer_id = 0;
	
	if(g_sco_play)
	{
#if APP_SUPPORT_BT_RECORD==1
		if(g_bt_record_flag == 1)
		{
			mediaSendCommand(MC_STOP, 0);
			mediaSendCommand(MC_CLOSE, 0);
			g_bt_record_flag = 0;
			BT_GeneRecFile(FALSE); // generate file for next record
		}
#endif
		mediaSendCommand(MC_STOP, 0);
		g_sco_play = 0;
	}
	
	ev.nEventId = EV_BT_POWEROFF_REQ;
	COS_SendEvent(MOD_BT,  &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
	GUI_DisplayMessage(0, GUI_STR_BTCLOSEING, 0, GUI_MSG_FLAG_DISPLAY);
	set_bt_ops_state(BT_OPS_CLOSED);
	
	while(1)
	{
		COS_WaitEvent(MOD_APP, &ev, COS_WAIT_FOREVER);
		if(ev.nEventId == EV_BT_POWEROFF_CNF)
		{
			break;
		}
	}
	
	clean_bt_ops_state(BT_OPS_CLOSED);
	g_bt_state = BT_CLOSED;
	#ifdef PROFILE_CONNECT
	if(hfp_con_timer)
	{
		COS_KillTimer(hfp_con_timer);
		hfp_con_timer = 0;
	}
	
	if(a2dp_con_timer)
	{
		COS_KillTimer(a2dp_con_timer);
		a2dp_con_timer = 0;
	}
	#endif
	if(bt_connect_timer)
	{
		COS_KillTimer(bt_connect_timer);
		bt_connect_timer = 0;
	}
	
	//media_PlayInternalAudio(GUI_AUDIO_BT_POWEROFF, 2, FALSE);
	//while(GetToneStatus())
	//{
	//	MESSAGE_Sleep_EXT(1);
	//}
	GUI_DisplayMessage(0, GUI_STR_BTCLOSESUCESS, 0, GUI_MSG_FLAG_WAIT);
	
	return 0;
}


INT32 BT_Set_Visiable(INT8 visiable)
{
	if(g_bt_state >= BT_CONNECTED || g_bt_state == BT_CLOSED)
	{
		return 0;
	}
	
	MGR_SetDiscoverableMode(visiable);
	MGR_SetPairingMode(visiable);
	BT_Execute();
	hal_HstSendEvent(SYS_EVENT, 0x19887701);
	if(visiable)
	{
		hal_HstSendEvent(SYS_EVENT, 0x19887700);
		GUI_DisplayMessage(0, GUI_STR_BTVISIABLE, 0, GUI_MSG_FLAG_WAIT);
#if APP_SUPPORT_BT_AUTOVISIBLE==0
		media_PlayInternalAudio(GUI_AUDIO_BT_SCAN, 1, FALSE);
		g_bt_timer = 180;
#else
		g_bt_timer = 999 * 2;
#endif
		g_bt_powerof_timer = 5*60*2;
		g_bt_state = BT_SCAN;
	}
	else
	{
		GUI_DisplayMessage(0, GUI_STR_BTINVISIABLE, 0, GUI_MSG_FLAG_WAIT);
		g_bt_state = BT_IDLE;
		g_bt_ops_state = 0;
	}
	
	return 0;
}


void BT_DisplayIcon(INT32 x, INT32 y)
{
	switch(g_bt_state)
	{
	case BT_CLOSED:
		break;
		
	case BT_IDLE:
		GUI_ResShowImage(GUI_IMG_BLUETOOTH, 0, x, y);      //bt ±ê??????
		break;
		
	case BT_SCAN:
		GUI_ResShowImage(GUI_IMG_BLUETOOTH, 1, x, y);
		break;
		
	default:
		GUI_ResShowImage(GUI_IMG_BLUETOOTH, 2, x, y);
		break;
	}
	
	return;
}


void BT_UpdateLed()
{
	return;//warkey 2.1 //和displaymain里面重复了
	
	if(g_current_module == FUN_BT)			//add by gary    切换
	{
		//add by gary    切换
		if(g_bt_state > BT_CONNECTED)
		{
			LED_SetBgPattern(GUI_LED_BT_CONNECT);
		}
		else
		{
			LED_SetBgPattern(GUI_LED_BT_CLOSED + g_bt_state);
		}
	}											//add by gary    切换
	
	return;
}


BOOL is_inquiring = FALSE;
BOOL is_paring = FALSE;
INT32 BT_DisplayMain(void)
{
	UINT32 led_mode;
#if APP_SUPPORT_RGBLCD==1
	INT8 *name, *string;
	t_bdaddr *addr;
	
	if(!g_bt_connect_pending && AP_Support_LCD() && !is_inquiring && !is_paring)
	{
		ui_auto_update = FALSE;
		{
			const region_t  reg_Scroll = {0, 64, LCD_WIDTH, 16};
			
			GUI_ClearScreen(NULL);
			//GUI_ResShowImage(GUI_IMG_BLUETOOTH,g_bt_state==BT_SCAN,0,0);         //bt ±ê??????
			BT_DisplayIcon(0, 0);
#if APP_SUPPORT_BT_RECORD==1
			if(g_bt_record_flag == 1)
			{
				GUI_ResShowImage(GUI_IMG_MENU_BTDEVICEOPTION, 3, 16, 0);
			}
#endif
			
			GUI_DisplayBattaryLever();
#if APP_SUPPORT_USBSTORAGE_BG==1
			if(GetUsbCableStatus())
			{
				GUI_ResShowImage(GUI_IMG_DEVICE, 2, 48, 0);
			}
#endif
			
#if APP_SUPPORT_OPP==1
			//显示引起文件传输中断的不同原因
			//------------start--------------------
			if(stop_recv == STOP_RECV_SD_OUT)
			{
				string = GUI_GetString(GUI_STR_SD_OUT);
				GUI_DisplayText(0, 48, string);
				GUI_UpdateScreen(NULL);
				COS_Sleep(1000);
				if(!(g_bt_ops_state & BT_OPS_FILE_TRANS_ABORT))
				{
					hal_HstSendEvent(SYS_EVENT, 0x19870000);
		      if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
						g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
				}
				stop_recv = 0;
			}
			
			if(stop_recv == STOP_RECV_ABORT)
			{
				string = GUI_GetString(GUI_STR_BT_FILE_ABORT);
				hal_HstSendEvent(SYS_EVENT, 0x19870617);
				GUI_DisplayText(0, 48, string);
				GUI_UpdateScreen(NULL);
				COS_Sleep(1000);
				if(!(g_bt_ops_state & BT_OPS_FILE_TRANS_ABORT))
				{
					hal_HstSendEvent(SYS_EVENT, 0x19870001);
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)		
						g_bt_state = BT_CONNECTED;
			     else 
							g_bt_state = BT_IDLE;
				}
				stop_recv = 0;
			}
			//-------------end-------------------
#endif
			//显示通话结束
			//-------------start-----------------
			if(call_status == BT_CALL_STATUS_END)
			{
				string = GUI_GetString(GUI_STR_CALL_END);
				GUI_DisplayText(0, 48, string);
				GUI_DisplayTimeText(40, 0, GUI_TIME_DISPLAY_AUTO, (g_bt_call_time / 2) * 1000);
				GUI_UpdateScreen(NULL);
				
				COS_Sleep(1000);
				GUI_ClearScreen(NULL);
				call_status = BT_CALL_STATUS_NONE;
				g_bt_call_time = 0;
				if(g_bt_state == BT_CALL)
				{
                 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
					g_bt_state = BT_CONNECTED;
					else 
							g_bt_state = BT_IDLE;
				}
#if APP_SUPPORT_OPP==1
				if(have_recv_file_size < recv_file_size)
				{
					g_bt_state = BT_FILE;
					BT_Send_Msg_Up(EV_BT_FILE_RSV_IND, 0);
				}
#endif
			}
			//-------------end-------------------
			
			string = GUI_GetString(GUI_STR_BT_CLOSED + g_bt_state);
#if APP_SUPPORT_OPP==1
			//add by wuxiang
			//解决蓝牙接收文件，信息显示方式状态不统一
			//
			/*
			if(scrollregion.y < 48)
			{
				scrollregion.x = 0;
				scrollregion.y = 48;
				scrollregion.width = LCD_WIDTH;
				scrollregion.height = 16;
			}
			*/
			//add end
#endif
			if(g_bt_device_change)
			{
				g_bt_device_change = 0;
#if APP_SUPPORT_OPP==1
				if(g_bt_state == BT_FILE)
				{
					sprintf(g_bt_currdevice, string, bt_file_name);
				}
				else
#endif
				{
					if(g_bt_state >= BT_CONNECTED)
					{
						name = g_pBT_vars->device_list[g_bt_cur_device].name;
						addr = &(g_pBT_vars->device_list[g_bt_cur_device].addr);
					}
					else
					{
						name = g_bt_local_name; //g_btconfig.local_name;
						addr = &g_pBT_vars->local_addr;
					}
					
					strcpy(g_bt_currdevice, name);
					strcat(g_bt_currdevice, "(");
					MGR_PrintBDAddress(*(t_bdaddr*)addr, g_bt_currdevice + strlen(g_bt_currdevice));
					strcat(g_bt_currdevice, ")    ");
				}
				
				GUI_SetScrollRegin(&reg_Scroll, 3);
				
				GUI_Scroll(g_bt_currdevice, TRUE);
			}
			else
			{
				GUI_Scroll(g_bt_currdevice, FALSE);
			}
			
			if(g_bt_state <= BT_CONNECTED)
			{
				if(g_bt_state == BT_SCAN)
				{
					sprintf(g_bt_msg, string, g_bt_timer / 2);
					GUI_DisplayText(0, 48, g_bt_msg);
				}
				else if(g_bt_sco_handle)
				{
					GUI_ResShowString(GUI_STR_BT_AUDIO, 0, 48);
				}
				else if(downLoading)
				{
					sprintf(g_bt_msg, "%d/%d", pb_index, phonebook_size);
					GUI_DisplayTextCenter(48, g_bt_msg);
				}
				else
				{
					GUI_DisplayText(0, 48, string);
				}
			}
			else
			{
				switch(g_bt_state)
				{
#if APP_SUPPORT_OPP==1
				case BT_FILE:
                    sprintf(g_bt_msg, "%dKB/%dKB", have_recv_file_size/1024, recv_file_size/1024);
					GUI_DisplayTextCenter(48, g_bt_msg);
					break;
#endif
					
				case BT_CALL:
					{
						u_int8 number[30];
						u_int8 length;
						switch(call_status)
						{
						case BT_CALL_STATUS_OUTGOING:
						case BT_CALL_STATUS_ALERT:
							string = GUI_GetString(GUI_STR_CALL_OUTGOING);
							break;
							
						case BT_CALL_STATUS_INCOMING:
							string = GUI_GetString(GUI_STR_CALL_INCOMING);
							break;
							
						default:
							string = GUI_GetString(GUI_STR_BT_CALL);
							//call_status = BT_CALL_STATUS_ACTIVE;
							break;
						}
						
						HF_Get_Number(number, &length);
						if(length == 0)
						{
							sprintf(g_bt_msg, string, GUI_GetString(GUI_STR_CALL_UNKNOW_NUMBER));
						}
						else
						{
							sprintf(g_bt_msg, string, number);
						}
						GUI_DisplayText(0, 48, g_bt_msg);
					}
					break;
					
				default:
					GUI_DisplayText(0, 48, string);
					break;
				}
			}
		}
		//add by wuxiang
		if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
		{
			GUI_DisplayTimeText(40, 0, GUI_TIME_DISPLAY_AUTO, (g_bt_call_time / 2) * 1000);
		}
		//add end
		
		GUI_UpdateScreen(NULL);                             //????
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LCD==1
	INT8 *name, *string;
	t_bdaddr *addr;
	
	if(!g_bt_connect_pending && AP_Support_LCD() && !is_inquiring && !is_paring)
	{
		ui_auto_update = FALSE;
		{
			const region_t  reg_Scroll = {0, 48, LCD_WIDTH, 16};
	
			GUI_ClearScreen(NULL);
	
			//GUI_ResShowImage(GUI_IMG_BLUETOOTH,g_bt_state==BT_SCAN,0,0);         //bt ±ê??????
			BT_DisplayIcon(0, 0);
	
#if APP_SUPPORT_BT_RECORD==1
			if(g_bt_record_flag == 1)
			{
				GUI_ResShowImage(GUI_IMG_MENU_BTDEVICEOPTION, 3, 16, 0);
			}
#endif
	
			GUI_DisplayBattaryLever();
	
#if APP_SUPPORT_USBSTORAGE_BG==1
			if(GetUsbCableStatus())
			{
				GUI_ResShowImage(GUI_IMG_DEVICE, 2, 48, 0);
			}
#endif
	
#if APP_SUPPORT_OPP==1
			//显示引起文件传输中断的不同原因
			//------------start--------------------
			if(stop_recv == STOP_RECV_SD_OUT)
			{
				string = GUI_GetString(GUI_STR_SD_OUT);
				GUI_DisplayText(0, 24, string);
				GUI_UpdateScreen(NULL);
				COS_Sleep(1000);
				if(!(g_bt_ops_state & BT_OPS_FILE_TRANS_ABORT))
				{
					hal_HstSendEvent(SYS_EVENT, 0x19870000);
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)		
						g_bt_state = BT_CONNECTED;
					else 
							g_bt_state = BT_IDLE;
				}
				stop_recv = 0;
			}
	
			if(stop_recv == STOP_RECV_ABORT)
			{
				string = GUI_GetString(GUI_STR_BT_FILE_ABORT);
				hal_HstSendEvent(SYS_EVENT, 0x19870617);
				GUI_DisplayText(0, 24, string);
				GUI_UpdateScreen(NULL);
				COS_Sleep(1000);
				if(!(g_bt_ops_state & BT_OPS_FILE_TRANS_ABORT))
				{
					hal_HstSendEvent(SYS_EVENT, 0x19870001);
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)		
						g_bt_state = BT_CONNECTED;
					else 
							g_bt_state = BT_IDLE;
				}
				stop_recv = 0;
			}
			//-------------end-------------------
#endif
			//显示通话结束
			//-------------start-----------------
			if(call_status == BT_CALL_STATUS_END)
			{
				string = GUI_GetString(GUI_STR_CALL_END);
				GUI_DisplayText(0, 24, string);
				GUI_DisplayTimeText(40, 0, GUI_TIME_DISPLAY_AUTO, (g_bt_call_time / 2) * 1000);
				GUI_UpdateScreen(NULL);
	
				COS_Sleep(1000);
				GUI_ClearScreen(NULL);
				call_status = BT_CALL_STATUS_NONE;
				g_bt_call_time = 0;
				if(g_bt_state == BT_CALL)
				{
                	 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
					g_bt_state = BT_CONNECTED;
					else 
							g_bt_state = BT_IDLE;
				}
#if APP_SUPPORT_OPP==1
				if(have_recv_file_size < recv_file_size)
				{
					g_bt_state = BT_FILE;
					BT_Send_Msg_Up(EV_BT_FILE_RSV_IND, 0);
				}
#endif
			}
			//-------------end-------------------
	
			string = GUI_GetString(GUI_STR_BT_CLOSED + g_bt_state);
#if APP_SUPPORT_OPP==1
			//add by wuxiang
			//解决蓝牙接收文件，信息显示方式状态不统一
			//
			/*
			if(scrollregion.y < 48)
					{
				scrollregion.x = 0;
				scrollregion.y = 48;
				scrollregion.width = LCD_WIDTH;
				scrollregion.height = 16;
			}
			*/
			//add end
#endif
	
			if(g_bt_device_change)
			{
				g_bt_device_change = 0;
#if APP_SUPPORT_OPP==1
				if(g_bt_state == BT_FILE)
				{
					sprintf(g_bt_currdevice, string, bt_file_name);
				}
				else
#endif
				{
					if(g_bt_state >= BT_CONNECTED)
					{
						name = g_pBT_vars->device_list[g_bt_cur_device].name;
						addr = &(g_pBT_vars->device_list[g_bt_cur_device].addr);
					}
					else
					{
						name = g_bt_local_name;//g_btconfig.local_name;
						addr = &g_pBT_vars->local_addr;
					}
	
					strcpy(g_bt_currdevice, name);
					strcat(g_bt_currdevice, "(");
					MGR_PrintBDAddress(*(t_bdaddr*)addr, g_bt_currdevice + strlen(g_bt_currdevice));
					strcat(g_bt_currdevice, ")    ");
				}
				GUI_SetScrollRegin(&reg_Scroll, 3);
	
				GUI_Scroll(g_bt_currdevice, TRUE);
			}
			else
			{
				GUI_Scroll(g_bt_currdevice, FALSE);
			}
	
			if(g_bt_state <= BT_CONNECTED)
			{
				if(g_bt_state == BT_SCAN)
				{
					sprintf(g_bt_msg, string, g_bt_timer / 2);
					GUI_DisplayText(0, 24, g_bt_msg);
				}
				else if(g_bt_sco_handle)
				{
					GUI_ResShowString(GUI_STR_BT_AUDIO, 0, 24);
				}
#if APP_SUPPORT_PBAP == 1
				else if(downLoading)
				{
					sprintf(g_bt_msg, "%d/%d", pb_index, phonebook_size);
					GUI_DisplayTextCenter(24, g_bt_msg);
				}
#endif
				else
				{
					GUI_DisplayText(0, 24, string);
				}
			}
			else
			{
				switch(g_bt_state)
				{
#if APP_SUPPORT_OPP==1
				case BT_FILE:
					sprintf(g_bt_msg, "%dKB/%dKB", have_recv_file_size / 1024, recv_file_size / 1024);
					GUI_DisplayTextCenter(24, g_bt_msg);
					break;
#endif
	
				case BT_CALL:
					{
						u_int8 number[30];
						u_int8 length;
						switch(call_status)
						{
						case BT_CALL_STATUS_OUTGOING:
						case BT_CALL_STATUS_ALERT:
							string = GUI_GetString(GUI_STR_CALL_OUTGOING);
							break;
	
						case BT_CALL_STATUS_INCOMING:
							string = GUI_GetString(GUI_STR_CALL_INCOMING);
							break;
	
						default:
							string = GUI_GetString(GUI_STR_BT_CALL);
							//call_status = BT_CALL_STATUS_ACTIVE;
							break;
						}
	
						HF_Get_Number(number, &length);
						if(length == 0)
						{
							sprintf(g_bt_msg, string, GUI_GetString(GUI_STR_CALL_UNKNOW_NUMBER));
						}
						else
						{
							sprintf(g_bt_msg, string, number);
						}
						GUI_DisplayText(0, 24, g_bt_msg);
					}
					break;
	
				case BT_PLAY:
					GUI_ResShowImage(GUI_IMG_MUSICEQS, g_pBT_vars->eq_type, POSITION_E + 2, 0);
					GUI_DisplayText(0, 24, string);
					break;
	
				default:
					GUI_DisplayText(0, 24, string);
					break;
				}
			}
		}
		//add by wuxiang
		if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
		{
			GUI_DisplayTimeText(40, 0, GUI_TIME_DISPLAY_AUTO, (g_bt_call_time / 2) * 1000);
		}
		//add end
		GUI_UpdateScreen(NULL);                             //????
		ui_auto_update = TRUE;
	}
#elif APP_SUPPORT_LED8S==1
	INT8 *name, *string;
	t_bdaddr *addr;
	
	if(!g_bt_connect_pending && AP_Support_LED())
	{
		GUI_ClearScreen(NULL);
#if APP_SUPPORT_OPP==1
		//显示文件传输中断
		//------------start--------------------
		if(stop_recv)
		{
			GUI_DisplayText(0, 0, "STOP");
			GUI_UpdateScreen(NULL);
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
			else 
							g_bt_state = BT_IDLE;
			stop_recv = 0;
		}
#endif
		//-------------end-------------------
		//显示通话结束
		//-------------start-----------------
		if(call_status == BT_CALL_STATUS_END)
		{
			GUI_DisplayText(0, 0, "END");
			GUI_UpdateScreen(NULL);
			COS_Sleep(1000);
			GUI_ClearScreen(NULL);
			call_status = BT_CALL_STATUS_NONE;
			g_bt_call_time = 0;
			if(g_bt_state == BT_CALL)
			{
				 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
			}
#if APP_SUPPORT_OPP==1
			if(have_recv_file_size < recv_file_size)
			{
				g_bt_state = BT_FILE;
				BT_Send_Msg_Up(EV_BT_FILE_RSV_IND, 0);
			}
#endif
		}
		//-------------end-------------------
		//显示通话时间
		//-------------start-----------------
		if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
		{
			GUI_DisplayTimeLEDs((g_bt_call_time / 2) * 1000);
		}
		//-------------end-------------------
		else
		{
			GUI_ClearScreen(NULL);
			GUI_ResShowPic(GUI_ICON_BLUETOOTH, 0, 0);
			switch(g_bt_state)
			{
			case BT_CLOSED:
				GUI_DisplayText(0, 0, " DIS");
				break;
	
			case BT_IDLE:
				GUI_DisplayText(0, 0, "IDLE");
				break;
	
			case BT_SCAN:
				GUI_DisplayText(0, 0, "SCAN");
				break;
	
			case BT_CONNECTED:
				GUI_DisplayText(0, 0, "CONN");
				break;
	
			case BT_CALL:
				GUI_DisplayText(0, 0, "CALL");
				break;
	
			case BT_PLAY:
				GUI_ResShowPic(GUI_ICON_PLAY, 0, 0);
				GUI_DisplayText(0, 0, "A2DP");
				break;
	
#if APP_SUPPORT_OPP==1
			case BT_FILE:
				GUI_ResShowPic(GUI_ICON_SD, 0, 0);
				GUI_DisplayText(0, 0, "FILE");
				break;
#endif
			}
		}
		GUI_UpdateScreen(NULL);
	}
	
#elif APP_SUPPORT_LED==1
	if(!g_bt_connect_pending)
	{
#if APP_SUPPORT_OPP==1
		//文件传输中断
		//------------start--------------------
		if(stop_recv)
		{
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
			else 
							g_bt_state = BT_IDLE;
			stop_recv = 0;
		}
#endif
		//-------------end-------------------
		//通话结束
		//-------------start-----------------
		if(call_status == BT_CALL_STATUS_END)
		{
			call_status = BT_CALL_STATUS_NONE;
			hal_HstSendEvent(SYS_EVENT, 0x19889009);
			COS_Sleep(300);
			if(g_bt_call_time > 0)
			{
				g_bt_call_time = 0;
				COS_Sleep(1000);
			}
			if(g_bt_state == BT_CALL)
			{
				 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
			}
#if APP_SUPPORT_OPP==1
			if(have_recv_file_size < recv_file_size)
			{
				g_bt_state = BT_FILE;
				BT_Send_Msg_Up(EV_BT_FILE_RSV_IND, 0);
			}
#endif
		}
	}
#endif
	
#if 0
	if(g_bt_state >= BT_CONNECTED)
	{
		LED_SetPattern(GUI_LED_BT_CLOSED + g_bt_state, LED_LOOP_INFINITE);
	}
	else
	{
		LED_SetPattern(GUI_LED_NONE, 1);
	}
#else
	LED_SetPattern(GUI_LED_BT_CLOSED + g_bt_state, LED_LOOP_INFINITE);
#endif
	
	return 0;
}


INT32 BT_InputPin(void)
{
	return 0;
}


void BT_VolumeCb( INT32 Vol )
{
	INT32 vol_tmp = g_pBT_vars->volume;
	//extern UINT8 g_vol_set_by_ag;
	g_pBT_vars->volume = Vol;
	g_comval->Volume = Vol;
	if(g_bt_state == BT_CALL)//gary 2.1
	{
		if(Vol > MAX_BT_CALL_VOL_LEVEL)
		{
			Vol = MAX_BT_CALL_VOL_LEVEL;
		}
	}
	if(g_bt_state > BT_CONNECTED)
	{
		SetPAVolume(Vol);
	}
	else
	{
		SetInternalPAVolume(Vol);
	}
	
	if(g_bt_state == BT_CALL)
	{
		//if(g_vol_set_by_ag == 0xff)
		HF_Send_Speaker_Gain(Vol);
		/*else if(Vol > g_vol_set_by_ag)
		MESSAGE_SetEvent(AP_KEY_VOLSUB | AP_KEY_PRESS);
		else if(Vol < g_vol_set_by_ag)
		MESSAGE_SetEvent(AP_KEY_VOLADD | AP_KEY_PRESS);
		else
		g_vol_set_by_ag = 0xff;*/
	}
	else if(g_bt_state == BT_PLAY)
	{
		if(vol_tmp > Vol)
		{
			Avrcp_Send_Key(OPID_VOLUME_DOWN);
		}
		else if(vol_tmp < Vol)
		{
			Avrcp_Send_Key(OPID_VOLUME_UP);
		}
	}
}


INT32 BT_ChangeVol(UINT32 key)
{
	INT32 result;
	
#if APP_SUPPORT_RGBLCD==1
	if(AP_Support_LCD())
	{
		hal_HstSendEvent(SYS_EVENT, 0x1988a000);
		//show background
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
		
		slider.min = 0;
		slider.max = BT_VOLUME_MAX;
		slider.value = g_pBT_vars->volume;
		slider.step = 1;
		
		ui_auto_update = FALSE;
		result = GUI_Slider(& slider, BT_VolumeCb);
		ui_auto_update = TRUE;
		
		if (result != 0)
		{
			return result;
		}
		g_bt_need_draw = TRUE;
	}
	else
#elif APP_SUPPORT_LCD==1
	if(AP_Support_LCD())
	{
		hal_HstSendEvent(SYS_EVENT, 0x1988a000);
		//show background
		GUI_ClearScreen(NULL);
		GUI_ResShowPic(GUI_IMG_VOLUMEBK, 0, 0);
	
		slider.min = 0;
		slider.max = BT_VOLUME_MAX;
		slider.value = g_pBT_vars->volume;
		slider.step = 1;
	
		ui_auto_update = FALSE;
		result = GUI_Slider(& slider, BT_VolumeCb);
		ui_auto_update = TRUE;
	
		if (result != 0)
		{
			return result;
		}
		g_bt_need_draw = TRUE;
	}
	else
#endif
	{
		hal_HstSendEvent(SYS_EVENT, 0x19884000);
		hal_HstSendEvent(SYS_EVENT, g_pBT_vars->volume);
		
		key = key & 0xffff0000;
		hal_HstSendEvent(SYS_EVENT, key);
		if(key == AP_KEY_VOLADD || key == AP_KEY_NEXT)
		{
			g_pBT_vars->volume++;
			if(g_pBT_vars->volume > BT_VOLUME_MAX)
			{
				g_pBT_vars->volume = BT_VOLUME_MAX;
			}
		}
		
		if(key == AP_KEY_VOLSUB || key == AP_KEY_PREV)
		{
			g_pBT_vars->volume--;
			if(g_pBT_vars->volume < 0)
			{
				g_pBT_vars->volume = 0;
			}
		}
		
		BT_VolumeCb(g_pBT_vars->volume);
#if  XDL_APP_SUPPORT_TONE_TIP==1
		if(g_pBT_vars->volume >= BT_VOLUME_MAX || g_pBT_vars->volume <= 0)
		{
			lowpower_flag_for_tone = 0;
			MESSAGE_SetEvent(0xaaaa1111);
		}
#endif
	}
	
	return 0;
}


BOOL BT_ResetDir(void)
{
	UINT8 disk_ok = FALSE;
	
	//??????????????
	if(MountDisk(g_pBT_vars->location.disk))
	{
		disk_ok = TRUE;
	}
	else if(MountDisk(FS_DEV_TYPE_TFLASH))
	{
		disk_ok = TRUE;
		g_pBT_vars->location.disk = FS_DEV_TYPE_TFLASH;
	}
#if APP_SUPPORT_USB
	else if(MountDisk(FS_DEV_TYPE_USBDISK))
	{
		disk_ok = TRUE;
		g_pBT_vars->location.disk = FS_DEV_TYPE_USBDISK;
	}
#endif
	hal_HstSendEvent(SYS_EVENT, 0x0823000c);
	if(disk_ok)
	{
		hal_HstSendEvent(SYS_EVENT, 0x0823000b);
		if(ERR_SUCCESS != FS_ChangeSubDir(g_pBT_vars->location.subdir_entry))
		{
			hal_HstSendEvent(SYS_EVENT, 0x0823000d);
			FS_ChangeDir(FS_CD_ROOT);
			
			if(ERR_SUCCESS != FS_ChangeDir(BT_RECEIVED_DIR))
			{
				hal_HstSendEvent(SYS_EVENT, 0x0823000e);
				if(ERR_SUCCESS != FS_MakeDir(BT_RECEIVED_DIR, 0))
				{
					hal_HstSendEvent(SYS_EVENT, 0x0823000f);
					disk_ok = FALSE;          // ???¨?????§°?
				}
				else if(ERR_SUCCESS != FS_ChangeDir(BT_RECEIVED_DIR))
				{
					hal_HstSendEvent(SYS_EVENT, 0x08230010);
					disk_ok = FALSE;
				}
				else
				{
					hal_HstSendEvent(SYS_EVENT, 0x08230011);
					FS_GetSubDirEntry(&(g_pBT_vars->location.subdir_entry));
				}
			}
		}
	}
	hal_HstSendEvent(SYS_EVENT, 0x08230012);
	
	return disk_ok;
}


INT32 bt_menu_callback(INT32 type, INT32 value, INT32 param, UINT8 **string)
{
	if(MENU_CALLBACK_QUERY_ACTIVE == type)
	{
		return 0;
	}
	
	switch(value)
	{
	case BT_ACTIVE:		   /*Open Or Close BT*/
		/*1.?¤??????*/
		return g_bt_state == BT_CLOSED;
		
	case BT_DEACTIVE:
		return g_bt_state != BT_CLOSED;
		
	case UART_BT_VISIBLE:
	case BT_VISIBLE:  /*Set BT Visiable Or Not*/
		return g_bt_state == BT_IDLE;
		
	case UART_BT_INVISIBLE:
	case BT_INVISIBLE:  /*Set BT Visiable Or Not*/
		return g_bt_state == BT_SCAN;
		
	case BT_INQUIRY_DEVICE:
		return g_bt_state != BT_CLOSED;
	}
	
	return 0;
}


#if APP_SUPPORT_MAP==1
u_int8 send_type = MSG_DRAFT;
UINT8 letter_num = 0;
int chinese_num = 0;
int unicode_num = 0;
int send_msg_index = 0;
BOOL candidate = FALSE;

char send_msg_buf[120] = {0};
char display_msg_buf[120] = {0};
UINT8 utf8_buf[120] = {0};
char letter[10][7] = {0};
char chinese[3][6][13] = {0};
UINT16 unicode_result[13] = {0};
char current_chinese[13] = {0};
char letter_display[10][7] = {0};

void deal_GB_result(char *c_result, char *e_result, UINT16 *unicode)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int index = 0;
	char *tmp = c_result;
	UINT16 *unicode_ptr = 0;
	
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 6; j++)
		{
			memset(chinese[i][j], 0, 13);
		}
	}
	memset(letter, 0, 70);
	memset(letter_display, 0, 90);
	memset(unicode_result, 0, sizeof(unicode_result));
	letter_num = 0;
	chinese_num = 0;
	unicode_num = 0;
	
	i = 0;
	j = 0;
	
	if(*tmp == 0)
	{
		goto deal_e;
	}
	
	while(*tmp != 0 && chinese_num < 36)
	{
		while(*tmp != 0x0d && *(tmp + 1) != 0x0a)
		{
			chinese[chinese_num / 6][j][i++] = *tmp++;
		}
		chinese_num++;
		j++;
		i = 0;
		tmp += 2;
		if(j > 5)
		{
			j = 0;
		}
	}
	
deal_e:
	tmp = e_result;
	
	if(*tmp == 0)
	{
		goto deal_unicode;
	}
	while(*tmp != 0x0d && *(tmp + 1) != 0x0a)
	{
		tmp++;
	}
	tmp--;
	
	i = 0;
	while(*tmp != 0 && letter_num < 10)
	{
		while(*tmp == 0x0d && *(tmp + 1) == 0x0a)
		{
			tmp += 2;
		}
		while(*tmp != 0x0d && *(tmp + 1) != 0x0a)
		{
			if(letter_num == 0)
			{
				letter[i][j] = '*';
			}
			else
			{
				letter[i][j] = *tmp;
			}
			k++;
			tmp++;
			j++;
		}
		
		if(j > 0)
		{
			i++;
			j = 0;
			k = 0;
			letter_num++;
			tmp += 2;
		}
	}
	
deal_unicode:
	i = 0;
	unicode_ptr = unicode;
	while(unicode_num < chinese_num)
	{
		while(*unicode_ptr != 0x0d && *(unicode_ptr + 1) != 0x0a)
		{
			unicode_result[i++] = *unicode_ptr++;
			unicode_num++;
		}
		unicode_ptr += 2;
	}
	
	return;
}


Send_Msg_Info send_msg_info = {0};
/*
int send_msg_at(Send_Msg_Info *pSend_msg_info)
{
	HF_Send_Msg(pSend_msg_info->message_center,
			pSend_msg_info->dst_number,
			pSend_msg_info->msg);
	return 0;
}
int send_msg_at(void)
{
	HF_Send_Msg();
	return 0;
}
*/

void display_candidate(int index)
{
	int i = 0;
	int display_len = 0;
	static int pos_start = 0;
	static int pos_end = 0;
	static int last_start = 0;
	
	if(index % 12 == 0)
	{
		pos_start = 0;
		pos_end = 0;
		last_start = 0;
	}
	if((index % 12) / 2 >= pos_end)
	{
		last_start = pos_start;
		pos_start = pos_end;
	}
	else if((index % 12) / 2 < pos_start)
	{
		pos_end = pos_start;
		pos_start = last_start;
	}
	
	i = pos_start;
	while(display_len < 12 && i < 6)
	{
		if(display_len + strlen(chinese[index / 12][i + 1]) <= 12)
		{
			GUI_DisplayText((display_len / 2) * 16, 48, chinese[index / 12][i]);
		}
		display_len += strlen(chinese[index / 12][i]);
		if(display_len <= 12)
		{
			i++;
		}
	}
	pos_end = i;
	
	return;
}


void BT_Edit_Msg(void)
{
	return;
}


int BT_Msg_Input_Number(void)
{
	return 0;
}


void BT_Send_Msg(u_int8 type)
{
	send_type = type;
#if 0
	if(0 == BT_Msg_Input_Number())
	{
		return ;
	}
	BT_Edit_Msg();
#endif
	
	if(mas_tid > 0 || mas_cid > 0)
	{
		OBEX_Disconnect(mas_tid);
		common_var_init();
	}
	
	if(send_msg_step == 1)
	{
		OBEX_ConnectRequest(&g_pBT_vars->device_list[g_bt_cur_device].addr, SCLASS_UUID_MESSAGE_ACCESS_MSE, inputbuf_overlay);
		send_msg_step = 2;
	}
	
	return;
}


void MSG_Download_Timeout(void)
{
	downLoading = FALSE;
	COS_KillTimer(msg_timer);
	GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOAD_FAILED, NULL, GUI_MSG_FLAG_DISPLAY);
	COS_Sleep(1000);
	common_var_init();
	msg_timer = 0;
	
	return;
}


void BT_Map(void)
{
	int i = 0;
	//	downLoading = TRUE;
	
	if(history_step != 1 || phonebook_step != 1 || mas_step != 1)
	{
		GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOAD_FAILED, NULL, GUI_MSG_FLAG_WAIT);
		return ;
	}
	
#if APP_SUPPORT_MAP==1
	for(i = 0; i < 4; i++)
	{
		if(message_curr[i] == NULL)
		{
			message_curr[i] = (Message_info *)COS_Malloc(sizeof(Message_info));
			message_curr[i]->msg = NULL;
		}
		
		if(message_prev[i] == NULL)
		{
		
			message_prev[i] = (Message_info *)COS_Malloc(sizeof(Message_info));
			message_prev[i]->msg = NULL;
		}
		
		if(message_next[i] == NULL)
		{
		
			message_next[i] = (Message_info *)COS_Malloc(sizeof(Message_info));
			message_next[i]->msg = NULL;
		}
		
		if(message_curr[i]->msg == NULL)
		{
			message_curr[i]->msg = (char *)COS_Malloc(MSG_BUFF_LEN);
		}
		
		if(message_prev[i]->msg == NULL)
		{
			message_prev[i]->msg = (char *)COS_Malloc(MSG_BUFF_LEN);
		}
		
		if(message_next[i]->msg == NULL)
		{
			message_next[i]->msg = (char *)COS_Malloc(MSG_BUFF_LEN);
		}
		
		if(msg_show[i] == NULL)
		{
			msg_show[i] = (char *)COS_Malloc(MSG_BUFF_LEN);
		}
		
		memset(message_curr[i]->msg, 0, MSG_BUFF_LEN);
		memset(message_prev[i]->msg, 0, MSG_BUFF_LEN);
		memset(message_next[i]->msg, 0, MSG_BUFF_LEN);
		memset(msg_show[i], 0, MSG_BUFF_LEN);
	}
#endif
	
	GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOADING, NULL, GUI_MSG_FLAG_DISPLAY);
//	msg_timer = COS_SetTimer(30000, MSG_Download_Timeout, NULL, COS_TIMER_MODE_SINGLE);
	if(mas_step == 1)
	{
		hal_HstSendEvent(SYS_EVENT, 0x19880a00);
		OBEX_ConnectRequest(&g_pBT_vars->device_list[g_bt_cur_device].addr, SCLASS_UUID_MESSAGE_ACCESS_MSE, inputbuf_overlay);
		mas_step = 2;
	}
	
	return;
}
#endif


#if APP_SUPPORT_PBAP==1
void PB_Download_Timeout(void)
{
	downLoading = FALSE;
	COS_KillTimer(pb_timer);
	GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOAD_FAILED, NULL, GUI_MSG_FLAG_DISPLAY);
	COS_Sleep(1000);
	common_var_init();
	pb_timer = 0;
	
	return;
}


extern int pb_length;
extern int pb_size ;
extern UINT8 *pb_addr;
void BT_Read_Phone_Book(void)
{
	int i = 0;
	if(phonebook_info == NULL)
	{
		phonebook_info = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
		memset(phonebook_info, 0, sizeof(Phonebook_info));
	}
	for(i = 0; i < 4; i++)
	{
		if(phonebook_show[i] == NULL)
		{
			phonebook_show[i] = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
			memset(phonebook_show[i], 0, sizeof(Phonebook_info));
		}
	}
	NVRAMGetPhoneBookParam(&pb_length, &pb_index, &phonebook_size);
	pb_addr = NVRAMGetPhoneBookAddress();
	pb_addr += NVRAMGetPhoneBookOffset();
	hal_HstSendEvent(SYS_EVENT, 0x19880006);
	hal_HstSendEvent(SYS_EVENT, phonebook_size);
	if(phonebook_size == 0 || phonebook_size == 0xffffffff)
	{
		pb_addr = 0;
		phonebook_size = 0;
		pb_index = 0;
		pb_length = 0;
		GUI_DisplayMessage(0, GUI_STR_NO_PB_DATA, NULL, GUI_MSG_FLAG_WAIT);
	}
	else
	{
		MESSAGE_SetEvent(OBEX_SHOW_PHONEBOOK);
	}
	
	return;
}


void BT_Pb_Search(void)
{

	int i = 0;
	for(i = 0; i < 4; i++)
	{
		if(phonebook_show[i] == NULL)
		{
			phonebook_show[i] = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
			memset(phonebook_show[i], 0, sizeof(Phonebook_info));
		}
	}
	//for(i = 0; i < 4; i++)
	//		phonebook_info_clean(&phonebook_show[i]);
	GUI_DisplaySearchList(NULL, 4, phonebook_searchlist_callback);
	
	return;
}


void BT_Phone_Book_init(void)
{
	int i = 0;
	if(phonebook_info == NULL)
	{
		phonebook_info = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
		memset(phonebook_info, 0, sizeof(Phonebook_info));
	}
	hal_HstSendEvent(SYS_EVENT, 0x19883502);
	for(i = 0; i < 4; i++)
	{
		if(phonebook_show[i] == NULL)
		{
			phonebook_show[i] = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
			memset(phonebook_show[i], 0, sizeof(Phonebook_info));
		}
	}
	hal_HstSendEvent(SYS_EVENT, 0x19883503);
	NVRAMErasePhoneBook();
	pb_index = 0;
	pb_get_pos = 0;
	
	return;
}

#ifndef BT_PBAP_DATA_SAVE 
UINT8 pbap_buf[2048] = {0};
#endif

void BT_Phone_Book(void)
{
	int i = 0;
	
	hal_HstSendEvent(SYS_EVENT, 0x19883500);
#if APP_SUPPORT_MAP == 1
	if(history_step != 1 || phonebook_step != 1 || mas_step != 1)
#else
	if(history_step != 1 || phonebook_step != 1)
#endif
	{
		GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOAD_FAILED, NULL, GUI_MSG_FLAG_WAIT);
		
		return ;
	}
	
	hal_HstSendEvent(SYS_EVENT, 0x19883501);
	if(phonebook_info == NULL)
	{
		phonebook_info = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
		memset(phonebook_info, 0, sizeof(Phonebook_info));
	}
	hal_HstSendEvent(SYS_EVENT, 0x19883502);
	
	for(i = 0; i < 4; i++)
	{
		if(phonebook_show[i] == NULL)
		{
			phonebook_show[i] = (Phonebook_info *)COS_Malloc(sizeof(Phonebook_info));
			memset(phonebook_show[i], 0, sizeof(Phonebook_info));
		}
	}
	
	hal_HstSendEvent(SYS_EVENT, 0x19883503);
#ifdef BT_PBAP_DATA_SAVE
	NVRAMErasePhoneBook();	
#endif
	downLoading = TRUE;
	pb_index = 0;
	pb_get_pos = 0;
	hal_HstSendEvent(SYS_EVENT, 0x1988accc);
	hal_HstSendEvent(SYS_EVENT, phonebook_step);
	//GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOADING, NULL, GUI_MSG_FLAG_DISPLAY);
	//pb_timer = COS_SetTimer(15000, PB_Download_Timeout, NULL, COS_TIMER_MODE_SINGLE);
	if(phonebook_step == 1)
	{
#ifdef BT_PBAP_USE_AT	
		HF_Get_Char_Code();
		HF_Set_PhoneBook_PATH(PHONE_BOOK_PHONE, PHONE_BOOK_PHONE_BOOK);
#else

#ifdef BT_PBAP_DATA_SAVE
		OBEX_ConnectRequest(&g_pBT_vars->device_list[g_bt_cur_device].addr
			,SCLASS_UUID_PHONEBOOK_ACCESS_PSE, inputbuf_overlay);
#else
		OBEX_ConnectRequest(&g_pBT_vars->device_list[g_bt_cur_device].addr
			,SCLASS_UUID_PHONEBOOK_ACCESS_PSE, pbap_buf/*inputbuf_overlay*/);
#endif
		
#endif
		phonebook_step = 2;
	}
	hal_HstSendEvent(SYS_EVENT, 0x19883504);
	
	return;
}


void BT_Call_History(void)
{
	int i = 0;
	
#if APP_SUPPORT_MAP == 1
	if(phonebook_step != 1 || history_step != 1 || mas_step != 1 )
#else
	if(phonebook_step != 1 || history_step != 1)
#endif
	{
		GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOAD_FAILED, NULL, GUI_MSG_FLAG_WAIT);
		return ;
	}
	//	downLoading = TRUE;
	for(i = 0; i < 4; i++)
	{
		if(history_curr[i] == NULL)
		{
			history_curr[i] = (History_info *)COS_Malloc(sizeof(History_info));
			//memset(history_curr[i], 0, sizeof(History_info));
		}
		
		if(history_prev[i] == NULL)
		{
			history_prev[i] = (History_info *)COS_Malloc(sizeof(History_info));
			//memset(history_prev[i], 0, sizeof(History_info));
		}
		
		if(history_next[i] == NULL)
		{
			history_next[i] = (History_info *)COS_Malloc(sizeof(History_info));
			//memset(history_next[i], 0, sizeof(History_info));
		}
		
		if(history_show[i] == NULL)
		{
			history_show[i] = (History_info *)COS_Malloc(sizeof(History_info));
			//memset(history_show[i], 0, sizeof(History_info));
		}
	}
	
	history_info_clean(history_curr);
	history_info_clean(history_prev);
	history_info_clean(history_next);
	history_info_clean(history_show);
	
	//GUI_DisplayMessage(0, GUI_STR_PB_DOWNLOADING, NULL, GUI_MSG_FLAG_DISPLAY);
	pb_timer = COS_SetTimer(15000, PB_Download_Timeout, NULL, COS_TIMER_MODE_SINGLE);
	if(history_step == 1)
	{
#ifdef BT_PBAP_DATA_SAVE	
		OBEX_ConnectRequest(&g_pBT_vars->device_list[g_bt_cur_device].addr,SCLASS_UUID_PHONEBOOK_ACCESS_PSE,inputbuf_overlay);
#else
		OBEX_ConnectRequest(&g_pBT_vars->device_list[g_bt_cur_device].addr,SCLASS_UUID_PHONEBOOK_ACCESS_PSE,pbap_buf);
#endif		
		history_step = 2;
	}
	
	return;
}
#endif


BOOL BT_IsActive(VOID)
{
	return g_sco_play == 1;
}


#if APP_SUPPORT_BT_RECORD==1
void BT_GeneRecFile(BOOL reset)
{
	INT32 num;
	
	g_bt_record_flag = -1;
	if(reset)
	{
		if(MountDisk(FS_DEV_TYPE_TFLASH) && RECORD_ResetDir(&g_bt_record_file, BT_RECORD_DIR))
		{
			hal_HstSendEvent(SYS_EVENT, 0x13110703);
			g_bt_record_flag = 0;
		}
		else
		{
			return;
		}
		
		sprintf(g_bt_record_file.file_name, "CAL000  WAV");
	}
	
	num = GenNewName(&g_bt_record_file);
	if(num > 0)
	{
		sprintf(g_bt_record_file.file_name, "CAL%03d  WAV", num);
		g_bt_record_flag = 0;
	}
	
	return;
}
#endif


UINT32 event_detected_bt_pending = 0;
extern UINT32 event_detected_displaying_log;
void bt_pause_state_timeout(void *param)
{
	hal_HstSendEvent(SYS_EVENT, 0x19880099);
	hal_HstSendEvent(SYS_EVENT, g_bt_ops_state & BT_OPS_RECV_PAUSE);
	hal_HstSendEvent(SYS_EVENT, g_avrcp_status);
	clean_bt_ops_state(BT_OPS_BT_MODE_EXIT);
	if(g_avrcp_status == AVRCP_STATUS_PLAYING)
	{
		if(!(g_bt_ops_state & BT_OPS_RECV_PAUSE))
		{
			BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
		}
	}
	
	return;
}


#if defined(BT_DEVICE_SAVE)
void BT_Connect_Save(void)
{
	hal_HstSendEvent(SYS_EVENT, 0x1993fead);
	hal_HstSendEvent(SYS_EVENT, g_bt_state);
	hal_HstSendEvent(SYS_EVENT, call_status);
	MGR_SetDiscoverableMode(0);
	if(g_pBT_vars->default_device != g_bt_cur_device)
	{
		g_pBT_vars->default_device = g_bt_cur_device;
		NVRAMWriteData();
	}

#if XDL_APP_SUPPORT_TONE_TIP==1
	if(call_status==BT_CALL_STATUS_NONE)
	{
		if(g_bt_state==BT_CONNECTED)
		{
		media_PlayInternalAudio(GUI_AUDIO_BT_CONNECT, 1, FALSE);
		while(GetToneStatus())
		{
			MESSAGE_Sleep_EXT(1);
			}
		}
		else if(g_bt_state==BT_PLAY)
		{
			lowpower_flag_for_tone = 5;
			MESSAGE_SetEvent(0xaaaa1111);
		}
		hal_HstSendEvent(SYS_EVENT, 0x13070502);
	}
#endif	

	return;
}

void BT_Disconnect_Save(void)
{
	hal_HstSendEvent(SYS_EVENT, 0x1994fead);
	NVRAMWriteData();
	
	return;
}
#endif


#if defined(CLOSE_BT_SWITCH_MODE)
static int close_bt_mode = 0;
#endif

#if XDL_APP_SUPPORT_TONE_TIP==1
	static BOOL poweroff_flag = FALSE;
#endif

INT32 BT_HandleKey(UINT32 key)
{
	INT32 result = 0;
	INT32 temp = 0;
	Phonebook_info *pb = NULL;
	
	
	if(g_bt_connect_pending && key == BT_CONNECT_TIMEOUT)
	{
		hal_HstSendEvent(SYS_EVENT, 0x13062703);
		if(g_bt_connect_pending == 1)
		{
			g_avrcp_connect = 0;
		}
		if(g_bt_connect_pending == 2)
		{
			g_hfp_connect = 0;
		}
		g_bt_connect_pending = 0;
		MESSAGE_Initial(g_comval);
		
		return  GUI_DisplayMessage(0, GUI_STR_BT_CONNECT_FAIL, NULL, GUI_MSG_FLAG_WAIT);
	}
	else if(g_bt_connect_pending || is_paring )
	{
		//hal_HstSendEvent(SYS_EVENT, 0x13062704);
		//hal_HstSendEvent(SYS_EVENT, key);
		key = MESSAGE_HandleHotkey(key);
		if(key != 0)
		{
			event_detected_bt_pending = key;//save unexpected irq or event, e.g. USB or T card be plugged in
		}
		
		return result;
	}
	else if(is_inquiring)
	{
		hal_HstSendEvent(SYS_EVENT, 0x13062705);
		if(key == (AP_KEY_MODE | AP_KEY_PRESS))
		{
			is_inquiring = FALSE;
			inquiry_count = 0;
			MGR_InquiryCancel();
		}
		
		if(key == AP_MSG_BT_ACTIVE)
		{
			return RESULT_BT_ACTIVE;
		}
		
		return result;
	}
	
	//    hal_HstSendEvent(SYS_EVENT, 0x13062514);
	//    hal_HstSendEvent(SYS_EVENT, key);
	
#if XDL_APP_SUPPORT_TONE_TIP==1
	if(key == 0xad11dead)
	{
		hal_HstSendEvent(APP_EVENT, 0x130904dd);
		hal_HstSendEvent(APP_EVENT, g_bt_state);
		if(g_bt_state >= BT_CONNECTED)
		{
			BT_Disconnect_Device();
#if defined(CLOSE_BT_SWITCH_MODE)
			close_bt_mode = RESULT_STANDBY;
#else//warkey 2.1 //关机不成功
			return RESULT_STANDBY;
#endif
			poweroff_flag = TRUE;
			return result;
		}
	}
#endif
	switch(key)
	{
	case AP_MSG_WAIT_TIMEOUT:
		hal_HstSendEvent(SYS_EVENT, 0x11050100);
		if(g_bt_state == BT_CLOSED || (g_bt_state == BT_SCAN ) || (g_bt_state == BT_CONNECTED && g_bt_sco_handle == 0)
		   || (g_bt_state == BT_IDLE && g_bt_connect_pending + is_inquiring + is_paring == 0))
		{
			hal_HstSendEvent(SYS_EVENT, 0x11050111);
			APP_Sleep();
		}
		
		break;
#if 1		
#if defined(BT_DEVICE_SAVE)
	case EV_BT_CONNECT_IND:
		BT_Connect_Save();
		break;
		
	case EV_BT_DISCONNECT_IND:
		hal_HstSendEvent(SYS_EVENT, 0xbbbbffff);
		g_bt_state = BT_IDLE;
#if XDL_APP_SUPPORT_TONE_TIP==1
		if(poweroff_flag == TRUE)
		{
			poweroff_flag = FALSE;
			MESSAGE_SetEvent(0xad11dead);
		}
#if defined(CLOSE_BT_SWITCH_MODE)
		else if(close_bt_mode == 0)
#else
		else
#endif
		{
			media_PlayInternalAudio(GUI_AUDIO_BT_DISCONNECT, 1, FALSE);
			while(GetToneStatus())
			{
				MESSAGE_Sleep_EXT(1);
			}
		}
#endif
		//g_pBT_vars->default_device = -1;
		//BT_Disconnect_Save();
		
#if defined(CLOSE_BT_SWITCH_MODE)
		int temp = 0;
		if(close_bt_mode != 0)
		{
			temp = close_bt_mode;
			BT_Close_Bluetooth();
			close_bt_mode = 0;
			return temp;
		}
#endif
		break;
#endif
#endif		
#if defined(CLOSE_BT_SWITCH_MODE)
	case AP_MSG_LINE_IN:
		media_StopInternalAudio();
		if (g_bt_state >= BT_CONNECTED)
		{
			BT_Disconnect_Device();
			close_bt_mode = RESULT_LINE_IN;
			break;
		}
		return RESULT_LINE_IN;
		
	case AP_MSG_SD_IN:
		media_StopInternalAudio();
		if (g_bt_state >= BT_CONNECTED)
		{
			BT_Disconnect_Device();
			close_bt_mode = RESULT_MUSIC;
			break;
		}
		return RESULT_MUSIC;
		
	case AP_MSG_USB_CONNECT:
		media_StopInternalAudio();
		if (g_bt_state >= BT_CONNECTED)
		{
			BT_Disconnect_Device();
			close_bt_mode = RESULT_UDISK;
			break;
		}
		return RESULT_UDISK;
#endif
		
	case AP_MSG_RTC:
		g_bt_need_draw = TRUE;
		break;
		
	case AP_KEY_MODE|AP_KEY_HOLD:
		if(MESSAGE_IsHold())
		{
			if(g_bt_state >= BT_CONNECTED)
			{
				BT_Disconnect_Device();
#if APP_SUPPORT_PBAP==1
				//		if(pb_tid)
				//            	{
				//            		OBEX_Disconnect(pb_tid);
				//           	}
				common_var_init();
#elif APP_SUPPORT_MAP==1
				//				if(mas_tid)
				//            	{
				//            		OBEX_Disconnect(mas_tid);
				//            	}
				common_var_init();
#endif
			}
		}
		break;

	//case AP_KEY_MODE|AP_KEY_PRESS:
	case AP_KEY_PLAY|AP_KEY_HOLD:
		break;
		if(MESSAGE_IsHold())
		{
#if APP_SUPPORT_MENU==1
		if(AP_Support_MENU())
		{
			if(g_bt_state >= BT_CONNECTED)
			{
#if APP_SUPPORT_PBAP==1
				//				if(pb_tid)
				//            	{
				//            		OBEX_Disconnect(pb_tid);
				//            	}
				common_var_init();
				
#elif APP_SUPPORT_MAP==1
				//				if(mas_tid)
				//            	{
				//            		OBEX_Disconnect(mas_tid);
				//           	}
				common_var_init();
#endif
				result = GUI_Display_Menu(GUI_MENU_BTDEVICEOPTION, btoption_menu_callback);
			}
			else
			{
				result = GUI_Display_Menu(GUI_MENU_BT, bt_menu_callback);
			}
			
			if(result == RESULT_IGNORE)
			{
				result = 0;
			}
			
			if(result != 0)
			{
				return result;
			}
			
			g_bt_need_draw = TRUE;
			g_bt_device_change = 1;
		}
		else
#else//无菜单
		//if(g_bt_state<=BT_CONNECTED)
		hal_HstSendEvent(SYS_EVENT, 0x19888100);
		hal_HstSendEvent(SYS_EVENT, g_a2dp_play);
		hal_HstSendEvent(SYS_EVENT, call_status);
		if(bt_connect_timer)
		{
			COS_KillTimer(bt_connect_timer);
			bt_connect_timer = 0;
		}
		if(g_a2dp_play)
		{
			hal_HstSendEvent(SYS_EVENT, 0x19888101);
			set_bt_ops_state(BT_OPS_RECV_PAUSE);
			if(short_play_status)
			{
				Avrcp_Send_Key(OPID_PAUSE);
				set_bt_ops_state(BT_OPS_MUSIC_PAUSE);
				short_play_status = FALSE;
			}
			BT_StopAudio();
			short_play_status = FALSE;
			//		COS_SetTimer(3000, bt_pause_state_timeout, NULL, COS_TIMER_MODE_SINGLE);
		}
		if(call_status == BT_CALL_STATUS_NONE)
		{
			Set_Sep_State_Playing();
			COS_Sleep(200);
			set_bt_ops_state(BT_OPS_BT_MODE_EXIT);
			COS_SetTimer(3000, bt_pause_state_timeout, NULL, COS_TIMER_MODE_SINGLE);
#if defined(CLOSE_BT_SWITCH_MODE)
			media_StopInternalAudio();
			SetBtCloseFlag(TRUE);
			if (g_bt_state >= BT_CONNECTED)
			{
				BT_Disconnect_Device();
				COS_Sleep(1500);
				close_bt_mode = RESULT_MUSIC;
				break;
			}
			BT_Close_Bluetooth();
#endif
			return RESULT_MAIN;
		}
#endif
			}
			break;
			
	case BT_SETDEFAULT_DEVICE:
		/*Set Default Device*/
		g_pBT_vars->default_device = g_bt_cur_device;
		NVRAMWriteData();
		break;
		
	case BT_CONNECT_HFP:
		/*Connect Special Device*/
		BT_Connect_Handsfree(g_bt_cur_device);
		break;
		
	case BT_CONNECT_A2DP:
		/*Connect Special Device*/
		BT_Connect_A2DP(g_bt_cur_device);
		break;
		
	case BT_SWITCH_AUDIO:
		if(g_bt_sco_handle)
		{
			MGR_ReleaseSynchronous(g_bt_sco_handle);
		}
		else
		{
			t_bdaddr address;
			HF_Get_Current_BdAddr(&address);
			MGR_SetupSynchronous(address);
		}
		break;
		
#if APP_SUPPORT_BT_RECORD==1
	case BT_CALL_RECORD:
	case AP_KEY_REC|AP_KEY_PRESS:
		if(g_sco_play)
		{
			if(g_bt_record_flag == 0)
			{
				Open_param_t param;
				param.file_name = g_bt_record_file.file_name;
				param.mode = MEDIA_RECORD;
				param.type = Music_WAV;
				//open codec
				if(!mediaSendCommand(MC_OPEN, &param))
				{
					hal_HstSendEvent(SYS_EVENT, 0x12150101);
					g_bt_record_flag = -1;
				}
				else if(!mediaSendCommand(MC_RECORD, 0))
				{
					mediaSendCommand(MC_CLOSE, 0);
					hal_HstSendEvent(SYS_EVENT, 0x12150102);
					g_bt_record_flag = -1;
				}
				else
				{
					hal_HstSendEvent(SYS_EVENT, 0x12150103);
					g_bt_record_flag = 1;
				}
			}
		}
		break;
		
	case AP_MSG_SD_IN:
		BT_GeneRecFile(TRUE);
		key = MESSAGE_HandleHotkey(AP_MSG_SD_IN);
		if(key == RESULT_REDRAW)
		{
			g_bt_need_draw = TRUE;
		}
		else if((key != 0) || (result != 0))
		{
			if(key != 0)
			{
				return key;
			}
			else
			{
				return result;
			}
		}
		
		break;
		
	case AP_MSG_SD_OUT:
		g_bt_record_flag = -1;
		g_bt_need_draw = TRUE;
		break;
#endif
		
#if APP_SUPPORT_BTHID==1
	case BT_REMOTE_CONTROL:
		BT_Send_Msg_Up(EV_BT_HID_CONNECT_IND, 0);
		break;
#endif
		
	case BT_DISCONNECT:
		/*Disconnect current device*/
#if APP_SUPPORT_MENU==1
		BT_Disconnect_Device();
#endif
		break;
		
	case BT_DELETE:
		BT_Delete_Device(g_bt_cur_device);
		break;
		
#if APP_SUPPORT_PBAP==1
	case OBEX_SHOW_PHONEBOOK:
		ui_timeout_exit = FALSE;
		phonebook_step = 1;
		pb_index = 0;
		GUI_Display_List(LIST_TYPE_SINGLE_SELECT, 0, phonebook_size, 0, phonebook_list_callback, FALSE);
		hal_HstSendEvent(SYS_EVENT, 0x1988fc02);
		hal_HstSendEvent(SYS_EVENT, call_status);
		
		common_var_init();
		if((g_bt_ops_state & BT_OPS_MUSIC_PLAY) && call_status == BT_CALL_STATUS_NONE)
		{
			COS_Sleep(1000);
			result = RESULT_BT_ACTIVE;
			Avrcp_Send_Key(OPID_PLAY);
			BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
			clean_bt_ops_state(BT_OPS_MUSIC_PLAY);
		}
		break;
		
	case OBEX_SHOW_HISTORY:
		GUI_Display_List(LIST_TYPE_CUSTOM_IMAGE, GUI_IMG_CALL_TYPE, history_size, 0, history_list_callback, FALSE);
		if((g_bt_ops_state & BT_OPS_MUSIC_PLAY) && call_status == BT_CALL_STATUS_NONE)
		{
			Avrcp_Send_Key(OPID_PLAY);
			//g_bt_state = BT_PLAY;
			BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
			clean_bt_ops_state(BT_OPS_MUSIC_PLAY);
		}
		//        if(pb_tid)
		//   	{
		//    		OBEX_Disconnect(pb_tid);
		//    	}
		common_var_init();
		break;
#endif
		
	case BT_DELETEALL:
		BT_Delete_Device(0xFF);
		break;
		
#if APP_SUPPORT_PBAP==1
	case BT_PHONE_BOOK:
		{
			BT_Read_Phone_Book();
		}
		break;
		
	case BT_PB_SEARCH:
		{
			BT_Pb_Search();
		}
		break;
		
	case BT_PB_SYNC:
		{
#ifdef BT_PBAP_DATA_SAVE		
        if(call_status != BT_CALL_STATUS_NONE)
            return ;
	    if(g_a2dp_play)
        {
            set_bt_ops_state(BT_OPS_PBAP_PHONEBOOK_REQ);
            set_bt_ops_state(BT_OPS_MUSIC_PLAY);
             if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
            		g_bt_state = BT_CONNECTED;
					else 
							g_bt_state = BT_IDLE;
            Avrcp_Send_Key(OPID_PAUSE);
            short_play_status = FALSE;
        }   
        else
	clean_bt_ops_state(BT_OPS_PBAP_PHONEBOOK_REQ);		
#endif
	{
				common_var_init();
				//COS_Sleep(100);
				BT_Phone_Book();
			}
		}
		break;
		
	case BT_CALL_HISTORY:
		{
#ifdef BT_PBAP_DATA_SAVE		
        if(call_status != BT_CALL_STATUS_NONE)
            return ;
        if(g_a2dp_play)
        {
            set_bt_ops_state(BT_OPS_PBAP_HISTORY_REQ);
            set_bt_ops_state(BT_OPS_MUSIC_PLAY);
         //   play_start_cnf = TRUE;
             if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
            	g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
            Avrcp_Send_Key(OPID_PAUSE);
            short_play_status = FALSE;
        }       
        else
		clean_bt_ops_state(BT_OPS_PBAP_HISTORY_REQ);	
#endif			
        {
				common_var_init();
				BT_Call_History();
			}
		}
		break;
		
	case OBEX_GET_PHONEBOOK_USE_HF:
		{
			//HF_Get_PhoneBook(0, 0);
			//HF_Set_PhoneBook_UTF8();
			BT_Phone_Book_init();
			if(g_a2dp_play)
			{
				set_bt_ops_state(BT_OPS_MUSIC_PLAY);
		 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)	
            g_bt_state = BT_CONNECTED;
			else 
							g_bt_state = BT_IDLE;
				//    play_start_cnf = TRUE;
				Avrcp_Send_Key(OPID_PAUSE);
				short_play_status = FALSE;
			}
			HF_Set_PhoneBook_PATH(PHONE_BOOK_PHONE, PHONE_BOOK_PHONE_BOOK);
		}
		break;
		
	case OBEX_GET_HISTORY_USE_HF:
		{
			if(g_a2dp_play)
			{
				set_bt_ops_state(BT_OPS_MUSIC_PLAY);
		 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)	
            g_bt_state = BT_CONNECTED;
			else 
							g_bt_state = BT_IDLE;
         //   play_start_cnf = TRUE;
		    if(g_bt_state == BT_CONNECTED)
			{
            Avrcp_Send_Key(OPID_PAUSE);
            short_play_status = FALSE;
			}
        }   
			HF_Set_PhoneBook_PATH(PHONE_BOOK_PHONE, PHONE_BOOK_INCOMING_HISTORY);
		}
		break;
#endif
		
#if APP_SUPPORT_MAP==1
#if APP_SUPPORT_MSG_SEND==1
	case BT_SEND_MSG:
		{
			//send_msg_at();
			BT_Send_Msg(MSG_OUTBOX);
			break;
		}
#endif
		
	case BT_MAP:
		if(call_status != BT_CALL_STATUS_NONE)
		{
			return 0;
		}
		if(g_a2dp_play)
		{
			set_bt_ops_state(BT_OPS_MAP_MSG_REQ);
			set_bt_ops_state(BT_OPS_MUSIC_PLAY);
			//    play_start_cnf = TRUE;
         if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
            g_bt_state = BT_CONNECTED;
			else 
							g_bt_state = BT_IDLE;
			if(g_bt_state == BT_CONNECTED)
			{				
            Avrcp_Send_Key(OPID_PAUSE);
            short_play_status = FALSE;
			}
		}
		else
		{
			clean_bt_ops_state(BT_OPS_MAP_MSG_REQ);
			common_var_init();
			BT_Map();
		}
		break;
		
	case OBEX_SHOW_MSG_LIST:
		GUI_Display_List(LIST_TYPE_CUSTOM_IMAGE, GUI_IMG_CALL_TYPE, msg_size, 0, msg_list_callback, FALSE);
		if(g_bt_ops_state & BT_OPS_MUSIC_PLAY)
		{
			Avrcp_Send_Key(OPID_PLAY);
			BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
			clean_bt_ops_state(BT_OPS_MUSIC_PLAY);
		}
		//       if(mas_tid)
		//   	{
		//   		OBEX_Disconnect(mas_tid);
		//   	}
		//   	common_var_init();
		break;
		
	case OBEX_GET_MSG_USE_HF:
	
		break;
#endif
		
#if APP_SUPPORT_USBSTORAGE_BG==1
	case AP_MSG_USB_DISCONNECT:
		uctls_Close();
		g_bt_need_draw = TRUE;
		break;
#endif
		
#if XDL_APP_SUPPORT_TONE_TIP==1
	case 0xaaaa1111:	// xundon
		if(lowpower_flag_for_tone == 1)
		{
#if APP_SUPPORT_BTBATTERY==1
			if(g_bt_state >= BT_CONNECTED && g_hfp_connect)
			{
				BT_HFPSendBattery();
			}
#endif
			media_PlayInternalAudio(GUI_AUDIO_LOWPOWER, 1, FALSE);
		}
		else if(lowpower_flag_for_tone == 2)
		{
			media_PlayInternalAudio(GUI_AUDIO_CHARGE_COMPLETE, 1, FALSE);
		}
		else if(lowpower_flag_for_tone == 4)
		{
			media_PlayInternalAudio(GUI_AUDIO_BT_POWEROFF, 1, FALSE);
		}
		else if(lowpower_flag_for_tone==5)
		{
			media_PlayInternalAudio(GUI_AUDIO_BT_CONNECT, 1, FALSE);
		}
		else
		{
			media_PlayInternalAudio(GUI_AUDIO_ALARM1, 1, FALSE);
		}
		
		while(GetToneStatus())
		{
			MESSAGE_Sleep_EXT(1);
		}
		
		if(lowpower_flag_for_tone == 4)
		{
			MESSAGE_SetEvent(0xad11dead);
		}
		SetPAVolume(g_pBT_vars->volume);
		hal_HstSendEvent(SYS_EVENT, 0x66662222);
		break;
#endif
		
	case AP_KEY_VOLSUB | AP_KEY_DOWN:
	case AP_KEY_VOLADD | AP_KEY_DOWN:
	case AP_KEY_VOLSUB | AP_KEY_HOLD:
	case AP_KEY_VOLADD | AP_KEY_HOLD:
	case AP_KEY_NEXT   | AP_KEY_HOLD:
	case AP_KEY_PREV   | AP_KEY_HOLD:
		if(!GetToneStatus())
		{
			result = BT_ChangeVol(key);
		}
		break;
		
	default:                                        //???ü???í
#if APP_SUPPORT_LCD==1
		if((key & 0xffff) == EV_BT_MESSAGE_IND)
		{
			key = GUI_DisplayMessage(GUI_STR_BT, (key >> 16), g_bt_msg, GUI_MSG_FLAG_WAIT);
			if(key == 0)
			{
				break;
			}
			else
			{
				return key;
			}
		}
#endif
		
		key = MESSAGE_HandleHotkey(key);
		if(key == RESULT_REDRAW)
		{
			g_bt_need_draw = TRUE;
		}
		else if((key != 0) || (result != 0))
		{
			if(key != 0)
			{
				return key;
			}
			else
			{
				return result;
			}
		}
	}
	
	return result;
}


INT32 BT_Main(void)
{
	INT32 result = 0;
	UINT32 key;
	
	g_bt_need_draw  = TRUE;
	MESSAGE_Initial(g_comval);
	
	if(g_bt_state == BT_CALL && call_status == BT_CALL_STATUS_ACTIVE)
	{
		BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
		hal_HstSendEvent(SYS_EVENT, 0x13062509);
		return RESULT_BT_ACTIVE;
	}
	else if(g_bt_state == BT_PLAY && g_a2dp_play)
	{
		BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
		hal_HstSendEvent(SYS_EVENT, 0x13062509);
		return RESULT_BT_ACTIVE;
	}
	
	hal_HstSendEvent(SYS_EVENT, 0x1306250d);
	
	while(1)
	{
#if 1
		if(event_detected_bt_pending != 0)//handle detectd event when bt was pairing
		{
			hal_HstSendEvent(SYS_EVENT, 0x13062901);
			key =  event_detected_bt_pending;
			event_detected_bt_pending = 0;
			if(key == RESULT_BT_ACTIVE)
			{
				if(g_bt_state >= BT_CONNECTED)
				{
					if(g_bt_state == BT_CONNECTED)
					{
						MESSAGE_SetEvent(EV_BT_CONNECTED_IND);
					}
					else if(g_bt_state == BT_CALL)
					{
						MESSAGE_SetEvent(EV_BT_CALL_IND);
					}
					else if(g_bt_state == BT_PLAY)
					{
						MESSAGE_SetEvent(EV_BT_A2DP_PLAY_IND);
					}
					else if(g_bt_state == BT_FILE)
					{
						MESSAGE_SetEvent(EV_BT_FILE_RSV_IND);
					}
				}
				else
				{
					key = 0;
				}
			}
			
			return key;
		}
		
		if(event_detected_displaying_log != 0)//handle earlier event, usb or T card, when system was displaying log
		{
			hal_HstSendEvent(SYS_EVENT, 0x13063001);
			key =  event_detected_displaying_log;
			event_detected_displaying_log = 0;
			return key;
		}
		
		if(g_tCard_active_after_bt == 1)//handle t-card event when bt was activing
		{
			hal_HstSendEvent(SYS_EVENT, 0x13063101);
			g_tCard_active_after_bt = 0;
			if(gpio_detect_tcard())//check t-card status again
			{
				key = MESSAGE_HandleHotkey(EV_FS_TFLASH_DETECT | (1 << 16));
				return key;
			}
		}
		
		if(g_lineIn_active_after_bt == 1)//handle line-in event when bt was activing
		{
			hal_HstSendEvent(SYS_EVENT, 0x13063201);
			g_lineIn_active_after_bt = 0;
			if(gpio_detect_linein())//check line-in status again
			{
				key = MESSAGE_HandleHotkey(EV_LINE_IN | (1 << 16));
				return key;
			}
		}
#endif
		
		if(g_bt_need_draw && !downLoading)
		{
			BT_DisplayMain();
			g_bt_need_draw = 0;
		}
		
		if(g_bt_state == BT_CONNECTED)
		{
			return 0;
		}
		
		if(g_bt_state != BT_CLOSED && g_bt_connect_pending == 0)
		{
			if(!(g_bt_ops_state & BT_OPS_CLOSED))
			{
				MGR_SetConnectableMode(TRUE);
			}
		}
		
#if APP_SUPPORT_BT_AUTOVISIBLE==1
		if(g_bt_state == BT_IDLE && g_bt_connect_pending == 0)
		{
			BT_Set_Visiable(1);
			g_bt_need_draw = TRUE;
		}
#endif
		
		key = MESSAGE_Wait();
		switch(key)// && !downLoading)
		{
		case AP_MSG_RTC:
#if XDL_APP_SUPPORT_LOWBAT_DETECT == 1//warkey 2.1
			if(Get_Low_Bat_Refresh())
			{
				g_bt_need_draw = TRUE;
				Clean_Low_Bat_Refresh();
			}
#endif
#if APP_SUPPORT_BT_AUTOVISIBLE==0
			if(g_bt_state == BT_SCAN)
			{
				if(--g_bt_timer == 0)
				{
					BT_Set_Visiable(0);
					MESSAGE_Initial(g_comval);
				}
				if(--g_bt_powerof_timer == 0)
				{
					return RESULT_STANDBY;
				}
			}
#endif
			break;
			
		case AP_KEY_NULL:
			//g_bt_need_draw = 2;
			break;
			
		/*case AP_KEY_PLAY|AP_KEY_PRESS:
			hal_HstSendEvent(SYS_EVENT, 0x13063208);
			hal_HstSendEvent(SYS_EVENT, g_bt_state);
			hal_HstSendEvent(SYS_EVENT, g_bt_connect_pending);
			if(g_bt_state < BT_CONNECTED&& g_bt_connect_pending==0)
			{
				// connect last device
				if(g_pBT_vars->default_device >= 0)
				{
					connect_count = 1;
					MGR_SetConnectableMode(FALSE);
		        			bt_connect_timer = COS_SetTimer(20000, bt_auto_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
					BT_Connect_Handsfree(g_pBT_vars->default_device);
				}
			}
			break;*/
		
		//  case  AP_MSG_WAIT_TIMEOUT:
		case BT_ACTIVE:
			//case AP_KEY_PLAY|AP_KEY_DOWN:
			if(g_bt_state == BT_CLOSED)
			{
				result = BT_Active_Bluetooth();
				hal_HstSendEvent(SYS_EVENT, 0x13062503);
				hal_HstSendEvent(SYS_EVENT, result);
				if(result != 0)
				{
					return result;
				}
				g_bt_need_draw = TRUE;
			}
			break;
			
#if APP_SUPPORT_MENU==1
		case BT_INQUIRY_DEVICE:
			{
				is_inquiring = TRUE;
				inquiry_device = (bt_device_t *)COS_Malloc(sizeof(bt_device_t) * MAX_BT_DEVICE_LIST);
				GUI_DisplayMessage(0, GUI_STR_BT_INQUIRING, NULL, GUI_MSG_FLAG_DISPLAY);
				MGR_Inquiry(0x30, 0x9E8B33, 0, MAX_BT_DEVICE_LIST);
			}
			//BT_Send_Msg(MSG_OUTBOX);
			break;
			
		case BT_DEACTIVE:
			if(g_bt_state != BT_CLOSED)
			{
				result = BT_Close_Bluetooth();
				if(result != 0)
				{
					return result;
				}
				g_bt_need_draw = TRUE;
			}
			break;
			
		case UART_BT_VISIBLE:
		case BT_VISIBLE:
			if(g_bt_state == BT_CLOSED)
			{
				MESSAGE_SetEvent(AP_KEY_PLAY | AP_KEY_HOLD);
				result = BT_Active_Bluetooth();
				hal_HstSendEvent(SYS_EVENT, 0x13062504);
				hal_HstSendEvent(SYS_EVENT, result);
				if(result != 0)
				{
					return result;
				}
				g_bt_need_draw = TRUE;
				break;
			}
#endif
			
#if APP_SUPPORT_BT_AUTOVISIBLE==0
		case AP_KEY_PLAY|AP_KEY_HOLD:
			if(g_bt_state == BT_IDLE)
			{
				BT_Set_Visiable(1);
				g_bt_need_draw = TRUE;
				hal_HstSendEvent(SYS_EVENT, 0x13062511);
			}
			break;
#endif
			
#if APP_SUPPORT_MENU==1
		case UART_BT_INVISIBLE:
		case BT_INVISIBLE:
			if(g_bt_state == BT_SCAN)
			{
				BT_Set_Visiable(0);
				g_bt_need_draw = TRUE;
			}
			break;
			
		case BT_DEVICE_LIST:
			if(0 == g_pBT_vars->device_count)
			{
				GUI_DisplayMessage(0, GUI_STR_NODEVICELIST, 0, GUI_MSG_FLAG_KEY);
			}
			else
			{
				GUI_Display_List(LIST_TYPE_CUSTOM_IMAGE, GUI_IMG_BTDEVICELIST , g_pBT_vars->device_count, 0, btdevice_list_callback, FALSE);
				g_bt_device_change = 1;
			}
			break;
			
		case BT_DEVICEOPTION:
			if(AP_Support_MENU())
			{
#if APP_SUPPORT_PBAP==1
				phonebook_step = 1;
				history_step = 1;
#endif
#if APP_SUPPORT_MAP==1
				mas_step = 1;
				mns_step = 1;
				send_msg_step = 1;
#endif
				result = GUI_Display_Menu(GUI_MENU_BTDEVICEOPTION, btoption_menu_callback);
				if(result == RESULT_IGNORE)
				{
					result = 0;
				}
				if(result != 0)
				{
					return result;
				}
				g_bt_device_change = 1;
			}
			break;
			
		case BT_SET_PATH:
			{
				if(BT_ResetDir())
				{
					GUI_Directory(&g_pBT_vars->location, DIRECTORY_ALL, g_comval->langid);
				}
				else
				{
					GUI_DisplayMessage(0,  GUI_STR_MREADERR, 0, GUI_MSG_FLAG_WAIT);
				}
			}
			break;
#endif
			
		default:
			result = BT_HandleKey(key);
			if(result != 0)
			{
				return result;
			}
			if(g_bt_state == BT_CONNECTED)
			{
				return 0;
			}
		}
	};
	
	return result;
}


void BT_Set_DefaultBDAddr(void)
{
	UINT32 rand_lap;
	
	do
	{
		rand_lap = (rand() ^ (rand() << 8)) & 0xffffff;
	}
	while(rand_lap == 0 || ((rand_lap & 0xffff00) == 0x9E8B00)); // check lap valid
	
	g_pBT_vars->local_addr.bytes[0] = rand_lap & 0xff;
	g_pBT_vars->local_addr.bytes[1] = (rand_lap >> 8) & 0xff;
	g_pBT_vars->local_addr.bytes[2] = (rand_lap >> 16) & 0xff;
	
	g_pBT_vars->local_addr.bytes[3] = 0x50;
	g_pBT_vars->local_addr.bytes[4] = 0x58;
	g_pBT_vars->local_addr.bytes[5] = 0x00;
	NVRAMWriteData();
	
	return;
}


INT32 BT_HandleAPPMessage(void)
{
	COS_EVENT ev = {0};
	INT32 result = 0;
	INT8 *str;
	extern BOOL ui_timeout_exit;
	int index = 0;

	app_trace(APP_BT_TRC, "BT_HandleAPPMessage wait event");
	
	while(1)
	{
		COS_WaitEvent(MOD_APP, &ev, COS_WAIT_FOREVER);
		if(ev.nEventId >= EV_BT_BASE && ev.nEventId < EV_BT_END)
		{
			if(ev.nEventId == EV_BT_NOTIFY_IND)
			{
				continue;
			}
			break;
		}
	}
	
	app_trace(APP_BT_TRC, "BT_HandleAPPMessage event %d", ev.nEventId);
	MESSAGE_Initial(g_comval);
	
	if(ev.nEventId == EV_BT_A2DP_PLAY_IND && call_status == BT_CALL_STATUS_INCOMING && g_bt_state != BT_CALL)
	{
		ev.nEventId = EV_BT_CALL_IND;
	}
	hal_HstSendEvent(SYS_EVENT, 0x19880105);
	hal_HstSendEvent(SYS_EVENT, ev.nEventId);
	
	switch(ev.nEventId)
	{
#if APP_SUPPORT_MAP==1
	case EV_BT_SHOW_MSG:
		show_msg();
		result = 0;
		break;
#endif
		
	case EV_BT_HUMAN_VOICE_PLAY:
		break;
		
#if APP_SUPPORT_LCD==1
	case EV_BT_MESSAGE_IND:
		result = GUI_DisplayMessage(GUI_STR_BT, ev.nParam1, g_bt_msg, GUI_MSG_FLAG_WAIT);
		break;
		
	case EV_BT_SHOW_DEVICE:
		ui_timeout_exit = FALSE;
		result = GUI_Display_List(LIST_TYPE_CUSTOM_IMAGE, GUI_IMG_BTDEVICELIST, inquiry_count, 0, inquiry_device_list_callback, TRUE);
		ui_timeout_exit = TRUE;
		MGR_InquiryCancel();
		
		if(0 == result)
		{
			is_paring = TRUE;
			GUI_DisplayMessage(0, GUI_STR_BT_PAIRING, NULL, GUI_MSG_FLAG_DISPLAY);
			MGR_InitBonding(inquiry_device[current_inquiry_device].addr);
		}
		
		if(result == RESULT_IGNORE)
		{
			result = 0;
		}
		/*		for(index = 0; index < inquiry_count; index++)
				{
					memset(&inquiry_device[index], 0, sizeof(bt_device_t));
				}
		*/
		
		if(inquiry_device)
		{
			COS_Free(inquiry_device);
			inquiry_device = NULL;
		}
		break;
#endif
		
	case EV_BT_CONNECTED_IND:
		result = BT_Connect();
		break;
		
	case EV_BT_CALL_IND:
		hal_HstSendEvent(SYS_EVENT, 0x19880100);
#if APP_SUPPORT_PBAP==1
		if(phonebook_step > 1 || history_step > 1)
		{
			//                OBEX_Disconnect(pb_tid);
			downLoading = FALSE;
			common_var_init();
		}
#endif
		result = BT_Call();
		break;
		
	case EV_BT_A2DP_PLAY_IND:
		hal_HstSendEvent(SYS_EVENT, 0x19880103);
		if((call_status!=BT_CALL_STATUS_NONE)&&(call_status!=BT_CALL_STATUS_END))
		{	
			result=RESULT_BT_ACTIVE;
			break;
		}
		if(music_playing==0)
		{
			music_playing=1;
			result = BT_MusicPlay();
			music_playing=0;
		}
		break;
		
	case EV_BT_PLAY_IND:
		if(call_status != BT_CALL_STATUS_NONE)
		{
			result = BT_Call();
		}
		else if(g_bt_sco_handle)
		{
			result = BT_Connect();
		}
		break;
		
#if APP_SUPPORT_OPP==1
	case EV_BT_FILE_RSV_IND:
		if(g_bt_state == BT_CONNECTED)
		{
			is_connected = TRUE;
		}
		result = BT_ReceiveFile();
		if(have_recv_file_size == recv_file_size)
		{
			have_recv_file_size = 0;
			recv_file_size = 0;
		}
		break;
#endif
		
#if APP_SUPPORT_BTHID==1
	case EV_BT_HID_CONNECT_IND:
		result = BT_HID();
		break;
#endif
		
	default:
		hal_HstSendEvent(SYS_EVENT, 0x19880101);
		result = 0;
		break;
	}
	
#if APP_SUPPORT_OPP==1
	hal_HstSendEvent(SYS_EVENT, 0x1988e115);
	hal_HstSendEvent(SYS_EVENT, 0x19880100 + ev.nEventId);
	if(result == RESULT_BT_ACTIVE)
	{
		hal_HstSendEvent(SYS_EVENT, 0x19880090);
	}
	else
	{
		if(have_recv_file_size < recv_file_size)
		{
			result = RESULT_BT_ACTIVE;
			MESSAGE_SetEvent(EV_BT_FILE_RSV_IND);
			hal_HstSendEvent(SYS_EVENT, 0x19880091);
		}
		else if(have_recv_file_size)
		{
			have_recv_file_size = 0;
			recv_file_size = 0;
#if APP_SUPPORT_RGBLCD == 1
			hal_HstSendEvent(SYS_EVENT, 0x19880093);
			str = GUI_GetString(GUI_STR_FILE_COMPLETE);
			GUI_ClearScreen(NULL);
			GUI_DisplayText(0, 24, str);
			GUI_UpdateScreen(NULL);
			COS_Sleep(1000);
			GUI_ClearScreen(NULL);
#elif APP_SUPPORT_LCD == 1
			hal_HstSendEvent(SYS_EVENT, 0x19880093);
			str = GUI_GetString(GUI_STR_FILE_COMPLETE);
			GUI_ClearScreen(NULL);
			GUI_DisplayText(0, 24, str);
			GUI_UpdateScreen(NULL);
			COS_Sleep(1000);
			GUI_ClearScreen(NULL);
#endif
			if(is_connected)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19870003);
			 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)	
                g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
			}
			hal_HstSendEvent(SYS_EVENT, 0xaaaaaaaa);
		}
	}
	
	if((g_bt_ops_state & BT_OPS_MUSIC_PAUSE) && g_bt_ops_state & BT_OPS_FILE_TRANS_COMPLETE)
	{
		hal_HstSendEvent(SYS_EVENT, 0xbbbbbbbb);
		g_bt_state = BT_PLAY;
		clean_bt_ops_state(BT_OPS_MUSIC_PAUSE);
		clean_bt_ops_state(BT_OPS_FILE_TRANS_COMPLETE);
		//Avrcp_Send_Key(OPID_PLAY);
		//Avdtp_Send_Start();
		BT_Send_Msg_Up(EV_BT_PLAY_IND, 0);
		return RESULT_BT_ACTIVE;
	}
#endif
	
	app_trace(APP_BT_TRC, "BT_HandleAPPMessage return %d", result);
	return result;
}


extern int GB_init(void);
UINT8 last_mode=RESULT_NULL;

INT32 BT_Entry(INT32 param)
{
	INT32 result;
	COS_EVENT ev = {0};
	int i = 0;
	
#if defined(CLOSE_BT_SWITCH_MODE)
	SetBtCloseFlag(FALSE);
	if(close_bt_mode != 0)
	{
		hal_HstSendEvent(APP_EVENT, 0x13102801);
		close_bt_mode = 0;
	}
#endif
	
	g_bt_need_draw     = 1;
	g_bt_device_change = 1;
	if(g_bt_state < BT_CONNECTED)
	{
		g_bt_cur_device = -1;
	}
	g_bt_ecc_flag = 0;
	
#if APP_SUPPORT_LCD==1
	BT_Convert_Device_Name(g_btconfig.local_name, g_bt_local_name);
#endif
	/*Entry BT Mode */
	//NVRAMRead(&g_bt_vars,VM_AP_BLUETOOTH,sizeof(g_bt_vars));
	g_pBT_vars = (bt_vars_t*)NVRAMGetData(VM_AP_BLUETOOTH, sizeof(bt_vars_t));
	
#if APP_SUPPORT_PBAP==1
	pb_index = 0;
	NVRAMGetPhoneBookParam(&pb_length, NULL, &phonebook_size);
	pb_addr = NVRAMGetPhoneBookAddress();
	pb_addr += DSM_GetPhoneBookOffset();
#endif
	//    for(i = 0; i < MAX_BT_DEVICE_LIST; i++)
	//	    memset(g_pBT_vars->device_list[i].name, 0, MAX_BT_DEVICE_NAME);
	if(g_pBT_vars->magic != MAGIC_BLUETOOTH)
	{
		memset(g_pBT_vars, 0, sizeof(bt_vars_t));
		g_pBT_vars->magic        = MAGIC_BLUETOOTH;
		g_pBT_vars->device_count = 0;
		g_pBT_vars->default_device = -1;
		g_pBT_vars->volume = 7;
		g_pBT_vars->location.disk = FS_DEV_TYPE_TFLASH;
		g_pBT_vars->eq_type = 0;
		BT_Set_DefaultBDAddr();
	}
	else
	{
		if(g_bt_cur_device < 0 && g_pBT_vars->default_device >= 0 && g_pBT_vars->default_device < g_pBT_vars->device_count)
		{
			g_bt_cur_device = g_pBT_vars->default_device;
		}
	}
	
	// fix bt address
	if(g_bt_state == BT_CLOSED && g_btconfig.fix_address)
	{
		g_pBT_vars->local_addr.bytes[0] = g_btconfig.bt_address[0];
		g_pBT_vars->local_addr.bytes[1] = g_btconfig.bt_address[1];
		g_pBT_vars->local_addr.bytes[2] = g_btconfig.bt_address[2];
		
		g_pBT_vars->local_addr.bytes[3] = g_btconfig.bt_address[3];
		g_pBT_vars->local_addr.bytes[4] = g_btconfig.bt_address[4];
		g_pBT_vars->local_addr.bytes[5] = g_btconfig.bt_address[5];
	}
	
#if APP_SUPPORT_INDEPEND_VOLUMNE==0
	g_pBT_vars->volume = g_comval->Volume;
#endif
	gpio_SetMute(TRUE);
	SetInternalPAVolume(g_pBT_vars->volume);
	
#if APP_SUPPORT_BT_RECORD==1
	BT_GeneRecFile(TRUE);
#endif
	#if 1//XDL_APP_SUPPORT_TONE_TIP==1
			hal_HstSendEvent(SYS_EVENT, 0x11117723);

			media_PlayInternalAudio(GUI_AUDIO_BT_POWERON, 1, FALSE);
			while(GetToneStatus())
			{
				MESSAGE_Sleep_EXT(1);
			}
#endif

	hal_HstSendEvent(SYS_EVENT, 0x13062555);
	clean_bt_ops_state(BT_OPS_BT_MODE_EXIT);
	if(param == -1) // enter from main menu
	{
#if !defined(CLOSE_BT_SWITCH_MODE)		
		if(last_mode!=RESULT_NULL)
		{
			hal_HstSendEvent(APP_EVENT, 0x14070201);
			last_mode=RESULT_NULL;
		}
#endif
		if(g_bt_state == BT_CLOSED)
		{
			LED_SetPattern(GUI_LED_BT_CLOSED + BT_IDLE, LED_LOOP_INFINITE);
		}
		else
		{
			LED_SetPattern(GUI_LED_BT_CLOSED + g_bt_state, LED_LOOP_INFINITE);
		}
		if(g_bt_state == BT_CLOSED)
		{
			hal_HstSendEvent(APP_EVENT, 0x13090200);
			result = BT_Active_Bluetooth();
		}
		else if(g_bt_state >= BT_CONNECTED)
		{
			//已经连接的情况下不需要提示音也不需要回连
			hal_HstSendEvent(APP_EVENT, 0x13090201);
		}
		else
		{
			hal_HstSendEvent(APP_EVENT, 0x13090202);
			gpio_SetMute(TRUE);//warkey 2.0
			if(g_bt_cur_device >= 0)
			{
				MGR_SetConnectableMode(FALSE);
				connect_count = 1;
				bt_connect_timer = COS_SetTimer(3000, bt_auto_con_timeout, NULL, COS_TIMER_MODE_SINGLE);
			}
		}
		
		hal_HstSendEvent(SYS_EVENT, 0x13062505);
		hal_HstSendEvent(SYS_EVENT, result);
		hal_HstSendEvent(SYS_EVENT, g_bt_state);
		hal_HstSendEvent(SYS_EVENT, g_hfp_connect);
		hal_HstSendEvent(SYS_EVENT, g_avdtp_connect);
		hal_HstSendEvent(SYS_EVENT, g_avrcp_connect);
		//hal_HstSendEvent(SYS_EVENT, g_hid_connect);
		hal_HstSendEvent(SYS_EVENT, g_sco_play);
		hal_HstSendEvent(SYS_EVENT, g_bt_sco_handle);
		//hal_HstSendEvent(SYS_EVENT, is_play_music);
		hal_HstSendEvent(SYS_EVENT, media_GetInternalStatus());
		support_avrcp_status = FALSE;
		g_avrcp_status = AVRCP_STATUS_ERROR;
		set_bt_ops_state(BT_OPS_AVRCP_STATUS_ERROR);
		
		while(1)
		{
			hal_HstSendEvent(SYS_EVENT, 0x13062510);
			//hal_HstSendEvent(SYS_EVENT, play_falied);
			if(g_bt_ops_state & BT_OPS_MUSIC_PLAY_FAILED)
			{
				hal_HstSendEvent(SYS_EVENT, 0x1988c008);
				//    hal_HstSendEvent(SYS_EVENT, play_start_cnf);
				//g_a2dp_play = 0;
				//    if(play_start_cnf)
				{
					Avrcp_Send_Key(OPID_PLAY);
					//BT_Send_Msg_Up(EV_BT_A2DP_PLAY_IND, 0);
					clean_bt_ops_state(BT_OPS_MUSIC_PLAY_FAILED);
					short_play_status = TRUE;
					//         play_start_cnf = FALSE;
				}
				//is_play_music = FALSE;
			}
#if defined(CLOSE_BT_SWITCH_MODE)
			if((close_bt_mode != 0) && (g_hfp_connect == 0 || g_avdtp_connect == 0))
			{
				hal_HstSendEvent(APP_EVENT, 0x13102804);
				hal_HstSendEvent(APP_EVENT, g_hfp_connect);
				hal_HstSendEvent(APP_EVENT, g_avdtp_connect);
				hal_HstSendEvent(APP_EVENT, g_avrcp_connect);
				BT_Close_Bluetooth();
				break;
			}
#endif
			
#if APP_SUPPORT_BTHID==1
			if(g_bt_state == BT_IDLE && (g_hfp_connect || g_avdtp_connect || g_avrcp_connect || g_hid_connect))
#else
			if(g_bt_state == BT_IDLE && (g_hfp_connect || g_avdtp_connect || g_avrcp_connect))
#endif
			{
				hal_HstSendEvent(SYS_EVENT, 0x19870004);
				g_bt_state = BT_CONNECTED;
			}
#if APP_SUPPORT_BTHID==1
			else if(g_bt_state == BT_CONNECTED && (g_hfp_connect == 0 && g_avdtp_connect == 0 && g_avrcp_connect == 0 && g_hid_connect == 0))
#else
			else if(g_bt_state == BT_CONNECTED && (g_hfp_connect == 0 && g_avdtp_connect == 0 && g_avrcp_connect == 0))
#endif
			{
				g_bt_state = BT_IDLE;
				g_bt_ops_state = 0;
			}
			
			if(g_bt_state == BT_CONNECTED)
			{
#if APP_SUPPORT_OPP==1
				if(g_bt_ops_state & BT_OPS_FILE_TRANS_PAUSE)
				{
					result = BT_ReceiveFile();
				}
				else
#endif
				{
					result = BT_Connect();  // display connect screen
				}
			}
			else
			{
				result = BT_Main();  // display device list
			}
			
			hal_HstSendEvent(SYS_EVENT, 0x13062601);
			hal_HstSendEvent(SYS_EVENT, result);
			if(result != 0 && result != RESULT_BT && result != RESULT_BT_ACTIVE)
			{
				break;
			}
			
			while(result == RESULT_BT_ACTIVE)
			{
            	  if(g_hfp_connect || g_avdtp_connect ||g_avrcp_connect
#if APP_SUPPORT_BTHID==1
                && g_hid_connect==0
#endif
		  )
                result = BT_HandleAPPMessage();
		else
			result = 0;
			}
			
			if(result != 0)
			{
				break;
			}
		}
	}
	else // enter from bt event, show message
	{
#if !defined(CLOSE_BT_SWITCH_MODE)		
		hal_HstSendEvent(APP_EVENT, 0x14070202);
		last_mode=param;
#endif
		do
		{
			result = BT_HandleAPPMessage();
			hal_HstSendEvent(SYS_EVENT, 0x19880092);
		}
		while(result == RESULT_BT_ACTIVE);
		
		if(result == 0 || result == RESULT_MAIN)
		{
			result = param;
		}
	}
	
#if defined(CLOSE_BT_SWITCH_MODE)
	if(bt_connect_timer)
	{
		COS_KillTimer(bt_connect_timer);
		bt_connect_timer = 0;
	}
	if(g_bt_state >= BT_CONNECTED)
	{
		BT_Disconnect_Device();
		COS_Sleep(1500);
	}
	if(g_bt_state > BT_CLOSED)
	{
		BT_Close_Bluetooth();
	}
	g_bt_state = BT_CLOSED;
	g_hfp_connect = 0;
	g_avdtp_connect = 0;
	g_avrcp_connect = 0;
	SetBtCloseFlag(TRUE);
	if(close_bt_mode != 0)
	{
		hal_HstSendEvent(APP_EVENT, 0x13102802);
		result = close_bt_mode;
		close_bt_mode = 0;
	}
	hal_HstSendEvent(APP_EVENT, 0x131107dd);
	hal_HstSendEvent(APP_EVENT, result);
#endif
	if(g_bt_state > BT_CLOSED)
	{
		BT_Set_Visiable(0);
		MGR_SetConnectableMode(FALSE);
	}
	hal_HstSendEvent(SYS_EVENT, 0x19880800);
	//NVRAMWrite(&g_bt_vars,VM_AP_BLUETOOTH,sizeof(g_bt_vars));
	NVRAMWriteData();
	g_bt_record_flag = -1;
	
	return result;
}


static INT32 find_device(t_MGR_DeviceEntry *dbEntry)
{
	int i = 0;
	
	for(i = 0; i < inquiry_count; i++)
	{
		if(!strcmp(dbEntry->bdAddress.bytes, inquiry_device[i].addr.bytes))
		{
			return -1;
		}
	}
	
	return 0;
}

static void add_device(t_MGR_DeviceEntry *dbEntry)
{
	INT32 unicode_len = 0;
	char name_tmp[MAX_BT_DEVICE_NAME * 2] = {0};
	
	int i = inquiry_count;
	memcpy(inquiry_device[i].addr.bytes, dbEntry->bdAddress.bytes, SIZE_OF_BDADDR);
	memcpy(inquiry_device[i].link_key, dbEntry->linkKey, SIZE_OF_LINKKEY);
	inquiry_device[i].key_type = dbEntry->linkKeyType;
	inquiry_device[i].cod      = dbEntry->classOfDevice;
	inquiry_device[i].key_type = dbEntry->linkKeyType;
	unicode_len = UTF8_to_Unicode(name_tmp, dbEntry->name);
	if(unicode_len > 0)
	{
		memset(&name_tmp[unicode_len], 0, MAX_BT_DEVICE_NAME * 2 - unicode_len);
		gui_unicode2local(name_tmp, unicode_len);
		memcpy(inquiry_device[i].name, name_tmp, MAX_BT_DEVICE_NAME);
	}
	else
	{
		memcpy(inquiry_device[i].name, dbEntry->name, MAX_BT_DEVICE_NAME);
	}
	inquiry_count++;
	
	return;
}


extern UINT8 g_a2dp_play;
extern BOOL download_abort;
void BT_HandleManagerMessage(u_int16 msg_id, void* msg_data)
{
	//hal_HstSendEvent(SYS_EVENT,0x10200010);
	//hal_HstSendEvent(SYS_EVENT,msg_id);
	//hal_HstSendEvent(SYS_EVENT,msg_data);
    app_trace(APP_BT_TRC, "BT_HandleManagerMessage msg_id:%d", msg_id);
	switch(msg_id)
	{
	case MGR_ACL_CONNECT_IND:
		{
			mgr_acl_link_ind_t *pMsg = (mgr_acl_link_ind_t*)msg_data;
		app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_ACL_CONNECT_IND");	
			if(g_bt_state < BT_CONNECTED)
			{
				MGR_ConnectAcceptRsp(RDABT_NOERROR, pMsg->address);
				MGR_SetConnectableMode(FALSE);
				MGR_SetDiscoverableMode(FALSE);
			}
			else
			{
				MGR_ConnectAcceptRsp(RDABT_NORESOURCES, pMsg->address);
			}
		}
		break;
		
	case MGR_PIN_CODE_IND:
		{
			t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg_data;
			app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_PIN_CODE_IND");	
			MGR_PINResponse(dbEntry->bdAddress, 1, g_btconfig.password, strlen(g_btconfig.password));
		}
		break;
		
	case MGR_LINK_KEY_IND:
		{
			int index;
			t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg_data;
			app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_LINK_KEY_IND");	
			index = BT_Find_Device(dbEntry->bdAddress);
			if(index < 0)
			{
				MGR_LinkKeyResponse(dbEntry->bdAddress, FALSE, NULL, 0);
			}
			else
			{
				BT_Update_Device_Active(index);
				MGR_LinkKeyResponse(dbEntry->bdAddress, TRUE, g_pBT_vars->device_list[index].link_key, g_pBT_vars->device_list[index].key_type);
			}
		}
		break;
		
	case MGR_DISCOVERY_RESULT_IND:
		{
			t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg_data;
			int index;
			if(dbEntry == NULL)
			{
				break;
			}
			index = find_device(dbEntry);
			hal_HstSendEvent(SYS_EVENT, 0x25025020);
			hal_HstSendEvent(SYS_EVENT, index);
			hal_HstSendEvent(SYS_EVENT, inquiry_count);
			if(index == 0 && inquiry_count < MAX_BT_DEVICE_LIST)
			{
				add_device(dbEntry);
				hal_HstSendEvent(SYS_EVENT, 0x25025021);
				if(inquiry_count == 1)
				{
					BT_Send_Msg_Up(EV_BT_SHOW_DEVICE, 0);
				}
				else
				{
					GUI_Add_item();
				}
			}
		}
		break;
		
	case MGR_DISCOVERY_CNF:
		if(inquiry_count == 0 && is_inquiring)
		{
			if(inquiry_device)
			{
				COS_Free(inquiry_device);
				inquiry_device = NULL;
			}
			BT_Send_Msg_Up(EV_BT_MESSAGE_IND , GUI_STR_BT_INQUIRY_NO_DEVICE);
		}
		inquiry_count = 0;
		is_inquiring = FALSE;
		break;
		
	case MGR_BONDING_CNF:
		{
			mgr_bond_cnf_msg_t *msg = (mgr_bond_cnf_msg_t*)msg_data;
			app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_BONDING_CNF");	
			if(msg->result == RDABT_NOERROR)
			{
				int pos;
				t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg->device;
				int index = BT_Add_Device(dbEntry);
				//NVRAMWrite(&g_bt_vars,VM_AP_BLUETOOTH,sizeof(g_bt_vars));
				NVRAMWriteData();
#if APP_SUPPORT_PBAP==1
				if(phonebook_step == 0)
#endif
				{
					BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BTPAIREDSUCCESS);
				}
			}
			else
			{
				BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BTPAIREDFAILED);
			}
			is_paring = FALSE;
			//BT_Send_Msg_Up(BT_INVISIBLE, 0);
			if(g_bt_state == BT_SCAN)
			{
				MGR_SetDiscoverableMode(0);
				MGR_SetPairingMode(FALSE);
				g_bt_state = BT_IDLE;
			}
		}
		break;
		
	case MGR_OOB_DATA_IND:
		{
			t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg_data;
			app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_OOB_DATA_IND");	
			MGR_OOBDataResponse(dbEntry->bdAddress, FALSE, NULL, NULL);
		}
		break;
		
	case MGR_CONFIRM_IND:
		{
			mgr_ssp_ind_msg_t *msg = (mgr_ssp_ind_msg_t*)msg_data;
			t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg->device;
			app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_CONFIRM_IND");	
			MGR_ConfirmResponse(dbEntry->bdAddress, TRUE);
		}
		break;
		
	case MGR_PASSKEY_IND:
		{
			mgr_ssp_ind_msg_t *msg = (mgr_ssp_ind_msg_t*)msg_data;
			t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)msg->device;
			app_trace(APP_BT_TRC, "BT_HandleManagerMessage MGR_PASSKEY_IND");	
			MGR_PasskeyResponse(dbEntry->bdAddress, TRUE, 0x1234);
		}
		break;
		
#ifdef BT_SSP_VOC_SUPPORT
	case MGR_LOCALKEY_IND:
		{
			app_trace(APP_BT_TRC, "Receive MGR_LOCALKEY_IND");
			if(g_pBT_vars->local_key[0] == 0) // never generate local key yet
			{
				int i;
				UINT32 *pKey = g_pBT_vars->local_key;
				for(i = 0; i < 6; i++)
				{
					pKey[i] = rand() ^ (rand() << 16);
				}
				pKey[5] &= 0x7fffffff;
				if(pKey[0] == 0) // ensure local_key[0] != 0
				{
					pKey[0] = pKey[1] ^ pKey[2];
				}
				
				vpp_ECC_Generate_Local_Key(pKey);
				g_bt_ecc_flag = 1;
			}
			else
			{
				MGR_Write_DHKey(TRUE, g_pBT_vars->public_key[0], g_pBT_vars->public_key[1]);
			}
		}
		break;
		
	case MGR_PEERKEY_IND:
		app_trace(APP_BT_TRC, "Receive MGR_PEERKEY_IND");
		vpp_ECC_Generate_DHKey(g_pBT_vars->local_key, (UINT32*)msg_data);
		//hal_DbgAssert("MGR_PEERKEY_IND");
		g_bt_ecc_flag = 2;
	//break;
	
	case MGR_DHKEY_IND:
		//app_trace(APP_BT_TRC, "Receive MGR_DHKEY_IND");
		{
			INT res;
			INT32   dh_key[SIZE_OF_DHKEY / 4];
			
			res = vpp_ECC_GetKeyResult(dh_key, SIZE_OF_DHKEY / 4);
			while(res != 0)
			{
				COS_Sleep(100);
				res = vpp_ECC_GetKeyResult(dh_key, SIZE_OF_DHKEY / 4);
			}
			MGR_Write_DHKey(FALSE, dh_key, NULL);
		}
		break;
#endif
		
	case MGR_SCO_CONNECT_IND:
		{
			mgr_sco_link_t *msg = (mgr_sco_link_t*)msg_data;
			hal_HstSendEvent(SYS_EVENT, 0x1988cc02);
			hal_HstSendEvent(SYS_EVENT, msg->result);
			hal_HstSendEvent(SYS_EVENT, HF_Get_Current_Profile());
			hal_HstSendEvent(SYS_EVENT, g_bt_sco_handle);
			if(g_current_module != FUN_BT)
			{
				g_prev_module = g_current_module;
				hal_HstSendEvent(APP_EVENT, 0x13120604);
				set_bt_ops_state(BT_OPS_MODULE_CHANGE);
			}
			//HF_Get_CallStatus(&call_status);
			//gpio_SetMute(FALSE);
			//解决切换声道时产生的PA声
			SetPAVolume(g_pBT_vars->volume);//howe 2.1
			extern UINT8 bt_call_wait_volume;
			bt_call_wait_volume = 2;
			app_trace(APP_RECORD_TRC, ">>>>>>>MGR_SCO_CONNECT_IND!!");
			if(g_bt_state != BT_CALL && call_status != 0 && call_status != BT_CALL_STATUS_END)
			{
				BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
			}
			
			if(msg->result == RDABT_NOERROR)
			{
				g_bt_sco_handle = msg->sco_handle;
				if(HF_Get_Current_Profile() == ACTIVE_PROFILE_HEADSET || call_status != BT_CALL_STATUS_NONE)
				{
					//call_status = BT_CALL_STATUS_ACTIVE;
					hal_HstSendEvent(SYS_EVENT, 0x19880070);
					hal_HstSendEvent(SYS_EVENT, g_sco_play);
					hal_HstSendEvent(SYS_EVENT, g_bt_sco_handle);
					BT_Send_Msg_Up(EV_BT_CALL_IND, 0);
				}
				else
				{
					BT_Send_Msg_Up(EV_BT_PLAY_IND, 0);
				}
			}
		}
		break;
		
	case MGR_SCO_DISCONNECT_IND:
		{
			//解决切换声道时产生的PA声
			SetPAVolume(0);//howe 2.1 //gpio_SetMute(TRUE);
			COS_Sleep(5);
			g_bt_sco_handle = 0;
			if(HF_Get_Current_Profile() == ACTIVE_PROFILE_HEADSET)
			{
				hal_HstSendEvent(SYS_EVENT, 0x19870005);
				call_status = BT_CALL_STATUS_NONE;
				 if(g_hfp_connect || g_avdtp_connect || g_avrcp_connect
#if APP_SUPPORT_BTHID==1
	                || g_hid_connect
#endif	
			)
				g_bt_state = BT_CONNECTED;
				else 
							g_bt_state = BT_IDLE;
			}
			hal_HstSendEvent(SYS_EVENT, 0x19880030);
			hal_HstSendEvent(SYS_EVENT, call_status);
			hal_HstSendEvent(SYS_EVENT, g_sco_play);
			hal_HstSendEvent(SYS_EVENT, g_current_module);
			if(g_sco_play)
			{
#if APP_SUPPORT_BT_RECORD==1
				if(g_bt_record_flag == 1)
				{
					mediaSendCommand(MC_STOP, 0);
					mediaSendCommand(MC_CLOSE, 0);
					g_bt_record_flag = 0;
					BT_GeneRecFile(FALSE); // generate file for next record
				}
				else
#endif
					if(g_current_module == FUN_BT)
					{
						mediaSendCommand(MC_STOP, 0);
					}
				g_sco_play = 0;
			}
			
			if((call_status != BT_CALL_STATUS_END) && (call_status != BT_CALL_STATUS_NONE))
			{
				return ;
			}
		}
		break;
		
	case MGR_SCO_DATA_IND:
		if(0)
		{
			COS_EVENT ev = {0};
			ev.nEventId = MSG_MMC_AUDIODEC_SCO;
			ev.nParam1  = msg_data;
			COS_SendEvent(MOD_MED,  &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		}
		break;
		
	case MGR_READ_REGISTER_CNF:
		hal_HstSendEvent(SYS_EVENT, msg_data);
		break;
	}
	
	return;
}


void BT_Callback(rdabt_msg_t *message)
{
	u_int16 msg_id = message->msg_id;
#if  XDL_APP_BT_MSG_PRINTF==1//howe 2.1
	msg_Printf(msg_id);
#endif /* #if XDL_APP_MSG_PRINFT */
	if((msg_id >= MGR_CHANGE_LOCAL_NAME && msg_id <= SDP_SERVICE_SEARCH_CNF))
	{
		BT_HandleManagerMessage(msg_id, message->data);
	}
	else if((msg_id >= HFG_ACTIVATE_CNF && msg_id <= HFG_MISSED_CALL_HISTORY_IND))
	{
		BT_HandleHFPMessage(msg_id, message->data);
	}
	else if((msg_id >= A2DP_SIGNAL_CONNECT_CNF && msg_id <= AVRCP_PLAYER_STATUS_IND))
	{
		BT_HandleA2DPMessage(msg_id, message->data);
	}
#if APP_SUPPORT_OPP==1 || APP_SUPPORT_PBAP==1
	else if(message->msg_id >= OBEX_CLI_PUSH_SERVER_CNF &&  message->msg_id <= OBEX_SERVER_AUTHORIZE_IND)
	{
		BT_HandleObexMessage(msg_id, message->data);
	}
#endif
#if APP_SUPPORT_BTHID==1
	else if(message->msg_id >= HID_CONNECT_IND &&  message->msg_id <= HID_DISCONNECT_CNF)
	{
		BT_HandleHIDMessage(msg_id, message->data);
	}
#endif
#if APP_SUPPORT_SPP == 1
	else if(message->msg_id >= SPP_CONNECT_IND && message->msg_id <= SPP_DISCONNECT_CNF)
	{
		SPPMessageHandle(msg_id, message->data);
	}
#endif
	BT_Execute();
	
	return;
}

#endif
