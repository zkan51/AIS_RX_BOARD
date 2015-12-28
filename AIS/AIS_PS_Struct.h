/**
* @file    AIS_PS_Struct.h
* @author  Digital Design Team
* @version v1.0.0
* @date    12-10-2015
****************************************************************************************
* @brief
*    包含主要的AIS结构体定义与基本类型定义
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
****************************************************************************************
*/
#ifndef AIS_PS_Struct
#define AIS_PS_Struct

#include "myStdDef.h"
#include "stm32f4xx.h"

#define MAX_FRAME_BITS 512              // B类
#define MAXRESERVEDSLOTLEN			4//5 //记录最大的可存放的保留时隙信息的数组   add by wq

///* �����з������� */
//typedef signed        char int8_t;
//typedef signed short   int int16_t;
//typedef signed         int int32_t;


///* �����޷������� */
//typedef unsigned        char uint8_t;
//typedef unsigned short   int uint16_t;
//typedef unsigned         int uint32_t;

//typedef uint8_t         u8;
//typedef uint16_t        u16;
//typedef uint32_t        u32;

/* 定义信道指示 */
typedef enum{
	channelA = 0,
	channelB = 1,
	channelAB = 2   //add
} ChannelIndicator;

/* 定义标志 */
//typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef _Bool bool;
//typedef enum {RESET = 0, SET = !RESET} bool;
/* 定义CRC结果 */
typedef enum{
	CRCPASSED = 1,
	CRCFAILED = !CRCPASSED
} CRCResult;

/* 定义解调方法 */
typedef enum{
	DEMOD1DPDVD = 0,
	DEMOD2DPDVD = !DEMOD1DPDVD
} DemodMethod;

/* VDL消息解析后的数据结构 */
typedef struct  
{
	u8 frameBytes;             // 帧的字节数
	u8 frameData[133];         // 帧数据，最大不超过1064个比特
	ChannelIndicator channel;  // 信道指示
} FPGAVDLData;

typedef struct{
	u32 srcMMSI;              // 信源MMSI
	u32 dstMMSI1;             // 目的MMSI
	u8  msgID1_1;             // 消息ID1
	u16 slotOffset1_1;        // 时隙偏置1
	u8  msgID1_2;             // 消息ID2
	u16 slotOffset1_2;        // 时隙偏置2
	u32 dstMMSI2;             // 第二个目的MMSI
	u8  msgID2_1;             // 消息ID
	u16 slotOffset2_1;        // 时隙偏置

	ChannelIndicator rcvChannel;		//add by wq 接收的信道,若没有指定信道，则为0，指代不可用
	u16 rcvSlot;						//add by wq 接收的时隙号
} VDLMsg15;

//typedef struct{
//
//	u16 slotOffset;           // 时隙偏置
//	u8  numSlot;              // 时隙数目
//	u8  overTime;             // 超时时间，以分钟为单位
//	u16 increment;            // 增量
//
//}Msg20ReservedStruct;

/* 定义消息ID和指定时隙偏置 */
typedef struct  
{
	u8 msgId;
	u16 slotOffset;
} MsgIdSlotOffset;

typedef struct{
	u16 slotOffset;
	u8 numSlot;
	u8 overTime;
	u16 increment;
} ReserveSlotInfo;

typedef struct{
	
	//u32 baseMMSI;                 // 基站的MMSI
	u32 MMSI;
	//add by wq   
	//Msg20ReservedStruct reservedSlotInfo[MAXRESERVEDSLOTLEN];
	ReserveSlotInfo slotInfos[4];   // 预约的时隙信息
	u8 numOrderedSlots;             // 预约的时隙数量

	u16 rcvSlot;	//接收的时隙号   //？？？
	u32 bLongitude;	//基站的经度（需通过Msg4获得） //？？？
	u32 bLatitude;	//基站的纬度 //？？？
	u8 rcvUTCTime[3]; //接收的UTC时间，格式为hhmmss（时分秒） //？？？

} VDLMsg20;
typedef struct{
	//u32 baseMMSI;                 // 基站的MMSI
	u32 MMSI;
	u8  TxRxMode;             // Tx/Rx模式
	u32 logitude1;            // 经度1
	u32 latitude1;            // 纬度1
	u32 logitude2;            // 经度2
	u32 latitude2;            // 纬度2
	u8  isBroadcast;          // 是否是广播消息

	//add by wq 
	u8 handOverArea;			//切换区范围，单位：海里，数值为实际的，如：4，则为4海里范围内为切换区
	u8 rcvTime[5];				//接收时间，格式为mmddhhmmss,月天时分秒,分别存放
} VDLMsg22;

typedef struct{
	//u32 baseMMSI;                 // 指配台站的MMSI
	u32 MMSI;
	u32 logitude1;            // 经度1
	u32 latitude1;            // 纬度1
	u32 logitude2;            // 经度2
	u32 latitude2;            // 纬度2
	u8  stationType;          // 台站类型
	u8  shipType;             // 船舶类型
	u8  TxRxMode;             // Tx/Rx模式
	u8  reportInterval;       // 报告间隔
	u8  silentTime;           // 寂静时间，分钟为单位

	//add by wq
	u8 rcvTime[3];			//接收的时间 格式为ddmmss(天分秒)

} VDLMsg23;



/////////////for test add by wq///////////////
void initVDLMsg20Struct(VDLMsg20 * vdlMsg20);
//void initVDLMsg22Struct(VDLMsg22 * vdlMsg22,u8 newArea,u8 overlopArea);
void initVDLMsg23Struct(VDLMsg23 * vdlMsg23);

void initVDLMsg20StructPara(VDLMsg20 * vdlMsg20,u16 rcvSlot,u8 rcvhh,u8 rcvmm,u8 rcvss,u16 Offer1,u8 num1,u8 over1,u8 incre1,u16 Offer2,u8 num2,u8 over2,u8 incre2,u16 Offer3,u8 num3,u8 over3,u8 incre3,u16 Offer4,u8 num4,u8 over4,u8 incre4);

#endif
