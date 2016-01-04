#include "timer.h"
#include "stm32f10x_tim.h"
#include <stm32f10x.h>
#include "UART.h"
#include "misc.h"
#include "IWDG.h"
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		 
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  ); 
		//printf("%c",'a');			
			IWDG_Feed();	
  }   

		
}
// void TIM1_UP_IRQHandler(void)
// { 		 
// 	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
// 	{
// 		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  ); 
// 		printf("%c",'B');			    				   				     	    	
//  	}				    	    
// }
//ͨ�ö�ʱ��4�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM4_Int_Init(void)
{	
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  NVIC_InitTypeDef         NVIC_InitStructure;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
    TIM_TimeBaseStructure.TIM_Period=4999;//�����Զ���װ�ؼĴ�������ֵ		 								
    TIM_TimeBaseStructure.TIM_Prescaler= 7199;//����ʱ��Ƶ�ʳ���Ԥ��Ƶֵ10KHz������������5000Ϊ500ms				    
    //Tout=((4999+1)*(7199+1))/72=500000us=500ms
	  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM3
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_Cmd(TIM4, ENABLE);	
}
// void TIM1_Int_Init(void)
// {	
// 		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
// 	  NVIC_InitTypeDef         NVIC_InitStructure;
// 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
// 		
//     TIM_TimeBaseStructure.TIM_Period=7199;		 								
//     TIM_TimeBaseStructure.TIM_Prescaler= 4999;			    
//     TIM_TimeBaseStructure.TIM_ClockDivision=0; 		
//     TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
// 		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//     TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
// 	  TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );
// 	  TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM1�ж�,��������ж�

// 		NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //TIM3
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
// 		NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
// 		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
// 		NVIC_Init(&NVIC_InitStructure);
// 		
// 		TIM_Cmd(TIM1, ENABLE);	
// }














