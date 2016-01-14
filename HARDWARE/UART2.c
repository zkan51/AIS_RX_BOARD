/**********************************************************************
* @file      UART2.c
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/21   11:16
********************************************************************
* @brief     定义gps串口和状态机交互的数据结构体和功能函数的接口
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
**********************************************************************/
#include "UART2.h"
#include "UART1.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "includes.h"

uint8_t Uart2_GPS1[Uart2_GPS1_LEN];//UART1接收数组
uint8_t Uart2_GPS2[Uart2_GPS2_LEN];//UART1接收数组

	
static GPIO_InitTypeDef GPIO_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static DMA_InitTypeDef DMA_InitStructure;	
void UART2_Config(u32 bound)
{	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //开GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//开USART1时钟
//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2¸复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3
	
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO速度50M
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化IO
//USART2 初始化
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//打开发送和接收
  USART_Init(USART2, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART2, ENABLE);  //开启串口2
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	
//USART_ClearFlag(USART2, USART_FLAG_TC);	
//中断设置,使用定时器的断帧检测，不需要开启
//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//空闲中断
//  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//初始化
	//DMA使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  //打开DMA2的时钟

	DMA_DeInit(DMA1_Stream5); 	//DeInit   
	
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART2->DR));//源地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart2_GPS1;     //目的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;            //外设到内存
	DMA_InitStructure.DMA_BufferSize = Uart2_GPS1_LEN;               //数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //内存增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//数据长度8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //存储器数据长度8bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                //普通模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      // 优先级高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
 /*DMA DOUBLE Buffer mode*///双缓冲模式
  //DMA_DoubleBufferModeConfig(DMA1_Stream5,(u32)&Uart2_GPS2,DMA_Memory_0);//Memory0首先传输
  //DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);			
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化
	DMA_Cmd(DMA1_Stream5,ENABLE);     //使能
}

void USART2_IRQHandler(void)                                 
{     
	u16 i;
	u16 GPS_length;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
	{  
		  GPS_length = USART2->SR;  
 		  GPS_length = USART2->DR; //清除标志位
 			GPS_length = Uart2_GPS1_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
			printf("\r\n%d\r\n",GPS_length);				 
	}
	__nop();   
}

void  Putc_UART2(u8 ch)
{
	while((USART2->SR&0X40)==0);
	USART_SendData(USART2, ch);	
}


