/***********************************************************************
 *
 * MODULE NAME:    rdabt_hcit_uart.h
 * DESCRIPTION:    UART Transport for Bluetooth Host Stack
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
#ifndef _RDABT_ADP_TRANSPORT_UART_H_
#define _RDABT_ADP_TRANSPORT_UART_H_

#ifdef BT_SHAREMEM_TRANSPORT

t_api rdabt_hcit_sharemem_init(void);
t_api rdabt_hcit_sharemem_shutdown(void) ;

void rdabt_hcit_sharemem_rx_data(void);
t_api rdabt_hcit_sharemem_tx_pdu(t_DataBuf *pdu, u_int8 type, u_int16 flags, u_int16 hci_handle_flags);
t_api rdabt_hcit_sharemem_tx_data(u_int8 *pdu, u_int8 len);


#define RDABT_Tranport_Init           rdabt_hcit_sharemem_init
#define RDABT_Tranport_Shutdown       rdabt_hcit_sharemem_shutdown
#define RDABT_Tranport_Rx_Data        rdabt_hcit_sharemem_rx_data
#define RDABT_Tranport_Tx_Data        rdabt_hcit_sharemem_tx_data
#define RDABT_Tranport_Tx_PDU         rdabt_hcit_sharemem_tx_pdu

#else

t_api rdabt_hcit_uart_init(void);
t_api rdabt_hcit_uart_shutdown(void) ;
void rdabt_uart_rx_data(u_int8 *pdu,u_int32 length);
t_api rdabt_uart_tx_pdu(t_DataBuf *pdu, u_int8 type, u_int16 flags, u_int16 hci_handle_flags) ;
t_api rdabt_uart_tx_data(u_int8 *pdu, u_int8 len) ;

#define RDABT_Tranport_Init           rdabt_hcit_uart_init
#define RDABT_Tranport_Shutdown       rdabt_hcit_uart_shutdown
#define RDABT_Tranport_Rx_Data        rdabt_uart_rx_data
#define RDABT_Tranport_Tx_Data        rdabt_uart_tx_data
#define RDABT_Tranport_Tx_PDU         rdabt_uart_tx_pdu

#endif

#endif // _RDABT_ADP_TRANSPORT_UART_H_

