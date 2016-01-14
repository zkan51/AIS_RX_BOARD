#include "AIS_PS_Interface.h"
#include "AIS_PS_Struct.h"
#include <stdlib.h>
#include<stdio.h>

void printMsg15Info(VDLMsg15 msg15){
	printf("--------  Msg 15 Information -----------\n");
	printf("    srcMMSI = %d\n", msg15.srcMMSI);
	printf("     dstMMSI1 = %d,  msgId1_1 = %d,  slotOffset1_1 = %d\n", msg15.dstMMSI1, msg15.msgID1_1, msg15.slotOffset1_1);
	printf("                     msgId1_2 = %d,  slotOffset1_2 = %d\n", msg15.msgID1_2, msg15.slotOffset1_2);
	printf("     dstMMSI2 = %d,  msgId2_1 = %d,  slotOffset2_1 = %d\n", msg15.dstMMSI2, msg15.msgID2_1, msg15.slotOffset2_1);
	printf("--------  Print Over -----------\n\n");
}

void testGetMMSI(){
	u8 dataBufferMSB[8] = {1, 10, 7, 13, 10, 15, 1, 12};
	u8 dataBufferLSB[8] = {0, 6, 9, 15, 6, 11, 12, 7};
	u32 mmsi;
	mmsi = getMMSI(dataBufferMSB, 0, SET);
	printf("%d\n", mmsi);
}

void testGetMsgInfo(){
	u8 dataBufferMSB[] = {4, 8, 0, 5, 0};
	u8 dataBufferLSB[] = {1, 2, 0, 1, 4};
	MsgIdSlotOffset msgInfoMsb, msgInfoLsb;
	msgInfoMsb = getAskedMsgInfo(dataBufferMSB, 0, SET);
	msgInfoLsb = getAskedMsgInfo(dataBufferLSB, 0, RESET);

	printf("MSb  ----    msgId = %d     slotOffset = %d\n", msgInfoMsb.msgId, msgInfoMsb.slotOffset);
	printf("LSb  ----    msgId = %d     slotOffset = %d\n", msgInfoLsb.msgId, msgInfoLsb.slotOffset);
}

void testGetInfoForMsg15(){
	u8 dataBuffer1[] = {0, 3, 12, 1, 10, 7, 13, 10, 15, 1, 12, 3, 4, 15, 11, 5, 14, 3, 9, 2, 0, 0, 10};
	u8 dataBuffer2[] = {3, 12, 1, 10, 7, 13, 10, 15, 1, 12, 3, 4, 15, 11, 5, 14, 3, 9, 2, 0, 0, 10, 1, 8, 0, 0, 12, 0};
	u8 dataBuffer3[] = {3, 12, 1, 10, 7, 13, 10, 15, 1, 12, 3, 4, 15, 11, 5, 14, 3, 9, 2, 0, 0, 10, 1, 8, 0, 0, 12, 1, 3, 13, 14, 4, 3, 5, 5, 4, 12, 0, 5, 0};
	getInfoForMsg15(dataBuffer1, 1, 11, channelA);
	printMsg15Info(recVDLMsg15);

	getInfoForMsg15(dataBuffer2, 0, 14, channelA);
	printMsg15Info(recVDLMsg15);

	getInfoForMsg15(dataBuffer3, 0, 20, channelA);
	printMsg15Info(recVDLMsg15);
}

void testGetReserveSlotInfo(){
	u8 dataBufferMsg[] = {0, 0, 9, 10, 6, 0, 3, 12};
	u8 dataBufferLsb[] = {0, 0, 2, 6, 9, 8, 0, 15};

	ReserveSlotInfo slotInfoMsb, slotInfoLsb;
	slotInfoLsb = getReserveSlotInfo(dataBufferLsb, 0, RESET);
	slotInfoMsb = getReserveSlotInfo(dataBufferMsg, 0, SET);
	
	printf("--------   Slot Msg Information   -------------\n\n");
	printf("   Msb  ---   slot offset = %d,    numSlot = %d\n", slotInfoMsb.slotOffset, slotInfoMsb.numSlot);
	printf("              time out  = %d,   increment = %d\n", slotInfoMsb.overTime, slotInfoMsb.increment);
	printf("   Lsb  ---   slot offset = %d,    numSlot = %d\n", slotInfoLsb.slotOffset, slotInfoLsb.numSlot);
	printf("              time out  = %d,   increment = %d\n", slotInfoLsb.overTime, slotInfoLsb.increment);
}

void printMsg20Info(){
	int i;
	printf("---------------  Msg20 Information  -----------------\n");
	printf("      基站MMSI  ----   %d \n", recVDLMsg20.MMSI);
	for(i = 0; i < recVDLMsg20.numOrderedSlots; i++){
		printf("     第 %d 个消息内容 ---  偏置数目： %d,  时隙数目： %d,  超时: %d,  增量: %d\n", 
			i+1, recVDLMsg20.slotInfos[i].slotOffset, recVDLMsg20.slotInfos[i].numSlot, recVDLMsg20.slotInfos[i].overTime, recVDLMsg20.slotInfos[i].increment);
	}
}

void testGetInfoForMsg20(){
	u8 dataBuffer1[] = {5, 0, 0, 0, 15, 12, 1, 3, 4, 0, 0, 0, 10, 2, 10, 0, 0, 4};
	u8 dataBuffer2[] = {0, 5, 0, 0, 0, 15, 12, 1, 3, 4, 0, 0, 0, 10, 2, 10, 0, 0, 4, 0, 1, 1, 7, 0, 0, 7, 0};
	u8 dataBuffer3[] = {5, 0, 0, 0, 15, 12, 1, 3, 4, 0, 0, 0, 10, 2, 10, 0, 0, 4, 0, 1, 1, 7, 0, 0, 7, 0, 0, 8, 9, 6, 0, 2, 8, 0};

	getInfoForMsg20(dataBuffer3, 0, 17);
	printMsg20Info();
}

void testGetLongitudeFromBuffer(){
	u8 dataBuffer1[] = {0, 0, 2, 13, 0, 0, 0, 0};
	u8 dataBuffer2[] = {0, 0, 1, 6, 8, 0, 0, 0};
	u32 longitude = 0;

	getLongitudeFromBuffer(&longitude, dataBuffer2, 0, 1);
	printf("经度为  --- %d\n", longitude);
}

void testGetLatitudeFromBuffer(){
	u8 dataBuffer1[] = {0, 0, 2, 11, 8, 0, 0, 0};
	u8 dataBuffer2[] = {0, 0, 1, 5, 12, 0, 0, 0};
	u32 latitude = 0;

	getLatitudeFromBuffer(&latitude, dataBuffer2, 0, 1);
	printf("纬度为  --- %d\n", latitude);
}

void testGetInfoForMsg22(){
	u8 dataBuffer1[] = {5, 8, 0, 0, 15, 12, 1, 3, 4, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 6, 4, 0, 0, 5, 7, 0, 0, 1, 14, 12, 0, 0, 2, 12, 2, 8, 0, 0, 0, 0, 0};

	getInfoForMsg22(dataBuffer1, 0, 21);
	if (isVDLMsg22NewIn == SET)
	{
		printf("---------------  Msg22 Information  -----------------\n");
		printf("      基站MMSI  ----   %d \n", recVDLMsg22.MMSI);
		printf("           第一个经纬度   ---  经度(%d),  纬度(%d)\n", recVDLMsg22.logitude1, recVDLMsg22.latitude1);
		printf("           第二个经纬度   ---  经度(%d),  纬度(%d)\n", recVDLMsg22.logitude2, recVDLMsg22.latitude2);
		printf("              是否是广播(%d),    切换区范围(%d)\n", recVDLMsg22.isBroadcast, recVDLMsg22.handOverArea);
	}
}

void testGetInfoForMsg23(){
	u8 dataBuffer1[] = {5, 12, 0, 0, 15, 12, 1, 3, 4, 0, 0, 0, 2, 12, 8, 0, 0, 10, 14, 0, 0, 3, 13, 8, 0, 0, 5, 9, 5, 8, 12, 0, 0, 0, 0, 0, 9, 3, 8, 0};
	u8 dataBuffer2[] = {0, 5, 12, 0, 0, 15, 12, 1, 3, 4, 0, 0, 0, 2, 12, 8, 0, 0, 10, 14, 0, 0, 3, 13, 8, 0, 0, 5, 9, 5, 8, 12, 0, 0, 0, 0, 0, 9, 3, 8, 0};
	getInfoForMsg23(dataBuffer2, 1, 20);
	if (isVDLMsg23NewIn == SET)
	{
		printf("---------------  Msg23 Information  -----------------\n");
		printf("      基站MMSI  ----   %d \n", recVDLMsg23.MMSI);
		printf("           第一个经纬度   ---  经度(%d),  纬度(%d)\n", recVDLMsg23.logitude1, recVDLMsg23.latitude1);
		printf("           第二个经纬度   ---  经度(%d),  纬度(%d)\n", recVDLMsg23.logitude2, recVDLMsg23.latitude2);
		printf("              台站类型(%d),   船舶货物类型(%d)\n", recVDLMsg23.stationType, recVDLMsg23.shipType);
		printf("              Tx/Rx模式(%d),   报告间隔(%d),    寂静时间(%d)\n", recVDLMsg23.TxRxMode, recVDLMsg23.reportInterval, recVDLMsg23.silentTime);
	}
}
