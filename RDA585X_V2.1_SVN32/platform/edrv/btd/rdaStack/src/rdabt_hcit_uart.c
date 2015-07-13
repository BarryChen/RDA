/***********************************************************************
 *
 * MODULE NAME:    rdabt_hcit_uart.c
 * PROJECT CODE:   host side stack
 * DESCRIPTION:    HCI Transport for eCos Serial port
 * MAINTAINER:     John Sheehy
 * CREATION DATE:  1 May 2000
 *
 * SOURCE CONTROL: $Id: hcit_mtk_uart.c,v 1.4 2009/08/25 05:59:06 jianguo Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2011 RDA Microelectronics Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    1.May.2000 - JS - first import
 *
 *
 * ISSUES:
 *       
 ***********************************************************************/

#include "hal_sys.h"
#include "bt.h"
#include "hci.h"
#include "papi.h"
#include "rdabt_main.h"
#include "rdabt_uart.h"
#include "rdabt_hcit.h"

enum
{
    HCI_UART_STATE_RX_TYPE,
    HCI_UART_STATE_RX_HEADER,      
    HCI_UART_STATE_RX_DATA    
};

#if 0
static char *transportTypes[] = {        
    "HCI Command",
    "ACL Data",
    "SCO Data",
    "HCI Event",
    "RS232 Error",
    "RS232 Negotiation"
};
#endif

#define HCI_SCO_MERGE_COUNT         3

static u_int8 hci_uart_rx_state;
static t_DataBuf *hci_uart_read_buffer;
static u_int16 hci_uart_pend_length;                  /* the length of data have received */
static u_int16 hci_uart_rx_length;                    /* the length of data need receive */
static u_int8* hci_uart_rx_buf;
static u_int8  hci_uart_rx_head_buf[4];
static u_int8  hci_uart_rx_type; 
static const u_int8 hci_uart_head_length[] = {4,3,2};

int16 *MMC_SCOGetRXBuffer(void);
void MMC_SCOReceiveData(int32 length);
u_int8 rdabt_hci_get_max_sco_size(void);

t_api rdabt_hcit_uart_init(void) 
{
    hci_uart_rx_state = HCI_UART_STATE_RX_TYPE;
    hci_uart_pend_length = 0;
    hci_uart_rx_length = 1;
    hci_uart_rx_buf = hci_uart_rx_head_buf;
    return RDABT_NOERROR;
}

t_api rdabt_hcit_uart_shutdown(void) 
{
    return RDABT_NOERROR;
}

void rdabt_uart_rx_data(u_int8 *pdu,u_int32 length)
{
    extern BOOL g_test_mode;
    extern BOOL  g_CalbClock;
    
     //hal_HstSendEvent(SYS_EVENT,0x04060200);
    if(hci_uart_pend_length + length < hci_uart_rx_length)
    {
        if(hci_uart_rx_buf != NULL)
            pMemcpy(hci_uart_rx_buf + hci_uart_pend_length, pdu, length);
        hci_uart_pend_length += length;
     //hal_HstSendEvent(SYS_EVENT,0x04060300);
        return;
    }
//pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"HCI_Transport_Rx_Data hci_uart_pend_length=%d,length=%d,hci_uart_rx_length=%d ",hci_uart_pend_length,length,hci_uart_rx_length ));

    while(hci_uart_pend_length + length >=  hci_uart_rx_length)    
    {
        //if(hci_uart_rx_type != 3 || hci_uart_rx_state!=HCI_UART_STATE_RX_DATA) 
        if(hci_uart_rx_buf != NULL)
            pMemcpy(hci_uart_rx_buf + hci_uart_pend_length, pdu, hci_uart_rx_length - hci_uart_pend_length);
        pdu += hci_uart_rx_length - hci_uart_pend_length;
        length -= hci_uart_rx_length - hci_uart_pend_length;

    //pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"HCI_Transport_Rx_Data hci_uart_rx_state=%d ",hci_uart_rx_state ));
#if 0
        hal_HstSendEvent(SYS_EVENT,0x04060210);
        hal_HstSendEvent(SYS_EVENT,hci_uart_rx_state);
        hal_HstSendEvent(SYS_EVENT,pdu);
        hal_HstSendEvent(SYS_EVENT,hci_uart_pend_length);
        hal_HstSendEvent(SYS_EVENT,hci_uart_rx_length);
#endif    
        switch(hci_uart_rx_state)
        {
        case HCI_UART_STATE_RX_TYPE:
            hci_uart_rx_type = hci_uart_rx_buf[0];
            //pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Receive hci packet of %s\n", hci_uart_rx_type-1<6?transportTypes[hci_uart_rx_type-1]:"INVALID"));
            /* rx type error */
            if(hci_uart_rx_type < 2 || hci_uart_rx_type > 4)
            {
                hal_HstSendEvent(SYS_EVENT,0x04060010);
                hal_HstSendEvent(SYS_EVENT,hci_uart_rx_type);
                break;
            }
            hci_uart_rx_length = hci_uart_head_length[hci_uart_rx_type - 2];
            hci_uart_rx_buf = hci_uart_rx_head_buf;
            hci_uart_rx_state = HCI_UART_STATE_RX_HEADER;
            break;
        case HCI_UART_STATE_RX_HEADER:

            hci_uart_pend_length = hci_uart_rx_length;
            
            if(hci_uart_rx_type == 2) /* ACL data */
            {
                hci_uart_rx_length = ((u_int16)hci_uart_rx_buf[3]<<8) |  hci_uart_rx_buf[2];
                pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Read 4 bytes of ACL Data PDU header: 0x%02X 0x%02X 0x%02X 0x%02X \n", hci_uart_rx_buf[0], hci_uart_rx_buf[1], hci_uart_rx_buf[2], hci_uart_rx_buf[3]));
            }
            else if(hci_uart_rx_type == 3) /* SCO data */
            {
                hci_uart_rx_length =  hci_uart_rx_buf[2];
                pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Read 3 bytes of SCO Data PDU header: 0x%02X 0x%02X 0x%02X \n", hci_uart_rx_buf[0], hci_uart_rx_buf[1], hci_uart_rx_buf[2]));
            }
            else  /* Event */
            {
                hci_uart_rx_length =  hci_uart_rx_buf[1];
                pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Read 2 bytes of Event PDU header: 0x%02X 0x%02X\n", hci_uart_rx_buf[0], hci_uart_rx_buf[1]));
            }

            if(hci_uart_rx_type == 3) /* SCO data */
            {
                hci_uart_rx_buf = MMC_SCOGetRXBuffer();
                hci_uart_rx_state = HCI_UART_STATE_RX_DATA;
                break;
            }
            else
            {
                /* allocate a buffer for the PDU */        
                if (RDABT_NOERROR!=HCI_GetReceiveBuffer(&hci_uart_read_buffer, hci_uart_pend_length + hci_uart_rx_length))
                {  
                    pDebugPrintfEX((pLOGERROR,pLOGHCI,"Cannot allocate %d bytes of memory\n", hci_uart_pend_length + hci_uart_rx_length));
                    return;
                }                             
                pMemcpy(hci_uart_read_buffer->buf, hci_uart_rx_buf, hci_uart_pend_length); /* copy header bytes into PDU buffer*/
                hci_uart_read_buffer->dataLen= hci_uart_pend_length;
                hci_uart_rx_buf = hci_uart_read_buffer->buf+ hci_uart_pend_length;
            }
            
            if (hci_uart_read_buffer != NULL)
            {
                hci_uart_rx_state = HCI_UART_STATE_RX_DATA;
            }
            else
            {
                hci_uart_rx_state = HCI_UART_STATE_RX_TYPE;
            }
            
            if(hci_uart_rx_length != 0)
            {
                break;
            }
            /* for no payload, direct finish the packet */
        case HCI_UART_STATE_RX_DATA:
            //pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Read total %d bytes of %d PDU \n", hci_uart_rx_length,hci_uart_rx_type));
            // send message to core
            if(g_test_mode && !g_CalbClock && hci_uart_rx_type == HCI_pduEVENT)
            {
                if(hci_uart_read_buffer->buf[0]==HCI_evEXTENDEDINQUIRYRESULT||hci_uart_read_buffer->buf[0]==HCI_evINQUIRYRESULTWITHRSSI||hci_uart_read_buffer->buf[0]==HCI_evINQUIRYRESULT)
                    BT_Send_Msg_Up(EV_BT_READY_IND, 0);
                else if(hci_uart_read_buffer->buf[0]==HCI_evINQUIRYCOMPLETE)
                    BT_Send_Msg_Up(EV_BT_READY_IND, 1);

                hal_HstSendEvent(SYS_EVENT, 0x1306250c);
            }
            if(hci_uart_rx_type == 3) 
            {
            	if(hci_uart_rx_buf != NULL)
                MMC_SCOReceiveData(hci_uart_rx_length);
            }
            else
            {
                adp_data_msg_t data_msg;

        	    hci_uart_read_buffer->dataLen+= hci_uart_rx_length;
                data_msg.buff = hci_uart_read_buffer;
                data_msg.type = hci_uart_rx_type;
                hci_uart_read_buffer->transport=1;
                if(RDABT_NOERROR==RDABT_Send_Message(ADP_DATA_IND, RDABT_ADP, RDABT_HCI, sizeof(adp_data_msg_t), &data_msg))
                    pWakeUpScheduler();
                else
                {
                    hal_HstSendEvent(SYS_EVENT,0x04060020);
                    hal_HstSendEvent(SYS_EVENT,hci_uart_rx_type);
                    HCI_FreeReceiveBuffer(hci_uart_read_buffer, hci_uart_rx_type);
                }
            }
            hci_uart_rx_state = HCI_UART_STATE_RX_TYPE;
            
            break;
        }

        if(hci_uart_rx_state == HCI_UART_STATE_RX_TYPE)
        {
            hci_uart_rx_buf = hci_uart_rx_head_buf;
            hci_uart_rx_length = 1;
        }
        hci_uart_pend_length = 0;
    }
    /** save the last data **/
    if(length > 0 && hci_uart_rx_buf != NULL)
        pMemcpy(hci_uart_rx_buf, pdu, length);
    hci_uart_pend_length = length;
//     hal_HstSendEvent(SYS_EVENT,0x04060300);
}

t_api rdabt_uart_tx_sco_data(int16 *pdu, u_int16 handle, uint16 length)
{
	u_int8 pkt_head[4];
	u_int16 bytesWritten;

    //hal_HstSendEvent(SYS_EVENT,0x10280051);
    //hal_HstSendEvent(SYS_EVENT,count);
    //hal_HstSendEvent(SYS_EVENT,len);
    
    pkt_head[0] = 0x03;
    pkt_head[1] = handle&0xff;
    pkt_head[2] = (handle>>8)&0xff;
    pkt_head[3] = length;

    rdabt_adp_uart_tx(pkt_head, 4, &bytesWritten);
    rdabt_adp_uart_tx(pdu, length, &bytesWritten);
    if(bytesWritten!=length)
        return RDABT_HCITERROR;
    else
        return RDABT_NOERROR;
}

t_api rdabt_uart_tx_pdu(t_DataBuf *pdu, u_int8 type, u_int16 flags, u_int16 hci_handle_flags) 
{
	u_int16 bytesWritten;
	u_int8 *pkt_head;
       u_int16 pkt_head_len;

        if (flags & RDABT_HOST_BUF_HCI_HEADER_PRESENT)
		pkt_head_len=1;
	else
		pkt_head_len=5;
	pkt_head = pdu->buf-pkt_head_len;
    
	pkt_head[0]=type;
	
	if (pkt_head_len==5)
	{
		pkt_head[1]=hci_handle_flags & 0xFF;
		pkt_head[2]=hci_handle_flags >> 8;
		pkt_head[3]=pdu->dataLen& 0xFF;
		pkt_head[4]=pdu->dataLen>> 8;
	}

        rdabt_adp_uart_tx(pdu->buf-pkt_head_len,pdu->dataLen+pkt_head_len, &bytesWritten);
    
    //pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Wrote total %d bytes of %s PDU, dump: ", pdu->len,type-1<6?transportTypes[type-1]:"INVALID"));
    //pDebugDumpEX(pLOGDEBUG,pLOGHCI,pdu->data,pdu->len);

    HCI_FreeWriteBuffer(pdu, type);

    if (bytesWritten<pdu->dataLen+pkt_head_len) {
        return RDABT_HCITERROR;        
    }
    else
        return RDABT_NOERROR;
}

t_api rdabt_uart_tx_data(u_int8 *pdu, u_int8 len) 
{
	u_int16 bytesWritten;
    rdabt_adp_uart_tx(pdu,len, &bytesWritten);
    if (bytesWritten<len) {
        return RDABT_HCITERROR;        
    }
    else
        return RDABT_NOERROR;
}


