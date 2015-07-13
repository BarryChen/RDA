/***********************************************************************
 *
 * MODULE NAME:    host_msg.h
 * DESCRIPTION:    Host Message
 * MAINTAINER:     Tianwq
 *
 * SOURCE CONTROL: $Id: host_msg.h,v 1.1 2008/12/17 10:19:31 jianguo Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2010 RDA.
 *     All rights reserved.
 *
 ***********************************************************************/

#ifndef _RDA_BT_MSG_H_
#define _RDA_BT_MSG_H_

/* module id */
enum {
    RDABT_HOST_CORE   = 1,
    RDABT_ADP = 2,
    RDABT_HCI = 3,
    RDABT_PAL = 4,
    RDABT_MANAGER = 5,
    RDABT_L2CAP = 6,
    RDABT_AMP = 7,
    RDABT_SDP_CLI = 8,
    RDABT_SDP_SRV = 9,
    RDABT_RFCOMM = 10,
    RDABT_OBEX_CLI = 11,
    RDABT_OBEX_SRV = 12,
    RDABT_A2DP = 13,
    RDABT_AVRCP = 14,
    RDABT_HFP = 15,
    RDABT_HID = 16,
    RDABT_TCS = 17,
    RDABT_SPP = 18,
	RDABT_HCRP = 19,
	RDABT_L2CAP_TESTER = 20,
    RDABT_PAN=21,
    RDABT_MAX_MODULE
};

// message id group
enum {
    RDABT_MSG_ADP = (RDABT_ADP<<8),
    RDABT_MSG_HCI = (RDABT_HCI<<8),
    RDABT_MSG_MGR = (RDABT_MANAGER<<8),
    RDABT_MSG_SDP_CLI = (RDABT_SDP_CLI<<8),
    RDABT_MSG_A2DP = (RDABT_A2DP<<8),
    RDABT_MSG_AVRCP = (RDABT_AVRCP<<8),
    RDABT_MSG_OBEX_CLI = (RDABT_OBEX_CLI<<8),
    RDABT_MSG_OBEX_SRV = (RDABT_OBEX_SRV<<8),
    RDABT_MSG_HFP = (RDABT_HFP<<8),
    RDABT_MSG_HID = (RDABT_HID<<8),
    RDABT_MSG_SPP = (RDABT_SPP<<8),
    RDABT_MSG_HRCP =(RDABT_HCRP<<8),
    RDABT_MSG_PAN =(RDABT_PAN<<8),
};

#define RDABT_MSG_GROUP_MASK  0Xff00

#define RDABT_INTERNAL_MSG_FLAG  0X0000
#define RDABT_EXTNAL_MSG_FLAG  0X1000
    
// internal message id
enum {
    /* host core common message define */
    HOST_INIT_REQ= (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_ADP),
    HOST_INIT_CNF,
    HOST_SHUTDOWN_REQ,
    HOST_SHUTDOWN_CNF,

    /* adp message define */
    ADP_DATA_IND,
    ADP_TIMER_IND,
    ADP_DATA_REQ,
    ADP_WAKE_IND,
    ADP_LOG_IND,

    ADP_TRANSPORT_REQ,
    ADP_TRANSPORT_CNF,

    ADP_BT_NOTIFY,
    ADP_SLEEP_IND,

    /* hci message define */
    HCI_COMMAND_REQ = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_HCI),
    HCI_CMD_EXT_BUF_REQ,
    HCI_DATA_REQ,
    HCI_EVENT_IND,
    HCI_DATA_IND,

    HCI_RESET_REQ,
    HCI_RESET_CNF,

    /* manager message define */
    MGR_CHANGE_LOCAL_NAME = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_MGR),
    MGR_SET_SCAN_MODE,
    MGR_SET_AUTH_MODE,

    MGR_DISCOVERY_REQ,
    MGR_DISCOVERY_CANCEL_REQ,
    MGR_DISCOVERY_RESULT_IND,
    MGR_DISCOVERY_CNF,
    MGR_GET_REMOTE_NAME_REQ,
    MGR_GET_REMOTE_NAME_CNF,

    MGR_BONDING_IND,
    MGR_BONDING_RES,
    MGR_BONDING_REQ,
    MGR_LINK_KEY_IND,
    MGR_LINK_KEY_RSP,
    MGR_PIN_CODE_IND,
    MGR_PIN_CODE_RSP,
    MGR_OOB_DATA_IND,
    MGR_OOB_DATA_RSP,
    MGR_PASSKEY_IND,
    MGR_PASSKEY_RSP,
    MGR_CONFIRM_IND,
    MGR_CONFIRM_RSP,
    MGR_BONDING_CANCEL_REQ,
    MGR_BONDING_CNF,
    MGR_DELETE_PAIRED_DEVICE_REQ,

    MGR_ACL_CONNECT_IND, 
    MGR_SCO_CONNECT_IND,
    MGR_SCO_DISCONNECT_IND,
    MGR_SCO_DATA_IND,

    MGR_READ_REGISTER_CNF,

    MGR_RSSI_IND,
    MGR_CLIB_CLOCK_IND,

    MGR_LOCALKEY_IND,
    MGR_PEERKEY_IND,
    MGR_DHKEY_IND,

    /* SDP message define */
    SDP_SERVICE_SEARCH_REQ = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_SDP_CLI),
    SDP_SERVICE_SEARCH_CANCEL_REQ,
    SDP_SERVICE_SEARCH_CNF,

    /* a2dp message define */
    A2DP_SIGNAL_CONNECT_CNF = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_A2DP),
    A2DP_SIGNAL_DISCONN_CNF,
    A2DP_MEDIA_CONNECT_CNF,
    A2DP_MEDIA_DISCONN_CNF,
    A2DP_SEND_SEP_DISCOVER_CNF,
    A2DP_SEND_SEP_GET_CAPABILITIES_CNF,
    A2DP_SEND_SEP_SET_CONFIG_CNF,
    A2DP_SEND_SEP_RECONFIG_CNF,
    A2DP_SEND_SEP_OPEN_CNF,
    A2DP_SEND_SEP_START_CNF,
    A2DP_SEND_SEP_PAUSE_CNF,
    A2DP_SEND_SEP_CLOSE_CNF,
    A2DP_SEND_SEP_ABORT_CNF,
    A2DP_ACTIVATE_CNF,
    A2DP_DEACTIVATE_CNF,
    A2DP_SIGNAL_CONNECT_IND,
    A2DP_SIGNAL_DISCONNECT_IND,
    A2DP_MEDIA_CONNECT_IND,
    A2DP_MEDIA_DISCONNECT_IND,
    A2DP_MEDIA_DATA_IND,
    A2DP_SEND_SEP_DISCOVER_IND,
    A2DP_SEND_SEP_GET_CAPABILITIES_IND,
    A2DP_SEND_SEP_SET_CONFIG_IND,
    A2DP_SEND_SEP_RECONFIG_IND,
    A2DP_SEND_SEP_OPEN_IND,
    A2DP_SEND_SEP_START_IND,
    A2DP_SEND_SEP_PAUSE_IND,
    A2DP_SEND_SEP_CLOSE_IND,
    A2DP_SEND_SEP_ABORT_IND,
    A2DP_SEND_SEP_SECURITY_CONTROL_CNF,
    A2DP_SEND_SEP_GET_ALL_CAPABILITIES_CNF,
    A2DP_SEND_SEP_DELAY_REPORT_CNF,
    A2DP_SEND_SEP_SECURITY_CONTROL_IND,
    A2DP_SEND_SEP_GET_ALL_CAPABILITIES_IND,
    A2DP_SEND_SEP_DELAY_REPORT_IND,    
    A2DP_MESSAGE_END,

    /* avrcp message define */
    AVRCP_ACTIVATE_CNF = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_AVRCP),
    AVRCP_DEACTIVATE_CNF,
    AVRCP_CONNECT_CNF,
    AVRCP_CONNECT_IND,
    AVRCP_DISCONNECT_CNF,
    AVRCP_DISCONNECT_IND,
    AVRCP_CMD_FRAME_CNF,
    AVRCP_CMD_FRAME_IND,
    AVRCP_PLAYER_STATUS_IND,
    AVRCP_VENDOR_DATA_IND,

    /* OBEX message define*/
    OBEX_CLI_PUSH_SERVER_CNF = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_OBEX_CLI),
    OBEX_TRANSPORTCONNECT_RF_CNF,//RDABT_OBEX_CLIc send to RDABT_OBEX_CLI   
    OBEX_TRANSPORTCONNECT_RF2_CNF,// RDABT_OBEX_CLI send to RDABT_OBEX_CLI
    OBEX_CLI_RF_DATA_IND,
    OBEX_RETRANSMIT_HANDLER,//this message is useD by OPP client and  server
    OBEX_GETWRITEBUFFER_RETYR,

    OBEX_SERVER_RECEIVE_DATA_IND  = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_OBEX_SRV),//this message IS USED bye ftp server and opp server
    OBEX_SERVER_AUTHORIZE_IND,

    /* handsfree/headset gateway message define */
    HFG_ACTIVATE_CNF = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_HFP),
    HFG_DEACTIVATE_CNF,
    HFG_ACL_CONNECT_REQ,
    HFG_ACL_CONNECT_IND,
    HFG_ACL_DISCONNECT_IND,
    HFG_CHANNEL_CONNECT_IND,
    HFG_SERVER_CHANNEL_IND,
    HFG_CKPD_FROM_HS_IND,
    HFG_DATA_IND,
    HFG_RING_IND,           // send to hf to indicate incoming call
    HFG_CALL_RESPONSE_IND,  // send to ag to indicate hf accept, reject or hold the incoming call
    HFG_CALL_REQ_IND,       // send to ag to indicate the hf initialise call
    HFG_CALL_STATUS_IND,    // send to hf to indicate the call status
     HFG_CALL_CLCC_CNF,
    HFG_NO_CARRIER_IND,

    HFG_CALL_WAIT_3WAY_IND, // send to hf and ag to indicate call wait and three way call status
    HFG_VOICE_RECOGNITION_IND, // send to hf and ag to indicate voice recognition status
    HFG_PHONE_STATUS_IND,    // send to hf to indicate the phone status
    HFG_NETWORK_OPERATOR_IND,    // send to hf the Network Operator string
    HFG_SPEAKER_GAIN_IND,      // send to hf and ag to indicate the speaker gain
    HFG_MIC_GAIN_IND,          // send to hf and ag to indicate the mic gain
	HFG_PHONE_BOOK_IND,
	HFG_PHONE_BOOK_SET_CODE_IND,
    HFG_PHONE_BOOK_GET_CODE_IND,
    HFG_PHONE_BOOK_Q_CODE_IND,
    HFG_PHONE_BOOK_SET_PATH_IND,
    HFG_PHONE_BOOK_ERR_IND,
    HFG_PHONE_BOOK_SET_CODE_ERR_IND,
    HFG_PHONE_BOOK_GET_CODE_ERR_IND,
    HFG_PHONE_BOOK_Q_CODE_ERR_IND,
    HFG_PHONE_BOOK_SET_PATH_ERR_IND,
    
    HFG_INCOMING_HISTORY_IND,
    HFG_OUTCOMING_HISTORY_IND,
    HFG_MISSED_CALL_HISTORY_IND,
    HFG_BATTERY_OK_IND,
    
    
	/* spp message define */
    SPP_CONNECT_IND = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_SPP),
    SPP_CONNECT_CNF,   
    SPP_DATA_IND,
    SPP_DATA_CNF,
    SPP_FLOWCTRL_IND,
    SPP_DISCONNECT_IND,
    SPP_DISCONNECT_CNF,

    /* hid message define */
    HID_CONNECT_IND = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_HID),
    HID_CONNECT_CNF,
    HID_CONTROL_IND,
    HID_SERVICE_ATTRIBUTE_IND,
    HID_INTERRUPT_DATA_IND,
    HID_GET_REPORT_IND,
    HID_SET_REPORT_IND,
    HID_GET_PROTOCOL_IND,
    HID_SET_PROTOCOL_IND,
    HID_GET_IDLE_IND,
    HID_SET_IDLE_IND,

    HID_DISCONNECT_IND,
   HID_DISCONNECT_CNF,
   
   /*hcrp message define*/
    HCRP_SIGNAL_CONNECT_CNF=(RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_HRCP),
    HCRP_SIGNAL_DISCONNECT_CNF,
    HCRP_SIGNAL_DISCONNECT_IND,
    HCRP_DATA_CONNECT_CNF,
    HCRP_DATA_DISCONNECT_CNF,
    HCRP_DATA_DISCONNECT_IND,
    HCRP_DATA_CHANNEL_CREDIT_GRANT_RSP,
    HCRP_DATA_CHANNEL_CREDIT_REQUEST_RSP,
    HCRP_GET_LPT_STATUS_RSP,

    /* PAN message define */
    PAN_CONNECT_IND = (RDABT_INTERNAL_MSG_FLAG+RDABT_MSG_PAN),
    PAN_CONNECT_CNF,
    PAN_ETHERNET_DATA_IND,
    PAN_DISCONNECT_IND,
    PAN_DISCONNECT_CNF,

};

/* task message define */
typedef enum
{
    RDABT_POWER_OFF_REQ=(RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_ADP),
    RDABT_POWER_ON_REQ,
    RDABT_EVM_NOTIFY,
    RDABT_TIMER_EXPIRY,
    RDABT_DATA_READ_IND,
    RDABT_DATA_SEND_IND,

    // HCI
    RDABT_HCI_FIRST_REQ_MSG =(RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_HCI),
    RDABT_CMD_SEND_REQ,
    RDABT_HCI_LAST_REQ_MSG,

    //manager
    RDABT_MGR_FIRST_REQ_MSG =(RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_MGR),
    RDABT_SET_SCAN_MODE_REQ,
    RDABT_DISCOVERY_REQ,
    RDABT_DISCOVERY_CANCEL_REQ,
    RDABT_BONDING_REQ,
    RDABT_PIN_CODE_RSP,
    RDABT_WRITE_LOCAL_NAME_REQ,
    RDABT_GET_REMOTE_NAME_REQ,
    RDABT_BONDING_CANCEL_REQ,
    RDABT_WRITE_SCANENABLE_MODE_REQ,
    RDABT_DELETE_PAIRED_DEVICE,
    
    RDABT_AUTHORIZE_REP,
    RDABT_MGR_LAST_REQ_MSG,

    // sdp client
    RDABT_SDP_CLIENT_FIRST_REQ_MSG = (RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_SDP_CLI),
    RDABT_SERVICE_SEARCH_REQ,
    RDABT_SDP_FREE_RESULT,
    RDABT_SDP_CLIENT_LAST_REQ_MSG,

    // hfp
    RDABT_HFP_FIRST_REQ_MSG =(RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_HFP),
    RDABT_HFG_CONNECT_REQ_MSG,
    RDABT_HFG_DISCONNECT_REQ_MSG,
    RDABT_HFG_SLC_REQ_MSG,
    RDABT_HF_CALL_REQ,
    RDABT_HF_CALL_STAT,
    RDABT_HFP_LAST_REQ_MSG,

    // spp
    RDABT_SPP_FIRST_REQ_MSG =(RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_SPP),
    RDABT_SPP_DISCONNECT_REQ,
    RDABT_SPP_TXDATA_REQ,
    RDABT_SPP_LAST_REQ_MSG,

    // dun
    RDABT_DUN_FIRST_REQ_MSG,
    RDABT_DUN_TXDATA_REQ,
    RDABT_DUN_DISCONNECT_REQ,
    RDABT_DUN_LAST_REQ_MSG,


    // obex client
    RDABT_OBEX_CLIENT_FIRST_REQ_MSG=(RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_OBEX_CLI),
    RDABT_OPP_CONNECT_REQ_MSG,
    RDABT_OPP_PUSH_PKT_REQ,
    RDABT_OPP_DISCON_REQ,
    RDABT_OPP_ABORT_REQ,
    //FTPC
    RDABT_FTPC_LISTING_OBJ_REQ,
    RDABT_FTPC_GET_REQ,
    RDABT_FTPC_REMOVE_REQ,
    RDABT_FTPC_CREATE_REQ,
    RDABT_OBEX_CLIENT_LAST_REQ_MSG,


    // obex server
    RDABT_OBEX_SVR_FIRST_REQ_MSG = (RDABT_EXTNAL_MSG_FLAG+RDABT_MSG_OBEX_SRV),
    RDABT_OPPS_AUTH_RES_MSG,
       //FTPS
    RDABT_FTPS_OBEX_RES,
    RDABT_OBEX_SVR_LAST_REQ_MSG,

}RDABT_MSG_Type;

struct st_rdabt_msg_t{
    u_int16 msg_id;
    u_int8 *data;                                     /* parameter with the message */
};

typedef struct st_rdabt_msg_t rdabt_msg_t;

/* hci data message structure */
typedef struct st_adp_data_msg {
    u_int8 type;
    u_int16 flags; 
    u_int16 hci_handle_flags;
    t_DataBuf *buff;
} adp_data_msg_t;

APIDECL1 t_api APIDECL2 RDABT_Send_Message(u_int16 msg_id, u_int8 thisModule, u_int8 destModule, u_int8 length, void *data);

#endif // _RDA_BT_MSG_H_
