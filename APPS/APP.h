#ifndef __APP_H
#define __APP_H

#include "stm32f4xx.h"
#include "includes.h"

extern u8 SPI2_RX0[];
extern u8 SPI2_RX1[];
//extern u8 SPI2_DMA_Tran;	

extern OS_FLAG_GRP *AIS_FLAG;//定义SPI2标志组指针
extern OS_FLAG_GRP *GPS_FLAG;//定义GPS标志组指针

void led_task(void *pdata);
void float_task(void *pdata);
void ais_task(void *pdata);
void App_TaskStart(void);

#endif

