/***********************************************************************
 *
 * MODULE NAME:    avdtp.h
 * DESCRIPTION:    a2dp profile platform interface
 * MAINTAINER:      Zhu Jianguo
 *
 * 
 * LICENSE:
 *     This source code is copyright (c) 2011 RDA Ltd.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 * 
 *
 * ISSUES:
 * 
 ***********************************************************************/
#ifndef RDABT_A2DP_H_
#define RDABT_A2DP_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Signalling header message type */
enum
{
	AVDTP_MESSAGE_TYPE_COMMAND = (0x0),
	AVDTP_MESSAGE_TYPE_ACCEPT = (0x02),
	AVDTP_MESSAGE_TYPE_REJECT = (0x03)
};

/* AVDTP signals */
typedef enum
{	
	AVDTP_NULL = (0x00),
	AVDTP_DISCOVER,
	AVDTP_GET_CAPABILITIES,
	AVDTP_SET_CONFIGURATION,
	AVDTP_GET_CONFIGURATION,
	AVDTP_RECONFIGURE,
	AVDTP_OPEN,
	AVDTP_START,
	AVDTP_CLOSE,
	AVDTP_SUSPEND,
	AVDTP_ABORT,
	AVDTP_SECURITY_CONTROL
} t_AVDTP_SIGNAL_ID;


/*!
	@brief Stream End Point role. 
*/
typedef enum
{
	SEP_SOURCE,				/*!< Stream Endpoint is the Source.*/
	SEP_SINK				/*!< Stream Endpoint is the Sink.*/
} t_SEP_ROLE; 



/*! 
	@brief The type of the SEP. This identifies the codec type being registered
	or used.
*/
typedef enum
{
	/*! Invalid SEP value. */
	A2DP_SEP_INVALID = 0,	
	/*! The SEP supports the SBC codec (support for this codec is
	  mandatory). */
	A2DP_SBC,				
	/*! The SEP supports the MPEG-1,2 AUdio codec. */
	A2DP_MPEG_AUDIO,		
	/*! The SEP supports the MPEG-2,4 AAC codec. */
	A2DP_MPEG_AAC,			
	/*! The SEP supports a codec from the ATRAC family. */
	A2DP_ATRAC,				
	/*! The SEP supports a vendor defined codec. */
	A2DP_VENDOR				
} t_A2DP_SEP_TYPE;

#ifndef MAX_NUM_REMOTE_SEIDS
#define MAX_NUM_REMOTE_SEIDS 4
#define MAX_NUM_REMOTE_CAPABILITIES 4

#define MAX_NUM_LOCAL_SEIDS 1
#define MAX_NUM_LOCAL_CAPABILITIES 2

/* media_type */
#define BT_A2DP_MEDIA_AUDIO 		0
#define BT_A2DP_MEDIA_VIDEO 		1
#define BT_A2DP_MEDIA_MULTIMEDIA 	2
#define BT_A2DP_MEDIA_UNKNOWN 	0xff

/* sep_type */
#define BT_A2DP_SOURCE 	0
#define BT_A2DP_SINK 		1

/* codec_type */
#define BT_A2DP_SBC 		1
#define BT_A2DP_MP3 		2
#define BT_A2DP_AAC 		3
#define BT_A2DP_ATRAC 		4
#define BT_A2DP_NON_A2DP        5
#endif

//A/V Content Protection Method
#define AVDTP_CONTENT_PROTECTION_DTCP      0x0001
#define AVDTP_CONTENT_PROTECTION_SCMS_T  0x0002
/*!
	@brief The capability to stream media. This is manditory for advance audio
	distribution profile.
*/
#define AVDTP_SERVICE_MEDIA_TRANSPORT		(1)		
/*!
	@brief The reporting capability. This is not currently supported.
*/
#define AVDTP_SERVICE_REPORTING				(2)		
/*!
	@brief The recovery capability. This is not currently supported.
*/
#define AVDTP_SERVICE_RECOVERY				(3)		
/*!
	@brief The content protection capability. This is not currently supported.
*/
#define AVDTP_SERVICE_CONTENT_PROTECTION	(4)		
/*!
	@brief The header compression capability. This is not currently supported.
*/
#define AVDTP_SERVICE_HEADER_COMPRESSION	(5)		
/*!
	@brief The multiplexing capability. This is not currently supported.
*/
#define AVDTP_SERVICE_MULTIPLEXING			(6)		
/*!
	@brief The codec capability for the Stream End Point.
*/
#define AVDTP_SERVICE_MEDIA_CODEC			(7)		
/*!
	@brief The delay reporting capability. 
*/
#define AVDTP_SERVICE_DELAY_REPORTING	       (8)		



#define MAX_CP_SCHEME_LENGTH                              (48)

typedef enum
{
    BT_A2DP_MESSAGE_TYPE_COMMAND = (0x0),
    BT_A2DP_MESSAGE_TYPE_ACCEPT = (0x02),
    BT_A2DP_MESSAGE_TYPE_REJECT = (0x03)
}enum_a2dp_signal_cmd_status;

/*!
	@brief Error codes.
*/
typedef enum
{
	avdtp_bad_header_format             = (0x01),	/*!< The request packet header format error that is not specified above ERROR_CODE. */
	avdtp_bad_length                    = (0x11),	/*!< The request packet length is not match the assumed length. */
	avdtp_bad_acp_seid                  = (0x12),	/*!< The requested command indicates an invalid ACP SEID (not addressable). */
	avdtp_sep_in_use                    = (0x13),	/*!< The SEP is in use. */
	avdtp_sep_not_in_use                = (0x14),	/*!< The SEP is not in use. */
	avdtp_bad_serv_category             = (0x17),	/*!< The value of Service Category in the request packet is not defined in AVDTP. */
	avdtp_bad_payload_format            = (0x18),	/*!< The requested command has an incorrect payload format (Format errors not specified in this ERROR_CODE). */
	avdtp_not_supported_command         = (0x19),	/*!< The requested command is not supported by the device. */
	avdtp_invalid_capabilities          = (0x1a),	/*!< The reconfigure command is an attempt to reconfigure a transport service capabilities of the SEP. Reconfigure is only permitted for application service capabilities. */
	avdtp_bad_recovery_type             = (0x22),	/*!< The requested Recovery Type is not defined in AVDTP. */
	avdtp_bad_media_transport_format    = (0x23),	/*!< The format of Media Transport Capability is not correct. */
	avdtp_bad_recovery_format           = (0x25),	/*!< The format of Recovery Service Capability is not correct. */
	avdtp_bad_rohc_format               = (0x26),	/*!< The format of Header Compression Service. */
	avdtp_bad_cp_format                 = (0x27),	/*!< The format of Content Protection Service Capability is not correct. */
	avdtp_bad_multiplexing_format       = (0x28),	/*!< The format of Multiplexing Service Capability is not correct. */
	avdtp_unsupported_configuration     = (0x29),	/*!< Configuration not supported. */
	avdtp_bad_state                     = (0x31)	/*!< Indicates that the ACP state machine is in an invalid state in order to process the signal.*/
} t_AVDTP_ERROR_CODE;



#ifndef __BT_A2DP_CODEC_TYPES__
#define __BT_A2DP_CODEC_TYPES__
typedef struct
{
	u_int8 in_use;
	u_int8 seid;
	u_int8 media_type;
	u_int8 sep_type;
    u_int8 get_cap;
    u_int8 have_this_sep;
} bt_sep_info_struct;

typedef struct
{
	u_int8 min_bit_pool;
	u_int8 max_bit_pool;
	u_int8 block_len; // b0: 16, b1: 12, b2: 8, b3: 4
	u_int8 subband_num; // b0: 8, b1: 4
	u_int8 alloc_method; // b0: loudness, b1: SNR
	u_int8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 16000
	u_int8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
} bt_a2dp_sbc_codec_cap_struct;

typedef struct
{
	u_int8 layer; // b0: layerIII, b1: layerII, b0: layerI
	u_int8 CRC;
	u_int8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
	u_int8 MPF; // is support MPF-2
	u_int8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 24000, b4: 22050, b5: 16000
	u_int8 VBR; // is support VBR
	u_int16 bit_rate; // bit-rate index in ISO 11172-3 , b0:0000 ~ b14: 1110
} bt_a2dp_mp3_codec_cap_struct; /* all MPEG-1,2 Audio */

typedef struct
{
	u_int8 object_type; // b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC
	u_int16 sample_rate; // b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000
	u_int8 channels; // b0: 2, b1: 1
	u_int8 VBR; // is supported VBR
	u_int32 bit_rate; // constant/peak bits per second in 23 bit UiMsbf, 0:unknown
} bt_a2dp_aac_codec_cap_struct; /* all MPEG-2,4 AAC */

typedef struct
{
	u_int8 version; // 1:ATRAC, 2:ATRAC2, 3:ATRAC3
	u_int8 channel_mode; // b0: joint stereo, b1: dual, b2: single
	u_int8 sample_rate; // b0: 48000, b1: 44100
	u_int8 VBR; // is supported VBR
	u_int32 bit_rate; // bit-rate index in ATRAC, b0: 0x0012 ~ b18: 0x0000
	u_int16 max_sul; // sound unit length in 16 bits UiMsbf
} bt_a2dp_atrac_codec_cap_struct; /* all ATRAC family */

typedef union
{
	bt_a2dp_sbc_codec_cap_struct sbc;
	bt_a2dp_mp3_codec_cap_struct mp3;
	bt_a2dp_aac_codec_cap_struct aac;
	bt_a2dp_atrac_codec_cap_struct atrac;
} bt_a2dp_audio_codec_cap_struct;

typedef struct
{
	u_int8 codec_type; // SBC, MP3
	bt_a2dp_audio_codec_cap_struct codec_cap;
} bt_a2dp_audio_cap_struct;
typedef struct
{
       u_int8 losc;
	u_int16 CP_type;
} bt_a2dp_content_protection_cap_struct;

typedef struct
{
       u_int8 isValid;
	u_int16 delay_value;
} bt_a2dp_delay_reporting_cap_struct;
#endif

typedef struct
{
    u_int16 connect_id;
    t_bdaddr device_addr;
} t_a2dp_signal_connect_ind;

typedef struct
{
    u_int16 connect_id;
    u_int8 acp_seid;
} t_a2dp_capabilities_get_ind;

typedef struct
{
    u_int8 length;
    u_int8 cp_scheme[MAX_CP_SCHEME_LENGTH];
} t_a2dp_security_control_ind;

typedef struct
{
    u_int16 length;
    u_int8 cap[128];
} t_a2dp_cmd_data_ind;

typedef struct
{
	u_int16 connect_id;
	u_int8 acp_seid;
	u_int8 int_seid;
	u_int8 stream_handle;
	bt_a2dp_audio_cap_struct audio_cap;
        bt_a2dp_content_protection_cap_struct cp_cap;
        bt_a2dp_delay_reporting_cap_struct dr_cap;
} t_a2dp_stream_config_ind;

typedef struct
{
	u_int16 connect_id;
	u_int16 result;
	u_int16 sep_num;
	bt_sep_info_struct sep_list[MAX_NUM_REMOTE_SEIDS];
} t_a2dp_sep_discover_cnf;

typedef struct
{
    u_int8   a2dp_supported;
    u_int16 supported_feature;
} t_a2dp_Supported_feature;

typedef struct 
{
	u_int16 cid;
	u_int16 result;
	t_bdaddr bdaddr;
	u_int16 mtu;
}a2dp_connect_ind_msg_t;

typedef struct 
{
	u_int16  cid;
	u_int16  result;
	t_bdaddr bdaddr;
}a2dp_disconnect_ind_msg_t;

typedef struct
{
    u_int16 connect_id;
    u_int8 acp_seid;
} t_a2dp_capabilities_get;

typedef struct
{
	u_int16 connect_id;
	u_int8 acp_seid;
	u_int8 int_seid;
	u_int8 stream_handle;
	bt_a2dp_audio_cap_struct audio_cap;
} t_a2dp_stream_config;

typedef struct
{
	u_int16 connect_id;
	u_int16 result;
	u_int16 sep_num;
	bt_sep_info_struct sep_list[MAX_NUM_REMOTE_SEIDS];
} t_a2dp_sep_discover;



APIDECL1 t_api APIDECL2 Avdtp_Register_Service(u_int8 role); // role: t_SEP_ROLE SEP_SOURCE/SEP_SINK
APIDECL1 t_api APIDECL2 Avdtp_Register_Sep(t_A2DP_SEP_TYPE sep_type, u_int8 role, u_int8 *caps, u_int8 cap_len);
APIDECL1 t_api APIDECL2 AVDTP_Setup_Connect(t_bdaddr address);
APIDECL1 t_api APIDECL2 AVDTP_Start_Media_Channel_Connection(void);
APIDECL1 t_api APIDECL2 AVDTP_Disconnect(void);
APIDECL1 t_api APIDECL2 Avdtp_Disconnect_Media_Channel(void);

APIDECL1 t_api APIDECL2 Avdtp_Get_Codec_Type(void);

APIDECL1 t_api APIDECL2 Avdtp_Send_Reconfigure(u_int16 connect_id, u_int8 acp_seid, u_int8 int_seid, u_int8 codec_type,bt_a2dp_audio_codec_cap_struct* audio_cap);
APIDECL1 t_api APIDECL2 Avdtp_Send_SetConfiguration(u_int16 connect_id, u_int8 acp_seid, u_int8 int_seid, u_int8 codec_type, bt_a2dp_audio_codec_cap_struct* audio_cap);
APIDECL1 t_api APIDECL2 Avdtp_Send_Open(u_int8 acp_seid);

APIDECL1 t_api APIDECL2 Avdtp_Send_Discover(u_int16 cid);
APIDECL1 t_api APIDECL2 Avdtp_Send_GetCapabilities(u_int8 seid, u_int16 cid);

APIDECL1 t_api APIDECL2 Avdtp_Send_Start(void);
APIDECL1 t_api APIDECL2 Avdtp_Send_Suspend(void);
APIDECL1 t_api APIDECL2 Avdtp_Send_Close(void);
APIDECL1 t_api APIDECL2 Avdtp_Send_Abort(void);

APIDECL1 t_api APIDECL2 Avdtp_Send_Data(u_int32 length, u_int8* p_data, u_int32 timestamp,u_int16 seq_num, u_int8 payload_type, u_int8 marker , u_int8 frames);
APIDECL1 t_api APIDECL2 Avdtp_Get_Data(u_int32 *length, u_int8* p_data, u_int32 size);
APIDECL1 t_api APIDECL2 Avdtp_Clean_Data(void);
APIDECL1 t_api APIDECL2 Avdtp_Stop_Data(void);

/* response use register sep data */
APIDECL1 t_api APIDECL2 Avdtp_Response_Discover(u_int16 cid);
APIDECL1 t_api APIDECL2 Avdtp_Response_Get_Capabilities(u_int16 cid, u_int8 seid);
void Avdtp_Send_Security_Control_Res(u_int8 result, u_int8 error_code, u_int8 length, u_int8 *data);


void Avdtp_Send_Res(u_int8 opcode, u_int8 result, u_int8 error_code);
void Avdtp_Send_Sep_Discover_Res(u_int16 cid, bt_sep_info_struct* sep_list, u_int8 result, u_int8 error_code);
void Avdtp_Send_Sep_Capabilities_Get_Res(u_int16 cid, u_int8 codec_type, bt_a2dp_audio_codec_cap_struct* audio_cap);
void Avdtp_Send_Sep_Stream_Config_Res(u_int16 result, u_int8 stream_handle);
void Avdtp_Send_Sep_Stream_Open_Res(u_int16 result, u_int8 stream_handle);
void Avdtp_Send_Sep_Stream_Start_Res(u_int16 result, u_int8 stream_handle);
void Avdtp_Send_Sep_Stream_Close_Res(u_int16 result, u_int8 stream_handle);
void Avdtp_Send_Sep_Stream_Abort_Res( u_int8 stream_handle);
void Avdtp_Send_Sep_Stream_Suspend_Res(u_int16 result, u_int8 stream_handle);

u_int8 avdtp_match_audio_capabilities( bt_a2dp_audio_cap_struct *audio_cap, bt_a2dp_audio_cap_struct *audio_cfg, u_int16 *result );

#define RDABT_Avdtp_Init(accept_avdtp)												Avdtp_Init(accept_avdtp)																																							
#define RDABT_Avdtp_Register_Sep( sep_type,  role,   caps,  cap_len)                                  Avdtp_Register_Sep( sep_type,  role,   caps,  cap_len)                                  
#define RDABT_AVDTP_Setup_Connect( address)                                                                 AVDTP_Setup_Connect( address)                                                                 
#define RDABT_AVDTP_Start_Media_Channel_Connection                                                          AVDTP_Start_Media_Channel_Connection                                                          
#define RDABT_AVDTP_Disconnect                                                                              AVDTP_Disconnect                                                                              
#define RDABT_Avdtp_Disconnect_Media_Channel                                                                Avdtp_Disconnect_Media_Channel                                                                
#define RDABT_Avdtp_Get_Codec_Type                                                                          Avdtp_Get_Codec_Type                                                                          
#define RDABT_Avdtp_Send_Reconfigure( connect_id,  acp_seid,  int_seid,  codec_type,   audio_cap)            Avdtp_Send_Reconfigure( connect_id,  acp_seid,  int_seid,  codec_type,   audio_cap)            
#define RDABT_Avdtp_Send_SetConfiguration( connect_id,  acp_seid,  int_seid,  codec_type,    audio_cap)      Avdtp_Send_SetConfiguration( connect_id,  acp_seid,  int_seid,  codec_type,    audio_cap)      
#define RDABT_Avdtp_Send_Open( acp_seid)                                                                    Avdtp_Send_Open( acp_seid)                                                                    
#define RDABT_Avdtp_Send_Discover( cid)                                                                     Avdtp_Send_Discover( cid)                                                                     
#define RDABT_Avdtp_Send_GetCapabilities( seid,  cid)                                                       Avdtp_Send_GetCapabilities( seid,  cid)                                                       
#define RDABT_Avdtp_Send_Start                                                                              Avdtp_Send_Start                                                                              
#define RDABT_Avdtp_Send_Suspend                                                                            Avdtp_Send_Suspend                                                                            
#define RDABT_Avdtp_Send_Close                                                                              Avdtp_Send_Close                                                                              
#define RDABT_Avdtp_Send_Abort                                                                              Avdtp_Send_Abort                                                                              
#define RDABT_Avdtp_Send_Data( length,    p_data,  timestamp, seq_num,  payload_type,  marker ,  frames)     Avdtp_Send_Data( length,    p_data,  timestamp, seq_num,  payload_type,  marker ,  frames)     
#define RDABT_Avdtp_Get_Data(  length,    p_data,   frames,   buff)                                          Avdtp_Get_Data(  length,    p_data,   frames,   buff)                                          
#define RDABT_Avdtp_Response_Discover( cid)                                                                 Avdtp_Response_Discover( cid)                                                                 
#define RDABT_Avdtp_Response_Get_Capabilities( cid,  seid)                                                  Avdtp_Response_Get_Capabilities( cid,  seid)                                                  
#define RDABT_Avdtp_Send_Res( opcode,  result, code)                                                              Avdtp_Send_Res( opcode,  result, code)                                                              
#define RDABT_Avdtp_Send_Sep_Discover_Res( cid,    sep_list,  result,  error_code)                     Avdtp_Send_Sep_Discover_Res( cid,    sep_list,  result,  error_code)                     
#define RDABT_Avdtp_Send_Sep_Capabilities_Get_Res( cid,  codec_type,    audio_cap)                           Avdtp_Send_Sep_Capabilities_Get_Res( cid,  codec_type,    audio_cap)                           
#define RDABT_Avdtp_Send_Sep_Stream_Config_Res( result,  stream_handle)                                     Avdtp_Send_Sep_Stream_Config_Res( result,  stream_handle)                                     
#define RDABT_Avdtp_Send_Sep_Stream_Open_Res( result,  stream_handle)                                       Avdtp_Send_Sep_Stream_Open_Res( result,  stream_handle)                                       
#define RDABT_Avdtp_Send_Sep_Stream_Start_Res( result,  stream_handle)                                      Avdtp_Send_Sep_Stream_Start_Res( result,  stream_handle)                                      
#define RDABT_Avdtp_Send_Sep_Stream_Close_Res( result,  stream_handle)                                      Avdtp_Send_Sep_Stream_Close_Res( result,  stream_handle)                                      
#define RDABT_Avdtp_Send_Sep_Stream_Abort_Res(  stream_handle)                                              Avdtp_Send_Sep_Stream_Abort_Res(  stream_handle)                                              
#define RDABT_Avdtp_Send_Sep_Stream_Suspend_Res( result,  stream_handle)                                    Avdtp_Send_Sep_Stream_Suspend_Res( result,  stream_handle)                                    
#define RDABT_avdtp_match_audio_capabilities(   audio_cap,   audio_cfg,   result )                          avdtp_match_audio_capabilities(   audio_cap,   audio_cfg,   result )                          



#ifdef __cplusplus
}
#endif

#endif //RDABT_A2DP_H_

