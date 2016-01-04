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



MKD_FrameStruct mkd_frameStruct;
AIS_RcvMsgIDStruct ais_rcvMsgIDStruct;
AIS_StaticDataStruct ais_staticDataStruct;
AIS_BBMMsgStruct ais_bbmMsgStruct;
AIS_RcvMsgIDStruct ais_msgIDStruct;


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
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart3_Rx1;     //目的地址
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

	/************************************************************************
	* Name      : usartMDKInit
	* Brief     : 
	*
	* Author    : Digital Design Team
	* Param     : 
	* Return    : void
	************************************************************************/
	//void usartMDKInit(void)
	//{
	//	GPIO_InitTypeDef GPIO_InitStructure;
	//	NVIC_InitTypeDef NVIC_InitStructure;
	//	USART_InitTypeDef USART_InitStructure;
	//	
	//	// Enable GPIO clock 
	//	RCC_APB2PeriphClockCmd(USARTz_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
	//	RCC_APB1PeriphClockCmd(USARTz_CLK,ENABLE);
	//	// Configure USARTy Rx as input floating 
	//	GPIO_InitStructure.GPIO_Pin = USARTz_RxPin;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//	GPIO_Init(USARTz_GPIO, &GPIO_InitStructure); 
	//	// Configure USARTy Tx as alternate function push-pull 
	//	GPIO_InitStructure.GPIO_Pin = USARTz_TxPin;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//	GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);
	//	
	//	// Configure the NVIC Preemption Priority Bits  
	//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  
	//	// Enable the USARTy Interrupt
	//	NVIC_InitStructure.NVIC_IRQChannel = USARTz_IRQn;
	//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//	NVIC_Init(&NVIC_InitStructure);
	//	
	//	USART_DeInit(USARTz);
	//	USART_InitStructure.USART_BaudRate = 4800;
	//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//	USART_InitStructure.USART_Parity = USART_Parity_No;
	//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//	USART_Init(USARTz, &USART_InitStructure);
	//	// Enable USARTy Receive interrupts 
	//	USART_ITConfig(USARTz, USART_IT_RXNE, ENABLE);
	//	// Enable the USARTy 
	//	USART_Cmd(USARTz, ENABLE);
	//}
	//

	/************************************************************************
	* Name      : getMKDAISInfo
	* Brief     : 根据接收的帧格式数据不同保存到相应的结构体中
	*
	* Author    : Digital Design Team
	* Param     : mkd_frameStruct-mkd上位机接收的一帧数据结构体
	* Param     : ais_staticDataStruct-ais相关的静态数据结构体
	* Param     : ais_bbmMsgStruct-ais消息中的BBM数据结构体
	* Param     : ais_msgIDStruct-记录消息ID号的结构体
	* Return    : void
	************************************************************************/
	//void getMKDAISInfo(MKD_FrameStruct * mkd_frameStruct,AIS_StaticDataStruct * ais_staticDataStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_RcvMsgIDStruct * ais_msgIDStruct)
	//{
	//	u8 i;
	//	char dataTmp[5];
	//	for(i = 0; i < 5; i++)
	//	{
	//		dataTmp[i] = mkd_frameStruct->rcvBuffer[i+1];
	//	}
	//	if(strcmp(dataTmp,"AIVSD")==0)//静态消息
	//	{
	//		getVSDStaticInfo(mkd_frameStruct,ais_staticDataStruct);
	//	}
	//	else if(strcmp(dataTmp,"AISSD")==0)//静态消息
	//	{
	//		getSSDStaticInfo(mkd_frameStruct,ais_staticDataStruct);
	//	}
	//	else if(strcmp(dataTmp,"AIBBM")==0)//报警消息
	//	{
	//		getBBMMsgInfo(mkd_frameStruct,ais_bbmMsgStruct);
	//		if (ais_msgIDStruct->msgState != MSGNEW && ais_bbmMsgStruct->rcvStateFlag == RECEIVED)//BBM消息接收完成，添加新的消息
	//		{
	//			ais_msgIDStruct->msgID = 14;
	//			ais_msgIDStruct->msgState = MSGNEW;
	//		}
	//	}
	//}
	//

	/************************************************************************
	* Name      : getVSDStaticInfo
	* Brief     : 将接收的一帧VSD格式数据保存到staticData结构体中
	*
	* Author    : Digital Design Team
	* Param     : mkd_frameStruct-MKD串口接收到的一帧数据数据结构体
	* Param     : ais_staticDataStruct-静态数据相关的数据结构体
	* Return    : void
	************************************************************************/
	//void getVSDStaticInfo(MKD_FrameStruct * mkd_frameStruct,AIS_StaticDataStruct * ais_staticDataStruct)
	//{
	//	u8 fieldTmp[5];
	//	u8 fieldTmpNum = 0;
	//	u8 i = 0;
	//	//fieldTmpNum = 0;
	//	//i = 0;
	//	while(mkd_frameStruct->rcvBuffer[i] != 0x2C)//找","
	//	{
	//		i ++;
	//	}
	//	i ++;
	//	while(mkd_frameStruct->rcvBuffer[i] != 0x2C)//找","
	//	{
	//		fieldTmp[fieldTmpNum] = mkd_frameStruct->rcvBuffer[i];//取第一个字段的船舶类型
	//		fieldTmpNum ++;
	//		i ++;
	//	}
	//	switch(fieldTmpNum)
	//	{
	//		case 1: ais_staticDataStruct->shipType = (fieldTmp[0]-0x30);break;
	//		case 2: ais_staticDataStruct->shipType = (fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30);break;
	//		case 3: ais_staticDataStruct->shipType = (fieldTmp[0]-0x30)*100+(fieldTmp[1]-0x30)*10+(fieldTmp[2]-0x30);break;
	//		default: ais_staticDataStruct->shipType = 0;//不可用或无船舶
	//	}
	//}
	//

	/************************************************************************
	* Name      : setSupplierInfo
	* Brief     : 设置供应商的值，供应商的值一般由本地程序中给出
	*			(根据定义的宏SUPPLIER来获取比特流)
	* Author    : Digital Design Team
	* Param     : ais_staicDataStruct- 设定结构体中的supplier数组的值
	* Return    : void
	************************************************************************/
	void setSupplierInfo(AIS_StaticDataStruct * ais_staicDataStruct)
	{
		u32 tmp = 0;
		u8 i = 0;
		tmp = (SUPPLIER & 0x3FFFFFFFC00)>>10;	//取高32位
		while(i < 32)
		{
			if ((tmp >> i)&0x01)
			{
				ais_staicDataStruct->supplier[i+10] = 1;
			}
			else
			{
				ais_staicDataStruct->supplier[i+10] = 0;
			}
			i ++;
		}
		tmp = SUPPLIER & 0x3FF;			//取低10位
		i = 0;
		while(i < 10)
		{
			if ((tmp >> i)&0x01)
			{
				ais_staicDataStruct->supplier[i] = 1;
			}
			else
			{
				ais_staicDataStruct->supplier[i] = 0;
			}
			i ++;
		}
	}

	/************************************************************************
	* Name      : change8bitASCIItoBit
	* Brief     : 将接收的61162中的标准8bitASCII码转换成1371中规定的6bitASCII码的二进制比特流
	*
	* Author    : Digital Design Team
	* Param     : len- 输入，8bitASCII码字节数据的长度
	* Param     : sour- 输入，8bitASCII码字节存放的地址
	* Param     : dest- 输出，转换的13716bit数据比特流的输出地址
	* Return    : void
	************************************************************************/
	void change8bitASCIItoBit(u8 len,u8 * sour,_Bool * dest)
	{
		u8 i = 0;
		u8 j = 0;
		u8 tmp = 0;
		while (i < len)
		{
			tmp = *(sour + i);
			if (tmp == 94)//保留字符, 则连续取后续的两个字符的数值(先低位，再高位)，组成一个字符信息
			{
				i ++;
				tmp = *(sour + i);
				i ++;
				tmp += (*(sour + i)<<4);
			}
			if (tmp >= 32) // 这个时候若还出现94，也认为是有效值
			{
				if (tmp >= 64)
				{
					tmp -= tmp;
				}
				//取低六位的值并存放到目标地址中
				j = 0;
				* dest = (* dest << (i * 6));
				while(j < 6)
				{
					if (tmp >> j)
					{
						* (dest + j) = 1;
					}
					else
					{
						* (dest + j) = 0;
					}
					j ++;
				}
			}
			else	// 不应该出现的数据，若出现了怎么处理
			{
				// to be added
			}
			i ++;

		}
	}
	
	/************************************************************************
	* Name      : changeMKD6bitASCIItoBit
	* Brief     : 将接收的61162中的6bitASCII码转换成1371中规定的6bitASCII码的二进制比特流
	*
	* Author    : Digital Design Team
	* Param     : len- 输入，6bitASCII码字节数据的长度
	* Param     : sour- 输入，6bitASCII码字节存放的地址
	* Param     : dest- 输出，转换的13716bit数据比特流的输出地址
	* Return    : void
	************************************************************************/
	void changeMKD6bitASCIItoBit(u8 len,u8 *sour,_Bool * dest)
	{
		u8 tmp[20] = {0};//对于B-CS安全类命令只占一个时隙长度，安全文本最大为16个6bit字符
		u8 i = 0;
		//先将61162中的6bitASCII码转换成标准8bitASCII码
		for (i = 0; i < len; i ++)
		{
			if (*(sour+i) < 40) //值小于101000
			{
				*(tmp+i) = *(sour+i) + 48;//加上00110000 
			}
			else//值大于等于101000
			{
				*(tmp+i) = *(sour+i) + 56;//加上00111000
			}
		}
		change8bitASCIItoBit(len,tmp,dest);
	}

	/************************************************************************
	* Name      : getSSDStaticInfo
	* Brief     : 将接收的一帧SSD格式数据保存到staticData结构体中
	*
	* Author    : Digital Design Team
	* Param     : mkd_frameStruct-MKD串口接收到的一帧数据数据结构体
	* Param     : ais_staticDataStruct-静态数据相关的数据结构体
	* Return    : void
	************************************************************************/
	void getSSDStaticInfo(MKD_FrameStruct * mkd_frameStruct,AIS_StaticDataStruct * ais_staticDataStruct)
	{
		u8 fieldTmp[22];
		u8 fieldTmpNum = 0;
		u8 i=0;//j=0;
		u32 dataTmp = 0;
		u8 fieldNum = 0;
		//fieldTmpNum = 0;
		//fieldNum = 0;
		for(i = 0; i < mkd_frameStruct->rcvBufferLength; i ++)
		{
			if(mkd_frameStruct->rcvBuffer[i] == 0x2C)//找到了“,”
			{
				switch(fieldNum)
				{
					case 1://呼号,按照8bit ASCII码存放，高位存在低地址 最多可能存放42bit的数据
					{
						/*j = 0;
						while(j < fieldTmpNum)
						{
							ais_staticDataStruct->shipCall[j] = fieldTmp[j];
							j ++;
						}*/
						change8bitASCIItoBit(fieldTmpNum,&fieldTmp[0],ais_staticDataStruct->shipCall);
						break;
					}
					case 2://船名,按照8bit ASCII码存放，高位存在低地址，最多可能存放120bit
					{
						/*j = 0;
						while(j < fieldTmpNum)
						{
							ais_staticDataStruct->shipName[j] = fieldTmp[j];
							j ++;
						}*/
						change8bitASCIItoBit(fieldTmpNum,&fieldTmp[0],ais_staticDataStruct->shipName);
						break;
					}
					case 3://位置A
					{
						dataTmp = ((fieldTmp[0]-0x30)*100 + (fieldTmp[1]-0x30)*10 + (fieldTmp[2]-0x30))&0x000001FF;
						ais_staticDataStruct->shipSize =0;//先清零，再准备重新赋值
						ais_staticDataStruct->shipSize |= (dataTmp << 21); 
						break;
					}
					case 4://位置B
					{
						dataTmp = ((fieldTmp[0]-0x30)*100 + (fieldTmp[1]-0x30)*10 + (fieldTmp[2]-0x30))&0x000001FF;
						ais_staticDataStruct->shipSize |= (dataTmp << 12);
						break;
					}
					case 5://位置C
					{
						dataTmp = ((fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30))&0x000000FF;
						ais_staticDataStruct->shipSize |= (dataTmp << 6);
						break;
					}
					case 6://位置D
					{
						dataTmp = ((fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30))&0x0000000FF;
						ais_staticDataStruct->shipSize |= dataTmp;
						break;
					}
					case 7://数据终端设置指示标志位
					{
						if(!(fieldTmp[0]-0x30))//0为可用
							ais_staticDataStruct->DTEIndicatorFlag = 1;
						else
							ais_staticDataStruct->DTEIndicatorFlag = 0;
						break;
					}
					default:;//其他不需要的内容
				}
				while(fieldTmpNum != 0)
				{
					fieldTmpNum --;
					fieldTmp[fieldTmpNum] = 0;
				}
				fieldNum ++;
			}			
			else//存放字段信息
			{
				fieldTmp[fieldTmpNum] = mkd_frameStruct->rcvBuffer[i];
				fieldTmpNum ++;
			}
		}
		
	}
	
	/************************************************************************
	* Name      : getBBMMsgInfo
	* Brief     : 将接收的一帧BBM格式数据保存到BBM结构体中
	*
	* Author    : Digital Design Team
	* Param     : mkd_frameStruct-MKD串口接收到的一帧数据数据结构体
	* Param     : ais_bbmMsgStruct-BBM报警消息相关的数据结构体
	* Return    : void
	************************************************************************/
	void getBBMMsgInfo(MKD_FrameStruct * mkd_frameStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct)
	{
		u8 fieldTmp[62];
		u8 fieldTmpNum = 0;
		u8 fieldNum = 0;
		u8 dataTmp = 0;
		u8 i,j;
		fieldTmpNum = 0;
		fieldNum = 0;
		for(i = 0; i < mkd_frameStruct->rcvBufferLength; i ++)
		{
			if(mkd_frameStruct->rcvBuffer[i] == 0x2C)//找到","
			{
				switch(fieldNum)
				{
					case 1://分段总数
					{
						ais_bbmMsgStruct->sentenceTotalNum = (fieldTmp[0]-0x30);
						break;
					}
					case 2://分段顺序号
					{
						dataTmp = (fieldTmp[0]-0x30);
						ais_bbmMsgStruct->sentenceOrderNum = dataTmp;
						if((dataTmp > 1 && dataTmp != (ais_bbmMsgStruct->sentenceOrderNum+1)) || (dataTmp > ais_bbmMsgStruct->sentenceTotalNum))
							ais_bbmMsgStruct->rcvStateFlag = RECEIVEWRONG;
						break;
					}
					case 3://顺序消息识别符
					{
						dataTmp = (fieldTmp[0]-0x30);
						if(ais_bbmMsgStruct->sentenceTotalNum == ais_bbmMsgStruct->sentenceOrderNum)//接收完成
						{
							if(ais_bbmMsgStruct->sentenceTotalNum == 1 || (ais_bbmMsgStruct->sentenceTotalNum > 1 && dataTmp == ais_bbmMsgStruct->sequentialMessageIdentifier))
							{
								ais_bbmMsgStruct->rcvStateFlag = RECEIVED;
								ais_bbmMsgStruct->sequentialMessageIdentifier = dataTmp;
								//给处增加接收的时隙号
							}
							else
							{
								ais_bbmMsgStruct->rcvStateFlag = RECEIVEWRONG;
								break;
							}
							
						}
						else if(ais_bbmMsgStruct->sentenceTotalNum > ais_bbmMsgStruct->sentenceOrderNum)//接收分段中
						{
							if(ais_bbmMsgStruct->sentenceOrderNum == 1 || (dataTmp == ais_bbmMsgStruct->sequentialMessageIdentifier))
							{
								ais_bbmMsgStruct->rcvStateFlag = RECEIVING;
								ais_bbmMsgStruct->sequentialMessageIdentifier = dataTmp;
							}
							else
							{
								ais_bbmMsgStruct->rcvStateFlag = RECEIVEWRONG;
								break;
							}
						}
						break;
					}
					case 4://广播通道
					{
						dataTmp = (fieldTmp[0]-0x30);
						if (dataTmp > 0)
						{
							ais_bbmMsgStruct->broadcastChannel = dataTmp - 1;
						}
						else
						{
							ais_bbmMsgStruct->broadcastChannel = 4;//未使用
						}
						break;
					}
					case 5://消息ID
					{
						if(fieldTmpNum == 2)//消息14
						{
							ais_bbmMsgStruct->msgID = (fieldTmp[0]-0x30)*10+(fieldTmp[1]-0x30);
						}
						else if(fieldTmpNum == 1)//消息8
						{
							ais_bbmMsgStruct->msgID = (fieldTmp[0]-0x30);
						}
						break;
					}
					case 6://封装消息内容
					{
						if(ais_bbmMsgStruct->sentenceOrderNum == 1 && ais_bbmMsgStruct->msgOldFlag == MSGOLD)//如果收到新的消息帧，且旧消息已经处理过了，则先清空数组，再重新赋值
						{
							while(ais_bbmMsgStruct->encapsulatedDataLength != 0)
							{
								ais_bbmMsgStruct->encapsulatedDataLength --;
								ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = 0;
							}
							ais_bbmMsgStruct->msgOldFlag = MSGNEW;
						}
						if(ais_bbmMsgStruct->rcvStateFlag != RECEIVEWRONG)//接收正常的情况下，保存数据到数组中
						{
							//此处将61162规定的6bitASCII码转换成1371规定的6bitASCII码对应的二进制流
							changeMKD6bitASCIItoBit(fieldTmpNum,fieldTmp,&ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength]);
							ais_bbmMsgStruct->encapsulatedDataLength += (fieldTmpNum * 6);
							/*for(j= 0;j<fieldTmpNum;j++)
							{
								ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = fieldTmp[j];
								ais_bbmMsgStruct->encapsulatedDataLength ++;
							}*/
						}
						if(ais_bbmMsgStruct->sentenceTotalNum == ais_bbmMsgStruct->sentenceOrderNum)//测试用，当接收完成，则可以重新接收新的一帧数据
							ais_bbmMsgStruct->msgOldFlag = MSGOLD;
						break;
					}
					default:;
				}
				while(fieldTmpNum != 0)//清空数据缓存中的信息
				{
					fieldTmpNum --;
					fieldTmp[fieldTmpNum] = 0;
				}
				fieldNum ++;
			}
			else
			{
				fieldTmp[fieldTmpNum] = mkd_frameStruct->rcvBuffer[i];
				fieldTmpNum ++;
			}
		}
	}

	/************************************************************************
	* Name      : rstMKDFrameStruct
	* Brief     : 清空MKD一帧数据结构体的内容
	*
	* Author    : Digital Design Team
	* Param     : mkd_frameStruct-MKD串口接收到的一帧数据数据结构体
	* Return    : void
	************************************************************************/
	//void rstMKDFrameStruct(MKD_FrameStruct * mkd_frameStruct)
	//{
	//	u8 i;
	//	mkd_frameStruct->receivingFlag = 0;
	//	mkd_frameStruct->receiveWrong = 0;
	//	mkd_frameStruct->receivedFlag = 0;
	//	for(i = 0;i <= mkd_frameStruct->rcvBufferLength; i++)
	//		mkd_frameStruct->rcvBuffer[i] = 0;
	//	mkd_frameStruct->rcvBufferLength = 0;
	//}


	////////////////for test ////////////////////////////////
	void testAIS_StaticStruct(AIS_StaticDataStruct * ais_staticDataStruct)
	{
		u8 i,j;
		ais_staticDataStruct->DTEIndicatorFlag = 1;
		i= 0;
		j = 42;
		while (i < j)
		{
			ais_staticDataStruct->shipCall[i] = 1;
			i ++;
		}
		i = 0;
		j = 120;
		while (i < j)
		{
			ais_staticDataStruct->shipName[i] = 1;
			i ++;
		}
		ais_staticDataStruct->shipSize = 2048;//
		ais_staticDataStruct->shipType = 99;
		setSupplierInfo(ais_staticDataStruct);
		//printf("\r\n");
		//printf("/////////testAIS_StaticStruct function start/////////////\r\n");
		//printf("DTEIndicatorFlag:   %d\n",ais_staticDataStruct->DTEIndicatorFlag);
		//printf("shipCall:			%x\n",ais_staticDataStruct->shipCall);
		//printf("shipName:			%x\n",ais_staticDataStruct->shipName);
		//printf("shipSize:			%d\n",ais_staticDataStruct->shipSize);
		//printf("shipType:			%d\n",ais_staticDataStruct->shipType);
		//printf("/////////testAIS_StaticStruct function end/////////////\r\n");
		//printf("\r\n");
	}
	
	void testAIS_BBMMsgStruct(AIS_BBMMsgStruct * ais_bbmMsgStruct)
	{
		u8 lenTmp = 96;
		ais_bbmMsgStruct->msgID = 14;
		ais_bbmMsgStruct->rcvSlot = 1250;
		ais_bbmMsgStruct->broadcastChannel = CHANNELA;
		while (ais_bbmMsgStruct->encapsulatedDataLength < lenTmp)
		{
			if (ais_bbmMsgStruct->encapsulatedDataLength % 4 == 0)
			{
				ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = 0;
			}
			else
			{
				ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = 1;
			}
			ais_bbmMsgStruct->encapsulatedDataLength ++;
		}
		//printf("\r\n");
		//printf("/////////testAIS_BBMMsgStruct function start/////////////\r\n");
		//printf("msgID:						%d\n",ais_bbmMsgStruct->msgID);
		//printf("rcvSlot:					%d\n",ais_bbmMsgStruct->rcvSlot);
		//printf("encapsulatedDataLength:     %d\n",ais_bbmMsgStruct->encapsulatedDataLength);
		//printf("encapsulatedData:			%x\n",ais_bbmMsgStruct->encapsulatedData);
		//printf("/////////testAIS_BBMMsgStruct function end/////////////\r\n");
		//printf("\r\n");

	}


