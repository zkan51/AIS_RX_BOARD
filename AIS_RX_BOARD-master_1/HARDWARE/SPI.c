#include "spi.h"
#include "uart1.h"
#include "includes.h"
u8 SPI2_RX0[SPI2_RX_LEN];
u8 SPI2_RX1[SPI2_RX_LEN];
//u8 SPI2_DMA_Tran=0 ;
extern OS_FLAG_GRP *AIS_FLAG;//定义SPI2标志组指针
static INT8U err;
void SPI2_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;	
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//开启GPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//开启SPI2时钟

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2); //PB12¸复用为SPI2	NSS
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB13¸复用为SPI2  SCK
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB14¸复用为SPI2  MISO
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB15¸复用为SPI2  MOSI
	
  //GPIOB12,13,14,15初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB3~5¸´ÓÃ¹¦ÄÜÊä³ö	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推免输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	//SPI2初始化设置
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//复位SPI2
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//停止复位SPI2

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI2单向或者双向的数据模式：SPI设置成双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//设置为从模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI传输数据为8bit
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第二个跳变沿（上升或者下降数据被采样）
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;		//NSS由硬件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率分频值256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //初始化SPI2
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI2

	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//42MHz全速运行
	
	SPI_I2S_DMACmd(SPI2,SPI_DMAReq_Rx,ENABLE);
	
//中断配置

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//设置DMA，双缓冲
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream3);//DMA1 通道0，数据流3
	
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(SPI2->DR));//源地址 
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI2_RX0;   //目的地址0
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;       //外设到内存
	DMA_InitStructure.DMA_BufferSize = SPI2_RX_LEN;               //数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //内存增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//数据长度8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据长度8bit 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                //循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      // 优先级高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;        //使用FIFO
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
 /*DMA DOUBLE Buffer mode*///双缓冲模式
  DMA_DoubleBufferModeConfig(DMA1_Stream3,(u32)&SPI2_RX1,DMA_Memory_0);//Memory0首先传输
  DMA_DoubleBufferModeCmd(DMA1_Stream3,ENABLE);	
	DMA_Init(DMA1_Stream3, &DMA_InitStructure);//初始化
	
	//SPI2_DMA_Tran = 0;//此时没有数据准备完成
	
	DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);//开启传输完成中断
	DMA_ITConfig(DMA1_Stream3,DMA_IT_TE,ENABLE);//传输错误（双缓冲模式）

	//DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);
	
	DMA_Cmd(DMA1_Stream3,ENABLE);     //使能 	
}   
//SPI2速度设置
//SPI速度=FAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//FAPB2的时钟一般为84MHz
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断是否有效
	SPI2->CR1&=0XFFC7;//位3~5清零，用来设置波特率
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI2,ENABLE); //使能SPI2 
} 
//SPI2读写一个字节
//TxData:要写入的数据
//返回值：SPI2读取到的数据
u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过SPI2发送一个字节
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个字节  
 
	return SPI_I2S_ReceiveData(SPI1); //返回收到的数据				    
}

//DMA传输完成中断
void DMA1_Stream3_IRQHandler(void)
{
	  // DMA_InitTypeDef DMA_InitStructure;
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3) != RESET)
	{
	   DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);
	  // printf("\r\nSPI2");
	   //SPI2_DMA_Tran = 1;//有准备好的数据了
		OSFlagPost(AIS_FLAG,0x01,OS_FLAG_SET,&err);
	}
}







