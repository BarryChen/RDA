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
#include "cos.h"
#include "rdabt_drv.h"

#include "rdabt_uart_drv.h"               /* header file for UART */
#include "rdabt_main.h"
#include "bt.h"
#include "rdabt_uart.h"
#include "rdabt_hcit.h"

extern rdabt_context_struct rdabt_cntx;
static UINT32 *rx_buff;

void rdabt_uart_unsleep(void);
#ifdef BT_UART_BREAK_INT_WAKEUP
extern VOID UART_Register_Wakeup_Cb(void (*cb)());
#endif

/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_stop
* DESCRIPTION
*   Stop uart
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_adp_uart_stop(void)
{
   UART_Close(BT_UART_PORT);
   if(rx_buff != NULL)
    {
        COS_Free(rx_buff);
        rx_buff = NULL;
    }
}


/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_start
* DESCRIPTION
*   Start UART for 5868
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_adp_uart_start(void)
{
   if (!UART_Open (BT_UART_PORT, MOD_BT))
    {
        hal_HstSendEvent(SYS_EVENT,0x03230404);
    }
   else
    {
        if(rx_buff == NULL)
            rx_buff = COS_Malloc(RDABT_RX_BUFF_SIZE);
    }
}

/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_configure
* DESCRIPTION
*   Configure baud rate of UART
* PARAMETERS
*   baud    IN    baud rate of UART port
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_adp_uart_configure(u_int32 baud,char flow_control)
{
   UART_SetDCBConfig (BT_UART_PORT, baud, flow_control);
}


/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_rx
* DESCRIPTION
*   Read Rx buffer in UART when receive READY_TO_READ_IND or DATA_TO_READ_IND
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
int stop_flag=0;
void rdabt_adp_uart_rx(void)
{
   UINT16 rx_length;
   UINT32 pos = 0;
   UINT8 status;
   rdabt_uart_unsleep();
   memset(rx_buff, 0, RDABT_RX_BUFF_SIZE);
   rx_length = UART_GetBytes(BT_UART_PORT, rx_buff+pos, RDABT_RX_BUFF_SIZE-pos*4, &status, MOD_BT);
   //if(rx_length>0)
   //kal_prompt_trace(MOD_BT, "rx_length: %d, rx_buff: %02x %02x %02x %02x %02x %02x %02x", rx_length, rx_buff[0], rx_buff[1], rx_buff[2], rx_buff[3], rx_buff[4], rx_buff[5], rx_buff[6]);
    //hal_HstSendEvent(SYS_EVENT,(pos<<16)|(rx_length&0xffff));
   if(rx_length >0)
    {
#if 0
        hal_HstSendEvent(SYS_EVENT,0x03230100);
        //hal_HstSendEvent(SYS_EVENT,rx_buff);
        hal_HstSendEvent(SYS_EVENT,*(rx_buff+pos));
        if(rx_length>4)
        hal_HstSendEvent(SYS_EVENT,*(rx_buff+pos+1));
        if(rx_length>8)
        hal_HstSendEvent(SYS_EVENT,*(rx_buff+pos+2));
        if(rx_length>12)
        hal_HstSendEvent(SYS_EVENT,*(rx_buff+pos+3));
        if(rx_length>16)
        hal_HstSendEvent(SYS_EVENT,*(rx_buff+pos+4));
        if(rx_length>20)
        hal_HstSendEvent(SYS_EVENT,*(rx_buff+pos+5));
#endif
        //if(stop_flag==0)
            rdabt_uart_rx_data(rx_buff,rx_length);
        //pos += (rx_length+3)/4;
        //if(pos*4 >= RDABT_RX_BUFF_SIZE*4/5)
        //    pos = 0;
    }
    // trace_mem();
}


/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_tx
* DESCRIPTION
*   Send data to Tx buffer
* PARAMETERS
*   buf            IN      buffer of data to be sent
*   num_to_send   IN       number of bytes to be sent
*   num_send      OUT     buffer to store actual size of data sent.
* RETURNS
*   TRUE if all bytes sent; otherwise, FALSE.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
unsigned char rdabt_adp_uart_tx(UINT8 *buf, unsigned short num_to_send, unsigned short *num_send)
{

   rdabt_uart_unsleep();   
   *num_send = (UINT16)UART_PutBytes(BT_UART_PORT, (UINT8*)buf, (UINT16)num_to_send, MOD_BT);

#if 0
    hal_HstSendEvent(SYS_EVENT,0x03230200);
    hal_HstSendEvent(SYS_EVENT,num_to_send);
    //hal_HstSendEvent(SYS_EVENT,*num_send);
    //hal_HstSendEvent(SYS_EVENT,buf);
    hal_HstSendEvent(SYS_EVENT,buf[0]|(buf[1]<<8)|(buf[2]<<16)|(buf[3]<<24));
    if(num_to_send>4)
    hal_HstSendEvent(SYS_EVENT,buf[4]|(buf[5]<<8)|(buf[6]<<16)|(buf[7]<<24));
    if(num_to_send>8)
    hal_HstSendEvent(SYS_EVENT,buf[8]|(buf[9]<<8)|(buf[10]<<16)|(buf[11]<<24));
#endif

   //kal_prompt_trace(0, "num_to_send: %d, num_send: %d", num_to_send, *num_send);
   if(*num_send == num_to_send)
      return TRUE;
   else
   {
      return FALSE;
   }
}


/*****************************************************************************
* FUNCTION
*   RDA5868_uart_rx_cb
* DESCRIPTION
*   Callback function when RX interrupt comes
* PARAMETERS
*   port	IN UART power number
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
static void rdabt_uart_rx_cb(UINT8 port)
{
    extern u_int8  rdabt_send_data_ready;
    COS_EVENT event = {0};
    event.nEventId = EV_BT_READY_TO_READ_IND;
    if(rdabt_send_data_ready<3)
    {
        //hal_HstSendEvent(SYS_EVENT,0x04070121);
        //hal_HstSendEvent(SYS_EVENT,rdabt_send_data_ready);
        COS_SendEvent(MOD_BT, &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        rdabt_send_data_ready ++;
    }
}


#if 0
static void rdabt_uart_tx_cb(UART_PORT port)
{
   ilm_struct *msg;
   ASSERT(port == BT_UART_PORT);

   msg = allocate_ilm(MOD_UART3_HISR);
   msg->src_mod_id = MOD_UART3_HISR;
   msg->dest_mod_id = MOD_BT;
   msg->sap_id = INVALID_SAP;
   msg->msg_id = MSG_ID_UART_READY_TO_WRITE_IND;
   msg->local_para_ptr = NULL;
   msg->peer_buff_ptr = NULL;
   msg_send_ext_queue_to_head(msg);
}
#endif

void rdabt_uart_register_cb(void)
{
    UINT8 status;
    char rx_buff[16];
   // clean the data in rx buff
  // UART_ClrRxBuffer   (BT_UART_PORT, MOD_BT);
   while(UART_GetBytes(BT_UART_PORT, rx_buff, 16, &status, MOD_BT));
  
  //  kal_prompt_trace(0,"rdabt_uart_register_cb");
   UART_Register_RX_cb(BT_UART_PORT, MOD_BT, rdabt_uart_rx_cb);
  // UART_Register_TX_cb(BT_UART_PORT, MOD_BT, rdabt_uart_tx_cb);

#ifdef BT_UART_BREAK_INT_WAKEUP
    UART_Register_Wakeup_Cb(rdabt_uart_eint_hdlr);
#endif
}

#define __RDABT_INITALL_BY_IIC__ 1

void rdabt_wirte_memory(unsigned int addr,const unsigned int *data,unsigned char len,unsigned char memory_type)
{
#ifdef __RDABT_INITALL_BY_IIC__
   if(memory_type == 0x00)
   {
     rdabt_iic_core_write_data(addr,data,len);  
   }
   else
#endif
   {
   unsigned short num_send,num_to_send;	
   unsigned char i;
   unsigned char data_to_send[256];
   unsigned int address_convert;
   data_to_send[0] = 0x01;
   data_to_send[1] = 0x02;
   data_to_send[2] = 0xfd;
   data_to_send[3] = len*4+6;
   data_to_send[4] = memory_type;
   if(memory_type == 0x10)
	       data_to_send[4] = 0x00;
//       else 
//        data_to_send[4] = 0x81;
   data_to_send[5] = len;
   if(memory_type == 0x01)
   {
      address_convert = addr*4+0x200;
      data_to_send[6] = address_convert;
      data_to_send[7] = address_convert>>8;
      data_to_send[8] = address_convert>>16;
      data_to_send[9] = address_convert>>24;	  
   }
   else
   {
      data_to_send[6] = addr;
      data_to_send[7] = addr>>8;
      data_to_send[8] = addr>>16;
      data_to_send[9] = addr>>24;
   }
   for(i=0;i<len;i++,data++)
   {
       unsigned char j=10+i*4;
       data_to_send[j] = *data;
       data_to_send[j+1] = *data>>8;
       data_to_send[j+2] = *data>>16;
       data_to_send[j+3] = *data>>24;
   }
   num_to_send = 4+data_to_send[3];
   rdabt_adp_uart_tx(data_to_send,num_to_send,&num_send);
   }	
}

void rdabt_write_pskey(unsigned char id, const unsigned int *data,unsigned char len)
{
    EDRV_TRACE(EDRV_BTD_TRC, 0, "rdabt_write_pskey id=%x, len=%d",id,len);
   {
   unsigned short num_send,num_to_send;	
   unsigned char i;
   unsigned char data_to_send[20];
   unsigned int address_convert;
   data_to_send[0] = 0x01;
   data_to_send[1] = 0x05;
   data_to_send[2] = 0xfd;
   data_to_send[3] = len+1;
   data_to_send[4] = id;

   num_to_send = 5;
   rdabt_adp_uart_tx(data_to_send,num_to_send,&num_send);
    RDABT_DELAY(2);
   num_to_send = len;
   rdabt_adp_uart_tx(data,num_to_send,&num_send);
   }	
}

