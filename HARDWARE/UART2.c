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

GPS_FrameStruct gps_frameStruct;
GPS_InfoStruct gps_infoStruct;
GPS_RMCMsgStruct gps_RMCMsgStruct;
GPS_VTGMsgStruct gps_VTGMsgStruct;
	
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
//    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
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
		  //DMA_Cmd(DMA1_Stream5,DISABLE); 
		  GPS_length = USART2->SR;  
 		  GPS_length = USART2->DR; //清除标志位 
 			GPS_length = Uart2_GPS1_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);	
//    for(i=0;i<GPS_length;i++)
//		{
////// 		Message_AIS[i]=Uart_Rx[i];//Message_AIS++;
// 				Putc_UART1(Uart2_GPS1[i]);
//    }
//    if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 1)
//		{DMA_MemoryTargetConfig(DMA1_Stream5,(u32)&Uart2_GPS1,DMA_Memory_0);}
//		else
//		{DMA_MemoryTargetConfig(DMA1_Stream5,(u32)&Uart2_GPS2,DMA_Memory_1);}
//    DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);	
//		OSFlagPost(GPS_FLAG,0x01,OS_FLAG_SET,&err);
			printf("\r\n%d\r\n",GPS_length);				 
		  //DMA_Cmd(DMA1_Stream5, ENABLE);//处理完,重开DMA   
	}
	__nop();   
}

void  Putc_UART2(u8 ch)
{
	while((USART2->SR&0X40)==0);
	USART_SendData(USART2, ch);	
}
/************************************************************************
* Name      : Get_GPS_RMCMsg
* Brief     : 将接收的一帧RMC格式数据保存到RMC结构体中
*
* Author    : Digital Design Team && Embedded Team
* Param     : buff            -GPS串口接收到的一帧数据结构体
* Param     : gps_RMCMsgStruct-RMC数据结构体
* Return    : void
************************************************************************/
void Get_GPS_RMCMsg(u8 *buff,GPS_RMCMsgStruct *gps_RMCMsgStruct,u16 length)
{
	u8 fieldTmp[20];
	u8 fieldTmpNum = 0;
	u8 Comma_Num = 0;
	u32 dataTmp = 0;
	u16 i;
	for(i = 0; i < length; i ++)
	{
		if(buff[i] == 0x2C || buff[i] == 0x2A)//检测到字段分隔符或校验和标志符
		{
			switch(Comma_Num)//根据字段号进行相应的操作
			{
				case 1://utc时间 hhmmss.ss
				{
					gps_RMCMsgStruct->utcTime = (((fieldTmp[0]-0x30)*10+(fieldTmp[1]-0x30))<<24)+
					                            (((fieldTmp[2]-0x30)*10+(fieldTmp[3]-0x30))<<16)+
												(((fieldTmp[4]-0x30)*10+(fieldTmp[5]-0x30))<<8)+
												((fieldTmp[7]-0x30)*10+(fieldTmp[8]-0x30));
				}
				case 2://状态 A-data valid V-navigation receiver warning导航接收机报警
				{
					gps_RMCMsgStruct->status = fieldTmp[0];
				}
				case 3://纬度
				{
					gps_RMCMsgStruct->latitudeL = (fieldTmp[5]-0x30) * 10000 + 
					                              (fieldTmp[6]-0x30) * 1000 + 
					                              (fieldTmp[7]-0x30) * 100 + 
					                              (fieldTmp[8]-0x30) * 10 + 
					                              (fieldTmp[9]-0x30); //小数分
					gps_RMCMsgStruct->latitudeH = (((fieldTmp[0]-0x30)*10+
					                              (fieldTmp[1]-0x30))<<8)+
					                              ((fieldTmp[2]-0x30)*10+
					                              (fieldTmp[3]-0x30));  //分别为度+分																		
				}
				case 4://纬度方向
				{
					gps_RMCMsgStruct->latitudeDir = fieldTmp[0]; //存的8bit ASCII码字符
				}
				case 5://经度
				{
					gps_RMCMsgStruct->longitudeL = (fieldTmp[6]-0x30) * 10000 + 
					                               (fieldTmp[7]-0x30) * 1000 + 
					                               (fieldTmp[8]-0x30) * 100 + 
					                               (fieldTmp[9]-0x30) * 10 + 
					                               (fieldTmp[10]-0x30);
					gps_RMCMsgStruct->longitudeH = (((fieldTmp[0]-0x30)*100+
					                               (fieldTmp[1]-0x30)*10 + 
					                               (fieldTmp[2]-0x30))<<8) +
								                   ((fieldTmp[3]-0x30)*10+
					                               (fieldTmp[4]-0x30));																		 
				}
				case 6://经度方向
				{
					gps_RMCMsgStruct->longitudeDir = fieldTmp[0];
				}
				case 7://地面航速
				{
					
				}
				case 8://地面航线
				{
					
				}
				case 9://日期 ddmmyy
				{
					gps_RMCMsgStruct->date = (((fieldTmp[0]-0x30)*10 + 
					                         (fieldTmp[1]-0x30))<<16)+
					
											 (((fieldTmp[2]-0x30)*10 + 
					                         (fieldTmp[3]-0x30))<<8)+
					
											 ((fieldTmp[4]-0x30)*10 + 
					                         (fieldTmp[5]-0x30));
				}
				case 10://磁变化
				{
				}
				case 11://磁变化方向
				{
					gps_RMCMsgStruct->magneticVarDir = fieldTmp[0];
				}
				case 12://模式指示器
				{
					gps_RMCMsgStruct->modeIndicator = fieldTmp[0];
				}
				default :;
			}
			while((fieldTmpNum) != 0)
			{
				fieldTmp[fieldTmpNum] = 0;
				fieldTmpNum --;
			}
			fieldTmp[0] = 0;
			Comma_Num ++;
		}
		else//接收字段数据内容
		{
			fieldTmp[fieldTmpNum++] = buff[i];
		}			
	}
}

///************************************************************************
//* Name      : rstGPSFrameStruct
//* Brief     : 清空GPS接收的一帧数据内容
//*
//* Author    : Digital Design Team
//* Param     : gps_frameStruct- GPS一帧数据内容的结构体
//* Return    : void
//************************************************************************/
//void rstGPSFrameStruct(GPS_FrameStruct * gps_frameStruct)
//{
//	u8 i;
//	gps_frameStruct->receivingFlag = 0;
//	gps_frameStruct->receiveWrong = 0;
//	gps_frameStruct->receivedFlag = 0;
//	for(i = 0;i <= gps_frameStruct->rcvBufferLength; i++)
//		gps_frameStruct->rcvBuffer[i] = 0;
//	gps_frameStruct->rcvBufferLength = 0;
//	
//}
//
///************************************************************************
//* Name      : rstGPSInfoStruct
//* Brief     : 清空GPS存储的综合数据内容 
//*
//* Author    : Digital Design Team
//* Param     : gps_infoStruct- gps综合数据结构体
//* Return    : void
//************************************************************************/
//void rstGPSInfoStruct(GPS_InfoStruct * gps_infoStruct)
//{
//	gps_infoStruct->longitude = 0;
//	gps_infoStruct->latitude = 0;
//	gps_infoStruct->SOG = 0;
//	gps_infoStruct->COG = 0;
//	gps_infoStruct->utcTime = 0;
//	gps_infoStruct->raimFlag = 0;
//}
//
///************************************************************************
//* Name      : changeASCToDigital
//* Brief     : 将ASCII码中对应的数字数据转换为十进制的数字数据
//*
//* Author    : Digital Design Team
//* Param     : data- GPS串行接收的ASCII码
//* Return    : u8
//************************************************************************/
//u8 changeASCToDigital(u8 data)
//{
//	u8 dataTmp = data;
//	if(0x30 <= data && data <= 0x39) //0~9对应的ASCII码值为30~39
//	{
//		dataTmp = (data - 0x30);
//	}
//	return dataTmp;
//}
//
///************************************************************************
//* Name      : getGPSRMCMsgInfo
//* Brief     : 将接收的一帧RMC格式数据保存到RMC结构体中
//*
//* Author    : Digital Design Team
//* Param     : gps_frameStruct-GPS串口接收到的一帧数据结构体
//* Param     : gps_RMCMsgStruct-RMC数据结构体
//* Return    : void
//************************************************************************/
//void getGPSRMCMsgInfo(GPS_FrameStruct * gps_frameStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct)
//{
//	u8 fieldTmp[20];
//	u8 fieldTmpNum = 0;
//	u8 fieldNum = 0;
//	u32 dataTmp = 0;
//	u8 i;
//	for(i = 0; i < gps_frameStruct->rcvBufferLength; i ++)
//	{
//		if(gps_frameStruct->rcvBuffer[i] == 0x2C || gps_frameStruct->rcvBuffer[i] == 0x2A)//检测到字段分隔符或校验和标志符
//		{
//			switch(fieldNum)//根据字段号进行相应的操作
//			{
//				case 1://utc时间 hhmmss.ss
//				{
//					gps_RMCMsgStruct->utcTime = (((fieldTmp[0]-0x30)*10+(fieldTmp[1]-0x30))<<24)+
//					                            (((fieldTmp[2]-0x30)*10+(fieldTmp[3]-0x30))<<16)+
//																		  (((fieldTmp[4]-0x30)*10+(fieldTmp[5]-0x30))<<8)+
//																			((fieldTmp[7]-0x30)*10+(fieldTmp[8]-0x30));
//				}
//				case 2://状态 A-data valid V-navigation receiver warning导航接收机报警
//				{
//					gps_RMCMsgStruct->status = fieldTmp[0];
//				}
//				case 3://纬度
//				{
//					gps_RMCMsgStruct->latitudeL = (fieldTmp[5]-0x30) * 10000 + (fieldTmp[6]-0x30) * 1000 + (fieldTmp[7]-0x30) * 100 + (fieldTmp[8]-0x30) * 10 + (fieldTmp[9]-0x30); //小数分
//					gps_RMCMsgStruct->latitudeH = (((fieldTmp[0]-0x30)*10+(fieldTmp[1]-0x30))<<8)+
//					                              ((fieldTmp[2]-0x30)*10+(fieldTmp[3]-0x30));  //分别为度+分
//																			
//				}
//				case 4://纬度方向
//				{
//					gps_RMCMsgStruct->latitudeDir = fieldTmp[0]; //存的8bit ASCII码字符
//				}
//				case 5://经度
//				{
//					gps_RMCMsgStruct->longitudeL = (fieldTmp[6]-0x30) * 10000 + (fieldTmp[7]-0x30) * 1000 + (fieldTmp[8]-0x30) * 100 + (fieldTmp[9]-0x30) * 10 + (fieldTmp[10]-0x30);
//					gps_RMCMsgStruct->longitudeH = (((fieldTmp[0]-0x30)*100+(fieldTmp[1]-0x30)*10 + (fieldTmp[2]-0x30))<<8) +
//																			   ((fieldTmp[3]-0x30)*10+(fieldTmp[4]-0x30));
//																			 
//				}
//				case 6://经度方向
//				{
//					gps_RMCMsgStruct->longitudeDir = fieldTmp[0];
//				}
//				case 7://地面航速
//				{
//					
//				}
//				case 8://地面航线
//				{
//					
//				}
//				case 9://日期 ddmmyy
//				{
//					gps_RMCMsgStruct->date = (((fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30))<<16)+
//																	 (((fieldTmp[2]-0x30)*10 + (fieldTmp[3]-0x30))<<8)+
//																	 ((fieldTmp[4]-0x30)*10 + (fieldTmp[5]-0x30));
//				}
//				case 10://磁变化
//				{
//				}
//				case 11://磁变化方向
//				{
//					gps_RMCMsgStruct->magneticVarDir = fieldTmp[0];
//				}
//				case 12://模式指示器
//				{
//					gps_RMCMsgStruct->modeIndicator = fieldTmp[0];
//				}
//				default :;
//			}
//			while((fieldTmpNum) != 0)
//			{
//				fieldTmp[fieldTmpNum] = 0;
//				fieldTmpNum --;
//			}
//			fieldTmp[0] = 0;
//			fieldNum ++;
//		}
//		else//接收字段数据内容
//		{
//			fieldTmp[fieldTmpNum++] = gps_frameStruct->rcvBuffer[i];
//		}
//			
//	}
//}
//
///************************************************************************
//* Name      : getGPSVTGMsgInfo
//* Brief     : 将接收的一帧VTG格式数据保存到VTG结构体中
//*
//* Author    : Digital Design Team
//* Param     : gps_frameStruct-GPS串口接收到的一帧数据结构体
//* Param     : gps_VTGMsgStruct-VTG数据结构体
//* Return    : void
//************************************************************************/
//void getGPSVTGMsgInfo(GPS_FrameStruct * gps_frameStruct,GPS_VTGMsgStruct * gps_VTGMsgStruct)
//{
//	u8 fieldTmp[20];
//	u8 fieldTmpNum = 0;
//	u8 fieldNum = 0;
//	u32 dataTmp = 0;
//	u8 i;
//	for(i = 0; i < gps_frameStruct->rcvBufferLength; i ++)
//	{
//		if(gps_frameStruct->rcvBuffer[i] == 0x2C || gps_frameStruct->rcvBuffer[i] == 0x2A)//检测到字段分隔符或校验和标志符
//		{
//			switch(fieldNum)//根据字段号进行相应的操作
//			{
//				case 1://地面航线，真实角度
//				{
//					
//				}
//				case 3://地面航线，磁角度
//				{
//					
//				}
//				case 5://地面航速，knots速度
//				{
//					
//																			
//				}
//				case 7://地面航速，km/h速度
//				{
//					
//				}
//				case 9://模式指示
//				{
//					gps_VTGMsgStruct->modeIndicator = fieldTmp[0];														 
//				}
//				default :;
//			}
//			while((fieldTmpNum) != 0)
//			{
//				fieldTmp[fieldTmpNum] = 0;
//				fieldTmpNum --;
//			}
//			fieldTmp[0] = 0;
//			fieldNum ++;
//		}
//		else//接收字段数据内容
//		{
//			fieldTmp[fieldTmpNum++] = gps_frameStruct->rcvBuffer[i];
//		}
//	}
//}
//
///************************************************************************
//* Name      : getGPSInfo
//* Brief     : 将接收的GPS串行数据，根据相应的消息帧存入到对应的结构体中
//*
//* Author    : Digital Design Team
//* Param     : gps_frameStruct-GPS串行数据结构体
//* Param     : gps_RMCMsgStruct-RMC消息结构体
//* Param     : gps_VTGMsgStruct-VTG消息结构体
//* Return    : void
//************************************************************************/
//void getGPSInfo(GPS_FrameStruct * gps_frameStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct,GPS_VTGMsgStruct * gps_VTGMsgStruct)
//{
//	u8 i;
//	char dataTmp[5];
//	for(i = 0; i < 5; i++)
//	{
//		dataTmp[i] = gps_frameStruct->rcvBuffer[i+1];
//	}
//	if(strcmp(dataTmp,"GPRMC")==0)//如果是RMC消息帧
//	{
//		getGPSRMCMsgInfo(gps_frameStruct,gps_RMCMsgStruct);
//	}
//	else if(strcmp(dataTmp,"GPVTG")==0)//
//	{
//		getGPSVTGMsgInfo(gps_frameStruct,gps_VTGMsgStruct);
//	}
//	else if(strcmp(dataTmp,"GPGGA")==0)//
//	{
//		
//	}
//	else if(strcmp(dataTmp,"GPGSA")==0)//
//	{
//	}
//	else if(strcmp(dataTmp,"GPGSV")==0)
//	{
//	}
//	else if(strcmp(dataTmp,"GPGLL")==0)
//	{
//	}
//	// to be added for the other msgs
//		
//	
//}

/////////////for test///////////////

void testGPS_InfoStruct(GPS_InfoStruct * gps_infoStruct)
{
	gps_infoStruct->broadBandFlag = 0;
	gps_infoStruct->COG = 2750;
	gps_infoStruct->commuFlag = 1;
	gps_infoStruct->deviceFlag = 1;
	gps_infoStruct->DSCFlag = 0;
	gps_infoStruct->HOG = 84;  //测试值
	gps_infoStruct->latitude = 1900;//0x21A31D8;纬度
	gps_infoStruct->longitude = 900;//0xD926F7D;经度
	gps_infoStruct->modeFlag = 0;
	gps_infoStruct->monitorFlag = 1;
	gps_infoStruct->msg22Flag = 0;
	gps_infoStruct->posAccurateFlag = 1;
	gps_infoStruct->raimFlag = 1;
	gps_infoStruct->SOG = 0;
	gps_infoStruct->utcTime = 50;
	gps_infoStruct->utctime[0] = 12;//月
	gps_infoStruct->utctime[1] = 31;//天
	gps_infoStruct->utctime[2] = 0;//12;//时 当前时间为12：50：00  
	gps_infoStruct->utctime[3] = 19;//50;//分
	gps_infoStruct->utctime[4] = 59;//0;秒


}

void testGPS_InfoStructPara(GPS_InfoStruct * gps_infoStruct,u32 lati,u32 longi,u8 utchh,u8 utcmm,u8 utcss)
{
	gps_infoStruct->broadBandFlag = 0;
	gps_infoStruct->COG = 2750;
	gps_infoStruct->commuFlag = 1;
	gps_infoStruct->deviceFlag = 1;
	gps_infoStruct->DSCFlag = 0;
	gps_infoStruct->HOG = 84;  //测试值
	gps_infoStruct->latitude = lati;//0x21A31D8;纬度
	gps_infoStruct->longitude = longi;//0xD926F7D;经度
	gps_infoStruct->modeFlag = 0;
	gps_infoStruct->monitorFlag = 1;
	gps_infoStruct->msg22Flag = 0;
	gps_infoStruct->posAccurateFlag = 1;
	gps_infoStruct->raimFlag = 1;
	gps_infoStruct->SOG = 0;
	gps_infoStruct->utcTime = 50;
	gps_infoStruct->utctime[0] = 12;//月
	gps_infoStruct->utctime[1] = 31;//天
	gps_infoStruct->utctime[2] = utchh;//0;//12;//时 当前时间为12：50：00  
	gps_infoStruct->utctime[3] = utcmm;//19;//50;//分
	gps_infoStruct->utctime[4] = utcss;//59;//0;秒	
}

