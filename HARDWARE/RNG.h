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
#ifndef  RNG_H
#define  RNG_H

#include "stm32f4xx.h" 

u8  RNG_Init(void);			//RNG 初始化
u32 RNG_Get_Random(void);//得到随机数
int RNG_Get_RandomRange(int min,int max);//生成[min,max]范围的随机数。







#endif

