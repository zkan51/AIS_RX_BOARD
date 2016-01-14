/**
********************************************************************
* @file      AISMissionGenerator.h
* @author    Digital Design Team
* @version   1.0.0
* @date      2015/12/28   16:46
********************************************************************
* @brief     
*			AIS任务生成模块，包括生成CRC，对数据进行去比特填充，
*           组帧（添加起始和结束缓冲区、训练序列、起始标志、CRC、
*           结束标志、结束缓冲区），NRZI编码
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#ifndef AISMissionGenerator
#define AISMissionGenerator

#include "AIS_PS_Struct.h"
#include "AIS_PS_Interface.h"

extern u8 gx[];

#define NumSlotsPerMinute 2250
#define NumSlotsPerSecond 37

// 比特填充操作
int bitStuff(u8 *srcDataBuffer, int numBits, u8 *dstDataBuffer, int dstOffset);

// 计算CRC
void crcCalculator(u8 *dataBuffer, u8 numBytes, u8 *crcResult);

// 对数据字段进行比特序列调整，按字节进行分组，每个字节最低有效位先输出
void bitAdjusting(u8 *dataBuffer, int numBytes);

/* 时隙数组 */
extern u8 missionSlot[NumSlotsPerMinute];
/* 待发送的任务数组 */
extern AISMission missionDatas[MaxMissionNum+MaxReTxMissionNum];

/* 生成任务，包括组帧以及分配时隙 */
void generateMission();
/* 处理一般的消息，此处指的是状态机安排的消息，并返回消息存储位置指示 */
void dealCommMission();
/* 处理重发的任务，并返回消息存储位置指示 */
void dealReTxMission();
/* 在missionDatas中寻找处于空闲状态的任务空间 */
int findIdleMissionIndex(FlagStatus isCommMission);
/* AIS信号组帧，包括CRC计算、比特顺序调整、比特填充 */
int aisFraming(u8 *srcMsg, u8 numBytes, AISMission *mission);
/* 将两个消息序列进行合并 */
void dataCombine(u8 *dstMsg, u8 *srcMsg1, u8 numByte1, u8 *srcMsg2, u8 numByte2);
// 在序列中添加前后标志位，并返回最终的字节数
int addStartEndTag(u8 *dataBuffer, int numMsgBits);
/* 将任务队列中的数据进行初始化操作，主要是将任务的状态改为已完成状态 */
void initMissionDatas();
/* 时隙选择算法，将根据输入的开始时隙以及时隙选择宽度，选择时隙，并写入指定的任务时隙部分 */
FlagStatus slotSelect(u16 rcvSlot, u8 generateSlotPeriod, int missionIndex);
/* 进行AIS消息组帧以及时隙选择，即 aisFraming和slotSelect的组合 */
FlagStatus aisFramingAndSlotSelect(u8 *dataBuffer, int numMsgBits, int missionIndex, u16 rcvSlot, u8 generateSlotPeriod);
/* 寻找待处理的消息，并返回消息号，如果没有找到需要处理的任务，则返回FRAMELENGTH */
u16 findFSMFrameDataIndex();

//-------------------------------------------------------------------------
// 一些工具方法
//-------------------------------------------------------------------------

// 获取指定位置的比特值
u8 getBitValue(u8 *dataBuffer, int idxBit);
// 将指定位置的值设置为指定的值
void setBitValue(u8 *dataBuffer, int idxbit, u8 value);
// 将指定位置之后的一个字节内容设置为指定的字节值
void setByteValue(u8 *dataBuffer, int offset, u8 byteValue);
// 将指定数据缓冲区初始化为data值
void clearDataBuffer(u8 *dataBuffer, int numBytes, u8 data);
// 将两个数据Buffer进行按位异或，并将结果存储在dstDataBuffer中
void xorDataBuffer(u8 *srcDataBuffer, u8 *dstDataBuffer, int numBytes);
// 将字节进行反转
u8 reverseByte(u8 data);
// 从指定的文件中读出数据字节
int readDataFromFile(char *filePath, u8 *dataBuffer);

#endif

