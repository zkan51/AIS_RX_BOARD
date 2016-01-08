/**
********************************************************************
* @file      MKD_test.c
* @author    Digital Design Team
* @version   
* @date      2015/12/8   20:25
********************************************************************
* @brief     ��д��λ���������ɸ���������Ϣ�Ĳ��Թ��ܴ���
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
* Brief     : ����VDM�Ĳ��Դ���
*
* Author    : Digital Design Team
* Param     : subSectionLen-��������һ����������Ϣ�����ֳɵķֶ��� ����ȡֵΪ 1 2 3 4 5
* Return    : void
************************************************************************/
void testVDMFunction(u8 subSectionLen)
{
	switch(subSectionLen)
	{
	case 1://һ����������Ϣ��1���ֶ� VDM 
		{

			initOtherDataStruct(&otherShipMsg,40); //��Ҫ���4��bit��6bit����Ϊ54
			isOtherShipMsgNewIn = SET;
			break;
		}
	case 2://һ����������Ϣ��2���ֶ� VDM
		{
			initOtherDataStruct(&otherShipMsg,61); //��Ҫ���4��bit��6bit����Ϊ82
			isOtherShipMsgNewIn = SET;
			break;
		}
	case 3://һ����������Ϣ��3���ֶ� VDM
		{
			initOtherDataStruct(&otherShipMsg,98); //��Ҫ���2��bit 6bit����Ϊ131
			isOtherShipMsgNewIn = SET;
			break;
		}
	case 4://��Ϣ18 VDO
		{
			testMsg18(1);//��ʹ�õ�����ģʽ����һ��������Ϣ����
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 5://��Ϣ19 VDO
		{
			testMsg19(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23);
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 6://��Ϣ24 VDO
		{
			testMsg24(1);//ʹ������ģʽ����һ��������Ϣ����
			getOwn61162ToBeFramedInfo(&mkd_controlStruct,&fsm_dataStruct);
			break;
		}
	case 7://��Ϣ14 VDO
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