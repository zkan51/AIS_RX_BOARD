/*********************************************************************
* @file      UART2.h
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/21   13:14
********************************************************************
* @brief     定义gps串口和状态机之间的数据结构体和功能函数的接口
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
**********************************************************************/
#ifndef  UART2_H
#define  UART2_H
#include "stm32f4xx.h" 
#include "stdio.h"
#include "myStdDef.h"
#include "AIS_PS_Struct.h"

#define Uart2_GPS1_LEN     1024 
#define Uart2_GPS2_LEN     1024 
extern uint8_t Uart2_GPS1[Uart2_GPS1_LEN];//UART1����
extern uint8_t Uart2_GPS2[Uart2_GPS2_LEN];//UART1ޓ˕

//u8 Flag_Uart_Send = 0;


void UART2_Config(u32 bound);
void  Putc_UART2(u8 ch);


#endif
