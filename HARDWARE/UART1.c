#include "UART1.h"
#include "misc.h"
#include "stdio.h"
#include "includes.h"
#include "APP.h"
uint8_t Uart1_Rx1[UART1_RX1_LEN];//UART1接收数组
uint8_t Uart1_Tx1[UART1_TX1_LEN];//UART1接收数组

static GPIO_InitTypeDef GPIO_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static DMA_InitTypeDef DMA_InitStructure;	
void UART1_Config(u32 bound)
{	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //开GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开USART1时钟
//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9¸复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO速度50M
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化IO
//USART1 初始化
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//打开发送和接收
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //开启串口1 
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);//开启UART1DMA接收数据使能（DMA2 Stream2 通道4）
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);//开启UART1DMA发送数据使能（DMA2 Stream7 通道4）	
//中断设置****UART1接收空闲*****
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//空闲中断

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//初始化
	//UART1发送DMA完成中断//

//DMA使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  //打开DMA2的时钟  

	DMA_DeInit(DMA2_Stream2); 	//DeInit   
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR));//源地址 
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart1_Rx1;     //目的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;            //外设到内存
	DMA_InitStructure.DMA_BufferSize = UART1_RX1_LEN;               //数据传输量
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
  DMA_Init(DMA2_Stream2, &DMA_InitStructure);//初始化
	DMA_Cmd(DMA2_Stream2,ENABLE);     //使能  
	
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR));//外设地址 
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart1_Tx1;     //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;          //内存到外设
	DMA_InitStructure.DMA_BufferSize = UART1_TX1_LEN;               //数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //内存增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//数据长度8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //存储器数据长度8bit 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      // 优先级高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA2_Stream7, &DMA_InitStructure);//初始化
	//DMA_Cmd(DMA2_Stream7,ENABLE);     //先不使能，需要发送的时候使能 	
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);//开启UART1传输完成中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler(void)                                 
{     
	uint32_t Length = 0;u16 i;	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{  
		  DMA_Cmd(DMA2_Stream2,DISABLE); 
		  Length = USART1->SR;  
 		  Length = USART1->DR; //清除标志位 
 			Length = UART1_RX1_LEN - DMA_GetCurrDataCounter(DMA2_Stream2);//获取本次接收数据长度		
		  printf("\r\nUART1 Receive Length:%d\r\n",Length);
      for(i=0;i<Length;i++)
 			{
// 			 Message_AIS[i]=Uart_Rx[i];//Message_AIS++;
 			   printf("%c ",Uart1_Rx1[i]);
      }	printf("\r\n");		
      DMA_SetCurrDataCounter(DMA2_Stream2,UART1_RX1_LEN); 				 
		  DMA_Cmd(DMA2_Stream2, ENABLE);//处理完,重开DMA   
	}
	__nop();   
}
void DMA2_Stream7_IRQHandler(void)
{ u8 err;
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) != RESET)
	{
		 OSIntEnter();

  	 DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
 		 DMA_Cmd(DMA2_Stream7,DISABLE);
		 //OSFlagPost(UART1_TX_FLAG,0x01,OS_FLAG_SET,&err);
		OSSemPost(UART1_Tx_FLAG);
		OSIntExit();
	}
}
void  Putc_UART1(u8 ch)
{
	while((USART1->SR&0X40)==0);
	USART_SendData(USART1, ch);	
}


