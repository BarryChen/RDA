/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#ifndef _AP_BT_HEADSET_H_
#define _AP_BT_HEADSET_H_

#define BT_CALL_STATUS_NONE             0x0000
#define BT_CALL_STATUS_INCOMING         0x0100
#define BT_CALL_STATUS_OUTGOING         0x0200
#define BT_CALL_STATUS_ALERT            0x0300
#define BT_CALL_STATUS_ACTIVE           0x0001

//add by wuxiang
#define BT_CALL_STATUS_END              0x0400
//add end
void BT_HandleHFPMessage(u_int16 msg_id, void* msg_data);
void BT_HandleA2DPMessage(u_int16 msg_id, void* msg_data);
INT32 BT_Connect(void);
void BT_HFPSendBattery(void);


#endif  // _AP_BT_HEADSET_H_


