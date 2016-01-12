/********************************************************************
* @file      APP.C
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/21   10:14
********************************************************************
* @brief     UCOS初始化、任务定义
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************/
#include "includes.h"
#include "LED.h"
#include "APP.h"
#include "stdio.h"
#include "UART1.h"
#include "UART2.h"
#include "UART3.h"
#include "SPI.h"
#include "RNG.h"
#include "ucos_ii.h"
#include "AIS_PS_Interface.h"
#include "MKDControl.h"

#define LED_TASK_PRIO 60
#define LED_STK_SIZE  64
OS_STK  LED_TASK_STK[LED_STK_SIZE];/****LED任务*******/

#define FLOAT_TASK_PRIO   20
#define FLOAT_STK_SIZE 128
OS_STK  FLOAT_TASK_STK[FLOAT_STK_SIZE];/****浮点任务****/

#define AIS_Tran_PRIO   10
#define AIS_TRAN_STK_SIZE 128
OS_STK  AIS_TASK_STK[AIS_TRAN_STK_SIZE];/**AIS接收缓存任务**/

#define GPS_Tran_PRIO   11
#define GPS_TRAN_STK_SIZE 128
OS_STK  GPS_TASK_STK[GPS_TRAN_STK_SIZE];/*GPS接收并解析任务*/

#define AIS_Analysis_PRIO 12
#define AIS_Analysis_STK_SIZE 128
OS_STK  AIS_Analysis_TASK_STK[AIS_Analysis_STK_SIZE];/*SPI数据解析并归类任务*/

#define AIS_MKD_PRIO 13
#define AIS_MKD_STK_SIZE 256
OS_STK	AIS_MKD_TASK_STK[AIS_MKD_STK_SIZE];/*上位机MKD封装AIS中产生的它船和自船消息为61162格式任务*/

#define AIS_MKD_TRANS_PRIO 14
#define AIS_MKD_TRANS_STK_SIZE 128
OS_STK AIS_MKD_TRANS_TASK_STK[AIS_MKD_TRANS_STK_SIZE];/*向上位机上报消息队列中缓存的消息任务（上报AIS消息及GPS消息）*/

#define GPS_MKD_TRANS_PRIO 15
#define GPS_MKD_TRANS_STK_SIZE 128
OS_STK GPS_MKD_TRANS_TASK_STK[AIS_MKD_TRANS_STK_SIZE];

/*  SPI AIS数据消息队列  */
OS_EVENT *QSem;//定义消息队列指针
void *MsgQeueTb[MSG_QUEUE_TABNUM];//定义消息指针数组，长度为20
//OS_MEM   *PartitionPt;//定义内存分区指针
uint8_t  Partition[MSG_QUEUE_TABNUM][400];

/*  AIS 向上位机上报61162消息的消息队列    */
OS_EVENT *Q_mkd_ais;//定义mkd消息队列指针
void * MsgQ_Mkd[MSG_QUEUE_MKD_NUM];//存放消息结构体首地址的指针数组
u8 MsgQ_Mkd_Buf[MSG_QUEUE_MKD_NUM][200];//存放消息内容封装内容的二维数据，其他的辅助性内容暂时不添加

OS_EVENT *Q_mkd_GPS;//定义GPS消息队列指针
void *MsgQ_GPS[MSG_QUEUE_MKD_NUM];
u8 MsgQ_GPS_Buf[MSG_QUEUE_MKD_NUM][800];//存放GPS的二维数组

OS_FLAG_GRP *AIS_FLAG;//定义SPI2(AIS数据)标志组指针
OS_FLAG_GRP *GPS_FLAG;//定义UART2(GPS数据)标志组指针
//OS_FLAG_GRP *UART1_TX_FLAG;//定义UART1发送标志组指针
OS_EVENT *UART1_Tx_FLAG;//定义UART发送信号量指针
OS_EVENT *Tx_Mkd_Mutex;//定义GPS和AIS上报的互斥信号量指针

/**********************************************************************************
* Name      : ais_tran_task
* Brief     : 将SPI2 FIFO中的AIS数据轮流读出来
*
* Param     : *pdata
* Return    : void
***********************************************************************************/
void ais_tran_task(void *pdata)
{    OS_CPU_SR cpu_sr=0;
	u32 i=0;
	INT8U err;
	while (1)
	{
		OSFlagPend(AIS_FLAG,0x01,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);
//		OS_ENTER_CRITICAL();//关闭全局中断
			if(DMA_GetCurrentMemoryTarget(DMA1_Stream3) == 1)
			{
				for(i=0;i<SPI2_RX_LEN;i++)
			     {Putc_UART1(SPI2_RX0[i]);}
				 Putc_UART1('a');
			}
			else
			{
				for(i=0;i<SPI2_RX_LEN;i++)
			     {Putc_UART1(SPI2_RX1[i]);}
				 Putc_UART1('b');
			}
			//OSQPost(QSem,(void*)SPI2_RX1);//发送到消息队列缓存
//			OS_EXIT_CRITICAL();//开启全局中断
		OSTimeDly(3);
	}
}
/**********************************************************************************
* Name      : ais_analysis_task
* Brief     : 将AIS消息队列里面的数据解析出来并且分类打包
*
* Param     : *pdata
* Return    : void
***********************************************************************************/
void ais_analysis_task(void *pdata)
{
	u8 *s;u8 err;
	s = OSQPend(QSem,0,&err);//从消息队列中获取消息
	//SIG_PS_FPGA_ParseRecData(s);
}
/**********************************************************************************
* Name      : gps_tran_task
* Brief     : 将UART2 FIFO中的GPS数据轮流读出来，并使用Get_GPS_RMCMsg解析
              GPSRMC数据放到GPS_RMC_Message结构体
* Param     : *pdata
* Return    : void
***********************************************************************************/
void gps_tran_task(void *pdata)
{ OS_CPU_SR cpu_sr=0;
	u32 i=0,GPS_length;static u8 MsgQ_GPS_Num;
	INT8U err;
	GPS_RMCMsgStruct GPS_RMC_Message;
	while (1)
	{
		OSFlagPend(GPS_FLAG,0x01,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);
		//OS_ENTER_CRITICAL();//关闭全局中断
		GPS_length=Uart2_GPS1_LEN-DMA_GetCurrDataCounter(DMA1_Stream5);	
//		for(i=1;i<GPS_length;i++)
//		{
//			//if(Uart2_GPS1[i]=='$')
//			 //  break;
//			Putc_UART3(Uart2_GPS1[i]);
//		}
//		Get_GPS_RMCMsg(Uart2_GPS1,&GPS_RMC_Message,i);
			getGPSInfomation(Uart2_GPS1,500,&gps_infoStruct);//提取GPS串口中的RMC信息帧到gps信息结构体中

			//将GPS的1s间隔接收到的字节流保存到消息队列中
			MsgQ_GPS_Buf[MsgQ_GPS_Num][0] = GPS_length / 255;//第一个字节记录消息内容的长度
			MsgQ_GPS_Buf[MsgQ_GPS_Num][1] = GPS_length % 255;
			printf("\r\nRcv GPS_length:%d\r\n",GPS_length);
		     
			for(i = 0; i < GPS_length; i ++)
			{
				MsgQ_GPS_Buf[MsgQ_GPS_Num][i+2] = Uart2_GPS1[i];
			}
			OSQPost(Q_mkd_GPS,&MsgQ_GPS_Buf[MsgQ_GPS_Num]);
			MsgQ_GPS_Num ++;
			MsgQ_GPS_Num %= MSG_QUEUE_MKD_NUM;
			//加入消息队列结束
		  DMA_Cmd(DMA1_Stream5, ENABLE);
			//OS_EXIT_CRITICAL();//开启全局中断
		  OSTimeDly(10);
	}
}

/************************************************************************
* Name      : mkd_control_task
* Brief     : 将从VDL层接收的它船消息或本船消息封装成61162协议格式，并缓冲到消息队列中
*             相关消息队列 为：Q_mkd，最多可以缓冲 MSG_QUEUE_MKD_NUM 条消息
* Author    :
* Param     :
* Return    : void
************************************************************************/
void mkd_control_task(void *pdata)
{
	u16 j = 0;u8 err;
	static u8 MsgQ_Mkd_Num;//用于记录当前存放消息的二维数据的更新偏置，在0~MSG_QUEUE_MKD_NUM-1之间循环
	while(1)
	{

		MsgQ_Mkd_Buf[MsgQ_Mkd_Num][0] = 0;//第一个字节记录消息内容的长度
		MsgQ_Mkd_Buf[MsgQ_Mkd_Num][1] = mkd_dataStruct.mkd_framedStruct[0].mkd_encapDataByteLen;//第一个字节记录消息内容的长度	
		for(j = 0; j < mkd_dataStruct.mkd_framedStruct[0].mkd_encapDataByteLen; j ++)
		{//printf("@");
			//将封装消息存放到二维数组中
			MsgQ_Mkd_Buf[MsgQ_Mkd_Num][j+2] = mkd_dataStruct.mkd_framedStruct[0].mkd_encapDataByte[j];
		}
		OSQPost(Q_mkd_ais,&MsgQ_Mkd_Buf[MsgQ_Mkd_Num]);
		
		MsgQ_Mkd_Num ++;
		MsgQ_Mkd_Num %= MSG_QUEUE_MKD_NUM;
		OSTimeDly(10);
	}
}

/************************************************************************
* Name      : mkd_trans_task
* Brief     : 将从VDL层接收的它船消息或本船消息封装成61162协议格式，并缓冲到消息队列中
*             相关消息队列 为：Q_mkd，在该消息中阻塞处理消息队列中的缓存，并发送到UART1上
* Author    :
* Param     :
* Return    : void
************************************************************************/
void ais_mkd_trans_task(void *pdata)
{
	INT8U err;
	uint8_t * pbuf;
	u16 length = 0;
	u16 i = 0;
	OS_CPU_SR  cpu_sr;
	while(1)
	{	
		pbuf = OSQPend(Q_mkd_ais,0,&err);//没有新消息，则一直等待
		//OS_ENTER_CRITICAL();
		length = pbuf[0] * 255 + pbuf[1];
		if(length!=0)
		{
			OSMutexPend(Tx_Mkd_Mutex,0,&err);
			OSSemPend(UART1_Tx_FLAG,0,&err);//在此等待上次发送完成。
			for(i = 0; i < length; i ++)
			{//Putc_UART1(pbuf[i+2]);
				Uart1_Tx1[i]=pbuf[i+2];//使用串口1将数据输出
			}
				DMA2_Stream7->NDTR = length;
				DMA_Cmd(DMA2_Stream7,ENABLE);
			  OSMutexPost(Tx_Mkd_Mutex);
		}
		//OS_EXIT_CRITICAL();
		OSTimeDly(10);
	}
}
/************************************************************************
* Name      : gps_trans_task
* Brief     : 将从VDL层接收的它船消息或本船消息封装成61162协议格式，并缓冲到消息队列中
*             相关消息队列 为：Q_GPS，在该消息中阻塞处理消息队列中的缓存，并发送到UART1上
* Author    :
* Param     :
* Return    : void
************************************************************************/
void gps_mkd_trans_task(void *pdata)
{
	INT8U err;
	uint8_t * pbuf;
	u16 length = 0;
	u16 i;
	while(1)
	{	
		pbuf=OSQPend(Q_mkd_GPS,0,&err);//没有新消息，则一直等待
		length=pbuf[0]*255+pbuf[1];
    if(length!=0)
		{
			OSMutexPend(Tx_Mkd_Mutex,0,&err);
			OSSemPend(UART1_Tx_FLAG,0,&err);//在此等待上次发送完成。
			for(i = 0; i < length; i ++)
			{//Putc_UART1(pbuf[i+2]);
				Uart1_Tx1[i]=pbuf[i+2];//使用串口1将数据输出
			}
			DMA2_Stream7->NDTR = length;//改变传输长度
			DMA_Cmd(DMA2_Stream7,ENABLE);//打开DMA传输
			OSMutexPost(Tx_Mkd_Mutex);//释放互斥信号量
		}
		//OS_EXIT_CRITICAL();
		OSTimeDly(10);
	}
}
/**********************************************************************************
* Name      : led_task
* Brief     : 最低优先级，LED流水灯指示系统工作状态
*
* Param     : *pdata
* Return    : void
***********************************************************************************/
void led_task(void *pdata)
{
  while(1)
	{
    GPIO_SetBits(GPIOC,GPIO_Pin_6);OSTimeDly(300);
		GPIO_SetBits(GPIOC,GPIO_Pin_7);OSTimeDly(300);
		GPIO_SetBits(GPIOC,GPIO_Pin_8);OSTimeDly(300);
		GPIO_SetBits(GPIOC,GPIO_Pin_9);OSTimeDly(300);
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);OSTimeDly(300);
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);OSTimeDly(300);
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);OSTimeDly(300);
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);OSTimeDly(300);
		//printf("\r\nRNG: %d",RNG_Get_RandomRange(0,255));
	}
}
/**********************************************************************************
* Name      : float_task
* Brief     : 浮点运算测试任务
*
* Param     : *pdata
* Return    : void
***********************************************************************************/
void float_task(void *pdata)
{
   OS_CPU_SR cpu_sr=0;
   static float float_num=0.01;
   while(1) 
	 {  
		float_num+=0.01f; 
		//OS_ENTER_CRITICAL(); // 开中断
		//OS_EXIT_CRITICAL(); // 关中断
		OSTimeDly(500); 
	 }	
}

/*********************************************************************************
* Name      : App_TaskStart
* Brief     : UCOS初始化、各项任务初始化、开启操作系统
*
* Param     : *pdata
* Return    : void
***********************************************************************************/
void App_TaskStart(void)//
{
	INT8U err;
  OSInit();	//
	//OSStatInit();//开启统计任务，OS_TASK_STAT_EN=1;统计信息存放在OSCPUsage。
	AIS_FLAG=OSFlagCreate(0,&err);//创建SPI2(AIS数据)标志组指针
	GPS_FLAG=OSFlagCreate(0,&err);//创建UART2(GPS数据)标志组指针
	//UART1_TX_FLAG=OSFlagCreate(0,&err);//创建UART1发送标志组指针
	UART1_Tx_FLAG=OSSemCreate(1);//创建UART1_Tx信号量
	Q_mkd_ais = OSQCreate(&MsgQ_Mkd[0],MSG_QUEUE_MKD_NUM);//创建和UART1(上位机相关）消息队列指针
  Q_mkd_GPS = OSQCreate(&MsgQ_GPS[0],MSG_QUEUE_MKD_NUM);//创建GPS消息队列
	Tx_Mkd_Mutex=OSMutexCreate(9,&err);//创建GPS和AIS上报数据的互斥信号量，使用时优先级为9
	//OSSemPost(UART1_Tx_FLAG);//UART1在此发送一次信号量才能触发第一次发送
	
	OSTaskCreate(led_task,  (void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],  LED_TASK_PRIO);
	OSTaskCreate(float_task,(void*)0,(OS_STK*)&FLOAT_TASK_STK[LED_STK_SIZE-1],FLOAT_TASK_PRIO);
	//OSTaskCreate(ais_tran_task, (void*)0,(OS_STK*)&AIS_TASK_STK[AIS_TRAN_STK_SIZE-1],AIS_Tran_PRIO);
	OSTaskCreate(gps_tran_task, (void*)0,(OS_STK*)&GPS_TASK_STK[GPS_TRAN_STK_SIZE-1],GPS_Tran_PRIO);

	//OSTaskCreate(ais_analysis_task,(void*)0,(OS_STK*)&AIS_Analysis_TASK_STK[AIS_Analysis_STK_SIZE-1],AIS_Analysis_PRIO);
  OSTaskCreate(mkd_control_task,(void*)0,(OS_STK*)&AIS_MKD_TASK_STK[AIS_MKD_STK_SIZE-1],AIS_MKD_PRIO);
	OSTaskCreate(ais_mkd_trans_task,(void*)0,(OS_STK*)&AIS_MKD_TRANS_TASK_STK[AIS_MKD_TRANS_STK_SIZE-1],AIS_MKD_TRANS_PRIO);
	OSTaskCreate(gps_mkd_trans_task,(void*)0,(OS_STK*)&GPS_MKD_TRANS_TASK_STK[GPS_MKD_TRANS_STK_SIZE-1],GPS_MKD_TRANS_PRIO);
	OSStart();	
}



















