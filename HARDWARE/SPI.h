#ifndef __SPI_H
#define __SPI_H
#include "stm32f4xx.h" 

#define SPI2_RX_LEN 50	
extern u8 SPI2_RX0[];
extern u8 SPI2_RX1[];
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




void SPI2_Init(void);			 //³õÊ¼»¯SPI1¿Ú
void SPI2_SetSpeed(u8 SpeedSet); //ÉèÖÃSPI1ËÙ¶È   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI1×ÜÏß¶ÁÐ´Ò»¸ö×Ö½Ú
		 
#endif

