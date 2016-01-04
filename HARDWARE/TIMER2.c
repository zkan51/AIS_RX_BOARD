#include "TIMER2.h"
#include "UART2.h"
#include "misc.h"
#include "UART1.h"
#include "UART2.h"
#include "includes.h"

static	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
static	NVIC_InitTypeDef NVIC_InitStructure;
static	GPIO_InitTypeDef GPIO_InitStructure;
static  TIM_ICInitTypeDef  TIM2_ICInitStructure;
static  TIM_OCInitTypeDef  TIM2_OCInitStructure;

extern OS_FLAG_GRP *GPS_FLAG;//定义GPS标志组指针
static INT8U err;

static void TIM2_SlaveTimer_SetConfig(void)
{
  uint32_t tmpsmcr = 0;
  uint32_t tmpccmr1 = 0;
  uint32_t tmpccer = 0;

 /* Get the TIMx SMCR register value */
  tmpsmcr = TIM2->SMCR;
  /* Reset the Trigger Selection Bits */
  tmpsmcr &= ~TIM_SMCR_TS;
  /* Set the Input Trigger source */
  tmpsmcr |= TIM_TS_TI1F_ED;
  /* Reset the slave mode Bits */
  tmpsmcr &= ~TIM_SMCR_SMS;
  /* Set the slave mode */
  tmpsmcr |=((uint32_t)0x0004);
  /* Write to TIMx SMCR */
  TIM2->SMCR = tmpsmcr;

	/* Disable the Channel 1: Reset the CC1E Bit */
	tmpccer = TIM2->CCER;
	TIM2->CCER &= ~TIM_CCER_CC1E;
	tmpccmr1 = TIM2->CCMR1;    

	/* Set the filter */
	tmpccmr1 &= ~TIM_CCMR1_IC1F;
	tmpccmr1 |= ((0) << 4);

	/* Write to TIMx CCMR1 and CCER registers */
	TIM2->CCMR1 = tmpccmr1;
	TIM2->CCER = tmpccer;  	
}
//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2!
void TIM2_Int_Init(void)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM2时钟
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIOA0,1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//IO速率100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //下拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2); //PA0¸PA1复用定时器2
	
  TIM_TimeBaseInitStructure.TIM_Period =0xffffffff; 	//定时器分频 计数器加1为1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler=83;  //自动重装载值
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //先上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //时钟分频1（84MHz）
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//
	
	
	TIM_SelectSlaveMode(TIM2,TIM_SlaveMode_Reset);//TIM2配置成SLAVE RESET，模式
	TIM_SelectInputTrigger(TIM2,TIM_TS_TI1F_ED);//TIM2触发模式为ED
  TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);
	//TIM2_SlaveTimer_SetConfig();
	
	//输入捕获配置
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 选择输入端IC1映射到TI1上
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频，不分频
  TIM2_ICInitStructure.TIM_ICFilter = 0x0;//IC1F=0000 不配置滤波器，提高捕获响应速度
  //TIM_ICStructInit(&TIM2_ICInitStructure);
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	

//输出比较设置	
  TIM2_OCInitStructure.TIM_OCMode=TIM_OCMode_Timing;
	TIM2_OCInitStructure.TIM_Pulse=0x2710;
	TIM2_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM2_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	//TIM2_OCInitStructure.OCFastMode=
	TIM_OC2Init(TIM2,&TIM2_OCInitStructure);
	//TIM2->CCR2=0x2710;
  //TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Disable);

	
	TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE); //开启更新中断和CC1IE捕获中断
	TIM_Cmd(TIM2,ENABLE); //使能定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4; //抢占优先级4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
//捕获状态
//[7]:0,没有成功捕获（溢出）,1成功捕获
//[6]:0,还没有捕获到低电平;1已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位处理器来说,1us计数加1,溢出时间4294秒:)
//u8  TIM2CH1_CAPTURE_STA=0;	//输入捕获状态		    				
//u32	TIM2CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)

//u16 TIM2CH1_Rising=0;
//u8 TIM2CH1_Falling=1;
//u32 TIM2CH1_Counter;
//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 	
//		if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//溢出
//		{		printf(" Update"); 	    
//			//if(TIM2CH1_Rising)//已经捕获到高电平
//			{
//				//TIM2CH1_Rising++;printf("  %d",TIM2CH1_Rising);
//			}	 
//		}
//		if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
//		{	printf(" CC1");
////			if(TIM2CH1_Rising)		//捕获到一个下降沿 		
////			{	  			
////				TIM2CH1_Falling=1;		//标记成功捕获到一次高电平脉宽
////				TIM2CH1_Rising=0;
////			  TIM2CH1_Counter=TIM_GetCapture1(TIM2);//获取当前的捕获值.
////				//printf("\r\n%d",TIM2CH1_Rising*4294);
////				if(TIM2CH1_Counter>1000)
////				  printf(" %d",TIM2CH1_Counter);
////	 			TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
////			}
////      if(TIM2CH1_Falling)			//第一次捕获上升沿
////			{
////				TIM2CH1_Counter=0;			//清空
////				TIM2CH1_Falling=0;
////				TIM2CH1_Rising=1;		//标记捕获到上升沿
////				//TIM_Cmd(TIM2,ENABLE ); 	//使能定时器
////	 			TIM_SetCounter(TIM2,0);
////			TIM_ITConfig(TIM2,TIM_IT_CC1,DISABLE);
////			TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);
////	 			TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
////				//TIM_Cmd(TIM2,ENABLE ); 	//使能定时器
////			}		    
//		}	
    if(TIM_GetITStatus(TIM2,TIM_IT_CC2)!=RESET)
    {
			printf("\r\n****\r\n");//TIM_SetCounter(TIM2,0);
			//TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
			//TIM_ITConfig(TIM2,TIM_IT_CC2,DISABLE);
			DMA_Cmd(DMA1_Stream5, DISABLE);
		  OSFlagPost(GPS_FLAG,0x01,OS_FLAG_SET,&err);	
		}			
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2|TIM_IT_Update); //清除中断标志位  	
}













