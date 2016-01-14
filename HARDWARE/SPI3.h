#ifndef __SPI3_H
#define __SPI3_H
#include "stm32f4xx.h" 

#define SPI3_TX_LEN 76	
extern u8 SPI3_TX0[];
extern u8 SPI3_TX1[];
//extern u8 SPI2_DMA_Tran;	
//#define FIFO_OK                  0
//#define FIFO_ERROR_PARAM        -1
//#define FIFO_ERROR_MEM          -2
//#define FIFO_ERROR_FULL         -3
//#define FIFO_ERROR_EMPTY        -4
//#define FIFO_ERROR_BUSY         -5

//typedef struct _FIFO_TYPE_
//{
//        u32 size;         //FIFO缓冲区大小
//        u32 front;        //FIFO下一读取位置
//        u32 staraddr;     //FIFO缓冲区起始地址
//        u32 endaddr;      //FIFO缓冲区结束地址
//        u32 buffer[1];    //实际长度由初始化分配内存!(memloc的时候确定)
//}FIFOTYPE;




void SPI3_Init(void);			 //SPI3初始化
void SPI3_SetSpeed(u8 SpeedSet); //设置SPI3时钟   
u8 SPI3_ReadWriteByte(u8 TxData);//SPI3读写一个字节
		 
#endif

