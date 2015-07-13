#ifndef UART2_H
#define UART2_H

#define UART_RX_BUFFER 255
#define UART_TX_BUFFER 255
#define UART2_PORT       2

void uart2_start(void);
void uart2_stop(void);
void uart2_wakeup();

void uart2_deal_data(UINT32 param);
UINT32 uart2_send_data(const UINT8* data, UINT32 length);

#endif
