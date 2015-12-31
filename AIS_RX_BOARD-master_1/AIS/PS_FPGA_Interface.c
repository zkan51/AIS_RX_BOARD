/**
********************************************************************
* @file      PS_FPGA_Interface.c
* @author    Digital Design Team
* @version   
* @date      2015/11/16   17:52
********************************************************************
* @brief     
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#include "AIS_PS_Interface.h"
#include "AIS_PS_Struct.h"
#include<stdlib.h>
#include<stdio.h>

/************************************************************************/
/* 定义一些变量                                                         */
/************************************************************************/

/* SPI数据缓冲存储区 */
u8 FPGA_VDL_DATA_BUFFER[MAX_FPGA_VDL_DATA_LEN];

//--------------------------------------
// 指示数据缓冲区数据是否有新的数据
// 当串口中断接收新的数据后，需要将该标志置为SET
// 而当数据被处理后，重新将其置为RESET
//--------------------------------------
FlagStatus isFPGAVDLDataNewIn = RESET;

/* 接收的消息15、20、消息22、消息23 */
VDLMsg15 recVDLMsg15;
FlagStatus isVDLMsg15NewIn = RESET;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

VDLMsg20 recVDLMsg20;
FlagStatus isVDLMsg20NewIn = RESET;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

VDLMsg22 recVDLMsg22;
FlagStatus isVDLMsg22NewIn = RESET;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

VDLMsg23 recVDLMsg23;
FlagStatus isVDLMsg23NewIn = RESET;                   // SET - 旧数据尚未处理，  RESET - 旧数据已经处理，可以覆盖

/* 接收的他船消息 */
FPGAVDLData otherShipMsg;
FlagStatus isOtherShipMsgNewIn = RESET;

/* 帧头标志常数 */
const static u8 FPGA_VDL_FRAME_HEADER = 0xAD;


/************************************************************************
* Name   : SIG_PS_FPGA_ParseRecData
* Brief  : 解析FPGA送来的解调数据
* Author : ZuoDahua
* Param  : void
* Return : void
************************************************************************/
void SIG_PS_FPGA_ParseRecData(void){

	u16 headerIdx = 0;           // 帧头位置
	u8 frameLen;                 // 帧长
	ChannelIndicator demodChan;  // AIS通道
	CRCResult demodCRCResult;    // 解调crc结果
	DemodMethod demodMethod;     // 解调方法
	u8 msgId;
	u8 idx;
	u8 msgData[133];


	/* 如果没有新的数据，则直接返回 */
	if(isFPGAVDLDataNewIn == RESET)
		return;
	else
		isFPGAVDLDataNewIn = RESET;

	/* 首先寻找帧头 */
	while(headerIdx < MAX_FPGA_VDL_DATA_LEN-4){
		if(FPGA_VDL_DATA_BUFFER[headerIdx] == FPGA_VDL_FRAME_HEADER)
			break;
		else
			headerIdx++;
	}

	/* 没有找到帧头 */
	if(headerIdx == MAX_FPGA_VDL_DATA_LEN-4)
		return;

	/* 抓取帧长数据 */
	headerIdx++;
	frameLen = FPGA_VDL_DATA_BUFFER[headerIdx++] << 4;
	frameLen += FPGA_VDL_DATA_BUFFER[headerIdx];
	if(frameLen > 133 || (MAX_FPGA_VDL_DATA_LEN-headerIdx<frameLen*2))         // AIS数据帧最大不会超过1064个比特，即133个字节
		return;

	/* 抓取部分解调参数 */
	headerIdx++;
	demodCRCResult = (FPGA_VDL_DATA_BUFFER[headerIdx]&0x04) == 4 ? CRCPASSED : CRCFAILED;
	demodMethod = (FPGA_VDL_DATA_BUFFER[headerIdx] & 0x02) == 2 ? DEMOD2DPDVD : DEMOD1DPDVD;
	demodChan = (FPGA_VDL_DATA_BUFFER[headerIdx] & 0x01) == 1 ? channelB : channelA;

	/* 抓取接收数据 */
	// 比特顺序的调整工作在FPGA中已经实现，MCU不用再做处理
	headerIdx++;
	msgId = ((FPGA_VDL_DATA_BUFFER[headerIdx]&0x0F)<<2) | ((FPGA_VDL_DATA_BUFFER[headerIdx+1]&0x0C)>>2);
	switch (msgId)
	{
	case 15 : 
		if(isVDLMsg15NewIn == RESET){
			getInfoForMsg15(FPGA_VDL_DATA_BUFFER, headerIdx+2, frameLen, demodChan);
		}
		break;
	case 20:
		if (isVDLMsg20NewIn == RESET)
		{
			getInfoForMsg20(FPGA_VDL_DATA_BUFFER, headerIdx+2, frameLen);
		}
		break;
	case 22:
		if (isVDLMsg22NewIn == RESET)
		{
			getInfoForMsg22(FPGA_VDL_DATA_BUFFER, headerIdx+2, frameLen);
		}
		break;
	case 23:
		if (isVDLMsg23NewIn == RESET)
		{
			getInfoForMsg23(FPGA_VDL_DATA_BUFFER, headerIdx+2, frameLen);
		}
		break;
	default:
		if(isOtherShipMsgNewIn == RESET){   // 已经被处理过
			getInfoForOtherShip(FPGA_VDL_DATA_BUFFER, headerIdx+2, frameLen, demodChan);
		}
		break;
	}
}

/************************************************************************
* Name   : getInfoForMsg15
* Brief  : 从FPGA送上来的数据中获取消息15所需信息，将获取的数据直接写在
*          全局变量recVDLMsg15
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组
*          start        -  数据起始地址
*          frameLen     -  帧长，以字节为单位
*          channelIndicator  -  消息15的通道指示
* Return : void
************************************************************************/
void getInfoForMsg15(const u8 *dataBuffer, u8 start, u8 frameLen, ChannelIndicator channelIndicator){
	u8 idx;
	u8 shiftNums;
	u8 numDealHalfBytes;
	MsgIdSlotOffset msgInfo;
	/* 消息15的比特数目范围是88 ~ 160 */
	if(dataBuffer == NULL || start < 0 || frameLen < 11){
		isVDLMsg15NewIn = RESET;
		return;
	}

	/* 初始化消息结构体 */
	recVDLMsg15.srcMMSI = 0;
	recVDLMsg15.dstMMSI1 = 0;
	recVDLMsg15.dstMMSI2 = 0;
	recVDLMsg15.msgID1_1 = 0;
	recVDLMsg15.msgID1_2 = 0;
	recVDLMsg15.msgID2_1 = 0;

	isVDLMsg15NewIn = SET;
	recVDLMsg15.rcvChannel = channelIndicator;

	/* 获取信源ID */
	idx = start + 2;
	recVDLMsg15.srcMMSI = getMMSI(dataBuffer, idx, SET);

	//---------------------------------------------------------
	/* 获取目的ID1以及相关数据 */
	//---------------------------------------------------------
	idx = start + 10;
	recVDLMsg15.dstMMSI1 = getMMSI(dataBuffer, idx, SET);
	idx = start + 17;
	// 首先获取第一个询问消息
	msgInfo = getAskedMsgInfo(dataBuffer, idx, RESET);
	recVDLMsg15.msgID1_1 = msgInfo.msgId;
	recVDLMsg15.slotOffset1_1 = msgInfo.slotOffset;
	idx = start + 22;
	// 判断是否还有询问的消息
	if(frameLen <= 13)
		return;

	msgInfo = getAskedMsgInfo(dataBuffer, idx, RESET);
	recVDLMsg15.msgID1_2 = msgInfo.msgId;
	recVDLMsg15.slotOffset1_2 = msgInfo.slotOffset;
	idx = start + 27;

	//------------------------------------------------------
	// 获取目的ID2以及相关数据
	//------------------------------------------------------
	if(frameLen < 19)
		return;
	recVDLMsg15.dstMMSI2 = getMMSI(dataBuffer, idx, RESET);
	idx = start + 35;
	msgInfo = getAskedMsgInfo(dataBuffer, idx, SET);
	recVDLMsg15.msgID2_1 = msgInfo.msgId;
	recVDLMsg15.slotOffset2_1 = msgInfo.slotOffset;
}

/************************************************************************
* Name   : getInfoForMsg20
* Brief  : 从FPGA送上来的数据中获取消息20所需信息，将获取的数据直接写在
*          全局变量recVDLMsg20
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组
*          start        -  数据起始地址
*          frameLen     -  帧长，以字节为单位
* Return : void
************************************************************************/
void getInfoForMsg20(const u8 *dataBuffer, u8 start, u8 frameLen){
	u8 idx;

	if(dataBuffer == NULL || start < 0 || frameLen < 9){
		isVDLMsg20NewIn = RESET;
		return;
	}

	/* 初始化接收数据内存数据 */
	isVDLMsg20NewIn = SET;
	recVDLMsg20.MMSI = 0;
	for(idx = 0; idx < 4; idx++){
		recVDLMsg20.slotInfos[idx].numSlot = 0;
		recVDLMsg20.slotInfos[idx].slotOffset = 0;
		recVDLMsg20.slotInfos[idx].overTime = 0;
		recVDLMsg20.slotInfos[idx].increment = 0;
	}
	recVDLMsg20.numOrderedSlots = 0;

	/* 获取基站MMSI */
	idx = start + 2;
	recVDLMsg20.MMSI = getMMSI(dataBuffer, idx, SET);

	if (isValidBSMMSI(recVDLMsg20.MMSI) == RESET)
	{
		isVDLMsg20NewIn = RESET;
		return;
	}
	/* 获取第一个预约时隙信息 */
	idx = start + 10;
	recVDLMsg20.slotInfos[0] = getReserveSlotInfo(dataBuffer, idx, SET);
	recVDLMsg20.numOrderedSlots = 1;

	/* 判断是否还有第二个预约时隙 */
	if(frameLen < 13)
		return;
	idx = start + 17;
	recVDLMsg20.slotInfos[1] = getReserveSlotInfo(dataBuffer, idx, RESET);
	recVDLMsg20.numOrderedSlots++;
	/* 判断是否有第三个预约时隙 */
	if(frameLen < 17)
		return;
	idx = start + 25;
	recVDLMsg20.slotInfos[2] = getReserveSlotInfo(dataBuffer, idx, SET);
	recVDLMsg20.numOrderedSlots++;
	/* 判断是否有第四个预约时隙 */
	if(frameLen < 20)
		return;
	idx = start + 32;
	recVDLMsg20.slotInfos[3] = getReserveSlotInfo(dataBuffer, idx, RESET);
}

/************************************************************************
* Name   : getInfoForMsg22
* Brief  : 从FPGA送上来的数据中获取消息22所需信息，将获取的数据直接写在
*          全局变量recVDLMsg22
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组
*          start        -  数据起始地址
*          frameLen     -  帧长，以字节为单位
* Return : void
************************************************************************/
void getInfoForMsg22(const u8 *dataBuffer, u8 start, u8 frameLen){
	u8 idx;
	if(dataBuffer == NULL || start < 0 || frameLen != 21){
		isVDLMsg22NewIn = RESET;
		return;
	}

	/* 初始化接收数据内存数据 */
	isVDLMsg22NewIn = SET;
	idx = start + 2;
	recVDLMsg22.MMSI = getMMSI(dataBuffer, idx, SET);
	if(isValidBSMMSI(recVDLMsg22.MMSI) == RESET){
		isVDLMsg22NewIn = RESET;
		return;
	}

	/* 获取消息22中包含的主要数据 */
	idx = start + 16;
	recVDLMsg22.TxRxMode = dataBuffer[idx++];
	getLongitudeFromBuffer(&recVDLMsg22.logitude1, dataBuffer, idx, 1);
	idx = start + 21;
	getLatitudeFromBuffer(&recVDLMsg22.latitude1, dataBuffer, idx, 3);
	idx = start + 26;
	getLongitudeFromBuffer(&recVDLMsg22.logitude2, dataBuffer, idx, 0);
	idx = start + 30;
	getLatitudeFromBuffer(&recVDLMsg22.latitude2, dataBuffer, idx, 2);
	idx = start + 34;
	recVDLMsg22.isBroadcast = dataBuffer[idx] & 0x01;
	idx++;
	recVDLMsg22.handOverArea = (dataBuffer[idx++] & 0x03) << 1;
	recVDLMsg22.handOverArea += (dataBuffer[idx] & 0x08) >> 3;
}

/************************************************************************
* Name   : getInfoForMsg23
* Brief  : 从FPGA送上来的数据中获取消息23所需信息，将获取的数据直接写在
*          全局变量recVDLMsg23
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组
*          start        -  数据起始地址
*          frameLen     -  帧长，以字节为单位
* Return : void
************************************************************************/
void getInfoForMsg23(const u8 *dataBuffer, u8 start, u8 frameLen){
	u8 idx;
	if(dataBuffer == NULL || start < 0 || frameLen != 20){
		isVDLMsg23NewIn = RESET;
		return;
	}

	/* 初始化接收数据内存数据 */
	isVDLMsg23NewIn = SET;
	idx = start + 2;
	recVDLMsg23.MMSI = getMMSI(dataBuffer, idx, SET);
	if(isValidBSMMSI(recVDLMsg23.MMSI) == RESET){
		isVDLMsg23NewIn = RESET;
		return;
	}

	/* 获取消息23中包含的主要数据 */
	idx = start + 10;
	getLongitudeFromBuffer(&recVDLMsg23.logitude1, dataBuffer, idx, 0);
	idx = start + 14;
	getLatitudeFromBuffer(&recVDLMsg23.latitude1, dataBuffer, idx, 2);
	idx = start + 18;
	getLongitudeFromBuffer(&recVDLMsg23.logitude2, dataBuffer, idx, 3);
	idx = start + 23;
	getLatitudeFromBuffer(&recVDLMsg23.latitude2, dataBuffer, idx, 1);
	idx = start + 27;
	recVDLMsg23.stationType = (dataBuffer[idx++] & 0x03) << 2;
	recVDLMsg23.stationType += (dataBuffer[idx] & 0x0C) >> 2;
	recVDLMsg23.shipType = (dataBuffer[idx++] & 0x03) << 6;
	recVDLMsg23.shipType += dataBuffer[idx++] << 2;
	recVDLMsg23.shipType += (dataBuffer[idx] & 0x0C) >> 2;
	idx = start + 36;
	recVDLMsg23.TxRxMode = (dataBuffer[idx] & 0x0C) >> 2;
	recVDLMsg23.reportInterval = (dataBuffer[idx++] & 0x03) << 2;
	recVDLMsg23.reportInterval += (dataBuffer[idx] & 0x0C) >> 2;
	recVDLMsg23.silentTime = (dataBuffer[idx++] & 0x03) << 2;
	recVDLMsg23.silentTime += (dataBuffer[idx] & 0x0C) >> 2;
}

/************************************************************************
* Name   : getInfoForOtherShip
* Brief  : 从FPGA送上来的数据中获取他船的消息内容，会改变全局变量
*     		    otherShipMsg和isOtherShipMsgNewIn;
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组
*          start        -  数据起始地址
*          frameLen     -  帧长，以字节为单位
*          channelIndicator  -  接收通道指示
* Return : void
************************************************************************/
void getInfoForOtherShip(const u8 *dataBuffer, u8 start, u8 frameLen, ChannelIndicator channelIndicator){
	u8 idxByte;
	if (dataBuffer == NULL || start < 0 || frameLen < 0)
	{
		isOtherShipMsgNewIn = RESET;
		return;
	}
	otherShipMsg.frameBytes = frameLen;
	otherShipMsg.channel = channelIndicator;
	for(idxByte = 0; idxByte < frameLen; idxByte++){
		otherShipMsg.frameData[idxByte] = dataBuffer[start+2*idxByte] << 4;
		otherShipMsg.frameData[idxByte] += dataBuffer[start+2*idxByte+1];
	}
}

/************************************************************************
* Name   : getMMSI
* Brief  : 从数据缓存中获取MMSI号
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组，每个字节仅低4位有效
*          start        -  数据起始地址
*          isMSB        -  指示是否在高位比特，只有两种可能，在第3位和在第1位
*                              isMSB == SET，则指示从第3位开始，否则起始位为1
* Return : u32          -  30位的MMSI号
*          如果出现异常，则返回MMSI位数大于30位，本设计直接返回2^31 = 2147483648
************************************************************************/
u32 getMMSI(u8 const *dataBuffer, u8 start, FlagStatus isMSB){
	u32 mmsi;
	u8 idx;
	u8 numShift;
	if(dataBuffer == NULL || start < 0)
		return 2147483648;

	if(isMSB == SET){
		mmsi = 0;
		numShift = 26;
		for(idx = start; idx < start + 7; idx++){
			mmsi += dataBuffer[idx] << numShift;
			numShift -= 4;
		}
		mmsi += (dataBuffer[idx]&0x0C) >> 2;
		return mmsi;
	}else{
		mmsi = (dataBuffer[start++] & 0x03) << 28;
		numShift = 24;
		for(idx = start; idx < start + 7; idx++){
			mmsi += dataBuffer[idx] << numShift;
			numShift -= 4;
		}
		return mmsi;
	}	
}

/************************************************************************
* Name   : getAskedMsgInfo
* Brief  : 从数据缓存中获取被询问消息的信息，包括消息号和时隙偏置
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组，每个字节仅低4位有效
*          start        -  数据起始地址
*          isMSB        -  指示是否在高位比特，只有两种可能，在第3位和在第1位
*                              isMSB == SET，则指示从第3位开始，否则起始位为1
* Return : MsgIdSlotOffset -  被询问消息的信息，包括消息ID和时隙偏置，消息ID为
*							  6个比特，而时隙偏置则为12个比特
************************************************************************/
MsgIdSlotOffset getAskedMsgInfo(u8 const *dataBuffer, u8 start, FlagStatus isMSB){
	MsgIdSlotOffset msgInfo;

	if(isMSB == SET){
		msgInfo.msgId = dataBuffer[start++] << 2;
		msgInfo.msgId += (dataBuffer[start] & 0x0C) >> 2;
		msgInfo.slotOffset = (dataBuffer[start++] & 0x03) << 10;
		msgInfo.slotOffset += dataBuffer[start++] << 6;
		msgInfo.slotOffset += dataBuffer[start++] << 2;
		msgInfo.slotOffset += dataBuffer[start] & 0x0C;
	}else{
		msgInfo.msgId = (dataBuffer[start++] & 0x03) << 4;
		msgInfo.msgId += dataBuffer[start++];
		msgInfo.slotOffset = dataBuffer[start++] << 8;
		msgInfo.slotOffset += dataBuffer[start++] << 4;
		msgInfo.slotOffset += dataBuffer[start];
	}
	return msgInfo;
}

/************************************************************************
* Name   : getReserveSlotInfo
* Brief  : 从数据缓存中获取基站预约的时隙的一些信息
* Author : ZuoDahua
* Param  : dataBuffer   -  数据缓存数组，每个字节仅低4位有效
*          start        -  数据起始地址
*          isMSB        -  指示是否在高位比特，只有两种可能，在第3位和在第1位
*                              isMSB == SET，则指示从第3位开始，否则起始位为1
* Return : ReserveSlotInfo -  被预约的时隙信息
************************************************************************/
ReserveSlotInfo getReserveSlotInfo(u8 const *dataBuffer, u8 start, FlagStatus isMSB){
	ReserveSlotInfo slotInfo;
	u8 idx;
	if(isMSB == SET){
		slotInfo.slotOffset = 0;
		for(idx = 0; idx < 3; idx++)
			slotInfo.slotOffset += dataBuffer[start++] << (4*(2-idx));
		slotInfo.numSlot = dataBuffer[start++];
		slotInfo.overTime = (dataBuffer[start] & 0x0E) >> 1;
		slotInfo.increment = (dataBuffer[start++] & 0x01) << 10;
		slotInfo.increment += dataBuffer[start++] << 6;
		slotInfo.increment += dataBuffer[start++] << 2;
		slotInfo.increment += (dataBuffer[start] & 0x0C) >> 2;
	}else{
		slotInfo.slotOffset = (dataBuffer[start++] & 0x03) << 10;
		for(idx = 0; idx < 2; idx++)
			slotInfo.slotOffset += dataBuffer[start++] << (6 - 4*idx);
		slotInfo.slotOffset += (dataBuffer[start] & 0x0C) >> 2;
		slotInfo.numSlot = (dataBuffer[start++] & 0x03) << 2;
		slotInfo.numSlot += (dataBuffer[start] & 0x0C) >> 2;
		slotInfo.overTime = (dataBuffer[start++] & 0x03) << 1;
		slotInfo.overTime += (dataBuffer[start] & 0x08) >> 3;
		slotInfo.increment = (dataBuffer[start++] & 0x07) << 8;
		for(idx = 0; idx < 2; idx++)
			slotInfo.increment += dataBuffer[start++] << (4*(1-idx));
	}
	return slotInfo;
}

/************************************************************************
* Name   : isValidBSMMSI
* Brief  : 判断当前输入的MMSI是否是合法的基站的MMSI
* Author : ZuoDahua
* Param  : MMSI  -  待验证的MMSI号
*        岸站的MMSI特征为 ：    00MIDXXXX
*              MID 为地区编号，M最低位2，最高位7
* Return : bool  -   是否合法
************************************************************************/
bool isValidBSMMSI(u32 MMSI){
	u32 minMMSI = 2000000;
	u32 maxMMSI = 8000000;
	if (MMSI > minMMSI && MMSI < maxMMSI)
		return SET;
	else
		return RESET;
}

/************************************************************************
* Name   : getLongitudeFromBuffer
* Brief  : 从数据Buffer中获取经度信息
* Author : ZuoDahua
* Param  : longitude      --- 经度信息，必须传入一个已经开辟空间的内存区域，18个比特
*          dataBuffer     --- 数据信息缓冲区
*          startByte      --- 起始字节
*          startBit       --- 起始字节中的起始比特，0是高位，3是低位
* Return : void
************************************************************************/
void getLongitudeFromBuffer(u32 *const longitude, const u8 *dataBuffer, u8 startByte, u8 startBit){
	u16 lenStrm = 18;
	u32 firstByteNum, lastByteNum;
	u8 numBytes;
	u8 idx;
	if (longitude == NULL || dataBuffer == NULL || startByte < 0 || startBit < 0)
	{
		return;
	}
	switch (startBit)
	{
	case 0:
		firstByteNum = (dataBuffer[startByte]) << (lenStrm-4);
		numBytes = 3;
		lastByteNum = (dataBuffer[startByte+4] & 0x0C) >> 2;
		lenStrm -= 4;
		break;
	case 1:
		firstByteNum = (dataBuffer[startByte] & 0x07) << (lenStrm-3);
		numBytes = 3;
		lastByteNum = (dataBuffer[startByte+4] & 0x0E) >> 1;
		lenStrm -= 3;
		break;
	case 2:
		firstByteNum = (dataBuffer[startByte] & 0x03) << (lenStrm-2);
		numBytes = 4;
		lastByteNum = 0;
		lenStrm -= 2;
		break;
	case 3:
		firstByteNum = (dataBuffer[startByte] & 0x01) << (lenStrm-1);
		numBytes = 4;
		lastByteNum = (dataBuffer[startByte+5] & 0x08) >> 3;
		lenStrm -= 1;
		break;
	}
	*longitude = firstByteNum;
	startByte++;
	for(idx = 0; idx < numBytes; idx++){
		*longitude += (dataBuffer[startByte+idx]) << (lenStrm-4);
		lenStrm -= 4;
	}
	*longitude += lastByteNum;
}

/************************************************************************
* Name   : getLatitudeFromBuffer
* Brief  : 从数据Buffer中获取纬度信息
* Author : ZuoDahua
* Param  : latitude       --- 纬度信息，必须传入一个已经开辟空间的内存区域，17个比特
*          dataBuffer     --- 数据信息缓冲区
*          startByte      --- 起始字节
*          startBit       --- 起始字节中的起始比特，0是高位，3是低位
* Return : void
************************************************************************/
void getLatitudeFromBuffer(u32 *const latitude, const u8 *dataBuffer, u8 startByte, u8 startBit){
	u16 lenStrm = 17;
	u32 firstByteNum, lastByteNum;
	u8 numBytes;
	u8 idx;
	if (latitude == NULL || dataBuffer == NULL || startByte < 0 || startBit < 0)
	{
		return;
	}
	switch (startBit)
	{
	case 0:
		firstByteNum = (dataBuffer[startByte]) << (lenStrm-4);
		numBytes = 3;
		lastByteNum = (dataBuffer[startByte+4] & 0x08) >> 3;
		lenStrm -= 4;
		break;
	case 1:
		firstByteNum = (dataBuffer[startByte] & 0x07) << (lenStrm-3);
		numBytes = 3;
		lastByteNum = (dataBuffer[startByte+4] & 0x0C) >> 2;
		lenStrm -= 3;
		break;
	case 2:
		firstByteNum = (dataBuffer[startByte] & 0x03) << (lenStrm-2);
		numBytes = 3;
		lastByteNum = (dataBuffer[startByte+4] & 0x0E) >> 1;
		lenStrm -= 2;
		break;
	case 3:
		firstByteNum = (dataBuffer[startByte] & 0x01) << (lenStrm-1);
		numBytes = 4;
		lastByteNum = 0;
		lenStrm -= 1;
		break;
	}
	*latitude = firstByteNum;
	startByte++;
	for(idx = 0; idx < numBytes; idx++){
		*latitude += (dataBuffer[startByte+idx]) << (lenStrm-4);
		lenStrm -= 4;
	}
	*latitude += lastByteNum;
}


void showNumInString(unsigned int num, int strLength){
	int i;
	char *str = (char*) calloc(strLength, sizeof(char));
	num2str(str, num, strLength);
	for(i = 0; i < strLength; i++){
		printf("%d", *(str+i));
	}
	printf("\n");
	free(str);
}

void num2str(char *str, unsigned int num, int strLength){
	int index = strLength-1;
	if (str == NULL)
	{
		return;
	}
	do 
	{
		str[index--] = (num%10) & 0xFF;
		num /= 10;
	} while (num > 0 || index >= 0);
}
