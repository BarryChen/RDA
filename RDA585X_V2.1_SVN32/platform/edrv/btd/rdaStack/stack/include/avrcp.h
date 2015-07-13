/***********************************************************************
 *
 * MODULE NAME:    avrcp.h
 * PROJECT CODE:   host side stack
 * DESCRIPTION:    avrcp platform interface
 * MAINTAINER:     Mark Donnelly
 * CREATION DATE:  08 March 2000
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2001 RDA Microelectronics
 *     All rights reserved.
 *
 * REVISION HISTORY:
 * 
 *
 * ISSUES:
 * 
 ***********************************************************************/
#ifndef _RDABT_AVRCP_H_
#define _RDABT_AVRCP_H_



/*! 
	@brief Operation ID, used to identify operation. See table 9.21 AV/C Panel
	Subunit spec. 1.1 #
*/
typedef enum
{
	OPID_SELECT				= (0x0),
	OPID_UP,
	OPID_DOWN,
	OPID_LEFT,
	OPID_RIGHT,
	OPID_RIGHT_UP,
	IPID_RIGHT_DOWN,
	OPID_LEFT_UP,
	OPID_LEFT_DOWN,
	OPID_ROOT_MENU,
	OPID_SETUP_MENU,
	OPID_CONTENTS_MENU,
	OPID_FAVOURITE_MENU,
	OPID_EXIT,
	/* 0x0e to 0x1f Reserved */
	OPID_0					= (0x20),
	OPID_1,
	OPID_2,
	OPID_3,
	OPID_4,
	OPID_5,
	OPID_6,
	OPID_7,
	OPID_8,
	OPID_9,
	OPID_DOT,
	OPID_ENTER,
	OPID_CLEAR,
	/* 0x2d - 0x2f Reserved */
	OPID_CHANNEL_UP			= (0x30),
	OPID_CHANNEL_DOWN,
	OPID_SOUND_SELECT,
	OPID_INPUT_SELECT,
	OPID_DISPLAY_INFORMATION,
	OPID_HELP,
	OPID_PAGE_UP,
	OPID_PAGE_DOWN,
	/* 0x39 - 0x3f Reserved */
	OPID_POWER				= (0x40),
	OPID_VOLUME_UP,
	OPID_VOLUME_DOWN,
	OPID_MUTE,
	OPID_PLAY,
	OPID_STOP,
	OPID_PAUSE,
	OPID_RECORD,
	OPID_REWIND,
	OPID_FAST_FORWARD,
	OPID_EJECT,
	OPID_FORWARD,
	OPID_BACKWARD,
	/* 0x4d - 0x4f Reserved */
	OPID_ANGLE				= (0x50),
	OPID_SUBPICTURE,
	/* 0x52 - 0x70 Reserved */
	OPID_F1					= (0x71),
	OPID_F2,
	OPID_F3,
	OPID_F4,
	OPID_F5,
	OPID_VENDOR_UNIQUE		= (0x7e)
	/* Ox7f Reserved */
} t_AVC_OPERATION_ID; 

typedef enum
{
    AVRCP_PDU_GETCAPABILITIES                                     = 0X10, 
    AVRCP_PDU_LISTPLAYERAPPLICATIONSETTINGATTRIBUTES              = 0X11, 
    AVRCP_PDU_LISTPLAYERAPPLICATIONSETTINGVALUES                  = 0X12, 
    AVRCP_PDU_GETCURRENTPLAYERAPPLICATIONSETTINGVALUE             = 0X13, 
    AVRCP_PDU_SETPLAYERAPPLICATIONSETTINGVALUE                    = 0X14, 
    AVRCP_PDU_GETPLAYERAPPLICATIONSETTINGATTRIBUTETEXT            = 0X15, 
    AVRCP_PDU_GETPLAYERAPPLICATIONSETTINGVALUETEXT                = 0X16, 
    AVRCP_PDU_INFORMDISPLAYABLECHARACTERSET                       = 0X17, 
    AVRCP_PDU_INFORMBATTERYSTATUSOFCT                             = 0X18, 
    AVRCP_PDU_GETELEMENTATTRIBUTES                                = 0X20, 
    AVRCP_PDU_GETPLAYSTATUS                                       = 0X30, 
    AVRCP_PDU_REGISTERNOTIFICATION                                = 0X31, 
    AVRCP_PDU_REQUESTCONTINUINGRESPONSE                           = 0X40, 
    AVRCP_PDU_ABORTCONTINUINGRESPONSE                             = 0X41, 
} t_AVRCP_PDU_ID;

typedef enum
{
    AVRCP_STATUS_STOPPED,
    AVRCP_STATUS_PLAYING,
    AVRCP_STATUS_PAUSED,
    AVRCP_STATUS_FWD_SEEK,
    AVRCP_STATUS_REV_SEEK,
    AVRCP_STATUS_ERROR = 0xff
} t_AVRCP_PLAY_STATUS;

typedef enum
{
    AVRCP_EVENT_PLAYBACK_STATUS_CHANGED=(0x01),
    AVRCP_EVENT_TRACK_CHANGED=(0x02) ,
    AVRCP_EVENT_TRACK_REACHED_END=(0x03),
    AVRCP_EVENT_TRACK_REACHED_START=(0x04),
    AVRCP_EVENT_PLAYBACK_POS_CHANGED=(0x05),
    AVRCP_EVENT_BATT_STATUS_CHANGED=(0x06),
    AVRCP_EVENT_SYSTEM_STATUS_CHANGED=(0x07),
    AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED=(0x08),
} t_AVRCP_EVENT;

typedef enum
{
	AVRCP_DEVICE_NONE,
	AVRCP_TARGET,
	AVRCP_CONTROLLER,
	AVRCP_TARGET_AND_CONTROLLER
} t_AVRCP_DEVICE_TYPE;


typedef enum 
{
	AVRCP_INITIALISING,
	AVRCP_READY,
	AVRCP_CONNECTING,
	AVRCP_CONNECTED
} t_AVRCP_STATE;

typedef enum
{
    AVRCP_ATTR_TITLE = 1,
    AVRCP_ATTR_ARTIST,
    AVRCP_ATTR_ALBUM,
    AVRCP_ATTR_TRACK,
    AVRCP_ATTR_TOTAL,
    AVRCP_ATTR_GENRE,
    AVRCP_ATTR_POSITION,
} t_AVRCP_MEDIA_ATTRIBUTE;

typedef struct
{
    u_int8   seq_id;
    u_int8   c_type;
    u_int8   subunit_type;
    u_int8   subunit_id;
    u_int16  data_len;
    u_int16  profile_id;
    u_int8   frame_data[20];
} t_avrcp_cmd_frame_ind;

typedef struct
{
    u_int8 connect_id;
    u_int16 result;
    t_bdaddr device_addr;
}t_avrcp_connect_inf;


typedef struct 
{
    u_int8 connect_id;
    u_int8 status;
    u_int32 totalTime;
    u_int32 currentTime;
}t_avrcp_status_ind;

typedef struct 
{
    u_int8 connect_id;
    u_int8 pdu_id;
    u_int16 length;
    u_int8 *param;
}t_avrcp_data_ind;


#ifdef __cplusplus
extern "C" {
#endif

APIDECL1 t_api APIDECL2 Avrcp_RegisterService(u_int8 role); // t_AVRCP_DEVICE_TYPE
APIDECL1 t_api APIDECL2 Avrcp_Set_State(t_AVRCP_STATE state);
APIDECL1 t_api APIDECL2 Avrcp_Connect_Req(t_bdaddr bdaddr);
APIDECL1 t_api APIDECL2 Avrcp_Disconnect_Req(void);

APIDECL1 t_api APIDECL2 Avrcp_Get_UnitInfo(u_int8 unit_type);
APIDECL1 t_api APIDECL2 Avrcp_Send_Key(u_int8 key);
APIDECL1 t_api APIDECL2 Avrcp_Send_Press(u_int8 key);
APIDECL1 t_api APIDECL2 Avrcp_Send_Release(u_int8 key);
APIDECL1 t_api APIDECL2 Avrcp_Update_Player_Status(void);
APIDECL1 t_api APIDECL2 Avrcp_Get_Media_Attribute(void);

APIDECL1 t_api APIDECL2 Avrcp_Metadata_Transfer(u_int8 pdu_id, u_int8 *param, u_int16 param_length);

APIDECL1 t_api APIDECL2 Avrcp_Send_Cmd_Response(u_int8 seq_id, u_int16 profile_id, u_int8 result, u_int8 subunit_type, u_int8 subunit_id, u_int8 frame_data);

void Get_Avrcp_Status(void);

#define RDABT_Avrcp_Set_State( state)       Avrcp_Set_State( state)     
#define RDABT_Avrcp_Connect_Req( bdaddr)    Avrcp_Connect_Req( bdaddr)  
#define RDABT_Avrcp_Disconnect_Req          Avrcp_Disconnect_Req        
#define RDABT_Avrcp_Send_Key(key)           Avrcp_Send_Key(key)         
#define RDABT_Avrcp_Send_Press(key)         Avrcp_Send_Press(key)       
#define RDABT_Avrcp_Send_Release(key)       Avrcp_Send_Release(key)     

#ifdef __cplusplus
}
#endif

#endif //_RDABT_AVRCP_H_

