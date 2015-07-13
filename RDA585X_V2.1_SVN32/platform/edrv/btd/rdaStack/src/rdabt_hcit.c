/***********************************************************************
 *
 * MODULE NAME:    rdabt_uart.c
 * DESCRIPTION:    uart driver
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
#include "cs_types.h"

#include "rdabt_main.h"
#include "bt.h"
#include "rdabt_hcit.h"
#include "hal_sharemem.h"
#include "hal_sys.h"

#ifdef BT_SHAREMEM_TRANSPORT
extern rdabt_context_struct rdabt_cntx;
u_int8 event_rx_page, cmd_tx_page, cmd_rx_page;

t_api rdabt_hcit_sharemem_init(void)
{
    hal_ShareMemInit();
    event_rx_page = 0;
    cmd_tx_page = 0;
    cmd_rx_page = 0;
    return 0;
}

t_api rdabt_hcit_sharemem_shutdown(void) 
{

    return 0;
}


void rdabt_hcit_sharemem_rx_data(void)
{
    u_int16 *head;
    u_int8 *buff;
    u_int16 rx_type, rx_len, head_len;
    struct st_t_dataBuf *rx_buf;
    adp_data_msg_t data_msg;
    u_int16 header[2];
    u_int8 page;
    u_int8 wakeup = 0;

    do{
        head = hal_ShareMem_GetHeader();
        
        rx_type = head[0];
        rx_len = head[1];

        if(rx_type == 6)
        {
            buff = hal_ShareMem_GetData();
            cmd_rx_page = buff[0];
            rdabt_cntx.tx_busy = 0;
            hal_ShareMem_NextData(1);
            wakeup = 1;
            break;
        }
        if(rx_type < 2 || rx_type > 4)
            break;

        //hal_HstSendEvent(SYS_EVENT,0x10190000+rx_type);
        //hal_HstSendEvent(SYS_EVENT,rx_len);
        
        if (RDABT_NOERROR!=HCI_GetReceiveBuffer(&rx_buf, rx_len))
            break;

        pMemcpy(rx_buf->buf, hal_ShareMem_GetData(), rx_len); /* copy header bytes into PDU buffer*/
        rx_buf->dataLen = rx_len;
        
        //hal_HstSendEvent(SYS_EVENT,rx_buf->buf[0]+(rx_buf->buf[3]<<16)+(rx_buf->buf[4]<<24));

        data_msg.buff = rx_buf;
        data_msg.type = rx_type;
        rx_buf->transport = TRANSPORT_UART_ID;
        RDABT_Send_Message(ADP_DATA_IND, RDABT_ADP, RDABT_HCI, sizeof(adp_data_msg_t), &data_msg);
        hal_ShareMem_NextData(rx_len);
        page = hal_ShareMem_GetRxPage();
        if(page != event_rx_page)
        {
            hal_ShareMem_SendData(&page, 1);
            header[0] = 0x6;
            header[1] = 1;
            hal_ShareMem_SendHeader(header);
            event_rx_page = page;
            //break;
        }
        //hal_HstSendEvent(SYS_EVENT,page);
        wakeup = 1;
    }while(1);

    if(wakeup)
        pWakeUpScheduler();

}

t_api rdabt_hcit_sharemem_tx_pdu(t_DataBuf *pdu, u_int8 type, u_int16 flags, u_int16 hci_handle_flags)
{
	u_int16 bytesWritten;
	u_int8 *pkt_head;
    u_int16 pkt_head_len;
    u_int16 header[2];
    u_int32 critical;
    u_int8 page;

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

    header[0] = type;
    header[1] = pdu->dataLen+pkt_head_len-1;

    //hal_HstSendEvent(SYS_EVENT,0x10170000 + type);
    //hal_HstSendEvent(SYS_EVENT,pdu->buf[0]+(pdu->buf[1]<<8));

    critical = hal_SysEnterCriticalSection();

    hal_ShareMem_SendData(pkt_head+1,pdu->dataLen+pkt_head_len-1);
    hal_ShareMem_SendHeader(header);

    hal_SysExitCriticalSection(critical);
    
    HCI_FreeWriteBuffer(pdu, type);

    page = hal_ShareMem_GetTxPage();
    if(page != cmd_tx_page)
    {
        if(cmd_tx_page != cmd_rx_page
            &&(page+1 == cmd_rx_page || (cmd_rx_page == 0 && page == 3)))
            rdabt_cntx.tx_busy = 1;
        cmd_tx_page = page;
    }

    return 0;
}

t_api rdabt_hcit_sharemem_tx_data(u_int8 *pdu, u_int8 len)
{
    u_int16 header[2];
    u_int32 critical;

    //hal_HstSendEvent(SYS_EVENT,0x10170000 + pdu[0]);
    //hal_HstSendEvent(SYS_EVENT,len);

    header[0] = pdu[0];
    header[1] = len-1;

    critical = hal_SysEnterCriticalSection();
    hal_ShareMem_SendData(pdu+1, len-1);
    hal_ShareMem_SendHeader(header);
    hal_SysExitCriticalSection(critical);

    return 0;
    
}
#endif

