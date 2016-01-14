/**
********************************************************************
* @file      AISMissionGenerator.h
* @author    Digital Design Team
* @version   1.0.0
* @date      2015/12/28   16:46
********************************************************************
* @brief     
*			AIS��������ģ�飬��������CRC�������ݽ���ȥ������䣬
*           ��֡�������ʼ�ͽ�����������ѵ�����С���ʼ��־��CRC��
*           ������־����������������NRZI����
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

// ����������
int bitStuff(u8 *srcDataBuffer, int numBits, u8 *dstDataBuffer, int dstOffset);

// ����CRC
void crcCalculator(u8 *dataBuffer, u8 numBytes, u8 *crcResult);

// �������ֶν��б������е��������ֽڽ��з��飬ÿ���ֽ������Чλ�����
void bitAdjusting(u8 *dataBuffer, int numBytes);

/* ʱ϶���� */
extern u8 missionSlot[NumSlotsPerMinute];
/* �����͵��������� */
extern AISMission missionDatas[MaxMissionNum+MaxReTxMissionNum];

/* �������񣬰�����֡�Լ�����ʱ϶ */
void generateMission();
/* ����һ�����Ϣ���˴�ָ����״̬�����ŵ���Ϣ����������Ϣ�洢λ��ָʾ */
void dealCommMission();
/* �����ط������񣬲�������Ϣ�洢λ��ָʾ */
void dealReTxMission();
/* ��missionDatas��Ѱ�Ҵ��ڿ���״̬������ռ� */
int findIdleMissionIndex(FlagStatus isCommMission);
/* AIS�ź���֡������CRC���㡢����˳�������������� */
int aisFraming(u8 *srcMsg, u8 numBytes, AISMission *mission);
/* ��������Ϣ���н��кϲ� */
void dataCombine(u8 *dstMsg, u8 *srcMsg1, u8 numByte1, u8 *srcMsg2, u8 numByte2);
// �����������ǰ���־λ�����������յ��ֽ���
int addStartEndTag(u8 *dataBuffer, int numMsgBits);
/* ����������е����ݽ��г�ʼ����������Ҫ�ǽ������״̬��Ϊ�����״̬ */
void initMissionDatas();
/* ʱ϶ѡ���㷨������������Ŀ�ʼʱ϶�Լ�ʱ϶ѡ���ȣ�ѡ��ʱ϶����д��ָ��������ʱ϶���� */
FlagStatus slotSelect(u16 rcvSlot, u8 generateSlotPeriod, int missionIndex);
/* ����AIS��Ϣ��֡�Լ�ʱ϶ѡ�񣬼� aisFraming��slotSelect����� */
FlagStatus aisFramingAndSlotSelect(u8 *dataBuffer, int numMsgBits, int missionIndex, u16 rcvSlot, u8 generateSlotPeriod);
/* Ѱ�Ҵ��������Ϣ����������Ϣ�ţ����û���ҵ���Ҫ����������򷵻�FRAMELENGTH */
u16 findFSMFrameDataIndex();

//-------------------------------------------------------------------------
// һЩ���߷���
//-------------------------------------------------------------------------

// ��ȡָ��λ�õı���ֵ
u8 getBitValue(u8 *dataBuffer, int idxBit);
// ��ָ��λ�õ�ֵ����Ϊָ����ֵ
void setBitValue(u8 *dataBuffer, int idxbit, u8 value);
// ��ָ��λ��֮���һ���ֽ���������Ϊָ�����ֽ�ֵ
void setByteValue(u8 *dataBuffer, int offset, u8 byteValue);
// ��ָ�����ݻ�������ʼ��Ϊdataֵ
void clearDataBuffer(u8 *dataBuffer, int numBytes, u8 data);
// ����������Buffer���а�λ��򣬲�������洢��dstDataBuffer��
void xorDataBuffer(u8 *srcDataBuffer, u8 *dstDataBuffer, int numBytes);
// ���ֽڽ��з�ת
u8 reverseByte(u8 data);
// ��ָ�����ļ��ж��������ֽ�
int readDataFromFile(char *filePath, u8 *dataBuffer);

#endif

