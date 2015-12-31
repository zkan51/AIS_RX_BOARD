/**
********************************************************************
* @file      myStdDef.h
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   16:13
********************************************************************
* @brief     定义全局通用的数据结构体的通用类型
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef __myStdDef_h
#define __myStdDef_h

//////////////////////////////define ///////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
typedef   signed          char int8;
typedef   signed short     int int16;
typedef   signed           int int32;
typedef   signed       __int64 int64;

    /* exact-width unsigned integer types */
typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned           int u32;
typedef unsigned       __int64 u64;
//typedef bool				   _Bool;

//////////////////////////////////enum  defination for Msg////////////////////////////////////////////
	
typedef enum{
	//NOCHANNEL,
	CHANNELA,
	CHANNELB,				//在实际信道使用中，0-channelA 1-channelB
	CHANNELAB
}BchannelIndicator;		

typedef enum{
	RECEIVING = 0x11,										
	RECEIVED = 0x22,										
	RECEIVEWRONG = 0x33								
		
} RcvState;

typedef enum{
	MSGNEW = 0x11,											//新组帧消息所需内容，待后续完成组帧
	MSGOLD = 0x22,												//组帧消息已经正确处理过，包括自主消息已经组帧处理和询问消息的响应已经正确发出，
	MSGSENDSUCCEED = 0x33,									//
	MSGSENDFAILD = 0x44,									//用于指代询问类消息发送失败，需要再次重新发送
	MSGHANDLING = 0x55										//用于指代询问类消息正在处理，还没有发送成功与否的结果
} MsgHandleState;

typedef enum {
	INQUIMODE = 0x11,
	SAFEMODE = 0x12,
	ASSIGNMODE = 0x22,
	AUTONOMODE = 0x33
} ControlModeIndicator;

typedef enum {
	INVALID = 0,
	COUNTING = 0x11,
	COUNTED = 0x22
}CountIndicator;

typedef enum {
	
	SAFEMSG = 0x10,			//安全消息
	ASSIGNMSG = 0x20,		//指配模式消息
	AUTONOMSG = 0x30,		//自主模式消息
	INTERROMSG = 0x40,		//询问模式消息

	AAMSG18 = 0x21,			//指配模式下的周期报告Msg18
	AAMSG24 = 0x22,			//指配模式下的周期报告Msg24
	ASSIGNADDMSG18 = 0x23,	//Msg22 指配模式下的切换区
	A22MSG18 = 0x24,		//Msg22 指配模式下

	INTERROMSG18 = 0x41,	//询问模式 询问Msg18
	INTERROMSG19 = 0x42,	//询问模式 询问Msg19
	INTERROMSG24 = 0x43		//询问模式 询问Msg24
}MsgTypeIndicator;


//typedef enum {
//	COUNTINGG = 0x11,
//	COUNTED = 0x22
//}SycleCount;

#endif
