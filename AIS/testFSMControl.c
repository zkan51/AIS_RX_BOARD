/**
********************************************************************
* @file      testFSMControl.c
* @author    Digital Design Team
* @version   测试FSMControl.c中对应的子功能函数的输入及输出内容
* @date      2015/11/23   15:05
********************************************************************
* @brief     
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "FSMControl.h"
#include "UART2.h"
#include <stdlib.h>
//#include <conio.h>
//#include "AIS_PS_Interface.h"
//#include "AIS_PS_Struct.h"



////////////////////////for test //////////////////////////////////////////////////
/************************************************************************
* Name      : prinfBinary
* Brief     : 将字节按照二进制打印，bug是若高位是0，则打印不出来
*
* Author    : Digital Design Team
* Param     : num- 输入，待打印的字节变量
* Return    : void
************************************************************************/
void prinfBinary(u8 num)
{
	int i = num;
	char s[10];
	//itoa(i, s, 2);   //转换成字符串，进制基数为2
	printf("%s",s);  //输出
}

/************************************************************************
* Name      : testVDL_Msg15
* Brief     : 测试用msg15内容
*			（若无消息询问，则ID号和时隙偏置均为0）
* Author    : Digital Design Team
* Param     : rcvSlot- 输入，接收时隙号
* Param     : ID1- 输入，被询问ID1
* Param     : O1- 输入，被询问ID1的时隙偏置
* Param     : ID2- 输入，被询问ID2
* Param     : O2- 输入，被询问ID2的时隙偏置
* Param     : ID3- 输入，被询问ID3
* Param     : O3- 输入，被询问ID3的时隙偏置
* Param     : vdlMsg15- 输出，待写入的消息15结构体
* Return    : void
************************************************************************/
void testVDL_Msg15(u16 rcvSlot,u8 ID1,u16 O1,u8 ID2,u16 O2,u8 ID3,u16 O3,VDLMsg15 * vdlMsg15)
{
	vdlMsg15->dstMMSI1 = OWNMMSI;
	vdlMsg15->dstMMSI2 = OWNMMSI;
	vdlMsg15->msgID1_1 = ID1;
	vdlMsg15->msgID1_2 = ID2;
	vdlMsg15->msgID2_1 = ID3;
	vdlMsg15->rcvChannel = CHANNELA;
	vdlMsg15->rcvSlot = rcvSlot;
	vdlMsg15->slotOffset1_1 = O1;
	vdlMsg15->slotOffset1_2 = O2;
	vdlMsg15->slotOffset2_1 = O3;
	vdlMsg15->srcMMSI = 222222222;
}

////测试用各个需要的结构体的内容
//void initMsgContent(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,GPS_InfoStruct * gps_infoStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23)
//{
//	testGPS_InfoStruct(gps_infoStruct);
//	testAIS_BBMMsgStruct(ais_bbmMsgStruct);
//	testAIS_StaticStruct(ais_staticDataStruct);
//	//testVDL_Msg15(vdlMsg15);
//	initVDLMsg20Struct(vdlMsg20);
//	isVDLMsg15NewIn = SET;
//	ais_rcvMsgIDStruct->msgState = MSGNEW;
//	isVDLMsg20NewIn = SET;
//	//initVDLMsg22Struct(vdlMsg22);
//	//isVDLMsg22NewIn = SET;
//	initVDLMsg23Struct(vdlMsg23);
//	isVDLMsg23NewIn = SET;
//
//}

/************************************************************************
* Name      : testCollectNewMsg
* Brief     : 采集各个串口中断中接收到的新的消息ID号，并对相应的消息进行分类，打印出输入和输出的内容信息
*
* Author    : Digital Design Team
* Param     : ais_rcvMsgIDStruct- 输入，检测消息14
* Param     : isVDLMsg15NewIn- 输入，检测消息15的新消息标志
* Param     : vdlMsg15- 输入，vdl消息15的内容结构体，使用其中询问的具体信息
* Param     : isVDLMsg20NewIn- 输入，检测消息20的新消息标志
* Param     : isVDLMsg22NewIn-输入，检测消息22的新消息标志
* Param     : isVDLMsg23NewIn- 输入，检测消息23的新消息标志
* Param     : fsm_controlStruct-输出，状态机控制结构体，修改记录分类的新消息ID
* Return    : void
************************************************************************/
void testCollectNewMsg(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,FlagStatus isVDLMsg15NewIn,VDLMsg15 * vdlMsg15,FlagStatus isVDLMsg20NewIn,FlagStatus isVDLMsg22NewIn,FlagStatus isVDLMsg23NewIn,FSM_ControlStruct * fsm_controlStruct)
{
	u8 i = 0;
	//collectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	printf("\r\n");
	printf("/////////testCollectNewMsg function start/////////////\r\n");
	printf("input msgs are:   \n");
	if (ais_rcvMsgIDStruct->msgState == MSGNEW)
	{
		printf("Msg14\n");
	}
	if (isVDLMsg15NewIn == SET)
	{
		printf("Msg15\n");
		if (vdlMsg15->msgID1_1)
			printf("inquiry for:  %d\n",vdlMsg15->msgID1_1);
		if (vdlMsg15->msgID1_2)
			printf("inquiry for:  %d\n",vdlMsg15->msgID1_2);
		if (vdlMsg15->msgID2_1)
			printf("inquiry for:  %d\n",vdlMsg15->msgID2_1);		
	}
	if (isVDLMsg20NewIn == SET)
	{
		printf("Msg20\n");
	}
	if (isVDLMsg22NewIn == SET)
	{
		printf("Msg22\n");
	}
	if (isVDLMsg23NewIn == SET)
	{
		printf("Msg23\n");
	}
	printf("collect safe and  inquiry msgs are:\r\n");
	i = 0;
	while (i < fsm_controlStruct->msgTypeLen1)
	{
		printf("%d\n",fsm_controlStruct->controlMsgType[0][i]);
		i ++;
	}
	if (i == 0)
	{
		printf("NONE!\n");
	}
	i = 0;
	printf("collect assign msgs are:\r\n");
	while (i < fsm_controlStruct->msgTypeLen2)
	{
		printf("%d\n",fsm_controlStruct->controlMsgType[1][i]);
		i ++;
	}
	if (i == 0)
	{
		printf("NONE!\n");
	}
	printf("/////////testCollectNewMsg function end/////////////\r\n");
	printf("\r\n");
}

/************************************************************************
* Name      : testUpdataMsg20Struct
* Brief     : 测试更新消息20的内容
*
* Author    : Digital Design Team
* Param     : vdlMsg20- 输入，FPGA接收的消息20结构体
* Param     : fsm_msg20Struct- 输出，GPS接收的GPS信息，此处主要使用到GPS的UTC时间和经纬度
* Param     : fsm_dataStruct- 输出，FSM的数据结构体，将新的保留时隙更新至reservedSlot数组中
* Return    : void
************************************************************************/
void testUpdataMsg20Struct(VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct)
{
	u16 i = 0;
	u8 k = 0;
	printf("\r\n");
	printf("/////////testUpdataMsg20Struct function start/////////////\r\n");
	printf("Msg20's reserved slots are:\n");
	if (vdlMsg20->slotInfos[0].slotOffset && vdlMsg20->slotInfos[0].numSlot && vdlMsg20->slotInfos[0].overTime)
	{
		printf("baseStation's Longitude:%d\n",vdlMsg20->bLongitude);
		printf("baseStation's Latitude:%d\n",vdlMsg20->bLatitude);
		printf("rcvSlot:		%d\n",vdlMsg20->rcvSlot);
		printf("rcvTime(hhmmss):		%d    ",vdlMsg20->rcvUTCTime[0]);
		printf("%d    ",vdlMsg20->rcvUTCTime[1]);
		printf("%d    \n",vdlMsg20->rcvUTCTime[2]);
		printf("the 1's slots:\n");
		printf("slotOffset:     %d\n",vdlMsg20->slotInfos[0].slotOffset);
		printf("continueNum:    %d\n",vdlMsg20->slotInfos[0].numSlot);
		printf("overTime(min):  %d\n",vdlMsg20->slotInfos[0].overTime);
		printf("\n");
	}
	if (vdlMsg20->slotInfos[0].increment)
	{
		printf("the 2's group slots:\n",k+1);
		printf("slotOffset:     %d\n",vdlMsg20->slotInfos[1].slotOffset);
		printf("continueNum:    %d\n",vdlMsg20->slotInfos[1].numSlot);
		printf("overTime(min):  %d\n",vdlMsg20->slotInfos[1].overTime);
		printf("\n");
	}
	k = 1;
	while( k < MAXRESERVEDSLOTLEN)
	{
		if (vdlMsg20->slotInfos[k].increment)
		{
			printf("the %d's group slots:\n",k+2);
			printf("slotOffset:     %d\n",vdlMsg20->slotInfos[k+1].slotOffset);
			printf("continueNum:    %d\n",vdlMsg20->slotInfos[k+1].numSlot);
			printf("overTime(min):  %d\n",vdlMsg20->slotInfos[k+1].overTime);
			printf("\n");
		}
		k ++;
	}
	printf("write down content in fsm_msg20Struct are:\n");
	if (fsm_msg20Struct->rsvdSlotStruct[0].overTime)
	{
		printf("baseStation Longitude:	%d\n",fsm_msg20Struct->bLongitude);
		printf("baseStation Latitude:	%d\n",fsm_msg20Struct->blatitude);
	}
	while (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] && fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] != 24)
	{
		printf("the %d's group reserved slots\n",i+1);
		printf("true slotNum:			%d\n",fsm_msg20Struct->rsvdSlotStruct[i].slotNum);
		printf("continueSlotNum:		%d\n",fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum);
		printf("overTime(hhmmss):		%d    ",fsm_msg20Struct->rsvdSlotStruct[i].overTime[0]);
		printf("%d    ",fsm_msg20Struct->rsvdSlotStruct[i].overTime[1]);
		printf("%d    \n",fsm_msg20Struct->rsvdSlotStruct[i].overTime[2]);
		i ++;
	}
	printf("the all reserved slots are:\n");
	i = 0;
	k = 0;
	while (i < 282)
	{
		if (fsm_dataStruct->reservedSlotByte[i])
		{
			for (k = 8;k > 0;k --)
			{
				if ((fsm_dataStruct->reservedSlotByte[i]>>(k-1))&0x01)
				{
					printf("%d          ",(i*8)+8-k);
				}
			}
		}
		i ++;
	}
	/*while (i < 2250)
	{
		if (fsm_dataStruct->reservedSlot[i])
		{
			printf("%d          ",i);
		}
		i ++;
	}*/
	printf("\n");
	printf("/////////testUpdataMsg20Struct function end/////////////\r\n");
	printf("\r\n");
}

/************************************************************************
* Name      : testUpdataMsg22Struct
* Brief     : 测试更新消息22的内容
*
* Author    : Digital Design Team
* Param     : vdlMsg22- 输入，FPGA接收的消息22结构体
* Param     : fsm_msg22Struct- 输出，FSM的消息22的结构体，将消息22的内容进行保存，以供后续使用
* Return    : void
************************************************************************/
void testUpdataMsg22Struct(VDLMsg22 * vdlMsg22,FSM_Msg22Struct * fsm_msg22Struct)
{
	u32 tmp1 = 0;
	u32 tmp2 = 0;
	u8 i = 0;
	u8 j = 0;
	printf("\n");
	printf("/////////testUpdataMsg22Struct function start/////////////\r\n");
	printf("input vdlMsg22's information are:\n");
	printf("isBroadcast(0-broadcast,1-addressed):		%d\n",vdlMsg22->isBroadcast);
	printf("latitude1:			%d\n",vdlMsg22->latitude1);
	printf("logitude1:			%d\n",vdlMsg22->logitude1);
	printf("latitude2:			%d\n",vdlMsg22->latitude2);
	printf("logitude2:			%d\n",vdlMsg22->logitude2);

	printf("\n");
	printf("output fsm_msg22Struct's information are\n");
	if (vdlMsg22->isBroadcast)
	{
		printf("address info:\n");
		printf("own MMSI		%d\n",OWNMMSI);
		tmp1 = (vdlMsg22->logitude1&0x0003ffff)<<12 ;
		tmp1 += vdlMsg22->latitude1&0x00000fff;
		tmp2 = (vdlMsg22->logitude2&0x0003ffff)<<12 ;
		tmp2 += vdlMsg22->latitude2&0x00000fff;
		if (tmp1 == OWNMMSI || tmp2 == OWNMMSI)
		{
			printf("addressed MMSI is the same as own\n");
		}
		printf("handOverArea:	%d\n",fsm_msg22Struct->addrManageStruct.handOverArea);
		printf("transChannel:	%d\n",fsm_msg22Struct->addrManageStruct.transChannel);
		printf("overTime(mmddhhmm):		%d   ",fsm_msg22Struct->addrManageStruct.overTime[0]);
		printf("%d   ",fsm_msg22Struct->addrManageStruct.overTime[1]);
		printf("%d   ",fsm_msg22Struct->addrManageStruct.overTime[2]);
		printf("%d   ",fsm_msg22Struct->addrManageStruct.overTime[3]);
		printf("\n");
	}
	else
	{
		printf("broadcast info:\n");
		i = 0;
		while (i < 8)
		{
			if(fsm_msg22Struct->groupManageStruct[i].overTime[0] != 13)
			{
				printf("the %d's group info:\n",i+1);
				printf("northEastLatitude:		%f\n",fsm_msg22Struct->groupManageStruct[i].northEastLatitude);
				printf("northEastLogitude:		%f\n",fsm_msg22Struct->groupManageStruct[i].northEastLongitude);
				printf("southWestLatitude:		%f\n",fsm_msg22Struct->groupManageStruct[i].southWestLatitude);
				printf("southWestLongitude:		%f\n",fsm_msg22Struct->groupManageStruct[i].southWestLongitude);
				printf("\n");
				j ++;
			}
			i ++;
		}
		//printf("\n");
		printf("the total broad info numbers are:	%d\n",j);

		printf("\n");
	}
	//printf("\n");
	printf("/////////testUpdataMsg22Struct function end/////////////\r\n");
}

/************************************************************************
* Name      : testUpdataMsg23Struct
* Brief     : 测试更新消息23的内容
*
* Author    : Digital Design Team
* Param     : vdlMsg23- 输入，FPGA接收的消息23结构体
* Param     : gps_infoStruct- 输入 GPS接收的信息结构体
* Param     : fsm_msg23Struct- 输出，FSM的消息23的结构体，将消息23的内容进行保存，以供后续使用
* Return    : void
************************************************************************/
void testUpdataMsg23Struct(VDLMsg23 * vdlMsg23,GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct)
{
	double gps_logi = 0.0;
	double gps_lati = 0.0;
	double vdl_logi1 = 0.0;
	double vdl_lati1 = 0.0;
	double vdl_logi2 = 0.0;
	double vdl_lati2 = 0.0;
	printf("\n");
	printf("/////////testUpdataMsg23Struct function start/////////////\r\n");
	printf("input vdlMsg23's infomation are:\n");
	printf("baseMMSI:				%d\n",vdlMsg23->MMSI);
	printf("northEastLatitude:		%d\n",vdlMsg23->latitude1);
	printf("northEaseLogitude:		%d\n",vdlMsg23->logitude1);
	printf("southWestLatitude:		%d\n",vdlMsg23->latitude2);
	printf("southWestLogitude:		%d\n",vdlMsg23->logitude2);
	printf("shipType:				%d\n",vdlMsg23->shipType);
	printf("stationType:			%d\n",vdlMsg23->stationType);
	printf("rcvTime(hhmmss):		%d    ",vdlMsg23->rcvTime[0]);
	printf("%d    ",vdlMsg23->rcvTime[1]);
	printf("%d    \n",vdlMsg23->rcvTime[2]);
	printf("reportInterval(second):			%d\n",vdlMsg23->reportInterval);
	printf("Tx/RxMode:				%d\n",vdlMsg23->TxRxMode);
	printf("silentInterval:			%d\n",vdlMsg23->silentTime);

	changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&gps_lati,&gps_logi);
	changeMinuteToDegree(vdlMsg23->latitude1,vdlMsg23->logitude1,1,&vdl_lati1,&vdl_logi1);
	changeMinuteToDegree(vdlMsg23->latitude2,vdlMsg23->logitude2,1,&vdl_lati2,&vdl_logi2);
	
	if (gps_logi > vdl_logi1 || gps_logi < vdl_logi2 || gps_lati > vdl_lati1 || gps_lati < vdl_lati2)//如不在指配的区域内，则返回
	{
		printf("own ship isn't in the area assigned!!!\n");
	}
	else
	{
		printf("own ship is in the area assigned!!!\n");
		printf("output fsm_msg23Struct's infomation are:\n");
		printf("northEaseLatitude:		%f\n",fsm_msg23Struct->northEastLatitude);
		printf("northEaseLogitude:		%f\n",fsm_msg23Struct->northEastLongitude);
		printf("southWestLatitude:		%f\n",fsm_msg23Struct->southWestLatitude);
		printf("southWestLogitude:		%f\n",fsm_msg23Struct->southWestLongitude);
		printf("Tx/RxMode:				%d\n",fsm_msg23Struct->transChannel);
		printf("reportInterval:			%d\n",fsm_msg23Struct->reportTime);
		printf("silenceTime(hhmmss):	%d    ",fsm_msg23Struct->silenceTime[0]);
		printf("%d    ",fsm_msg23Struct->silenceTime[1]);
		printf("%d    \n",fsm_msg23Struct->silenceTime[2]);
		printf("overTime(hhmmss):		%d    ",fsm_msg23Struct->overTime[0]);
		printf("%d    ",fsm_msg23Struct->overTime[1]);
		printf("%d    \n",fsm_msg23Struct->overTime[2]);
	}
	printf("\n");
	printf("/////////testUpdataMsg23Struct function end/////////////\r\n");
}

/************************************************************************
* Name      : testCheckFSMMsg20Struct
* Brief     : 测试当前船站是否处于Msg23规定的指配模式的输入和输出
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口接收到到的实时的动态数据信息，使用其中的GPS获取的经纬度和接收的UTC时间
* Param     : fsm_msg20Struct- 输入，存储的之前接收的VDL msg23的相关内容
* Param     : fsm_dataStruct- 输出，数据结构体，查看当前总的保留时隙号
* Return    : void
************************************************************************/
void testCheckFSMMsg20Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct)
{
	u16 i = 0;
	u8 j = 0;
	u16 k = 0;
	u16 lenBias = 0;
	printf("\n");
	printf("/////////testCheckFSMMsg20Struct function start/////////////\r\n");
	printf("GPS_InfoStruct's utctime(hhmmss):		%d    ",gps_infoStruct->utctime[2]);	//时
	printf("%d    ",gps_infoStruct->utctime[3]);
	printf("%d    \n",gps_infoStruct->utctime[4]);
	i = 0;
	k = 0;
	while(i < RESERVEDSLOTLENGTH)
	{
		if (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] != 24 && fsm_msg20Struct->rsvdSlotStruct[i].overTime[0])//时 不为24则认为有效
		{
			 k ++;
			printf("the %d's group reserved slots are:\n",k);
			j = 0;
			lenBias = fsm_msg20Struct->rsvdSlotStruct[i].slotNum;
			while (j < fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum)
			{
				printf("%d     ",lenBias+j);
				j ++;
			}
			printf("\n");
			printf("overTime(hhmmss):		%d    ",fsm_msg20Struct->rsvdSlotStruct[i].overTime[0]);
			printf("%d	  ",fsm_msg20Struct->rsvdSlotStruct[i].overTime[1]);
			printf("%d	\n",fsm_msg20Struct->rsvdSlotStruct[i].overTime[2]);
		}
		if (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] == 24 && fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum != 0)//实际存在保留时隙，且时 部分内容 ,等于24时，认为超时了
		{
			printf("those slots are not reserved:\n");
			j = 0;
			lenBias = fsm_msg20Struct->rsvdSlotStruct[i].slotNum;
			while (j < fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum)
			{
				printf("%d     ",lenBias+j);
				j ++;
			}
			printf("\n");
		}
		i ++;
	}
	printf("fsm_dataStruct's reserved slots now are:\n");
	i = 0;
	k = 0;
	while (i < 282)
	{
		if (fsm_dataStruct->reservedSlotByte[i])
		{
			for (k = 8;k > 0;k --)
			{
				if ((fsm_dataStruct->reservedSlotByte[i]>>(k-1))&0x01)
				{
					printf("%d          ",(i*8)+8-k);
				}
			}
		}
		i ++;
	}
	/*while(i < 2250)
	{
		if (fsm_dataStruct->reservedSlot[i])
		{
			k ++;
			printf("%d     ",i);
		}
		i ++;
	}*/
	if (k == 0)
	{
		printf("NONE!");
	}
	printf("\n");

	printf("\n");
	printf("/////////testCheckFSMMsg20Struct function end/////////////\r\n");

}

/************************************************************************
* Name      : testCheckFSMMsg22Struct
* Brief     : 测试当前船站是否处于Msg22规定的指配模式的输入输出
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口接收到到的实时的动态数据信息，使用其中的GPS获取的经纬度和接收的UTC时间
* Param     : fsm_controlStruct-输入，存储的之前接收的VDL msg22的相关内容
* Return    : void
************************************************************************/
void testCheckFSMMsg22Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg22Struct * fsm_msg22Strcut)
{
	u8 i = 0;
	double logi = 0.0;
	double lati = 0.0;
	printf("\n");
	printf("/////////testCheckFSMMsg22Struct function start/////////////\r\n");
	printf("gps_infoStruct's information are:\n");
	printf("Latitude:		%d\n",gps_infoStruct->latitude);
	printf("Longitude:		%d\n",gps_infoStruct->longitude);
	printf("utcTime(mmddhhmmss):		%d	  ",gps_infoStruct->utctime[0]);
	printf("%d	  ",gps_infoStruct->utctime[1]);
	printf("%d	  ",gps_infoStruct->utctime[2]);
	printf("%d	  ",gps_infoStruct->utctime[3]);
	printf("%d	  \n",gps_infoStruct->utctime[4]);

	printf("fsm_msg22Struct's information are:\n");
	printf("the addressed assign area's overtime(mmddhhmm):		%d   ",fsm_msg22Strcut->addrManageStruct.overTime[0]);
	printf("%d    ",fsm_msg22Strcut->addrManageStruct.overTime[1]);
	printf("%d	  ",fsm_msg22Strcut->addrManageStruct.overTime[2]);
	printf("%d	  \n",fsm_msg22Strcut->addrManageStruct.overTime[3]);
	printf("the broadcast assign area's valid overtime(mmddhhmm):\n");	
	i = 0;
	while(i < 8)
	{
		if (fsm_msg22Strcut->groupManageStruct[i].overTime[0] != 13)
		{
			printf("the %d's group area:\n",i + 1);
			printf("overtime(mmddhhmm):		%d    ",fsm_msg22Strcut->groupManageStruct[i].overTime[0]);
			printf("%d	   ",fsm_msg22Strcut->groupManageStruct[i].overTime[1]);
			printf("%d	   ",fsm_msg22Strcut->groupManageStruct[i].overTime[2]);
			printf("%d	   \n",fsm_msg22Strcut->groupManageStruct[i].overTime[3]);
		}
		i ++;
	}

	if (gps_infoStruct->utctime[0] == fsm_msg22Strcut->addrManageStruct.overTime[0] && 
		gps_infoStruct->utctime[1] == fsm_msg22Strcut->addrManageStruct.overTime[1] && 
		gps_infoStruct->utctime[2] == fsm_msg22Strcut->addrManageStruct.overTime[2] && 
		gps_infoStruct->utctime[3] == fsm_msg22Strcut->addrManageStruct.overTime[3]) //分别对应 月天时分
	{
		printf("the addressed area is overtimed!!!\n");
	}
	if (gps_infoStruct->utctime[2] == fsm_msg22Strcut->addrManageStruct.assignOverTime[0] &&
		gps_infoStruct->utctime[3] == fsm_msg22Strcut->addrManageStruct.assignOverTime[1] &&
		gps_infoStruct->utctime[4] == fsm_msg22Strcut->addrManageStruct.assignOverTime[2])//当前指配命令超时 分别对应 时分秒)
	{
		printf("the addressed assign msg is overtimed!!!\n");
	}
	i = 0;
	while(i < 8)//检测8个区域的内容是否超时
	{
		if (gps_infoStruct->utctime[0] == fsm_msg22Strcut->groupManageStruct[i].overTime[0] &&
			gps_infoStruct->utctime[1] == fsm_msg22Strcut->groupManageStruct[i].overTime[1] &&
			gps_infoStruct->utctime[2] == fsm_msg22Strcut->groupManageStruct[i].overTime[2] &&
			gps_infoStruct->utctime[3] == fsm_msg22Strcut->groupManageStruct[i].overTime[3]) //分别对应 月天时分
		{
			printf("the %d 's broadcast area is overtimed!!!\n",i+1);
		}
		if (gps_infoStruct->utctime[2] == fsm_msg22Strcut->groupManageStruct[i].assignOverTime[0] &&
			gps_infoStruct->utctime[3] == fsm_msg22Strcut->groupManageStruct[i].assignOverTime[1] &&
			gps_infoStruct->utctime[4] == fsm_msg22Strcut->groupManageStruct[i].assignOverTime[2])//当前指配命令超时 分别对应 时分秒)
		{
			printf("the %d's assign msg is overtimed!!!\n",i+1);
		}
		i ++;
	}
	if (fsm_msg22Strcut->addrManageStruct.assignOverTime[0] != 24)
	{
		printf("own ship is in the addressed area!!!\n");
	}
	else
	{
		changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&lati,&logi);
		i = 0;
		while (i < 8)
		{
			if (fsm_msg22Strcut->groupManageStruct[i].assignOverTime[0] != 24 &&
				lati >= fsm_msg22Strcut->groupManageStruct[i].southWestLatitude && lati <= fsm_msg22Strcut->groupManageStruct[i].northEastLatitude &&
				logi >= fsm_msg22Strcut->groupManageStruct[i].southWestLongitude && logi <= fsm_msg22Strcut->groupManageStruct[i].northEastLongitude) //在指配区
			{
				//需加入切换区的判断 to be added
				printf("the last broadcast area own ship inside is: %d\n ",fsm_msg22Strcut->lastArea);
				printf("now,own ship is in the %d's broadcast area!!!\n",i+1);
				printf("the broadcast assign area's position is:\n");
				printf("northEastLatitude:		%f\n",fsm_msg22Strcut->groupManageStruct[i].northEastLatitude);
				printf("northEastLongitude:		%f\n",fsm_msg22Strcut->groupManageStruct[i].northEastLongitude);
				printf("southWestLatitude:		%f\n",fsm_msg22Strcut->groupManageStruct[i].southWestLatitude);
				printf("southWestLongitude:		%f\n",fsm_msg22Strcut->groupManageStruct[i].southWestLongitude);
				break;
			}
			i ++;
		}
		if (i == 8)
		{
			printf("own ship isn't in the addressed assign area!!!\n");
			printf("own ship isn't in any broadcast area!!!\n");
		}
	}

	printf("\n");
	printf("/////////testCheckFSMMsg22Struct function end/////////////\r\n");
}

/************************************************************************
* Name      : testCheckFSMMsg23Struct
* Brief     : 测试当前船站是否处于Msg23规定的指配模式的输入输出
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口接收到到的实时的动态数据信息，使用其中的GPS获取的经纬度和接收的UTC时间
* Param     : fsm_msg23Struct-输入，存储的之前接收的VDL msg23的相关内容
* Return    : void
************************************************************************/
void testCheckFSMMsg23Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct)
{
	double lati = 0.0;
	double logi = 0.0;
	printf("\n");
	printf("/////////testCheckFSMMsg23Struct function start/////////////\r\n");
	printf("gps_infoStruct's information are:\n");
	printf("Latitude:		%d\n",gps_infoStruct->latitude);
	printf("Longitude:		%d\n",gps_infoStruct->longitude);
	printf("utcTime(mmddhhmmss):		%d	  ",gps_infoStruct->utctime[0]);
	printf("%d	  ",gps_infoStruct->utctime[1]);
	printf("%d	  ",gps_infoStruct->utctime[2]);
	printf("%d	  ",gps_infoStruct->utctime[3]);
	printf("%d	  \n",gps_infoStruct->utctime[4]);

	if (fsm_msg23Struct->overTime[0] == 24)
	{
		printf("the assign msg is overtimed!!!\n");
	}
	//若未超时，则检查是否在区域内，若在区域内，则还要检查寂静期是否结束
	else
	{
		printf("the assign msg is valid!!!\n");
		printf("fsm_msg23Struct's information are:\n");
		printf("the assigned area is:\n");
		printf("northEastLatitude:		%f\n",fsm_msg23Struct->northEastLatitude);
		printf("northEastLongitude:		%f\n",fsm_msg23Struct->northEastLongitude);
		printf("southWestLatitude:		%f\n",fsm_msg23Struct->southWestLatitude);
		printf("southWestLongitude:		%f\n",fsm_msg23Struct->southWestLongitude);
		printf("overTime(hhmmss):		%d	 ",fsm_msg23Struct->overTime[0]);
		printf("%d	 ",fsm_msg23Struct->overTime[1]);
		printf("%d	 \n",fsm_msg23Struct->overTime[2]);
		printf("slienceTime(hhmmss):	%d	 ",fsm_msg23Struct->silenceTime[0]);
		printf("%d	 ",fsm_msg23Struct->silenceTime[1]);
		printf("%d	 \n",fsm_msg23Struct->silenceTime[2]);
		changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&lati,&logi);
		if (lati < fsm_msg23Struct->southWestLatitude || lati > fsm_msg23Struct->northEastLatitude ||
			logi < fsm_msg23Struct->southWestLatitude || logi > fsm_msg23Struct->northEastLongitude) //若不在区域内
		{
			printf("own ship isn't in the assigned area!!!\n");
			printf("\n");
			printf("/////////testCheckFSMMsg23Struct function end/////////////\r\n");
			return;
		}
		//在区域内，则判断寂静期
		printf("own ship is in the assigned area!!!\n");
		if (fsm_msg23Struct->silenceTime[0] != 24)//寂静期时间内
		{
			printf("the silence time is valid!!!\n");
		}
		else
		{
			printf("the silence time is overtime!!!\n");
		}
	}
	printf("\n");
	printf("/////////testCheckFSMMsg23Struct function end/////////////\r\n");
}

/************************************************************************
* Name      : testGenerateCycleContent
* Brief     : 测试指配模式和自主模式下，产生周期性的报告消息，包括Msg18、Msg24
*			（在实际情况中未使用）
* Author    : Digital Design Team
* Param     : workType- 输入，用于指示当前所处的工作模式，取值为AUTO、MSG20、MSG22、MSG20HANDOVER、MSG23、MSG23SILENCE					    
* Param     : gps_infoStruct- 输入，GPS信息结构体，封装了GPS中的航线、航速、经纬度、UTC时间等
* Param     : ais_staticDataStruct- 输入，封装了上位机中给出的静态信息
* Param     : rcvVDLMsg15- 输入，vdl消息15的内容
* Param     : fsm_controlStruct- 输入，状态机的控制结构体
* Param     : fsm_dataStruct- 输出，状态机的数据结构体
* Return    : void
************************************************************************/
void testGenerateCycleContent(u8 workType,GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * rcvVDLMsg15,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
//	u8 i = 0;
//	u8 j = 0;
//	u8 k = 0;
	printf("\n");
	printf("/////////testGenerateCycleContent function start/////////////\r\n");
	printf("the current work type is\n");
	switch(workType)
	{
		case AUTO: printf("automation!!!\n");break;
		case MSG20: printf("msg20 is valid!!!\n");break;
		case MSG22: printf("msg22 is valid,but not in the handover area!!!\n");break;
		case MSG22HANDOVER: printf("msg22 is in the handover area!!!\n");break;
		case MSG23: printf("msg23 is valid,but not in the silence period!!!\n");break;
		case MSG23SILENCE:printf("msg23 is in the silence period!!!\n");break;
		default:break;
	}
	if (fsm_controlStruct->msg18Count.countFlag == COUNTED)
	{
		printf("the generating time for cycle msg18 is now completed!!!\n");
	}
	if (fsm_controlStruct->msg24Count.countFlag == COUNTED)
	{
		printf("the generating time for cycle msg24 is now completed!!!\n");
	}
	if (fsm_controlStruct->msg18AddCount.countFlag == COUNTED)
	{
		printf("the generating time for handover area cycle msg18 is now completed!!!\n");
	}
	printf("\n");
	printf("/////////testGenerateCycleContent function end/////////////\r\n");
}

/************************************************************************
* Name      : testGenerateMsg14Content
* Brief     : 测试产生消息14的相应内容，测试其输入和输出内容
*
* Author    : Digital Design Team
* Param     : ais_bbmMsgStruct- 输入，VDL消息14的相关内容
* Param     : fsm_dataStruct- 输出，输出消息结构体的相应内容
* Param     : frameBias- 输入，当前存放了消息14内容的fsm_dataStruct->fsm_frameStruct[frameBias]数组的偏置数
* Return    : void
************************************************************************/
void testGenerateMsg14Content(AIS_BBMMsgStruct * ais_bbmMsgStruct,FSM_DataStruct * fsm_dataStruct,u8 frameBias)
{
	u8 k = 0;
	u8 i = frameBias;
	printf("\n");
	printf("/////////testGenerateMsg14Content function start/////////////\r\n");

	printf("\n");
	printf("the input information for generating Msg14 is:\n");
	printf("\n");
	printf("msgType:					SAFE\n");
	printf("MMSI:						%d\n",OWNMMSI);
	printf("rcvChannel(0-A 1-B 2-alter):	        %d\n",ais_bbmMsgStruct->broadcastChannel);
	printf("rcvSlot:					%d\n",ais_bbmMsgStruct->rcvSlot);
	printf("safeInfoLen:				%d\n",ais_bbmMsgStruct->encapsulatedDataLength);
	printf("safeInfo:					\n");
	for(k = 0; k < ais_bbmMsgStruct->encapsulatedDataLength; k ++)
	{
		printf("%d",ais_bbmMsgStruct->encapsulatedData[k]);
		if (k%49 == 0 && k)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("\n");
	printf("the output information of Msg14 to be framed is:\n");
	printf("\n");

	printf("encapDataLen:		%d\n",fsm_dataStruct->fsm_frameStruct[i].encapDataLen);
	printf("encapData:\n");
	for (k = 0; k < (fsm_dataStruct->fsm_frameStruct[i].encapDataLen / 8); k ++)
	{
		//printf("%d",fsm_dataStruct->fsm_frameStruct[i].encapData[k]);
		prinfBinary(fsm_dataStruct->fsm_frameStruct[i].encapDataByte[k]);
		printf("\n");
		//switch(k)//测试消息19的
		//{
		//	case 5:case 7:case 37:case 39:
		//	printf("\n");
		//	break;
		//}
	}
	printf("\n");
	printf("rcvSlot:			%d\n",fsm_dataStruct->fsm_frameStruct[i].rcvSlot);
	printf("candidateSlotPeriod(second):	%d\n",fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod);
	printf("broadChannel(0-A,1-B):			%d\n",fsm_dataStruct->fsm_frameStruct[i].broadChannel);

	printf("\n");
	printf("/////////testGenerateMsg14Content function end/////////////\r\n");
}

/************************************************************************
* Name      : testGenerateMsg18Content
* Brief     : 测试产生消息18的内容的内容，测试输入和输出
*
* Author    : Digital Design Team
* Param     : msgType- 输入，指示当前生成Msg18的模式，ASSIGNMSG-指配,AUTONOMSG-自主,INTERROMSG-询问,ASSIGNADDMSG18-切换区，A22MSG18-Msg22指配命令下产生Msg18
* Param     : gps_infoStruct- 输入，GPS动态消息
* Param     : fsm_dataStruct- 输出，输出结构体的相应内容
* Param     : rcvVDLMsg15- 输入，vdl 消息15的内容，当时询问模式下产生的消息18，则需要使用相关的信道和响应时隙信息
* Param     : alterChannel- 输入，用于指示在没有指定信道时，使用交替的信道作为发射信道
* Param     : frameBias- 输入，当前存放了消息18内容的fsm_dataStruct->fsm_frameStruct[frameBias]数组的偏置数
* Return    : void
************************************************************************/
void testGenerateMsg18Content(MsgTypeIndicator msgType,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel,u8 frameBias)
{
	u8 i = 0;
	u8 k = 0;
	printf("\n");
	printf("/////////testGenerateMsg18Content function start/////////////\r\n");
	printf("\n");
	printf("the input information for generating Msg18 is:\n");
	printf("\n");
	switch (msgType)
	{
		case AUTONOMSG: printf("msgType:			AUTO\n");break;
		case INTERROMSG: printf("msgType:			INTERROGATION\n");break;
		case A22MSG18: printf("msgType:			ASSIGN(Msg22 valid)\n");break;
		case ASSIGNMSG: printf("msgType:			ASSIGN(not msg22)\n");break;
		//case ASSIGNADDMSG18: printf("msgType:			Msg22 valid,and in the handover area\n");break;
		default:printf("msgType:			ASSIGN(not msg22)\n");break;
	}
	printf("SOG(0~1022):				%d\n",gps_infoStruct->SOG);
	printf("posAccurateFlag(1-H,0-L):	%d\n",gps_infoStruct->posAccurateFlag);
	printf("longitude:					%x\n",gps_infoStruct->longitude);
	printf("latitude:					%x\n",gps_infoStruct->latitude);
	printf("COG(0~3599):				%d\n",gps_infoStruct->COG);
	printf("HOG(0~359):					%d\n",gps_infoStruct->HOG);
	printf("utcTime(ss):				%d\n",gps_infoStruct->utctime[3]);
	printf("deviceFlag(1.B-CS):			%d\n",gps_infoStruct->deviceFlag);
	printf("monitorFlag(1-valid):		%d\n",gps_infoStruct->monitorFlag);
	printf("DSCFlag(1-valid):			%d\n",gps_infoStruct->DSCFlag);
	printf("bBandFlag(Msg22 relevant):	%d\n",gps_infoStruct->broadBandFlag);
	if ((msgType & 0xF0) == 0x20)//指配模式
	{
		if (msgType == A22MSG18)
		{
			printf("msg22Flag(1-valid):			1\n");
		}
		else
		{
			printf("msg22Flag(1-valid):			0\n");
		}
		printf("modeFlag(1-assign,0-auto):	1\n");
	}
	else//询问或者自主模式
	{
		printf("msg22Flag(1-valid):			0\n");
		printf("modeFlag(1-assign,0-auto):	0\n");
	}
	printf("raimFlag(1-valid):			%d\n",gps_infoStruct->raimFlag);
	printf("commuFlag(1-CS):			%d\n",gps_infoStruct->commuFlag);
	if ((msgType&0xF0) == INTERROMSG)	//询问消息
	{
		printf("\n");
		printf("interrogation information is:\n");
		printf("\n");
		printf("rcvChannel(0-channelA 1-channelB):		%d\n",rcvVDLMsg15->rcvChannel);
		printf("rcvSlot:				%d\n",rcvVDLMsg15->rcvSlot);
		if (rcvVDLMsg15->msgID1_1 == 18 && rcvVDLMsg15->slotOffset1_1 && OWNMMSI == rcvVDLMsg15->dstMMSI1)
		{
			printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset1_1);
		}
		else if (rcvVDLMsg15->msgID1_2 == 18 && rcvVDLMsg15->slotOffset1_2 && OWNMMSI == rcvVDLMsg15->dstMMSI1)
		{
			printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset1_2);
		}
		else if (rcvVDLMsg15->msgID2_1 == 18 && rcvVDLMsg15->slotOffset2_1 && OWNMMSI == rcvVDLMsg15->dstMMSI2)
		{
			printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset2_1);
		}
		else
		{
			printf("interrogation isn't offer the offset slot!!!\n");
		}
	}
	else
	{
		printf("the current slot is:		%d\n",fsm_dataStruct->realSlot);
	}
	printf("\n");
	printf("the output information of Msg18 to be framed is:\n");
	printf("\n");
	i = frameBias;
	printf("encapDataLen:		%d\n",fsm_dataStruct->fsm_frameStruct[i].encapDataLen);
	printf("encapData:\n");
	for (k = 0; k < (fsm_dataStruct->fsm_frameStruct[i].encapDataLen/8); k ++)
	{
		prinfBinary(fsm_dataStruct->fsm_frameStruct[i].encapDataByte[k]);
		printf("\n");
	}
	printf("\n");
	printf("rcvSlot:			%d\n",fsm_dataStruct->fsm_frameStruct[i].rcvSlot);
	printf("candidateSlotPeriod(second):	%d\n",fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod);
	printf("broadChannel(0-A,1-B):			%d\n",fsm_dataStruct->fsm_frameStruct[i].broadChannel);
	switch (fsm_dataStruct->fsm_frameStruct[i].msgType)
	{
		case AUTONOMSG: printf("msgType:			AUTO\n");break;
		case INTERROMSG: printf("msgType:			INTERROGATION\n");break;
		case ASSIGNMSG: printf("msgType:			ASSIGNMSG\n");break;
		case A22MSG18: printf("msgType:			Msg22 valid,and not in the handover area\n");break;
		case AAMSG18: printf("msgType:			ASSIGN(not msg22)\n");break;
		case ASSIGNADDMSG18: printf("msgType:			Msg22 valid,and in the handover area\n");break;
		default:printf("msgType:			%d\n",fsm_dataStruct->fsm_frameStruct[i].msgType);break;
	}
	printf("\n");
	printf("/////////testGenerateMsg18Content function end/////////////\r\n");
}

/************************************************************************
* Name      : testGenerateMsg19Content
* Brief     : 测试生成消息19的数据内容 ，测试输入和输出
*
* Author    : Digital Design Team
* Param     : msgType-输入，消息类型
* Param     : ais_staticDataStruct-输入，上位机端的静态数据内容结构体
* Param     : gps_infoStruct-输入，gps端的动态数据内容结构体
* Param     : fsm_dataStruct-输出，输出的待组帧的结构体
* Param     : vdlMsg15-输入，GPGA处接收到的询问消息结构体
* Param     : alterChannel-输入，交替发送的信道指示
* Return    : void
************************************************************************/
void testGenerateMsg19Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * vdlMsg15,_Bool alterChannel)
{
	u16 k = 0;

	printf("\n");
	printf("/////////testGenerateMsg19Content function start/////////////\r\n");
	printf("\n");
	printf("the input information for generating Msg19 is:\n");
	printf("\n");
	
	printf("SOG(0~1022):				%d\n",gps_infoStruct->SOG);
	printf("posAccurateFlag(1-H,0-L):	%d\n",gps_infoStruct->posAccurateFlag);
	printf("longitude:					%x\n",gps_infoStruct->longitude);
	printf("latitude:					%x\n",gps_infoStruct->latitude);
	printf("COG(0~3599):				%d\n",gps_infoStruct->COG);
	printf("HOG(0~359):					%d\n",gps_infoStruct->HOG);
	printf("utcTime(ss):				%d\n",gps_infoStruct->utctime[4]);
	printf("shipName:					\n");
	for(k = 0; k < 120; k ++)
	{
		printf("%d",ais_staticDataStruct->shipName[k]);
		if (k == 59)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("shipSize:					%d\n",ais_staticDataStruct->shipSize);
	printf("postionType(1-GPS):			1\n");	//测试设定定位装置类型为GPS
	printf("raimFlag(1-valid):			%d\n",gps_infoStruct->raimFlag);
	printf("dteFlag(0-valid):			1\n");
	printf("modeFlag(0-auto 1-assign):	%d\n",gps_infoStruct->modeFlag);

	printf("\n");
	printf("interrogation information is:\n");
	printf("\n");
	printf("rcvChannel(0-channelA 1-channelB):		%d\n",vdlMsg15->rcvChannel);
	printf("rcvSlot:				%d\n",vdlMsg15->rcvSlot);
	if (vdlMsg15->msgID1_1 == 19 && vdlMsg15->slotOffset1_1 >= 10 && OWNMMSI == vdlMsg15->dstMMSI1)
	{
		printf("slotOffset:				%d\n",vdlMsg15->slotOffset1_1);
	}
	else if (vdlMsg15->msgID1_2 == 19 && vdlMsg15->slotOffset1_2 && OWNMMSI == vdlMsg15->dstMMSI1)
	{
		printf("slotOffset:				%d\n",vdlMsg15->slotOffset1_2);
	}
	else if (vdlMsg15->msgID2_1 == 19 && vdlMsg15->slotOffset2_1 && OWNMMSI == vdlMsg15->dstMMSI2)
	{
		printf("slotOffset:				%d\n",vdlMsg15->slotOffset2_1);
	}
	else
	{
		printf("interrogation isn't offer the offset slot!!!\n");
	}

	printf("\n");
	printf("the output information of Msg19 to be framed is:\n");
	printf("\n");

	printf("encapDataLen:		%d\n",fsm_dataStruct->fsm_frame19Struct.encapDataLen);
	printf("encapData:\n");
	for (k = 0; k < (fsm_dataStruct->fsm_frame19Struct.encapDataLen/8); k ++)
	{
		prinfBinary(fsm_dataStruct->fsm_frame19Struct.encapDataByte[k]);
		printf("\n");
		//printf("%d",fsm_dataStruct->fsm_frame19Struct.encapData[k]);
		//switch(k)//测试消息19的
		//{
		//case 5:case 7:case 37:case 45:case 55:case 56: case 84: case 111: case 123: case 132: case 138:
		//case 142: case 262: case 270: case 300: case 304: case 305: case 306: case 307: case 311:
		//	printf("\n");
		//	break;
		//}
	}
	printf("\n");
	printf("rcvSlot:			%d\n",fsm_dataStruct->fsm_frame19Struct.rcvSlot);
	printf("candidateSlotPeriod(second):	%d\n",fsm_dataStruct->fsm_frame19Struct.generateSlotPeriod);
	printf("broadChannel(0-A,1-B):			%d\n",fsm_dataStruct->fsm_frame19Struct.broadChannel);

	printf("\n");
	printf("/////////testGenerateMsg19Content function end/////////////\r\n");
}
//part == 1 A 部分  part == 0 B 部分
/************************************************************************
* Name      : testgenerateMsg24Content
* Brief     : 测试生成消息24的待组帧内容，包括24A和24B的信息
*
* Author    : Digital Design Team
* Param     : msgType- 输入 消息类型 INTERROMSG  AUTONOMSG ASSIGNMSG
* Param     : ais_staticDataStruct- 输入 上位机静态信息结构
* Param     : fsm_dataStruct- 输出 待组帧的信息结构
* Param     : rcvVDLMsg15- 输入 FPGA接收的FPGA消息结构，在询问模式使用
* Param     : alterChannel- 输入 自主模式下交替信道指示 0-channelA 1-channelB
* Param     : frameBias- 输入，当前存放了消息24A或24B内容的fsm_dataStruct->fsm_frameStruct[frameBias]数组的偏置数
* Param     : part-  1 指代A部分， 0 指代B部分
* Return    : void
************************************************************************/
void testgenerateMsg24Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel,u8 frameBias,u8 part)
{
	u8 i = 0;
	u8 k = 0;
	i = frameBias;
	if (1 == part)//测试A部分内容
	{
		printf("\n");
		printf("/////////testGenerateMsg24AContent function start/////////////\r\n");
		printf("\n");
		printf("the input information for generating Msg24A is:\n");
		printf("\n");
		switch (msgType)
		{
		case AUTONOMSG: printf("msgType:			AUTO\n");break;
		case INTERROMSG: printf("msgType:			INTERROGATION\n");break;
		case ASSIGNMSG: printf("msgType:			ASSIGN\n");break;
		default:printf("msgType:			%d\n",msgType);break;
		}
		printf("MMSI:						%d\n",OWNMMSI);
		printf("shipName:					\n");
		for(k = 0; k < 120; k ++)
		{
			printf("%d",ais_staticDataStruct->shipName[k]);
			if (k == 59)
			{
				printf("\n");
			}
		}
		printf("\n");
		if ((msgType&0xF0) == INTERROMSG)	//询问消息
		{
			printf("\n");
			printf("interrogation information is:\n");
			printf("\n");
			printf("rcvChannel(0-channelA 1-channelB):		%d\n",rcvVDLMsg15->rcvChannel);
			printf("rcvSlot:				%d\n",rcvVDLMsg15->rcvSlot);
			if (rcvVDLMsg15->msgID1_1 == 24 && rcvVDLMsg15->slotOffset1_1 >= 10 && OWNMMSI == rcvVDLMsg15->dstMMSI1)
			{
				printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset1_1);
			}
			else if (rcvVDLMsg15->msgID1_2 == 24 && rcvVDLMsg15->slotOffset1_2 >= 10 && OWNMMSI == rcvVDLMsg15->dstMMSI1)
			{
				printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset1_2);
			}
			else if (rcvVDLMsg15->msgID2_1 == 24 && rcvVDLMsg15->slotOffset2_1 >= 10 && OWNMMSI == rcvVDLMsg15->dstMMSI2)
			{
				printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset2_1);
			}
			else
			{
				printf("interrogation isn't offer the offset slot!!!\n");
			}
		}
		else
		{
			printf("the current slot is:		%d\n",fsm_dataStruct->realSlot);
		}
		printf("\n");
		printf("the output information of Msg24A to be framed is:\n");
		printf("\n");

		printf("encapDataLen:		%d\n",fsm_dataStruct->fsm_frameStruct[i].encapDataLen);
		printf("encapData:\n");
		for (k = 0; k < (fsm_dataStruct->fsm_frameStruct[i].encapDataLen/8); k ++)
		{
			prinfBinary(fsm_dataStruct->fsm_frameStruct[i].encapDataByte[k]);
			printf("\n");
			//printf("%d",fsm_dataStruct->fsm_frameStruct[i].encapData[k]);
			//switch(k)//测试消息24A的
			//{
			//case 5:case 7:case 37:case 39:case 159:
			//	printf("\n");
			//	break;
			//}
		}
		printf("\n");
		printf("rcvSlot:			%d\n",fsm_dataStruct->fsm_frameStruct[i].rcvSlot);
		printf("candidateSlotPeriod(second):	%d\n",fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod);
		printf("broadChannel(0-A,1-B):			%d\n",fsm_dataStruct->fsm_frameStruct[i].broadChannel);
		switch (fsm_dataStruct->fsm_frameStruct[i].msgType)
		{
		case AUTONOMSG: printf("msgType:			AUTO\n");break;
		case INTERROMSG: printf("msgType:			INTERROGATION\n");break;
		case ASSIGNMSG: printf("msgType:			ASSIGNMSG\n");break;
		default:printf("msgType:			%d\n",fsm_dataStruct->fsm_frameStruct[i].msgType);break;
		}
		printf("\n");
		printf("/////////testGenerateMsg24AContent function end/////////////\r\n");
	}
	else//测试B部分内容
	{
		printf("\n");
		printf("/////////testGenerateMsg24BContent function start/////////////\r\n");

		printf("the input information for generating Msg24B is:\n");
		printf("\n");
		switch (msgType)
		{
		case AUTONOMSG: printf("msgType:			AUTO\n");break;
		case INTERROMSG: printf("msgType:			INTERROGATION\n");break;
		case ASSIGNMSG: printf("msgType:			ASSIGN\n");break;
		default:printf("msgType:			%d\n",msgType);break;
		}
		printf("MMSI:						%d\n",OWNMMSI);
		printf("shipType:					%d\n",ais_staticDataStruct->shipType);
		printf("supplier:					\n");
		for (k = 42; k > 0; k --)
		{
			printf("%d",ais_staticDataStruct->supplier[k-1]);
		}
		printf("\n");
		printf("shipCall:					\n");
		for(k = 42; k > 0; k --)
		{
			printf("%d",ais_staticDataStruct->shipCall[k-1]);
		}
		printf("\n");
		printf("shipSize:					%d\n",ais_staticDataStruct->shipSize);
		if ((msgType&0xF0) == INTERROMSG)	//询问消息
		{
			printf("\n");
			printf("interrogation information is:\n");
			printf("\n");
			printf("rcvChannel(0-channelA 1-channelB):		%d\n",rcvVDLMsg15->rcvChannel);
			printf("rcvSlot:				%d\n",rcvVDLMsg15->rcvSlot);
			if (rcvVDLMsg15->msgID1_1 == 24 && rcvVDLMsg15->slotOffset1_1 >= 10 && OWNMMSI == rcvVDLMsg15->dstMMSI1)
			{
				printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset1_1);
			}
			else if (rcvVDLMsg15->msgID1_2 == 24 && rcvVDLMsg15->slotOffset1_2 >= 10 && OWNMMSI == rcvVDLMsg15->dstMMSI1)
			{
				printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset1_2);
			}
			else if (rcvVDLMsg15->msgID2_1 == 24 && rcvVDLMsg15->slotOffset2_1 >= 10 && OWNMMSI == rcvVDLMsg15->dstMMSI2)
			{
				printf("slotOffset:				%d\n",rcvVDLMsg15->slotOffset2_1);
			}
			else
			{
				printf("interrogation isn't offer the offset slot!!!\n");
			}
		}
		else
		{
			printf("the current slot is:		%d\n",fsm_dataStruct->realSlot);
		}
		printf("\n");
		printf("the output information of Msg24B to be framed is:\n");
		printf("\n");

		printf("encapDataLen:		%d\n",fsm_dataStruct->fsm_frameStruct[i].encapDataLen);
		printf("encapData:\n");
		for (k = 0; k < (fsm_dataStruct->fsm_frameStruct[i].encapDataLen/8); k ++)
		{
			prinfBinary(fsm_dataStruct->fsm_frameStruct[i].encapDataByte[k]);
			printf("\n");
			//printf("%d",fsm_dataStruct->fsm_frameStruct[i].encapData[k]);
			//switch(k)//测试消息24B的
			//{
			//case 5:case 7:case 37:case 39:case 47:case 89:case 131:case 161:case 167:
			//	printf("\n");
			//	break;
			//}
		}
		printf("\n");
		printf("rcvSlot(Assign+1125,other+375):		%d\n",fsm_dataStruct->fsm_frameStruct[i].rcvSlot);
		printf("candidateSlotPeriod(second):	%d\n",fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod);
		printf("broadChannel(0-A,1-B):			%d\n",fsm_dataStruct->fsm_frameStruct[i].broadChannel);
		switch (fsm_dataStruct->fsm_frameStruct[i].msgType)
		{
		case AUTONOMSG: printf("msgType:			AUTO\n");break;
		case INTERROMSG: printf("msgType:			INTERROGATION\n");break;
		case ASSIGNMSG: printf("msgType:			ASSIGNMSG\n");break;
		default:printf("msgType:			%d\n",fsm_dataStruct->fsm_frameStruct[i].msgType);break;
		}
		printf("\n");
		printf("/////////testGenerateMsg24BContent function end/////////////\r\n");
	}
}
