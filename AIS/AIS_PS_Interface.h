/**
****************************************************************************************
* @file    PS_FPGA_Interface.h
* @author  Digital Design Team
* @version v1.0.0
* @date    12-10-2015
****************************************************************************************
* @brief
*    该文件将声明FPGA和MCU之间的接口程序，包括对FPGA上报的接收信息解析、MCU对FPGA的
*    任务指派、以及对FPGA时隙号的解析
* 
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
****************************************************************************************
*/

#ifndef PS_FPGA_Interface
#define PS_FPGA_Interface

#include "AIS_PS_Struct.h"

/* SPI数据缓冲区的大小 */
#define MAX_FPGA_VDL_DATA_LEN     1024

/* SPI数据缓冲存储区 */
extern u8 FPGA_VDL_DATA_BUFFER[MAX_FPGA_VDL_DATA_LEN];

//--------------------------------------
// 指示数据缓冲区数据是否有新的数据
// 当串口中断接收新的数据后，需要将该标志置为SET
// 而当数据被处理后，重新将其置为RESET
//--------------------------------------
extern FlagStatus isFPGAVDLDataNewIn;

/* 接收的消息15、20、消息22、消息23 */
extern VDLMsg15 recVDLMsg15;
extern FlagStatus isVDLMsg15NewIn;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

extern VDLMsg20 recVDLMsg20;
extern FlagStatus isVDLMsg20NewIn;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

extern VDLMsg22 recVDLMsg22;
extern FlagStatus isVDLMsg22NewIn;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

extern VDLMsg23 recVDLMsg23;
extern FlagStatus isVDLMsg23NewIn;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

/* 他船消息 */
extern FPGAVDLData otherShipMsg;
extern FlagStatus isOtherShipMsgNewIn;

/* 解析FPGA上报的信息 */
void SIG_PS_FPGA_ParseRecData(u8 *FPGA_VDL_DATA_BUFFER);

void SIG_PS_FPGA_POWER_DOWN_REQ(void);

/* 一些需要的工具 */
void getInfoForMsg15(const u8 *dataBuffer, u8 start, u8 frameLen, ChannelIndicator channelIndicator);
void getInfoForMsg20(const u8 *dataBuffer, u8 start, u8 frameLen);
void getInfoForMsg22(const u8 *dataBuffer, u8 start, u8 frameLen);
void getInfoForMsg23(const u8 *dataBuffer, u8 start, u8 frameLen);
void getInfoForOtherShip(const u8 *dataBuffer, u8 start, u8 frameLen, ChannelIndicator channelIndicator);
void getLongitudeFromBuffer(u32 *const longitude, const u8 *dataBuffer, u8 startByte, u8 startBit);
void getLatitudeFromBuffer(u32 *const latitude, const u8 *dataBuffer, u8 startByte, u8 startBit);

/** 检查MMSI是否为基站MMSI */
bool isValidBSMMSI(u32 MMSI);

u32  getMMSI(u8 const *dataBuffer, u8 start, FlagStatus isMSB);
MsgIdSlotOffset getAskedMsgInfo(u8 const *dataBuffer, u8 start, FlagStatus isMSB);
ReserveSlotInfo getReserveSlotInfo(u8 const *dataBuffer, u8 start, FlagStatus isMSB);

/* 一些测试工具 */
void showNumInString(unsigned int num, int strLength);
void num2str(char *str, unsigned int num, int strLength);

#endif

