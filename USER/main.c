#include "stm32f4xx.h"                  // Device header
#include "LED.h"
#include "includes.h"
#include "APP.h"
#include "UART1.h"
#include "UART2.h"
#include "SPI.h"
#include "SPI3.h"
#include "TIMER2.h"
#include "RNG.h"
//#include "stdio.h"
#include "FSMControl.h"
#include "testFSMControl.h"
#include "test.h"
#include "test_MKD.h"
#include "MKDControl.h"
//系统时钟初始化1ms，作为ucos的心跳
void Systick_Init(u8 SYSCLK)
{
	u32 reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	reload=SYSCLK/8;		// 每秒钟的计数   单位K
	reload*=1000000/OS_TICKS_PER_SEC;//根据设定溢出时间
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK
}

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	Systick_Init(168);//168MHz下1ms中断一次

	LED_Init();		
	GPIO_SetBits(GPIOC,GPIO_Pin_2);//打开GPS电源，IO在LED里面设置
	UART1_Config(38400);//上位机数据传输接口
	UART2_Config(9600);//GPS数据传输接口
	TIM2_Int_Init();//结合UART2进行不定长数据接收
	UART3_Config(115200);//测试串口

	RNG_Init();
	SPI2_Init();//AIS数据传输接口
	SPI3_Init();//AIS数据传输接口
	
	MKDInit(&mkd_controlStruct,&mkd_dataStruct);
	testVDMFunction(1);//产生VDM的测试数据
	//FSMInit(&fsm_controlStruct);
	//testMsg20(2);
  App_TaskStart();//开启U/COS

	while(1);
}


