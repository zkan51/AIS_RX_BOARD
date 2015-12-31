#ifndef  UART1_H
#define  UART1_H
#include "stm32f4xx.h" 
#include "stdio.h"

#define UART1_RX1_LEN     20 
#define UART1_RX2_LEN     20 
extern uint8_t Uart_Rx1[UART1_RX1_LEN];//UART1½ÓÊÕ
extern uint8_t Uart_Rx2[UART1_RX2_LEN];//UART1Þ“Ë•

//u8 Flag_Uart_Send = 0;

void UART1_Config(u32 bound);
void  Putc_UART1(u8 ch);

#endif
