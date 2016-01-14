/*********************************************************************
* @file      UART3.h
* @author    Embedded team
* @version   
* @date      2015/12/19   16:12
********************************************************************
* @brief     定义与上位机串口之间的数据交互的数据结构体和功能函数的接口
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#ifndef  UART3_H
#define  UART3_H
#include "stm32f4xx.h" 
#include "stdio.h"

#include "myStdDef.h"
#include "AIS_PS_Struct.h"

#define UART3_RX1_LEN     20 
#define UART3_RX2_LEN     20 
extern uint8_t Uart3_Rx1[UART3_RX1_LEN];//UART3
extern uint8_t Uart3_Rx2[UART3_RX2_LEN];//UART3

//u8 Flag_Uart_Send = 0;
	
	
void UART3_Config(u32 bound);

void  Putc_UART3(u8 ch);
#endif
