/*********************************************************************
* @file      RNG.h
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/21   13:14
********************************************************************
* @brief     RNG随机数发生器代码
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
**********************************************************************/
#include "RNG.h"
/************************************************************************
* Name      : RNG_Get_RandomRange
* Brief     : 用于产生min~max之间的随机数
*
* Author    : Embedded Team
* Param     : min,max
* Return    : u32- min~max之间的随机数
************************************************************************/
static void delay_100us(void)
{
	u32 i,j;
	for(i=0;i<184;i++)
	  for(j=0;j<100;j++);
}
/************************************************************************
* Name      : RNG_Init
* Brief     : 随机数模块初始化
*
* Author    : Embedded Team
* Param     : None
* Return    : 0：RNG初始化成功  1：RNG初始化失败
************************************************************************/
u8 RNG_Init(void)
{
	u16 retry=0; 
	
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);//开启RNG时钟,来自PLL48CLK
	
	RNG_Cmd(ENABLE);	//使能RNG
	
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)	//等待随机数就绪
	{
		retry++;
		delay_100us();
	}
	if(retry>=10000)return 1;//随机数发生器工作不正常
	
	return 0;
}
/************************************************************************
* Name      : RNG_Get_Random
* Brief     : 用于产生0~0xFFFFFFFF之间的随机数
*
* Author    : Embedded Team
* Param     : None
* Return    : u32- 0~0xFFFFFFFF之间的随机数
************************************************************************/
u32 RNG_Get_Random(void)
{	 
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);	//等待随机数就绪  
	return RNG_GetRandomNumber();	
}
/************************************************************************
* Name      : RNG_Get_RandomRange
* Brief     : 用于产生min~max之间的随机数
*
* Author    : Embedded Team
* Param     : min,max
* Return    : u32- min~max之间的随机数
************************************************************************/
int RNG_Get_RandomRange(int min,int max)
{ 
   return RNG_Get_Random()%(max-min+1) +min;
}


















