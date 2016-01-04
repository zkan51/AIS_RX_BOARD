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

#define LED_TASK_PRIO 60
#define LED_STK_SIZE  64
OS_STK  LED_TASK_STK[LED_STK_SIZE];

#define FLOAT_TASK_PRIO   15
#define FLOAT_STK_SIZE 128
OS_STK  FLOAT_TASK_STK[FLOAT_STK_SIZE];

#define AIS_Tran_PRIO   10
#define AIS_TRAN_STK_SIZE 128
OS_STK  AIS_TASK_STK[AIS_TRAN_STK_SIZE];

#define GPS_Tran_PRIO   11
#define GPS_TRAN_STK_SIZE 128
OS_STK  GPS_TASK_STK[GPS_TRAN_STK_SIZE];

OS_FLAG_GRP *AIS_FLAG;//定义SPI2(AIS数据)标志组指针
OS_FLAG_GRP *GPS_FLAG;//定义UART2(GPS数据)标志组指针

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
//			OS_EXIT_CRITICAL();//开启全局中断
//			SPI2_DMA_Tran=0;
		
		OSTimeDly(3);
	}
}
/**********************************************************************************
* Name      : gps_tran_task
* Brief     : 将UART2 FIFO中的GPS数据轮流读出来
*
* Param     : *pdata
* Return    : void
***********************************************************************************/
void gps_tran_task(void *pdata)
{    OS_CPU_SR cpu_sr=0;
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
			printf("\r\n%d",GPS_RMC_Message.utcTime);
			printf("\r\n%d",GPS_RMC_Message.status);
			printf("\r\n%d",GPS_RMC_Message.latitudeH);
			printf("\r\n%d",GPS_RMC_Message.latitudeL);
			printf("\r\n%d",GPS_RMC_Message.longitudeDir);
			printf("\r\n%d",GPS_RMC_Message.longitudeH);
			printf("\r\n%d",GPS_RMC_Message.longitudeL);
			printf("\r\n%d",GPS_RMC_Message.longitudeDir);
			printf("\r\n%d",GPS_RMC_Message.sog);
			printf("\r\n%d",GPS_RMC_Message.cog);
			printf("\r\n%d",GPS_RMC_Message.date);
			printf("\r\n%d",GPS_RMC_Message.modeIndicator);
			printf("\r\n%d",GPS_RMC_Message.magneticVar);
			printf("\r\n%d",GPS_RMC_Message.magneticVarDir);

		
		
		DMA_Cmd(DMA1_Stream5, ENABLE);
//			if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 1)
//			{
//				for(i=0;i<GPS_length;i++)
//			     {Putc_UART1(Uart2_GPS1[i]);}
//				 Putc_UART1(0x0D);
//				 Putc_UART1(0x0A);
//				for(i=0;i<30;i++)
//					 Putc_UART1(0x2A);
//					  Putc_UART1('a');
//			}
//			else
//			{
//				for(i=0;i<GPS_length;i++)
//			     {Putc_UART1(Uart2_GPS2[i]);}
//				 			 Putc_UART1(0x0D);
//				 Putc_UART1(0x0A);
//				for(i=0;i<30;i++)
//					 Putc_UART1(0x2A);
//					 Putc_UART1('b'); 
//			}
			//OS_EXIT_CRITICAL();//开启全局中断
		  OSTimeDly(3);
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
    GPIO_SetBits(GPIOC,GPIO_Pin_6);OSTimeDly(1000); 
		GPIO_SetBits(GPIOC,GPIO_Pin_7);OSTimeDly(1000);
		GPIO_SetBits(GPIOC,GPIO_Pin_8);OSTimeDly(1000); 
		GPIO_SetBits(GPIOC,GPIO_Pin_9);OSTimeDly(1000); 
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);OSTimeDly(1000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_7);OSTimeDly(1000);   
	  GPIO_ResetBits(GPIOC,GPIO_Pin_8);OSTimeDly(1000);	
    GPIO_ResetBits(GPIOC,GPIO_Pin_9);OSTimeDly(1000); 
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
		//printf("\r\n%d",float_num); //串口打印结果
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
	OSStart();	
}





















