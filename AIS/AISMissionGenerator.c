/**
********************************************************************
* @file      AISMissionGenerator.c
* @author    Didital Design Team
* @version   
* @date      2015/12/28   17:27
********************************************************************
* @brief     
*			AIS��������ģ�飬��������CRC�������ݽ���ȥ������䣬
*           ��֡�������ʼ�ͽ�����������ѵ�����С���ʼ��־��CRC��
*           ������־����������������NRZI����
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#include "AISMissionGenerator.h"
#include "FSMControl.h"
#include <stdlib.h>
#include <stdio.h>


// CRC �����õ������ɶ���ʽ
u8 gx[] = {0x10, 0x21};

u8 onesValue[] = {0xFF, 0xFF};

// ��¼��ǰҪ�������ͨ����ָʾ���
int toDealComMissionIndex;

// ��¼��ǰҪ������ط�����ı��
int toDealReMissionIndex;

/* ʱ϶���� */
u8 missionSlot[NumSlotsPerMinute];
/* �����͵��������� */
AISMission missionDatas[MaxMissionNum+MaxReTxMissionNum];

u8 crcResult[2];

/* �������˳������Ϣ���к�CRC������������Ϊ41���ֽڣ���Ϣ19��312���أ� */
u8 msgAndCRCBuffer[41];

/************************************************************************
* Name      : bitStuff
* Brief     : 
*			�����ݽ��б�����䣬��ԭʼ�����з���5����1���Ͳ�һ��0��
*           ��󷵻��ܵı�����
* Author    : ZuoDahua
* Param     : srcDataBuffer   ---   ԭʼ����Buffer���Ա�����ʽ���
* Param     : dstDataBuffer   ---   ��������Buffer���Ա�����ʽ���
* Param     : numBits      ---   ָʾ���ݱ��ص�����
* Param     : dstOffset   ---  ָʾ����Ŀ�����ݴ洢����ƫ�Ʊ�����
* Return    : int          ---   �������ȥ�������������ܵı�����
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
*		���������ֶε�CRC��CRC��16�����أ����ɶ���ʽΪ x16 + x12 + x5 + 1
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numBytes
* Param     : crcResult
* Return    : void
************************************************************************/
void crcCalculator(u8 *dataBuffer, u8 numBytes, u8 *crcResult){
	int i, j;
	u8 lsbData;

	// ��CRC������������г�ʼ������
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
*	�������ֶν��б������е��������ֽڽ��з��飬ÿ���ֽ������Чλ�����
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
*		��FSMControl.h�н���fsm_dataStruct���飬
*   �ҵ�״̬���°��ŵ�����Ȼ����о�����������ɹ�����
*   ������ԭʼ��Ϣ������֡�Լ�����ʱ϶
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void generateMission(){
	// �ȴ��������Ĵ���������
	dealCommMission();
	// �ٴ�����Ҫ�ط�������
	dealReTxMission();
}

/************************************************************************
* Name      : dealCommMission
* Brief     : 
*		����һ�����Ϣ���˴�ָ����״̬�����ŵ���Ϣ����������Ϣ�洢λ��ָʾ
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void dealCommMission(){
	int missionIndex;
	u8 *msgBuffer;    // ��Ϣ����
	u16 msgBitLength; // ��Ϣ�ı��س���
	int frameByteNum;
	u16 missionDataIndex;

	missionIndex = findIdleMissionIndex(SET);
	// ������µ���Ϣ19�����ȴ�����Ϣ19
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
*		�����ط������񣬲�������Ϣ�洢λ��ָʾ
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void dealReTxMission(){
	int missionIndex;
	u8 *msgBuffer;    // ��Ϣ����
	u16 msgBitLength; // ��Ϣ�ı��س���
	int frameByteNum;
	u16 missionDataIndex;
	u16 idx;
	u16 deltaSlot;

	missionIndex = findIdleMissionIndex(RESET);
	// Ѱ����Ҫ�ط�������
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
*		Ѱ�Ҵ��������Ϣ����������Ϣ�ţ����û���ҵ���Ҫ���������
*       �򷵻�FRAMELENGTH
*       Ϊ�˱�֤�����ǰ��˳���ϵ������Ѱַ��ÿ�ζ���toDealMissionIndex��ʼ
*       ����
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
*		����AIS��Ϣ��֡�Լ�ʱ϶ѡ�񣬼� aisFraming��slotSelect�����
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numMsgBits
* Param     : rcvSlot
* Param     : generateSlotPeriod
* Return    : FlagStatus   ---   ����������ɳɹ������᷵��SET
*                                �����������ʧ�ܣ����᷵��RESET
************************************************************************/
FlagStatus aisFramingAndSlotSelect(u8 *dataBuffer, int numMsgBits, int missionIndex, u16 rcvSlot, u8 generateSlotPeriod){
	int frameByteNum;

	// �ж���Ϣ�����Ƿ�8��������
	if (numMsgBits % 8 != 0 || missionIndex == MaxMissionNum+MaxReTxMissionNum)
	{
		return RESET;
	}

	frameByteNum = aisFraming(dataBuffer, numMsgBits, &missionDatas[missionIndex]);
	if (slotSelect(rcvSlot, generateSlotPeriod, missionIndex) == SET)
	{
		// ���ʱ϶ѡ��ɹ�������Ҫ���������isDone���Ϊ ��δ���״̬��
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
*		ʱ϶ѡ���㷨������������Ŀ�ʼʱ϶�Լ�ʱ϶ѡ���ȣ�ѡ��ʱ϶��
*		��д��ָ��������ʱ϶����
* Author    : ZuoDahua
* Param     : rcvSlot
* Param     : generateSlotPeriod   ---    ʱ����������Ϊ��λ
* Param     : missionIndex
* Return    : FlagStatus   ---    ���ʱ϶ѡ����ɣ����᷵��SET��
*                                 ���ʱ϶ѡ��ʧ�ܣ�������RESET
************************************************************************/
FlagStatus slotSelect(u16 rcvSlot, u8 generateSlotPeriod, int missionIndex){
	u16 idx_slot;
	u16 slotRange;
	u16 selSlots[10];
	// �������Ϣ19����generateSlotPeriod == 0
	if (generateSlotPeriod == 0x00)
	{
		missionDatas[missionIndex].missionSlotCnt = 1;
		missionSlot[rcvSlot] = missionIndex;
		return SET;
	}
	// ���������Ҫ����ʱ϶ѡ��
	slotRange = generateSlotPeriod * NumSlotsPerSecond;

	missionDatas[missionIndex].missionSlotCnt = 0;
	for (idx_slot = 0; idx_slot < slotRange; idx_slot++)
	{
		if ((getBitValue(fsm_dataStruct.reservedSlotByte, rcvSlot+idx_slot) == 0x00) && (missionSlot[rcvSlot+idx_slot] == MaxReTxMissionNum+MaxMissionNum))
		{
			// ��ǰʱ϶����
			selSlots[missionDatas[missionIndex].missionSlotCnt++] = rcvSlot + idx_slot;
			// ���ѡ��10��ʱ϶���Ϳ����˳�ѭ��
			if (missionDatas[missionIndex].missionSlotCnt == 10)
			{
				break;
			}
		}
	}
	// ���ʱ϶û��ѡ��ã��򽫸�����
	if (missionDatas[missionIndex].missionSlotCnt == 0)
	{
		// ��ʱ��ʱû�����κβ������Ժ���Ҫ�Ļ������Ը�
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
*		��missionDatas��Ѱ�Ҵ��ڿ���״̬������ռ�
* Author    : ZuoDahua
* Param     : isCommMission --- ָʾ�Ƿ���Ѱ����ͨ���� 
				SET - Ѱ����ͨ���� RESET - Ѱ���ط�����
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
*	AIS�ź���֡������CRC���㡢����˳��������������
* Author    : ZuoDahua
* Param     : srcMsg --- ԭʼ����Ϣ���ݣ���״̬������
* Param     : numBytes --- ��Ϣ���ݵ��ֽ���
* Param     : mission --- �����֡������ݴ洢��ַ
* Return    : int --- �����ֽ���
************************************************************************/
int aisFraming(u8 *srcMsg, u8 numBytes, AISMission *mission){
	int numBits;
	int totalBits;
	int totalBytes;
	// �Ƚ���CRC����
	crcCalculator(srcMsg, numBytes, crcResult);
	// ������Ϣ���еı���˳��
	bitAdjusting(srcMsg, numBytes);
	// ������˳������Ϣ˳���CRC�������
	dataCombine(msgAndCRCBuffer, srcMsg, numBytes, crcResult, 2);
	// ���б������
	numBits = numBytes + 2;
	numBits <<= 3;
	totalBits = bitStuff(msgAndCRCBuffer, numBits, mission->frameData, 8);
	// ����������ӱ�־λ "0111_1110"
	totalBytes = addStartEndTag(mission->frameData, totalBits);
	return totalBytes;
}

/************************************************************************
* Name      : addStartEndTag
* Brief     : 
*		�����������ǰ���־λ�����������յ��ֽ���
* Author    : ZuoDahua
* Param     : dataBuffer
* Param     : numMsgBits
* Return    : int
************************************************************************/
int addStartEndTag(u8 *dataBuffer, int numMsgBits){
	int totalBits, i;
	// �����ڵ�һ���ֽڴ������ʼ��־λ
	dataBuffer[0] = 0x7E;
	// �ڽ�β��ʼ����ӽ�����־λ
	setByteValue(dataBuffer, numMsgBits+8, 0x7E);
	// ���0�������в��㵽�ֽڱ���
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
*		����������е����ݽ��г�ʼ����������Ҫ�ǽ������״̬��Ϊ�����״̬
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void initMissionDatas(){
	int i;
	for(i = 0; i < MaxMissionNum+MaxReTxMissionNum; i++){
		missionDatas[i].isDone = SET;
	}
	// �������������ų�ʼ��Ϊ0
	toDealComMissionIndex = 0;
	toDealReMissionIndex = 0;
	// ��ʼ��ʱ϶�ŵ�����
	for (i = 0; i < NumSlotsPerMinute; i++)
	{
		missionSlot[i] = MaxMissionNum + MaxReTxMissionNum;
	}
}
       
/************************************************************************
* Name      : dataCombine
* Brief     : 
*	��������Ϣ���н��кϲ�
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
*		�������ֽڽ��з�ת
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
*		����������Buffer���а�λ��򣬲�������洢��dstDataBuffer��
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
*		��ָ�������ݻ��������г�ʼ������ʼֵΪָ����dataֵ
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
*		���ֽ������и���ȡ��ָ���ı���
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
*	��ָ��λ�õ�ֵ����Ϊָ����ֵ
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
*		��ָ��λ��֮���һ���ֽ���������Ϊָ�����ֽ�ֵ
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
*		��ָ�����ļ��ж��������ֽ�
* Author    : ZuoDahua
* Param     : filePath  ---  �ļ�·��
* Param     : dataBuffer  ---  ���ݴ洢�ռ�
* Return    : int  ---  ���������ֽ�����
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
	printf("\n\n ---------  ���ڴ��ļ� (%s) �л�ȡ���ݣ���ȴ�    --------\n\n", filePath);
	numByte = 0;
	while (fscanf(file, "%d", &data) != EOF)
	{
		dataBuffer[numByte++] = data;
	}
	fclose(file);

	printf("\n\n ---------------  ���ݻ�ȡ��ϣ��ܹ�%d���ֽ�    ----------------------\n\n", numByte);

	return numByte;
}



