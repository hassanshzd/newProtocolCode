#ifndef __UART_H
#define __UART_H

#include "typedefs.h"
#include "stm32f10x_usart.h"
#include "user.h"

void USART3_Init(void);
void USART3_SendChar(char c);
void USART3_SendString(const char *str);
void USART3_SendArray(u8* data_addr, u16 len);
void USART3_RxIntByte(u8 data_byte);
void hndl_rx_pkt(void);

#endif
