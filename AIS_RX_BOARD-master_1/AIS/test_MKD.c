/**
********************************************************************
* @file      MKD_test.c
* @author    Digital Design Team
* @version   
* @date      2015/12/8   20:25
********************************************************************
* @brief     编写上位机部分生成各个具体消息的测试功能代码
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "test_MKD.h"
#include "test.h"
#include "testFSMControl.h"




/************************************************************************
* Name      : testVDMFunction
* Brief     : 生成VDM的测试代码
*
* Author    : Digital Design Team
* Param     : subSectionLen-用于设置一个完整的消息被划分成的分段数 现在取值为 1 2 3 4 5
* Return    : void
************************************************************************/
void testVDMFunction(u8 subSectionLen)
{
	switch(subSectionLen)
	{
	case 1://一个完整的消息是1个分段 VDM 
		{

			initOtherDataStruct(&otherShipMsg,40); //需要填充4个bit，6bit长度为54
			isOtherShipMsgNewIn = SET;
			break;
		}
	case 2://一个完整的消息是2个分段 VDM
		{
			initOtherDataStruct(&otherShipMsg,61); //需要填充4个bit，6bit长度为82
			isOtherShipMsgNewIn = SET;
			break;
		}
	case 3://一个完整的消息是3个分段 VDM
		{
			initOtherDataStruct(&otherShipMsg,98); //需要填充2个bit 6bit长度为131
			isOtherShipMsgNewIn = SET;
			break;
		}
	case 4://消息18 VDO
		{
			testMsg18(1);//先使用的自主模式产生一个测试消息内容
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 5://消息19 VDO
		{
			testMsg19(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23);
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 6://消息24 VDO
		{
			testMsg24(1);//使用自主模式产生一个测试消息内容
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 7://消息14 VDO
		{
			testMsg14(&(fsm_controlStruct.ais_bbmMsgStruct),&ais_rcvMsgIDStruct);
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 8:
		{
			initOtherDataFixedStruct(&mkd_controlStruct.mkd_toBeFrameStruct,fillBias);
			updataToBeFrameInfo(&mkd_controlStruct.mkd_toBeFrameStruct,VDM);
			updataVDMorVDOFrame(&mkd_controlStruct.mkd_toBeFrameStruct,&mkd_dataStruct);
			return;
		}
	default:break;
	}
	mkd_collectNewMsg(&isOtherShipMsgNewIn,&otherShipMsg,&mkd_controlStruct,&mkd_dataStruct);
}