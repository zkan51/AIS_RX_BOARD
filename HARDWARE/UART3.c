/*
* @file      UART3.c
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/19   15:44
********************************************************************
* @brief     用于处理MCU和上位机之间的数据交互，数据协议为61162-1
* 			 上位机MKD给MCU的数据是船舶静态数据，安全数据，参数输入数据以及相应的确认信号，分别为：VSD/SSD,BBM,ACA,ACK
*			 MCU给上位机MKD的数据是自己的船舶消息，它船消息，报警消息以及相应的确认消息，分别为：VDO,VDM,ALR/TXT,ABK
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
**********************************************************************/
#include "UART3.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "includes.h"
uint8_t Uart3_Rx1[UART3_RX1_LEN];//UART1接收数组
uint8_t Uart3_Rx2[UART3_RX2_LEN];//UART1接收数组


static GPIO_InitTypeDef GPIO_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static DMA_InitTypeDef DMA_InitStructure;	
void UART3_Config(u32 bound)
{	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //开GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//开USART1时钟
//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOA9¸复用为USART1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOA10
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO速度50M
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化IO
//USART3 初始化
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//打开发送和接收
    USART_Init(USART3, &USART_InitStructure); //初始化串口1
	
    USART_Cmd(USART3, ENABLE);  //开启串口1
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);	
//中断设置
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//空闲中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//初始化
	//DMA使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  //打开DMA2的时钟

	DMA_DeInit(DMA1_Stream1); 	//DeInit   
	
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART3->DR));//源地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(Uart3_Rx1);     //目的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;            //外设到内存
	DMA_InitStructure.DMA_BufferSize = UART3_RX1_LEN;               //数据传输量
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
    DMA_Init(DMA1_Stream1, &DMA_InitStructure);//初始化
	DMA_Cmd(DMA1_Stream1,ENABLE);     //使能
}

void USART3_IRQHandler(void)                                 
{     
	uint32_t Length = 0;
	u16 i;
//	uint8_t index,*pt,*pt0,err;	

	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  
	{  
		//flag_translate=1;
		  DMA_Cmd(DMA1_Stream1,DISABLE); 
		  Length = USART3->SR;  
 		  Length = USART3->DR; //清除标志位
 			Length = UART3_RX1_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
      //if(45<Length<55)
			//{		
					//OSQPost(QSem,(void *)Uart_Rx);
				printf("\r\n%d\r\n",Length);
         for(i=0;i<Length;i++)
 			  {
// 					Message_AIS[i]=Uart_Rx[i];//Message_AIS++;
 					printf("%c ",Uart3_Rx1[i]);
         }		
// 			pt=OSMemGet(PartitionPt,&err);
// 			pt0=pt;
// 			for(index=0;index<50;index++)
// 			{
// 				*pt=Uart_Rx[index];
// 				pt++;
// 			}
// 			OSQPost(QSem,(void *)pt0);	
//		   printf("\r\nDMA OK ");
			 
//           for(index=0;index<50;index++)	
//          {
//            Partition[myCnt][index]=Uart_Rx[index];
//          }
//        OSQPost(QSem,(void *)Partition[myCnt]); 
//        myCnt++;
//        myCnt  = myCnt%(MSG_QUEUE_TABNUM);   
//        printf("myCnt:%d\n\r!",myCnt);	
		  //}		
      DMA_SetCurrDataCounter(DMA1_Stream1,UART3_RX1_LEN); 				 
		 // DMA2_Stream2->NDTR = UART1_RX_LEN;//重装填，并让接收地址从0开始
		  DMA_Cmd(DMA1_Stream1, ENABLE);//处理完,重开DMA
	}

	__nop();   
}

void  Putc_UART3(u8 ch)
{
	while((USART3->SR&0X40)==0);
	USART_SendData(USART3, ch);	
}
//标准库需要的支持函数
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit() 避免办主机模式  
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
	USART3->DR = (u8) ch;      
	return ch;
}

	


