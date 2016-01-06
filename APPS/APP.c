/**
********************************************************************
* @file      APP.C
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/21   10:14
********************************************************************
* @brief     UCOS初始化、任务定义
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "includes.h"
#include "LED.h"
#include "APP.h"
#include "stdio.h"
#include "UART1.h"
#include "UART2.h"
#include "SPI.h"
#include "RNG.h"
#include "ucos_ii.h"
#include "AIS_PS_Interface.h"
#define LED_TASK_PRIO 60
#define LED_STK_SIZE  64
OS_STK  LED_TASK_STK[LED_STK_SIZE];/****LED任务*******/

#define FLOAT_TASK_PRIO   15
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

OS_FLAG_GRP *AIS_FLAG;//定义SPI2(AIS数据)标志组指针
OS_FLAG_GRP *GPS_FLAG;//定义UART2(GPS数据)标志组指针

/*SPI AIS数据消息队列*/
OS_EVENT *QSem;//定义消息队列指针
void *MsgQeueTb[MSG_QUEUE_TABNUM];//定义消息指针数组，长度为20
OS_MEM   *PartitionPt;//定义内存分区指针
uint8_t  Partition[MSG_QUEUE_TABNUM][400];

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
			OSQPost(QSem,(void*)SPI2_RX1);//发送到消息队列缓存
//			OS_EXIT_CRITICAL();//开启全局中断
//			SPI2_DMA_Tran=0;
		
		OSTimeDly(3);
	}
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
	u32 i=0,GPS_length;
	INT8U err;
	GPS_RMCMsgStruct GPS_RMC_Message;
	while (1)
	{
		OSFlagPend(GPS_FLAG,0x01,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);
		//OS_ENTER_CRITICAL();//关闭全局中断
		GPS_length=Uart2_GPS1_LEN-DMA_GetCurrDataCounter(DMA1_Stream5);	
		for(i=1;i<GPS_length;i++)
		{
			if(Uart2_GPS1[i]=='$')
			   break;
			Putc_UART1(Uart2_GPS1[i]);
		}
			Get_GPS_RMCMsg(Uart2_GPS1,&GPS_RMC_Message,i);
			printf("\r\n%x",GPS_RMC_Message.utcTime);
//			printf("\r\n%d",GPS_RMC_Message.status);
//			printf("\r\n%d",GPS_RMC_Message.latitudeH);
//			printf("\r\n%d",GPS_RMC_Message.latitudeL);
//			printf("\r\n%d",GPS_RMC_Message.longitudeDir);
//			printf("\r\n%d",GPS_RMC_Message.longitudeH);
//			printf("\r\n%d",GPS_RMC_Message.longitudeL);
//			printf("\r\n%d",GPS_RMC_Message.longitudeDir);
//			printf("\r\n%d",GPS_RMC_Message.sog);
//			printf("\r\n%d",GPS_RMC_Message.cog);
//			printf("\r\n%d",GPS_RMC_Message.date);
//			printf("\r\n%d",GPS_RMC_Message.modeIndicator);
//			printf("\r\n%d",GPS_RMC_Message.magneticVar);
//			printf("\r\n%d",GPS_RMC_Message.magneticVarDir);
		DMA_Cmd(DMA1_Stream5, ENABLE);
			//OS_EXIT_CRITICAL();//开启全局中断
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
	SIG_PS_FPGA_ParseRecData(s);
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
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);OSTimeDly(1300); 
		printf("\r\nRNG: %d",RNG_Get_RandomRange(0,255));
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
/**********************************************************************************
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
	OSTaskCreate(led_task,  (void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],  LED_TASK_PRIO);
	OSTaskCreate(float_task,(void*)0,(OS_STK*)&FLOAT_TASK_STK[LED_STK_SIZE-1],FLOAT_TASK_PRIO);
	OSTaskCreate(ais_tran_task, (void*)0,(OS_STK*)&AIS_TASK_STK[AIS_TRAN_STK_SIZE-1],AIS_Tran_PRIO);
  OSTaskCreate(gps_tran_task, (void*)0,(OS_STK*)&GPS_TASK_STK[GPS_TRAN_STK_SIZE-1],GPS_Tran_PRIO);
	//OSTaskCreate(ais_analysis_task,(void*)0,(OS_STK*)&AIS_Analysis_TASK_STK[AIS_Analysis_STK_SIZE-1],AIS_Analysis_PRIO);
	OSStart();	
}






















