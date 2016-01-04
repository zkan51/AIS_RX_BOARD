#include "LED.h"
#include "stm32f4xx_gpio.h"

void LED_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8 | GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//≥ı ºªØGPS  IO
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}








































