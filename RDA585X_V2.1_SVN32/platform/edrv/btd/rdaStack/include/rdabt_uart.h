/***********************************************************************
 *
 * MODULE NAME:    rdabt_uart.h
 * DESCRIPTION:    UART Driver for Bluetooth Host Stack
 * AUTHOR:         Tianwq
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2011 RDA Microelectronics.
 *     All rights reserved.
 *
 * REMARKS:
 *
 ***********************************************************************/
#ifndef _RDABT_ADP_UART_DRV_H_
#define _RDABT_ADP_UART_DRV_H_

#define BT_UART_PORT	1

void rdabt_adp_uart_start(void);
void rdabt_adp_uart_stop(void);
void rdabt_adp_uart_configure(u_int32 baud,char flow_control);
void rdabt_uart_register_cb(void);

void rdabt_adp_uart_rx(void);
u_int8 rdabt_adp_uart_tx(u_int8 *buf, u_int16 num_to_send, u_int16 *num_send);
void rdabt_uart_eint_hdlr(void);


#endif // _RDABT_ADP_UART_DRV_H_

