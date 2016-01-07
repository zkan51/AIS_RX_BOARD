/**
********************************************************************
* @file      testMKDControl.c
* @author    Digital Design Team
* @version   
* @date      2015/12/8   17:12
********************************************************************
* @brief     编写MKDControl.c中的主要功能函数的对应测试功能代码
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "testMKDControl.h"
#include "stdio.h"

#include "string.h"
//!AIVDM,1,1,0,A,H6:aLCQPTp<P4pL`T4pO;G3G<03,2*77
char str[] = "H6:aLCQPTp<P4pL`T4pO;G3G<03";
u8 fillBias = 2;

void initOtherDataStruct(FPGAVDLData * vdlOtherData,u8 len)
{
	u8 i = 0;
	vdlOtherData->channel = channelA;
	for (i = 0; i < len; i ++)
	{
		vdlOtherData->frameData[i] = 0xC0;
	}
	vdlOtherData->frameBytes = len;
	
}

void initOtherDataFixedStruct(MKD_ContentToBeFramedStruct * toBeFramedStruct,u8 fillBias)
{
	u8 i = 0;
	toBeFramedStruct->aisChannel = channelA;
	for (i = 0; i < strlen(str); i ++)
	{
		toBeFramedStruct->frameEncapContentByte[i] = str[i];
	}
	toBeFramedStruct->frameEncapContentByteLen = i;
	toBeFramedStruct->fillBits = fillBias;
}

void testUpdataVDMorVDOFrame(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_FramedStruct * mkd_framedStruct)
{
	u8 j = 0;
	printf("\n");
	printf("/////////testUpdataVDMorVDOFrame function start/////////////\r\n");
	printf("the input information is :\n");
	printf("frameID: %c",mkd_toBeframeStruct->frameType);
	for (j = 0; j < 5; j ++)
	{
		printf("%c",mkd_toBeframeStruct->frameHead[j]);//打印成字符串的形式
	}
	printf("\n");
	printf("totalNum:		%d\n",mkd_toBeframeStruct->totalNum);
	printf("sequenceNum:	%d\n",mkd_toBeframeStruct->sequenceNum);
	printf("encapLen:		%d\n",mkd_toBeframeStruct->frameEncapContentByteLen);
	printf("encapData:		\n");
	for (j = 0; j < mkd_toBeframeStruct->frameEncapContentByteLen; j ++)
	{
		printf("%c",mkd_toBeframeStruct->frameEncapContentByte[j]);
		if (j % 59 == 0 && j != 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("fillBits:		%d\n",mkd_toBeframeStruct->fillBits);
	printf("the output information is:\n");
	printf("msgState:		",mkd_framedStruct->mkd_msgState);
	switch(mkd_framedStruct->mkd_msgState)
	{
	case MSGNEW: printf("MSGNEW\n");break;
	case MSGOLD:printf("MSGOLD\n");break;
	default:printf("%d\n",mkd_framedStruct->mkd_msgState);
	}
	printf("broadChannel(0-A,1-B):	%d\n",mkd_framedStruct->mkd_broadChannel);
	printf("encapDataLen(byte):		%d\n",mkd_framedStruct->mkd_encapDataByteLen);
	printf("encapData:\n");
	for (j = 0; j < mkd_framedStruct->mkd_encapDataByteLen; j ++)
	{
		printf("%c",mkd_framedStruct->mkd_encapDataByte[j]);
		if (j % 59 == 0 && j != 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("/////////testUpdataVDMorVDOFrame function end/////////////\r\n");

}
void testChangeOtherShipContentTo61162(FPGAVDLData * otherShipMsg,MKD_ContentToBeFramedStruct *mkd_toBeframeStruct)
{
	u8 i = 0;
	printf("\n");
	printf("/////////testChangeOtherShipContentTo61162 function start/////////////\r\n");
	printf("the input information is:\n");
	printf("frameLen:		%d\n",otherShipMsg->frameBytes);
	printf("frameData:		\n");
	for (i = 0; i < otherShipMsg->frameBytes; i ++)
	{
		printf("%x ",otherShipMsg->frameData[i]);
		if (i % 59 == 0 && i != 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("the output information is:\n");
	printf("encapDataLen:		%d\n",mkd_toBeframeStruct->frameEncapContentByteLen);
	printf("encapData:\n");
	for (i = 0; i < mkd_toBeframeStruct->frameEncapContentByteLen; i ++)
	{
		printf("%c ",mkd_toBeframeStruct->frameEncapContentByte[i]);
		if (i % 59 == 0 && i != 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("fillBits:		%d\n",mkd_toBeframeStruct->fillBits);
	printf("\n");

	printf("\n");
	printf("/////////testChangeOtherShipContentTo61162 function end/////////////\r\n");
}

void testChangeOwnShipContentTo61162(OwnShipInfo_Struct * ownShipInfo_struct,MKD_ContentToBeFramedStruct *mkd_toBeframeStruct)
{
	u8 i = 0;
	printf("\n");
	printf("/////////testChangeOwnShipContentTo61162 function end/////////////\r\n");
	printf("the input information is:\n");
	printf("frameLen:		%d\n",ownShipInfo_struct->vdl_encapDataByteLen);
	printf("frameData:		\n");
	for (i = 0; i < ownShipInfo_struct->vdl_encapDataByteLen; i ++)
	{
		printf("%x ",ownShipInfo_struct->vdl_encapDataByte[i]);
		if (i % 59 == 0 && i != 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("the output information is:\n");
	printf("encapDataLen:		%d\n",mkd_toBeframeStruct->frameEncapContentByteLen);
	printf("encapData:\n");
	for (i = 0; i < mkd_toBeframeStruct->frameEncapContentByteLen; i ++)
	{
		printf("%c ",mkd_toBeframeStruct->frameEncapContentByte[i]);
		if (i % 59 == 0 && i != 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("fillBits:		%d\n",mkd_toBeframeStruct->fillBits);

	printf("\n");
	printf("/////////testChangeOwnShipContentTo61162 function end/////////////\r\n");
}

void testMkd_collectOtherShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,u8 i)
{
	u8 j = 0;
	printf("\n");
	printf("/////////testMkd_collectOtherShipMsg function start/////////////\r\n");
	printf("the input information is :\n");
	printf("rcvChannel:			%d\n",vdlOtherData->channel);
	printf("frameDataLen(byte):		%d\n",vdlOtherData->frameBytes);
	printf("frameData:			\n");
	for (i = 0; i < vdlOtherData->frameBytes; i ++)
	{
		printf("%d ",vdlOtherData->frameData[i]);
		if (i%59 == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("the output information is:\n");
	printf("broadChannel:		%d\n",mkd_dataStruct->mkd_framedStruct[i].mkd_broadChannel);
	printf("encapDatalen(byte):		%d\n",mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen);
	printf("encapDataByte:	\n");
	for (j = 0; j < mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen; i ++)
	{
		printf("%d ",mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[j]);
		if (j % 59 == 0)
		{
			printf("\n");
		}
	}
	printf("msgState:		%d\n",mkd_dataStruct->mkd_framedStruct[i].mkd_msgState);
	//printf("",mkd_dataStruct->mkd_framedStruct)
	printf("\n");
	printf("/////////testMkd_collectOtherShipMsg function end/////////////\r\n");
}