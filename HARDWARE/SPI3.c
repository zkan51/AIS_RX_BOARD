#include "spi3.h"
#include "uart1.h"
#include "includes.h"
u8 SPI3_TX0[SPI3_TX_LEN];
//u8 SPI3_TX1[SPI3_TX_LEN];
//u8 SPI2_DMA_Tran=0 ;
//extern OS_FLAG_GRP *AIS_FLAG;//定义SPI2标志组指针
//static INT8U err;
void SPI3_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;	
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOA, ENABLE);//开启GPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//开启SPI3时钟

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SPI3);//PA15¸复用为SPI3	NSS
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3); //PB3¸复用为SPI3  SCK
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3); //PB4¸复用为SPI3  MISO
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3); //PB5¸复用为SPI3  MOSI
	
  //GPIOB12,13,14,15初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//PB3~5¸串口初始化
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推免输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//SPI3初始化设置

	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置SPI3单向或者双向的数据模式：SPI设置成双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI传输数据为8bit
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第一个跳变沿（上升或者下降数据被采样）
	SPI_InitStructure.SPI_NSS =  0x0000;		//NSS由硬件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率分频值256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式

  //SPI3->CR1 |=0x1000;
  SPI3->CR2 =0x0004;
	SPI_Init(SPI3, &SPI_InitStructure);  //初始化SPI3



	SPI_Cmd(SPI3, ENABLE); //使能SPI3

	SPI3_SetSpeed(SPI_BaudRatePrescaler_2);//42MHz全速运行
	
	SPI_I2S_DMACmd(SPI3,SPI_DMAReq_Tx,ENABLE);
	
//中断配置

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//设置DMA
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream7);//DMA1 通道0，数据流7
	
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)(&(SPI3->DR));//源地址 
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)SPI3_TX0;   //目的地址0
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;       //外设到内存
	DMA_InitStructure.DMA_BufferSize = SPI3_TX_LEN;               //数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //内存增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//数据长度8bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据长度8bit 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //不使用循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      // 优先级高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;        //使用FIFO
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream7, &DMA_InitStructure);//初始化
	
	DMA_ITConfig(DMA1_Stream7,DMA_IT_TC,ENABLE);//开启传输完成中断

	//DMA_Cmd(DMA1_Stream7,ENABLE);     //使能 	使能就发送数据了
}   
//SPI3速度设置
//SPI速度=FAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//FAPB2的时钟一般为84MHz
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断是否有效
	SPI3->CR1&=0XFFC7;//位3~5清零，用来设置波特率
	SPI3->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI3,ENABLE); //使能SPI2 
} 
//SPI2读写一个字节
//TxData:要写入的数据
//返回值：SPI2读取到的数据
u8 SPI3_ReadWriteByte(u8 TxData)
{		 			 
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI3, TxData); //通过SPI3发送一个字节
		
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个字节  
 
	return SPI_I2S_ReceiveData(SPI3); //返回收到的数据				    
}

//DMA传输完成中断
void DMA1_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TCIF7) != RESET)
	{
		OSIntEnter();
		 DMA_Cmd(DMA1_Stream7,DISABLE);
	   DMA_ClearITPendingBit(DMA1_Stream7,DMA_IT_TCIF7);
		
	   //SPI2_DMA_Tran = 1;//有准备好的数据了
		//OSFlagPost(AIS_FLAG,0x01,OS_FLAG_SET,&err);
		OSIntExit();
	}
}







