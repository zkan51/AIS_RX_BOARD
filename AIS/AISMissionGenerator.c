/**
********************************************************************
* @file      AISMissionGenerator.c
* @author    Didital Design Team
* @version   
* @date      2015/12/28   17:27
********************************************************************
* @brief     
*			AIS任务生成模块，包括生成CRC，对数据进行去比特填充，
*           组帧（添加起始和结束缓冲区、训练序列、起始标志、CRC、
*           结束标志、结束缓冲区），NRZI编码
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#include "AISMissionGenerator.h"
#include "FSMControl.h"
#include <stdlib.h>
#include <stdio.h>


// CRC 计算用到的生成多项式
u8 gx[] = {0x10, 0x21};

u8 onesValue[] = {0xFF, 0xFF};

// 记录当前要处理的普通任务指示标号
int toDealComMissionIndex;

// 记录当前要处理的重发任务的标号
int toDealReMissionIndex;

/* 时隙数组 */
u8 missionSlot[NumSlotsPerMinute];
/* 待发送的任务数组 */
AISMission missionDatas[MaxMissionNum+MaxReTxMissionNum];

u8 crcResult[2];

/* 缓存调整顺序后的消息序列和CRC结果，长度最大为41个字节（消息19长312比特） */
u8 msgAndCRCBuffer[41];

/************************************************************************
* Name      : bitStuff
* Brief     : 
*			对数据进行比特填充，在原始数据中发现5个连1，就补一个0，
*           最后返回总的比特数
* Author    : ZuoDahua
* Param     : srcDataBuffer   ---   原始数据Buffer，以比特形式存放
* Param     : dstDataBuffer   ---   最终数据Buffer，以比特形式存放
* Param     : numBits      ---   指示数据比特的数量
* Param     : dstOffset   ---  指示最终目的数据存储区的偏移比特数
* Return    : int          ---   返回添加去比特填充后数据总的比特数
************************************************************************/
int bitStuff(u8 *srcDataBuffer, int numBits, u8 *dstDataBuffer, int dstOffset){
	int cnt = 0;
	int srcIndex, dstIndex, numZeros;
	u8 currentBit;

	if (numBits == 0)
	{
		return -1;
	}

	for (srcIndex = 0, dstIndex = 0; srcIndex < numBits; srcIndex++)
	{
		currentBit = getBitValue(srcDataBuffer, srcIndex);
		setBitValue(dstDataBuffer, (dstOffset+dstIndex++), currentBit);
		if (currentBit == 1)
		{
			if (cnt == 4)
			{
				setBitValue(dstDataBuffer, (dstOffset+dstIndex++), 0);
				cnt = 0;
			}else{
				cnt++;
			}
		}else{
			cnt = 0;
		}
	}
	return dstIndex;
}


/************************************************************************
* Name      : crcCalculator
* Brief     : 
*		计算数据字段的CRC，CRC共16个比特，生成多项式为 x16 + x12 + x5 + 1
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numBytes
* Param     : crcResult
* Return    : void
************************************************************************/
void crcCalculator(u8 *dataBuffer, u8 numBytes, u8 *crcResult){
	int i, j;
	u8 lsbData;

	// 对CRC结果缓冲区进行初始化操作
	clearDataBuffer(crcResult, 2, 0xFF);

	for(i = 0; i < numBytes; i++){
		for (j = 0; j < 8; j++){
			lsbData = getBitValue(crcResult, 0);
			crcResult[0] = crcResult[0] << 1;
			setBitValue(crcResult, 7, getBitValue(crcResult, 8));
			crcResult[1] = crcResult[1] << 1;

			if (lsbData == 1)
			{
				xorDataBuffer(gx, crcResult, 2);
			}
			if (getBitValue(dataBuffer, (i<<3)+7-j) == 1)
			{
				xorDataBuffer(gx, crcResult, 2);
			}
		}
	}

	xorDataBuffer(onesValue, crcResult, 2);
}

/************************************************************************
* Name      : bitAdjusting
* Brief     : 
*	对数据字段进行比特序列调整，按字节进行分组，每个字节最低有效位先输出
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numBytes
* Return    : void
************************************************************************/
void bitAdjusting(u8 *dataBuffer, int numBytes){
	int i;
	for(i = 0; i < numBytes; i++){
		dataBuffer[i] = reverseByte(dataBuffer[i]);
	}
}

/************************************************************************
* Name      : generateMission
* Brief     : 
*		从FSMControl.h中解析fsm_dataStruct数组，
*   找到状态机新安排的任务，然后进行具体的任务生成工作，
*   包括对原始信息进行组帧以及安排时隙
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void generateMission(){
	// 先处理正常的待发送任务
	dealCommMission();
	// 再处理需要重发的任务
	dealReTxMission();
}

/************************************************************************
* Name      : dealCommMission
* Brief     : 
*		处理一般的消息，此处指的是状态机安排的消息，并返回消息存储位置指示
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void dealCommMission(){
	int missionIndex;
	u8 *msgBuffer;    // 消息内容
	u16 msgBitLength; // 消息的比特长度
	int frameByteNum;
	u16 missionDataIndex;

	missionIndex = findIdleMissionIndex(SET);
	// 如果有新的消息19，则先处理消息19
	if (fsm_dataStruct.fsm_frame19Struct.msgState == MSGNEW)
	{
		msgBuffer = fsm_dataStruct.fsm_frame19Struct.encapDataByte;
		msgBitLength = fsm_dataStruct.fsm_frame19Struct.encapDataLen;
		if (aisFramingAndSlotSelect(msgBuffer, msgBitLength, missionIndex, fsm_dataStruct.fsm_frame19Struct.rcvSlot, 0) == SET)
		{
			fsm_dataStruct.fsm_frame19Struct.msgState = MSGOLD;
		}
	}else
	{
		missionDataIndex = findFSMFrameDataIndex();
		msgBuffer = fsm_dataStruct.fsm_frameStruct[missionDataIndex].encapDataByte;
		msgBitLength = fsm_dataStruct.fsm_frameStruct[missionDataIndex].encapDataLen;
		if (aisFramingAndSlotSelect(msgBuffer, msgBitLength, missionIndex, fsm_dataStruct.fsm_frameStruct[missionDataIndex].rcvSlot, fsm_dataStruct.fsm_frameStruct[missionDataIndex].generateSlotPeriod) == SET)
		{
			fsm_dataStruct.fsm_frameStruct[missionDataIndex].msgState = MSGOLD;
		}
	}
}

/************************************************************************
* Name      : dealReTxMission
* Brief     : 
*		处理重发的任务，并返回消息存储位置指示
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void dealReTxMission(){
	int missionIndex;
	u8 *msgBuffer;    // 消息内容
	u16 msgBitLength; // 消息的比特长度
	int frameByteNum;
	u16 missionDataIndex;
	u16 idx;
	u16 deltaSlot;

	missionIndex = findIdleMissionIndex(RESET);
	// 寻找需要重发的任务
	missionDataIndex = MaxReTxMissionNum;
	for (idx = 0; idx < MaxReTxMissionNum; idx++)
	{
		if (missionDatas[MaxMissionNum+((idx+toDealReMissionIndex)%MaxReTxMissionNum)].needReTx == SET)
		{
			deltaSlot = abs(missionDatas[MaxMissionNum+((idx+toDealReMissionIndex)%MaxReTxMissionNum)].lastTxSlot - fsm_dataStruct.realSlot);
			if (deltaSlot >= NumSlotsPerMinute/2 && deltaSlot < NumSlotsPerMinute/2+40)
			{
				missionDataIndex = idx;
				toDealReMissionIndex = (idx+toDealReMissionIndex) % MaxReTxMissionNum;
				break;
			}
		}
	}
	if (missionDataIndex == MaxReTxMissionNum)
	{
		return;
	}
	if (slotSelect(fsm_dataStruct.realSlot, 10, missionIndex) == SET)
	{
		missionDatas[MaxMissionNum+toDealReMissionIndex].needReTx = RESET;
	}
}

/************************************************************************
* Name      : findFSMFrameDataIndex
* Brief     : 
*		寻找待处理的消息，并返回消息号，如果没有找到需要处理的任务，
*       则返回FRAMELENGTH
*       为了保证任务的前后顺序关系，任务寻址将每次都从toDealMissionIndex开始
*       查找
* Author    : ZuoDahua
* Return    : u16
************************************************************************/
u16 findFSMFrameDataIndex(){
	u16 missionIndex, idxMission;
	missionIndex = FRAMELENGTH;
	for (idxMission = 0; idxMission < FRAMELENGTH; idxMission++)
	{
		if (fsm_dataStruct.fsm_frameStruct[(idxMission+toDealComMissionIndex)%FRAMELENGTH].msgState == MSGNEW)
		{
			missionIndex = idxMission;
			toDealComMissionIndex = (idxMission + 1) % FRAMELENGTH;
			break;
		}
	}
	return missionIndex;
}


/************************************************************************
* Name      : aisFramingAndSlotSelect
* Brief     : 
*		进行AIS消息组帧以及时隙选择，即 aisFraming和slotSelect的组合
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numMsgBits
* Param     : rcvSlot
* Param     : generateSlotPeriod
* Return    : FlagStatus   ---   如果任务生成成功，将会返回SET
*                                如果任务生成失败，将会返回RESET
************************************************************************/
FlagStatus aisFramingAndSlotSelect(u8 *dataBuffer, int numMsgBits, int missionIndex, u16 rcvSlot, u8 generateSlotPeriod){
	int frameByteNum;

	// 判断消息长度是否8的整数倍
	if (numMsgBits % 8 != 0 || missionIndex == MaxMissionNum+MaxReTxMissionNum)
	{
		return RESET;
	}

	frameByteNum = aisFraming(dataBuffer, numMsgBits, &missionDatas[missionIndex]);
	if (slotSelect(rcvSlot, generateSlotPeriod, missionIndex) == SET)
	{
		// 如果时隙选择成功，则需要将该任务的isDone标记为 “未完成状态”
		missionDatas[missionIndex].isDone = RESET;
		return SET;
	}else{
		missionDatas[missionIndex].isDone = SET;
		return RESET;
	}
}
/************************************************************************
* Name      : slotSelect
* Brief     : 
*		时隙选择算法，将根据输入的开始时隙以及时隙选择宽度，选择时隙，
*		并写入指定的任务时隙部分
* Author    : ZuoDahua
* Param     : rcvSlot
* Param     : generateSlotPeriod   ---    时间间隔，以秒为单位
* Param     : missionIndex
* Return    : FlagStatus   ---    如果时隙选择完成，将会返回SET，
*                                 如果时隙选择失败，将返回RESET
************************************************************************/
FlagStatus slotSelect(u16 rcvSlot, u8 generateSlotPeriod, int missionIndex){
	u16 idx_slot;
	u16 slotRange;
	u16 selSlots[10];
	// 如果是消息19，则generateSlotPeriod == 0
	if (generateSlotPeriod == 0x00)
	{
		missionDatas[missionIndex].missionSlotCnt = 1;
		missionSlot[rcvSlot] = missionIndex;
		return SET;
	}
	// 其余情况需要进行时隙选择
	slotRange = generateSlotPeriod * NumSlotsPerSecond;

	missionDatas[missionIndex].missionSlotCnt = 0;
	for (idx_slot = 0; idx_slot < slotRange; idx_slot++)
	{
		if ((getBitValue(fsm_dataStruct.reservedSlotByte, rcvSlot+idx_slot) == 0x00) && (missionSlot[rcvSlot+idx_slot] == MaxReTxMissionNum+MaxMissionNum))
		{
			// 当前时隙可用
			selSlots[missionDatas[missionIndex].missionSlotCnt++] = rcvSlot + idx_slot;
			// 如果选满10个时隙，就可以退出循环
			if (missionDatas[missionIndex].missionSlotCnt == 10)
			{
				break;
			}
		}
	}
	// 如果时隙没有选择好，则将该任务
	if (missionDatas[missionIndex].missionSlotCnt == 0)
	{
		// 此时暂时没有做任何操作，以后需要的话，可以改
		return RESET;
	}else{
		for(idx_slot = 0; idx_slot < missionDatas[missionIndex].missionSlotCnt; idx_slot++){
			missionSlot[selSlots[idx_slot]] = missionIndex;
		}
		return SET;
	}
}

/************************************************************************
* Name      : findIdleMissionIndex
* Brief     : 
*		在missionDatas中寻找处于空闲状态的任务空间
* Author    : ZuoDahua
* Param     : isCommMission --- 指示是否是寻找普通任务， 
				SET - 寻找普通任务， RESET - 寻找重发任务
* Return    : int
************************************************************************/
int findIdleMissionIndex(FlagStatus isCommMission){
	int i, start, end;
	int result = MaxMissionNum + MaxReTxMissionNum;
	if (isCommMission == RESET)
	{
		start = MaxMissionNum;
		end = result;
	}else{
		start = 0;
		end = MaxMissionNum;
	}
	for(i = start ; i < end; i++){
		if (missionDatas[i].isDone == SET)
		{
			result = i;
			break;
		}
	}
	return result;
}

/************************************************************************
* Name      : aisFraming
* Brief     : 
*	AIS信号组帧，包括CRC计算、比特顺序调整、比特填充
* Author    : ZuoDahua
* Param     : srcMsg --- 原始的消息数据，由状态机给出
* Param     : numBytes --- 消息数据的字节数
* Param     : mission --- 最后组帧后的数据存储地址
* Return    : int --- 返回字节数
************************************************************************/
int aisFraming(u8 *srcMsg, u8 numBytes, AISMission *mission){
	int numBits;
	int totalBits;
	int totalBytes;
	// 先进行CRC计算
	crcCalculator(srcMsg, numBytes, crcResult);
	// 调整消息序列的比特顺序
	bitAdjusting(srcMsg, numBytes);
	// 将调整顺序后的消息顺序和CRC进行组合
	dataCombine(msgAndCRCBuffer, srcMsg, numBytes, crcResult, 2);
	// 进行比特填充
	numBits = numBytes + 2;
	numBits <<= 3;
	totalBits = bitStuff(msgAndCRCBuffer, numBits, mission->frameData, 8);
	// 在序列中添加标志位 "0111_1110"
	totalBytes = addStartEndTag(mission->frameData, totalBits);
	return totalBytes;
}

/************************************************************************
* Name      : addStartEndTag
* Brief     : 
*		在序列中添加前后标志位，并返回最终的字节数
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numMsgBits
* Return    : int
************************************************************************/
int addStartEndTag(u8 *dataBuffer, int numMsgBits){
	int totalBits, i;
	// 首先在第一个字节处添加起始标志位
	dataBuffer[0] = 0x7E;
	// 在结尾开始处添加结束标志位
	setByteValue(dataBuffer, numMsgBits+8, 0x7E);
	// 最后补0，将序列补足到字节倍数
	totalBits = numMsgBits + 16;
	if (totalBits % 8 != 0)
	{
		for (i = 0; i < 8-(totalBits%8); i++)
		{
			setBitValue(dataBuffer, totalBits+i, 0);
		}
		totalBits += i;
	}
	return totalBits >> 3;
}

/************************************************************************
* Name      : initMissionDatas
* Brief     : 
*		将任务队列中的数据进行初始化操作，主要是将任务的状态改为已完成状态
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void initMissionDatas(){
	int i;
	for(i = 0; i < MaxMissionNum+MaxReTxMissionNum; i++){
		missionDatas[i].isDone = SET;
	}
	// 将待处理任务标号初始化为0
	toDealComMissionIndex = 0;
	toDealReMissionIndex = 0;
	// 初始化时隙号的内容
	for (i = 0; i < NumSlotsPerMinute; i++)
	{
		missionSlot[i] = MaxMissionNum + MaxReTxMissionNum;
	}
}
       
/************************************************************************
* Name      : dataCombine
* Brief     : 
*	将两个消息序列进行合并
* Author    : ZuoDahua
* Param     : dstMsg
* Param     : srcMsg1
* Param     : numByte1
* Param     : srcMsg2
* Param     : numBytes2
* Return    : void
************************************************************************/
void dataCombine(u8 *dstMsg, u8 *srcMsg1, u8 numByte1, u8 *srcMsg2, u8 numByte2){
	int i;
	for (i = 0; i < numByte1; i++)
	{
		dstMsg[i] = srcMsg1[i];
	}
	for (i = 0; i < numByte2; i++)
	{
		dstMsg[numByte1+i] = srcMsg2[i];
	}
}

/************************************************************************
* Name      : reverseByte
* Brief     : 
*		将输入字节进行反转
* Author    : ZuoDahua
* Param     : data
* Return    : u8
************************************************************************/
u8 reverseByte(u8 data){
	data = ((data & 0x55) << 1) | ((data & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	data = ((data & 0xF0) >> 4) | ((data & 0x0F) << 4);
}


/************************************************************************
* Name      : xorDataBuffer
* Brief     : 
*		将两个数据Buffer进行按位异或，并将结果存储在dstDataBuffer中
* Author    : ZuoDahua
* Param     : srcDataBuffer
* Param     : dstDataBuffer
* Param     : numBytes
* Return    : void
************************************************************************/
void xorDataBuffer(u8 *srcDataBuffer, u8 *dstDataBuffer, int numBytes){
	int i;
	for(i = 0; i < numBytes; i++){
		dstDataBuffer[i] ^= srcDataBuffer[i];
	}
}

/************************************************************************
* Name      : clearDataBuffer
* Brief     : 
*		将指定的数据缓冲区进行初始化，初始值为指定的data值
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numBytes
* Param     : data
* Return    : void
************************************************************************/
void clearDataBuffer(u8 *dataBuffer, int numBytes, u8 data){
	int i;
	for(i = 0; i < numBytes; i++){
		dataBuffer[i] = data;
	}
}


/************************************************************************
* Name      : getBitValue
* Brief     : 
*		从字节数组中根据取出指定的比特
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : idxBit
* Return    : u8
************************************************************************/
u8 getBitValue(u8 *dataBuffer, int idxBit){
	int i, j;
	u8 currentByte;
	i = idxBit >> 3;
	j = idxBit - (i << 3);
	currentByte = dataBuffer[i];
	return ((currentByte>>(7-j)) & 0x01);
}


/************************************************************************
* Name      : setBitValue
* Brief     : 
*	将指定位置的值设置为指定的值
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : idxbit
* Param     : value
* Return    : u8
************************************************************************/
void setBitValue(u8 *dataBuffer, int idxbit, u8 value){
	int i, j;
	i = idxbit >> 3;
	j = idxbit - (i << 3);
	if (value == 0)
	{
		dataBuffer[i] &= (~(0x01<<(7-j)));
	}else if (value == 1)
	{
		dataBuffer[i] |= (0x01 << (7-j));
	}
}

/************************************************************************
* Name      : setByteValue
* Brief     : 
*		将指定位置之后的一个字节内容设置为指定的字节值
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : offset
* Param     : byteValue
* Return    : void
************************************************************************/
void setByteValue(u8 *dataBuffer, int offset, u8 byteValue){
	int i;
	for(i = 0; i < 8; i++){
		setBitValue(dataBuffer, offset+i, (byteValue>>(7-i))&0x01);
	}
}


/************************************************************************
* Name      : readDataFromFile
* Brief     : 
*		从指定的文件中读出数据字节
* Author    : ZuoDahua
* Param     : filePath  ---  文件路径
* Param     : dataBuffer  ---  数据存储空间
* Return    : int  ---  返回数据字节数量
************************************************************************/
int readDataFromFile(char *filePath, u8 *dataBuffer){
	FILE *file;
	int data;
	int numByte;
	if (dataBuffer == NULL)
	{
		return -1;
	}
	file = fopen(filePath, "r");
	printf("\n\n ---------  正在从文件 (%s) 中获取数据，请等待    --------\n\n", filePath);
	numByte = 0;
	while (fscanf(file, "%d", &data) != EOF)
	{
		dataBuffer[numByte++] = data;
	}
	fclose(file);

	printf("\n\n ---------------  数据获取完毕，总共%d个字节    ----------------------\n\n", numByte);

	return numByte;
}



