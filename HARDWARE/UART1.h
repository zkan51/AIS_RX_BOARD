#ifndef  UART1_H
#define  UART1_H
#include "stm32f4xx.h" 
#include "stdio.h"

#define UART1_RX1_LEN     100 
#define UART1_TX1_LEN     800 
extern uint8_t Uart1_Rx1[UART1_RX1_LEN];//UART1接收缓存地址
extern uint8_t Uart1_Tx1[UART1_TX1_LEN];//UART1发送缓存地址

//u8 Flag_Uart_Send = 0;

void UART1_Config(u32 bound);
void  Putc_UART1(u8 ch);

#endif
