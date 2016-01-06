/**
********************************************************************
* @file      MKDControl.h
* @author    Digital Design Team
* @version   
* @date      2015/12/2   19:32
********************************************************************
* @brief     定义MKDControl.c中使用的结构体和函数接口
*			
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef _MKDControl_H
#define _MKDControl_H


#include "myStdDef.h"
#include "AIS_PS_Interface.h"
#include "UART2.h"
#include "UART3.h"


#include "FSMControl.h"

////////////////////////////////////define 声明区////////////////////////////////////////////////////
#define MAXMKDFRAMELEN 10//一个上位机执行间隔可以缓存的最大的向上位机上报的消息数
#define MAXOWNINFOLEN 3	 //一次上位机执行间隔中，可以缓存的最多的向上位机上报的本船的消息数

////////////////////////////////////enum 声明区////////////////////////////////////////////////////

typedef enum{

	safeCfmMsg = 0x11,//安全消息的确认消息


}MKD_MsgTypeIndicator;

typedef enum{
	VDM = 1,
	VDO = 2,
	ABK = 3,
	ALR = 4,
	TXT = 5
}MKD_IdIndicator;

////////////////////////////////////struct 声明区////////////////////////////////////////////////////
typedef struct{
	u8 frameType;//用于区分 固定格式$，或者是非固定格式!
	u8 frameHead[5];//用于存放消息类型号
	u8 totalNum;//总分段数，取值1~9
	u8 sentenceNum;//在总分段数中的顺序号，取值1~9
	u8 aisChannel;//在AIS信道上发送的指示，用A或B来指代
	u8 sequenceNum;//用于区分不同消息的序列号，在0~9之间循环
	u8 frameEncapContentByte[120];//存放1371按照61162协议格式对应的消息内容 ,可能会超过1个时隙长度 60+60
	u8 frameEncapContentByteLen;//存放1371消息转换为61162协议封装消息的6比特的长度
	u8 fillBits;			//为保证61162中封装的1371消息内容为6的整数倍，取值为0~5
}MKD_ContentToBeFramedStruct;

typedef struct{

	u8 mkd_encapDataByte[82];//61162协议内容，按照字节的形式进行存储 
	//封装内容按照协议内容顺序，按照数组下标从低到高，顺序依次写入
	u8 mkd_encapDataByteLen;//封装内容长度  字节长度
	BchannelIndicator mkd_broadChannel;//广播信道   
	MKD_MsgTypeIndicator mkd_msgType;//消息类型   用于区分安全类消息和一般消息，如果是安全类消息的确认消息，还是需要马上上传给上位机，不等待   
	MsgHandleState mkd_msgState;//消息处理的状态  使用MSGNEW-刚生成消息还没有上传给上位机 MSGOLD-已经上传给上位机，后续消息可以覆盖该结构体 

}MKD_FramedStruct;// 上位机中定义的它船和本船的上报格式消息，不区分具体内容，用于短消息，一帧就是一个分段，有的消息可能有多个分段


//typedef struct{


//}MKD_LFrameStruct;//上位机中定义需要将一个消息分成多个分组发送的消息，暂时没有想到相应的用处



typedef struct  
{
	u8 vdl_encapDataByte[40];//FPGA串口接收的它船的vdl链路的完整消息，考虑到1371中的消息19可能占到2个时隙，最大长度是312/8=39
	u8 vdl_encapDataByteLen;//FPGA串口接收的它船vdl链路消息的字节长度
	u8 vdl_channel;//消息的接收信道，A或B
	MsgHandleState vdlMsgState;// 接收消息的处理状态，MSGNEW 或者 MSGOLD

}OwnShipInfo_Struct;//接收的自船的vdl封装消息

typedef struct{

	MKD_ContentToBeFramedStruct mkd_toBeFrameStruct;//存放待组帧的通用消息结构体
	OwnShipInfo_Struct ownShipInfoStruct[MAXOWNINFOLEN];//自船存放的vdl封装信息结构体数组
	u8 ownShipInfoBias;//自船vdl信息结构体中信息处理的偏置，取值为0~MAXOWNINFOLEN-1

}MKD_ControlStruct;

typedef struct{

	MKD_FramedStruct mkd_framedStruct[MAXMKDFRAMELEN];//存放打包好的向上位机汇报的满足61162格式的消息

}MKD_DataStruct;//上位机中存放的用于和上位机交互的消息及其辅助内容




////////////////////////////////////变量接口声明////////////////////////////////////////////////////

extern MKD_ControlStruct mkd_controlStruct;
extern MKD_DataStruct mkd_dataStruct;



////////////////////////////////////function 声明区////////////////////////////////////////////////////
void rstMKDFramedStruct(MKD_FramedStruct * mkd_framedStruct);
void rstMKDContentToBeFramedStruct(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);

void chnage611626bitASCIIto8bitASCII(u8 len,u8 * sour,u8 * dest);
void change1371ContentsT61162(FSM_FrameStruct * fsm_frameStruct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);
void changeOtherShipCntentTo61162(FPGAVDLData * otherShipMsg,MKD_ContentToBeFramedStruct *mkd_toBeframeStruct);
void changeOwnShipContentTo61162(OwnShipInfo_Struct * ownShipInfo_struct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);
void updataToBeFrameInfo(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_IdIndicator id);

void updataVDMorVDOFrame(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_DataStruct * mkd_dataStruct);

void MKDInit(MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct);
void getOwn61162ToBeFramedInfo(MKD_ControlStruct * mkd_controlStruct,FSM_DataStruct * fsm_dataStruct);
//void mkd_collectOtherShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct);
void mkd_collecVDLShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,MKD_IdIndicator id);
void mkd_collectNewMsg(FlagStatus * isOtherShipMsgNewIn,FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct);


////////////////////////////////////others////////////////////////////////////////////////////
#endif
