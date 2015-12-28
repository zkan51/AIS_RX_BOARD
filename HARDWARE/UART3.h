/*********************************************************************
* @file      UART3.h
* @author    Embedded team
* @version   
* @date      2015/12/19   16:12
********************************************************************
* @brief     定义与上位机串口之间的数据交互的数据结构体和功能函数的接口
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#ifndef  UART3_H
#define  UART3_H
#include "stm32f4xx.h" 
#include "stdio.h"

#include "myStdDef.h"
#include "AIS_PS_Struct.h"

#define UART3_RX1_LEN     20 
#define UART3_RX2_LEN     20 
extern uint8_t Uart3_Rx1[UART3_RX1_LEN];//UART3
extern uint8_t Uart3_Rx2[UART3_RX2_LEN];//UART3

//u8 Flag_Uart_Send = 0;



#define MAXRCVLEN		255	 		//接收一帧的最大长度
#define SUPPLIER		4324745735185 //供应商 共42bit 十六进制为：3EEEEEEEC11
#define OWNMMSI			333333333 //111111111// 		//十进制,共30bit
#define RCVMSGLEN		10				//上位机处最多可以存储未处理的消息数

////////////////////////////////////struct 定义区///////////////////////////////////////////////	
typedef struct{
	u8 msgID;		  //存储上位机处接受的消息ID号 00-不可用，其余--接收的ID号
	u8 msgState;		//消息处理状态 MSGNEW-未处理   MSGOLD-已处理
}AIS_RcvMsgIDStruct; //只用来接收消息14么???

typedef struct{
		
	u8 receivingFlag;					//开始接收串行数据标志位
	u8 receivedFlag;					//接收完成一帧数据标志位
	u8 rcvBuffer[MAXRCVLEN];	//接收数据缓存
	u8 rcvBufferLength;				//接收数据长度
	u8 receiveWrong;					//接收数据出错，现在判断的一个方式是接收数据长度超过最大值

}MKD_FrameStruct;

typedef struct{
	u8 shipType;							//船舶类型，  0=不可用或无船舶=默认值，1-99可用
	_Bool shipCall[42];						//船呼号， 		接收为7个6bit ASCII码，@@@@@@@ 不可用，为默认值
	_Bool shipName[120];					//船名， 		接收为20个6bit ASCII码，20个@ 为不可用，为默认值
	_Bool supplier[42];						//供应商，		接收为7个6bit ASCII码，一般为系统给出的值，是一个常量
	u32 shipSize;							//船的尺寸，	29~21，20~12,11~6,5~0分别对应A,B,C,D的尺寸
														//						0~511, 0~511,0~63,0~63(单位m)
	_Bool DTEIndicatorFlag;		//数据终端设置指示标志位， 0=可用；1=不可用=默认值
			
}AIS_StaticDataStruct;

typedef struct{
	u8 sentenceTotalNum;								//分段总数，1~9，最小为1
	u8 sentenceOrderNum;								//分段顺序号，1~9
	u8 sequentialMessageIdentifier;			//顺序消息识别符，0~9，同一个消息的分段有相同的消息识别符
	BchannelIndicator broadcastChannel;								//AIS广播通道，0--A  1—B  2—A and B 
	u8 msgID;														//用于指示接收消息号
	_Bool encapsulatedData[256];						//消息14对应的封装消息
	u8 encapsulatedDataLength;					//消息14对应的消息内容的长度
	RcvState rcvStateFlag;										//接收状态  RECEIVING   RECEIVED     RECEIVEWRONG  
	MsgHandleState msgOldFlag;											//用于记录消息是否已经被处理过，如被处理，则置标志位，认为可以被覆盖   MSGNEW   MSGOLD
	u16 rcvSlot;								//接收到完整的封装消息的时隙号 
}AIS_BBMMsgStruct;



/////////////////////////////////////////外部接口申明区//////////////////////////////////////////////////////

extern MKD_FrameStruct mkd_frameStruct;
extern AIS_RcvMsgIDStruct ais_rcvMsgIDStruct;
extern AIS_StaticDataStruct ais_staticDataStruct;
extern AIS_BBMMsgStruct ais_bbmMsgStruct;
extern AIS_RcvMsgIDStruct ais_msgIDStruct;


/////////////////////////////////////////函数 定义区/////////////////////////////////////////////////////


void usartMDKInit(void);

void setSupplierInfo(AIS_StaticDataStruct * ais_staicDataStruct);
void change8bitASCIItoBit(u8 len,u8 * sour,_Bool * dest);
void changeMKD6bitASCIItoBit(u8 len,u8 *sour,_Bool * dest);
void getMKDAISInfo(MKD_FrameStruct * mkd_frameStruct,AIS_StaticDataStruct * ais_staticDataStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_RcvMsgIDStruct * ais_msgIDStruct);
void getVSDStaticInfo(MKD_FrameStruct * mkd_frameStruct,AIS_StaticDataStruct * ais_staticDataStruct);
void getSSDStaticInfo(MKD_FrameStruct * mkd_frameStruct,AIS_StaticDataStruct * ais_staticDataStruct);
void getBBMMsgInfo(MKD_FrameStruct * mkd_frameStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct);
void rstMKDFrameStruct(MKD_FrameStruct * mkd_frameStruct);



////////////////for test ////////////////////////////////
void testAIS_StaticStruct(AIS_StaticDataStruct * ais_staticDataStruct);
void testAIS_BBMMsgStruct(AIS_BBMMsgStruct * ais_bbmMsgStruct);	
	
	
	
void UART3_Config(u32 bound);

void  Putc_UART3(u8 ch);
#endif
