#ifndef __APP_H
#define __APP_H

#include "stm32f4xx.h"
#include "includes.h"

extern u8 SPI2_RX0[];
extern u8 SPI2_RX1[];
//extern u8 SPI2_DMA_Tran;	

extern OS_FLAG_GRP *AIS_FLAG;//定义SPI2标志组指针
extern OS_FLAG_GRP *GPS_FLAG;//定义GPS标志组指针
extern OS_EVENT *UART1_Tx_FLAG;//OS_FLAG_GRP *UART1_TX_FLAG;//定义UART1发送标志组指针
/*SPI AIS数据消息队列*/
#define MSG_QUEUE_TABNUM 20
extern OS_EVENT *QSem;//
//extern OS_MEM   *PartitionPt;
extern uint8_t  Partition[MSG_QUEUE_TABNUM][400];
extern  void *MsgQeueTb[MSG_QUEUE_TABNUM];

/* AIS 向上位机上报61162消息的消息队列  */
#define MSG_QUEUE_MKD_NUM 20
extern OS_EVENT *Q_mkd_ais;
extern u8 MsgQ_Mkd_Buf[MSG_QUEUE_MKD_NUM][200];

void led_task(void *pdata);
void float_task(void *pdata);
void ais_task(void *pdata);
void App_TaskStart(void);
void mkd_control_task(void *pdata);
void mkd_trans_task(void *pdata);

#endif

