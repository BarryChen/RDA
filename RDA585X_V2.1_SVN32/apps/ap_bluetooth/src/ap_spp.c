#include "cs_types.h"
#include "bt.h"
#include "bt_msg.h"

#include "spp.h"
#include "sdp.h"
#include "sap.h"
#include "ap_common.h"

UINT8 spp_port = 0;
UINT32 spp_recv_count = 0;

#define MAX_SPPDEV_NUM 3

u_int8 devCnt = 0;

typedef struct {
    u_int8 port;
    t_bdaddr addr;
    char stringBuf[200];
    u_int32 dataCnt;
}t_spp_dev;

t_spp_dev spp_dev[MAX_SPPDEV_NUM];

void BT_RegisterSPPService(void)
{
#if APP_SUPPORT_SPP == 1
	SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);
    SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);
    SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);
#endif
}

void spp_write_data_buf(u_int8*buf,  char* fmt,...)
{
#ifndef WIN32
    va_list args;

    va_start(args,fmt);
    vsnprintf(buf, 50, fmt, args);
    va_end(args);
#endif
}

UINT8 spp_send_timer = 0;
char spp_test_data[200] = "spp test sspp test sspp test sspp test sspp test sspp test sspp test sspp test sspp test sspp testspp test sspp test sspp test sspp test sspp test sspp test sspp test sspp test sspp test sspp test";
void spp_timer_cb(void* arg)
{
    t_spp_dev* dev = (t_spp_dev*)arg;
//    spp_write_data_buf(dev->stringBuf, "message %d to %s\n\r", dev->dataCnt, rdabt_mgr_printBDAddress(dev->addr));
//    app_trace(APP_BT_TRC, dev->stringBuf);
    app_trace(APP_BT_TRC, "SPP_Send_Data:%d \n", dev->dataCnt);	
    SPP_Send_Data(dev->stringBuf, strlen(dev->stringBuf), dev->port);
    dev->dataCnt++;
}


void SPPMessageHandle(u_int16 msg_id, void *msg_data)
{
    t_spp_msg *msg = (t_spp_msg*)msg_data;
	
    switch(msg_id)
    {
    case SPP_CONNECT_IND:
	 hal_HstSendEvent(SYS_EVENT, 0x20141513);
	 hal_HstSendEvent(SYS_EVENT, msg->result);
        app_trace(APP_BT_TRC, "recv SPP_CONNECT_IND ind, result = %d\n", msg->result);
        if(msg->result  ==RDABT_NOERROR)
        {
            memset(spp_dev[devCnt].stringBuf, 0, sizeof(spp_dev[devCnt].stringBuf));
            spp_dev[devCnt].port = msg->port;
            memcpy(spp_dev[devCnt].addr.bytes, msg->address.bytes, 6);
	     spp_test_data[199] = 0;		
	     strncpy(spp_dev[devCnt].stringBuf, spp_test_data, 199); 
            spp_send_timer = rdabt_timer_create(10, spp_timer_cb, (void *) (&(spp_dev[devCnt])), 2);
            devCnt++;
            devCnt %= 3;
            
            
            
//            app_trace(APP_BT_TRC, "data sent to address %s\n", rdabt_mgr_printBDAddress(msg->address));
        }
        else
        {
            //fail
        }

        
    case SPP_CONNECT_CNF:
        if(msg->result != RDABT_NOERROR)
        {
           break;
        }
	 app_trace(APP_BT_TRC, "recv SPP_CONNECT_CNF, result = %d\n", msg->result);
        if(msg->result  ==RDABT_NOERROR)
        {
            memset(spp_dev[devCnt].stringBuf, 0, sizeof(spp_dev[devCnt].stringBuf));
            spp_dev[devCnt].port = msg->port;
            memcpy(spp_dev[devCnt].addr.bytes, msg->address.bytes, 6);
	     spp_test_data[199] = 0;		
	     strncpy(spp_dev[devCnt].stringBuf, spp_test_data, 199); 
            spp_send_timer = rdabt_timer_create(10, spp_timer_cb, (void *) (&(spp_dev[devCnt])), 2);
            devCnt++;
            devCnt %= 3;
//            app_trace(APP_BT_TRC, "data sent to address %s\n", rdabt_mgr_printBDAddress(msg->address));
        }
	 spp_port = msg->port;
        break;
    case SPP_DISCONNECT_IND:
    case SPP_DISCONNECT_CNF:
        {
            if(msg->result  ==RDABT_NOERROR)
            {
      //          app_trace(APP_BT_TRC, "recv spp disconnect ind from %s \n", rdabt_mgr_printBDAddress(msg->address));
		  rdabt_cancel_timer(spp_send_timer);
		  spp_send_timer = 0;
    //            while(1);
                
    //            app_trace(APP_BT_TRC, "data sent to address %s\n", rdabt_mgr_printBDAddress(msg->address));
            }
            else
            {
                //fail
            }

        }
        break;

    case SPP_DATA_IND:
		{
			t_DataBuf *buff = (t_DataBuf*)msg->data;

            if(msg->uuid16 == SDP_SCLASS_UUID_SIM_ACCESS)
            {
            }
            else
            {
    			buff->buf[buff->dataLen] = 0;
			app_trace(APP_BT_TRC, "recv spp data:%s %d\n", buff->buf, spp_recv_count);
			spp_recv_count++;
#ifndef WIN32		
#ifdef UART2_SUPPORT
			uart2_send_data(buff->buf, buff->dataLen);
#endif
#endif
            }
            RSE_FreeWriteBuffer(buff);
		}
    case SPP_DATA_CNF:
        {

        }
	}
}

