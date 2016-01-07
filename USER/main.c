#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "includes.h"
#include "APP.h"
#include "UART1.h"
#include "UART2.h"
#include "SPI.h"
#include "TIMER2.h"
//#include "stdio.h"
#include "FSMControl.h"
#include "testFSMControl.h"
#include "test.h"
#include "test_MKD.h"
#include "MKDControl.h"

void Systick_Init(u8 SYSCLK)
{
//	NVIC_InitTypeDef NVIC_InitStructure;
	u32 reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	reload=SYSCLK/8;		// 每秒钟的计数   单位K
	reload*=1000000/OS_TICKS_PER_SEC;//根据设定溢出时间
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK
	
//  NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;//串口1中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//初始化
}

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	Systick_Init(168);//168MHz下1ms中断一次

	LED_Init();		
	GPIO_SetBits(GPIOC,GPIO_Pin_2);//打开GPS电源，IO在LED里面设置
	UART1_Config(115200);
	UART2_Config(9600);//GPS数据传输接口
	TIM2_Int_Init();
	UART3_Config(38400);//上位机数据传输接口
	//SPI2_Init();//AIS数据传输接口
	MKDInit(&mkd_controlStruct,&mkd_dataStruct);
	testVDMFunction(1);//产生VDM的测试数据
	//FSMInit(&fsm_controlStruct);
	//testMsg20(2);
	//printf("\n");
	//printf("all functions are tested over!!!!\r\n");
  App_TaskStart();

	while(1);
}


