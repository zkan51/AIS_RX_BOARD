/**
********************************************************************
* @file      test.c
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   12:16
********************************************************************
* @brief     ��дMsg14��Msg18��Msg19��Msg24�ڸ������������ɵ�����
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "test.h"

/////////////////////////gobal data////////////////////////////////////
u8 gpsTestData[500] = {0};

void testGPSInfo(u8 * buf,GPS_InfoStruct * gps_infoStruct)
{
	


}
/************************************************************************
* Name      : testMsg14
* Brief     : ����Msg14������
*
* Author    : Digital Design Team
* Param     : ais_bbmMsgStruct- ��λ������յ��İ�ȫ�����Ϣ���ݽṹ��
* Param     : ais_rcvMsgIDStruct- ָʾ��ǰ���µİ�ȫ��Ϣ14��ָʾ�ṹ��
* Return    : void
************************************************************************/
void testMsg14(AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct)
{
	//testAIS_BBMMsgStruct(ais_bbmMsgStruct);
	//ais_rcvMsgIDStruct->msgState = MSGNEW;

	//�ռ�����ϢID��
	collectNewMsg(ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);

	//�����ռ����µ���ϢID�ţ����а�ȫ��ѯ������Ϣ�������
	generateSafeAndInquiryContent(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
}

/************************************************************************
* Name      : testMsg18Info
* Brief     : ��������Msg18����������
*
* Author    : Digital Design Team
* Param     : ais_rcvMsgIDStruct- ���룬����ָʾ��ǰ��ȫ��Ϣ14������Ϣ���
* Param     : gps_infoStruct- ���룬GPS��Ϣ�ṹ��
* Param     : ais_bbmMsgStruct- ���룬��ȫ��Ϣ14����Ϣ���ݽṹ��
* Param     : ais_staticDataStruct- ���룬��λ�����봬����̬��Ϣ��صĽṹ��
* Param     : vdlMsg15- ���룬vdl��Ϣ15�����ݽṹ��
* Param     : vdlMsg20- ���룬vdl��Ϣ20�����ݽṹ��
* Param     : vdlMsg22- ���룬vdl��Ϣ22�����ݽṹ��
* Param     : vdlMsg23- ���룬vdl��Ϣ23�����ݽṹ��
* Param     : fsm_controlStruct- �����״̬���п��ƽṹ�壬��Ҫ�޸�����Ϣ��ID��
* Param     : fsm_dataStruct- �����״̬���е����ݽṹ�壬��Ҫ�޸ĵ�ǰ��ʱ϶��
* Param     : workType1- ���룬���ݲ�ͬ�Ĺ���������Ҫ��ѡ�������ͬ�ĳ�ʼ����Ϣ
* Return    : workTypeIndicator- ��������س�ʼ���Ĺ���״̬
************************************************************************/
workTypeIndicator testMsg18Info(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,GPS_InfoStruct * gps_infoStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,
			 FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct,u8 workType1)
{
	workTypeIndicator workType = AUTO;
	//u8 i = workType1;
	testGPS_InfoStruct(gps_infoStruct);//����gps��ֵ
	switch(workType1)
	{
	case 1://����ģʽ
		{
			workType = AUTO;
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 2://ѯ��ģʽ ָ������Ӧʱ϶
		{
			testVDL_Msg15(1120,18,10,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 3://ѯ��ģʽ δָ����Ӧʱ϶
		{
			testVDL_Msg15(1120,18,0,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 4://ָ��ģʽ��Msg20����,���ֳ�ʱ
		{
			//workType = MSG20;
			//isVDLMsg20NewIn = SET;
			testMsg20Info(gps_infoStruct, &recVDLMsg20,&fsm_controlStruct->fsm_msg20Struct,2);
			testGPS_InfoStructPara(gps_infoStruct,0,0,12,45,0);//����gps��ֵ 
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 5://ָ��ģʽ��Msg22����,�����л���
		{
			workType = MSG22;
			isVDLMsg22NewIn = SET;
			testMsg22(&recVDLMsg22,gps_infoStruct,1,0);//��һ������
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 6://ָ��ģʽ��Msg23δ�ڼž���
		{
			workType = MSG23;
			isVDLMsg23NewIn = SET;
			testMsg23Info(gps_infoStruct,3);//ָ����Ч��δ�ڼž���
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 7://ָ��ģʽ��Msg23�ž���
		{
			workType = MSG23SILENCE;
			isVDLMsg23NewIn = SET;
			testMsg23Info(gps_infoStruct,2);//ָ����Ч���ڼž���
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 8://ָ��ģʽ���㲥Msg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л���
		{
			testMsg22(&recVDLMsg22,gps_infoStruct,1,0);//��һ������
			testMsg23Info(gps_infoStruct,4);//ָ����Ч�����ڼž���
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,0,19,59);//��γ�ȣ�48.3�� ���ȣ�119�㣩
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 9://ָ��ģʽ��ѰַMsg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л���
		{
			testMsg22(&recVDLMsg22,gps_infoStruct,0,0);//��Ѱַ����
			testMsg23Info(gps_infoStruct,4);//ָ����Ч�����ڼž���
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,0,19,59);//��γ�ȣ�48.3�� ���ȣ�119�㣩
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 10://ָ��ģʽ���㲥Msg22��Msg23(δ�ڼž���)һ�����ã�Msg22���л���
		{
			//testMsg22(&recVDLMsg22,gps_infoStruct,10,3);//����3 
			testMsg22Handover(&fsm_controlStruct->fsm_msg22Struct,gps_infoStruct);
			testMsg23Info(gps_infoStruct,5);//ָ����Ч�����ڼž���
			testGPS_InfoStructPara(gps_infoStruct,28980000,69550200,0,19,59);//��γ�ȣ�48.3�� ���ȣ�115.917�㣩
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 11:////ָ��ģʽ��Msg22����,���л���
		{
			workType = MSG22;
			//isVDLMsg22NewIn = SET;
			//testMsg22(&recVDLMsg22,gps_infoStruct,10,3);//��3��������һ������δ2��
			testMsg22Handover(&fsm_controlStruct->fsm_msg22Struct,gps_infoStruct);
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	}
	return workType;
}

/************************************************************************
* Name      : testMsg18
* Brief     : ������Ϣ18������
*
* Author    : Digital Design Team
* Param     : workType- ���룬�������õ�ǰ����Msg18������
*			  1-����ģʽ  2-ѯ��ģʽ ָ������Ӧʱ϶  3-ѯ��ģʽ δָ����Ӧʱ϶ 
*			  4-ָ��ģʽ��Msg20����  5-ָ��ģʽ��Msg22����  6-ָ��ģʽ��Msg23δ�ڼž���
*			  7-ָ��ģʽ��Msg23�ž���  8-�㲥Msg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л��� 
*			  9-ѰַMsg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л���
*			  10-�㲥Msg22��Msg23(δ�ڼž���)һ�����ã�Msg22���л���
*			  11-Msg22����,���л���
* Return    : void
************************************************************************/
void testMsg18(u8 workType)
{
	workTypeIndicator worktype = testMsg18Info(&ais_rcvMsgIDStruct,&gps_infoStruct,&ais_bbmMsgStruct,&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&fsm_controlStruct,&fsm_dataStruct,workType);
	//�ռ�����ϢID��
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//�����ռ����µ���ϢID�ţ����а�ȫ��ѯ������Ϣ�������
	generateSafeAndInquiryContent(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);

	//��⵱ǰ������ģʽ
	worktype = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	setCycleCountNum(&fsm_controlStruct);
	//���ݱ�����ʱ��ָʾ��������Ӧ�����ڱ�����Ϣ
	generateCycleContent(worktype,&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);

}

/************************************************************************
* Name      : testMsg19
* Brief     : ����Msg19������
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- ���룬GPS����Ϣ�ṹ��
* Param     : ais_staticDataStruct-���룬��λ�����Ĵ�����̬��Ϣ�ṹ��
* Param     : vdlMsg15- ���룬vdl��Ϣ15�����ݽṹ��
* Param     : vdlMsg20- ���룬vdl��Ϣ20�����ݽṹ�壬����ͱ���Ϣ����ϵ����
* Param     : vdlMsg22- ���룬vdl��Ϣ22�����ݽṹ�壬�����뱾��Ϣ����ϵ����
* Param     : vdlMsg23- ���룬vdl��Ϣ23�����ݽṹ�壬�����뱾��Ϣ����ϵ����
* Return    : void
************************************************************************/
void testMsg19(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23)
{
	u8 i = 0;
	testGPS_InfoStruct(gps_infoStruct);//����gps��ֵ
	testAIS_StaticStruct(ais_staticDataStruct);
	testVDL_Msg15(1120,19,10,0,0,0,0,vdlMsg15); 
	isVDLMsg15NewIn = SET;

	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//�����ռ����µ���ϢID�ţ����а�ȫ��ѯ������Ϣ�������
	generateSafeAndInquiryContent(gps_infoStruct,ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
}

/************************************************************************
* Name      : testMsg24Info
* Brief     : ��������Msg24�ĳ�ʼ������
*
* Author    : Digital Design Team
* Param     : ais_staticDataStruct- ���룬��λ����̬���ݽṹ��
* Param     : vdlMsg15- �����vdl��Ϣ15���ݽṹ��
* Param     : vdlMsg20- �����vdl��Ϣ20���ݽṹ�壬����ͱ���Ϣ����ϵ����
* Param     : vdlMsg22- �����vdl��Ϣ22���ݽṹ�壬����ͱ���Ϣ����ϵ����
* Param     : vdlMsg23- �����vdl��Ϣ23���ݽṹ�壬����ͱ���Ϣ����ϵ����
* Param     : fsm_controlStruct- ���������������ָ�������£�����Msg24����ĳ�ʼ������
* Param     : fsm_dataStruct- ������������õ�ǰ��ʱ϶��
* Param     : workType1- ����������ã���ʵ������
* Return    : workTypeIndicator- �����ã�����ָʾ��ǰ���ɲ���������״̬
************************************************************************/
workTypeIndicator testMsg24Info(AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct,u8 workType1)
{
	workTypeIndicator workType = AUTO;
	u8 i = workType1;
	//testAIS_StaticStruct(ais_staticDataStruct);

	switch(i)
	{
	case 1://����ģʽ
		{
			workType = AUTO;
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 2://ѯ��ģʽ ָ������Ӧʱ϶
		{
			testVDL_Msg15(1120,24,10,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 3://ѯ��ģʽ δָ����Ӧʱ϶
		{
			testVDL_Msg15(1120,24,0,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 4://ָ��ģʽ��Msg20���ã����ֳ�ʱ
		{
			//workType = MSG20;
			//isVDLMsg20NewIn = SET;
			testMsg20Info(&gps_infoStruct, &recVDLMsg20,&fsm_controlStruct->fsm_msg20Struct,2);
			testGPS_InfoStructPara(&gps_infoStruct,0,0,12,45,0);//����gps��ֵ 
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 5://ָ��ģʽ��Msg22���ã������л���
		{
			workType = MSG22;
			isVDLMsg22NewIn = SET;
			testMsg22(&recVDLMsg22,&gps_infoStruct,1,0);//��һ������
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 6://ָ��ģʽ��Msg23δ�ڼž���
		{
			workType = MSG23;
			isVDLMsg23NewIn = SET;
			testMsg23Info(&gps_infoStruct,3);//ָ����Ч��δ�ڼž���
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 7://ָ��ģʽ��Msg23�ž���
		{
			workType = MSG23SILENCE;
			isVDLMsg23NewIn = SET;
			testMsg23Info(&gps_infoStruct,2);//ָ����Ч���ڼž���
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 8://ָ��ģʽ���㲥Msg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�Ƿ����л�����Ӱ�죬���ǲ���ΪMsg22�����л���
		{
			testMsg22(&recVDLMsg22,&gps_infoStruct,1,0);//��һ������
			testMsg23Info(&gps_infoStruct,4);//ָ����Ч�����ڼž���
			testGPS_InfoStructPara(&gps_infoStruct,28980000,71400000,0,19,59);//��γ�ȣ�48.3�� ���ȣ�119�㣩
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 9://ָ��ģʽ��ѰַMsg22��Msg23��δ�ڼž��ڣ�һ������,Msg22�Ƿ����л�����Ӱ�죬���ǲ���ΪMsg22�����л���
		{
			testMsg22(&recVDLMsg22,&gps_infoStruct,0,0);//��Ѱַ����
			testMsg23Info(&gps_infoStruct,4);//ָ����Ч�����ڼž���
			testGPS_InfoStructPara(&gps_infoStruct,28980000,71400000,0,19,59);//��γ�ȣ�48.3�� ���ȣ�119�㣩
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	}

	return workType;
}

/************************************************************************
* Name      : testMsg24
* Brief     : ������Ϣ24������
*
* Author    : Digital Design Team
* Param     : workType- ���룬�������õ�ǰ����Msg24������
*			  1-����ģʽ  2-ѯ��ģʽ ָ������Ӧʱ϶  3-ѯ��ģʽ δָ����Ӧʱ϶ 
*			  4-ָ��ģʽ��Msg20����  5-ָ��ģʽ��Msg22����,�����л���  6-ָ��ģʽ��Msg23δ�ڼž���
*			  7-ָ��ģʽ��Msg23�ž���  8-�㲥Msg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л��� 
*			  9-ѰַMsg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л���
*			  10-�㲥Msg22��Msg23(δ�ڼž���)һ�����ã�Msg22���л���
*			  11-Msg22����,���л���
* Return    : void
************************************************************************/
void testMsg24(u8 workType)
{
	workTypeIndicator worktype = testMsg24Info(&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&fsm_controlStruct,&fsm_dataStruct,workType);
	
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//��⵱ǰ������ģʽ
	worktype = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	setCycleCountNum(&fsm_controlStruct);

	//�����ռ����µ���ϢID�ţ����а�ȫ��ѯ������Ϣ�������
	generateSafeAndInquiryContent(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
	
	//���ݱ�����ʱ��ָʾ��������Ӧ�����ڱ�����Ϣ
	generateCycleContent(worktype,&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
}

/************************************************************************
* Name      : testMsg20Info
* Brief     : ��������Msg20�ĳ�ʼ������
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- �����gps��Ϣ���ݽṹ��
* Param     : vdlMsg20- �����vdl��Ϣ20���ݽṹ��
* Param     : fsm_msg20Struct- �����״̬������Ϣ20��ص����ݽṹ�壬��Ҫ���ڳ�ʼ����Ϣ20���ݵĳ�ʱ��Ч
* Param     : worktype- ���룬����״̬���ԣ��������ò�ͬ�Ĳ�������
* Return    : void
************************************************************************/
void testMsg20Info(GPS_InfoStruct * gps_infoStruct, VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,u8 worktype)
{
	u8 i = worktype;
	initMsg20Struct(fsm_msg20Struct);
	switch(i)
	{
	case 1://�����µ�Msg20���룬����4���µı���ʱ϶
		{
			initVDLMsg20Struct(vdlMsg20); //�趨Ϊ�̶�ֵ
			isVDLMsg20NewIn = SET;
			testGPS_InfoStructPara(gps_infoStruct,0,0,14,0,0);//����gps��ֵ
			break;
		}
	case 2://Msg20�Ĳ��ֱ���ʱ϶��ʱ
		{
			initVDLMsg20StructPara(vdlMsg20,1000,12,40,0, 10,5,5,6, 20,5,6,6, 30,5,5,6, 2000,5,6,0); 
			isVDLMsg20NewIn = SET;
			testGPS_InfoStructPara(gps_infoStruct,0,0,12,45,0);//����gps��ֵ 
			break;
		}
	case 3://Msg20��ȫ������ʱ϶��ʱ
		{
			initVDLMsg20StructPara(vdlMsg20,1000,12,40,0, 10,5,5,6, 20,5,5,6, 30,5,5,6, 2000,5,5,0); 
			isVDLMsg20NewIn = SET;
			testGPS_InfoStructPara(gps_infoStruct,0,0,12,45,0);//����gps��ֵ 
			break;
		}
	}

}
  
/************************************************************************
* Name      : testMsg20
* Brief     : ������Ϣ20������
*
* Author    : Digital Design Team
* Param     : workType- ���룬������������
*			  1-�����µ�Msg20���룬����4���µı���ʱ϶    2-Msg20�Ĳ��ֱ���ʱ϶��ʱ    3-Msg20��ȫ������ʱ϶��ʱ
* Return    : void
************************************************************************/
void testMsg20(u8 workType)
{
	testMsg20Info(&gps_infoStruct, &recVDLMsg20,&fsm_controlStruct.fsm_msg20Struct,workType);
	//�ռ�����ϢID��
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	
	//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//��⵱ǰ��վ�Ƿ�����ָ��ģʽ�����ڣ��򷵻ؾ����ģʽ״̬��Ϣ to be done
	workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
	printf("workType:  %d\n",workType);
}


void setMsg22RcvTime(VDLMsg22 * vdlMsg22,u8 month,u8 day,u8 hour,u8 minute,u8 second)
{
	vdlMsg22->rcvTime[0] = month;//��
	vdlMsg22->rcvTime[1] = day;//��
	vdlMsg22->rcvTime[2] = hour;//ʱ
	vdlMsg22->rcvTime[3] = minute;//��
	vdlMsg22->rcvTime[4] = second;//��
}
/************************************************************************
* Name      : testMsg22
* Brief     : ��������Msg22�ĳ�ʼ������
*
* Author    : Digital Design Team
* Param     : vdlMsg22- �����vdl��Ϣ20�����ݽṹ��
* Param     : gps_infoStruct- �����gps��Ϣ���ݽṹ��
* Param     : newArea- ���룬����ѡ��ǰ���Ե�����ĺţ�0-Ѱַ��1~9��Ӧ�����ص��Ĺ㲥���� >=10����ʼʹ��overlopArea�Ĳ�����
* Param     : overlopArea- ���룬���newAreaʹ�ã���newArea=10ʱ�����ڵ������ñ����ľ�γ��λ������������2~9�������ڲ�
*												  ��newArea=11ʱ���������õ�10������Χ������ǰ��2~9����������в����ص�
* Return    : void
************************************************************************/
void testMsg22(VDLMsg22 * vdlMsg22,GPS_InfoStruct * gps_infoStruct,u8 newArea,u8 overlopArea)
{

	u8 i = newArea;
	u8 j = overlopArea;
	isVDLMsg22NewIn = SET;
	vdlMsg22->isBroadcast = 0;//0-�㲥 1-Ѱַ
	//vdlMsg22->baseMMSI = 111111111;
	vdlMsg22->MMSI = 111111111;
	vdlMsg22->handOverArea = 5;
	//vdlMsg22->TxRxMode = 1;	//1-chnanelA 2-channelB 0-channelAB
	vdlMsg22->rcvTime[0] = 12;//��
	vdlMsg22->rcvTime[1] = 31;//��
	vdlMsg22->rcvTime[2] = 23;//ʱ
	vdlMsg22->rcvTime[3] = 50;//��
	vdlMsg22->rcvTime[4] = 59;//��
	switch(i)
	{
	case 0://Ѱַ ��Ӧ��OWNMMSI��3333333333
		{
			vdlMsg22->isBroadcast = 1;
			vdlMsg22->latitude1 = 853;
			vdlMsg22->logitude1 = 81380; 
			vdlMsg22->latitude2 = 0;
			vdlMsg22->logitude2 = 0;
			vdlMsg22->TxRxMode = 2;//TXB RXA/RXB
			break;
		}
	case 1://����1(������(γ�ȣ�50��  ���ȣ�120��)�����Ͻ�(γ�ȣ�48�� ���ȣ�118��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 72000;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 70800;
			vdlMsg22->TxRxMode = 1;//TXA RXA/RXB
			break;
		}
	case 2://����2(������(γ�ȣ�50��  ���ȣ�118��)�����Ͻ�(γ�ȣ�48�� ���ȣ�116��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 70800;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 69600;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 3://����3(������(γ�ȣ�50��  ���ȣ�116��)�����Ͻ�(γ�ȣ�48�� ���ȣ�114��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 69600;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 68400;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 4://����4(������(γ�ȣ�50��  ���ȣ�114��)�����Ͻ�(γ�ȣ�48�� ���ȣ�112��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 68400;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 67200;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 5://����5(������(γ�ȣ�50��  ���ȣ�112��)�����Ͻ�(γ�ȣ�48�� ���ȣ�110��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 67200;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 66000;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 6://����6(������(γ�ȣ�50��  ���ȣ�110��)�����Ͻ�(γ�ȣ�48�� ���ȣ�108��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 66000;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 64800;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 7://����7(������(γ�ȣ�50��  ���ȣ�108��)�����Ͻ�(γ�ȣ�48�� ���ȣ�106��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 64800;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 63600;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 8://����8(������(γ�ȣ�50��  ���ȣ�106��)�����Ͻ�(γ�ȣ�48�� ���ȣ�104��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 63600;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 62400;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 9://����9��������8������δ�ص�(������(γ�ȣ�50��  ���ȣ�104��)�����Ͻ�(γ�ȣ�48�� ���ȣ�102��))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 62400;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 61200;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	//case 12://���ֱ�����gpsλ�ã�����Msg22�����÷�Χ��
	//	{
	//		testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//������1��Χ�ڣ�γ�ȣ�48.3�㣬���ȣ�119�㣩
	//		vdlMsg22->latitude1 = 30000;//(������(γ�ȣ�50��  ���ȣ�104��)�����Ͻ�(γ�ȣ�48�� ���ȣ�102��))
	//		vdlMsg22->logitude1 = 62400;
	//		vdlMsg22->latitude2 = 28800;
	//		vdlMsg22->logitude2 = 61200;
	//		vdlMsg22->TxRxMode = 0;//TXA RXA/RXB
	//		//setMsg22RcvTime(vdlMsg22,)
	//		break;
	//	}
	case 10://���ò������λ�÷ֱ������������ĵ�2~��9�������ڲ�
		{
			isVDLMsg22NewIn = RESET;//����û���µ�Msg22�������޸�GPS�ľ�γ�� ,�޸�Ч���ǣ�����Ӧ�����ڣ��Ҳ����л�����Χ��
			switch(j)
			{
			case 2:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,70200000,12,59,52);//������2��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�117�㣩
					break;
				}
			case 3:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,69000000,12,59,53);//������3��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�115�㣩
					break;
				}
			case 4:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,67800000,12,59,54);//������4��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�113�㣩
					break;
				}
			case 5:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,66600000,12,59,55);//������5��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�111�㣩
					break;
				}
			case 6:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,65400000,12,59,56);//������6��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�109�㣩
					break;
				}
			case 7:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,64200000,12,59,57);//������7��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�107�㣩
					break;
				}
			case 8:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,63000000,12,59,58);//������8��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�105�㣩
					break;
				}
			case 9:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,61800000,12,59,59);//������9��Χ�ڣ�γ�ȣ�48.3��  ���ȣ�103�㣩
					break;
				}

			}
			break;
		}
	case 11://����11��ѡ������������������ص�
		{
			switch(j)//Ӧ���øò��������Ը��ָ������   to be changed 
			{
			
			case 2://�����ص�����2(������(γ�ȣ�51��  ���ȣ�118��)�����Ͻ�(γ�ȣ�49�� ���ȣ�116��)) ��γ�ȣ�48.3�� ���ȣ�117.917��
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,70750200,12,59,52);//������2��Χ��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 70800;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 69600;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 3://�����ص�����3(������(γ�ȣ�51��  ���ȣ�116��)�����Ͻ�(γ�ȣ�49�� ���ȣ�114��)) ��γ�ȣ�48.3�� ���ȣ�115.917��
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,69550200,12,59,53);//������3��Χ��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 69600;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 68400;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 4://�����ص�����4(������(γ�ȣ�51��  ���ȣ�114��)�����Ͻ�(γ�ȣ�49�� ���ȣ�112��))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,68350200,12,59,54);//������4��Χ�� ��γ�ȣ�48.3�� ���ȣ�113.917��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 68400;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 67200;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 5://�����ص�����5(������(γ�ȣ�51��  ���ȣ�112��)�����Ͻ�(γ�ȣ�49�� ���ȣ�110��))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,67150200,12,59,55);//������5��Χ�� ��γ�ȣ�48.3�� ���ȣ�111.917�� 
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 67200;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 66000;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 6://�����ص�����6(������(γ�ȣ�51��  ���ȣ�110��)�����Ͻ�(γ�ȣ�49�� ���ȣ�108��))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,65950200,12,59,56);//������6��Χ�� ��γ�ȣ�48.3�� ���ȣ�109.917��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 66000;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 64800;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 7://�����ص�����7(������(γ�ȣ�51��  ���ȣ�108��)�����Ͻ�(γ�ȣ�49�� ���ȣ�106��))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,64750200,12,59,57);//������7��Χ�� ��γ�ȣ�48.3�� ���ȣ�107.917��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 64800;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 63600;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 8://�����ص�����8(������(γ�ȣ�51��  ���ȣ�106��)�����Ͻ�(γ�ȣ�49�� ���ȣ�104��))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,63550200,12,59,58);//������8��Χ�� ��γ�ȣ�48.3�� ���ȣ�105.917��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 63600;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 62400;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 9://�����ص�����9(������(γ�ȣ�51��  ���ȣ�104��)�����Ͻ�(γ�ȣ�49�� ���ȣ�102��))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,62350200,12,59,59);//������9��Χ�� ��γ�ȣ�48.3�� ���ȣ�103.917��
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 62400;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 61200;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			default:;
			}
			break;
		}
	}
	
}

/************************************************************************
* Name      : testMsg22Handover
* Brief     : �����������������л���
*
* Author    : Digital Design Team
* Param     : fsm_msg22Struct- �����״̬���д����Ϣ22���ݽṹ��
* Param     : gps_infoStruct- �����gps��Ϣ���ݽṹ�壬��Ҫʹ�����еľ�γ�Ⱥ�utcʱ��
* Return    : void
************************************************************************/
void testMsg22Handover(FSM_Msg22Struct * fsm_msg22Struct,GPS_InfoStruct * gps_infoStruct)
{
	//���õ�ǰMsg22��ָ������������3����һ��������2������λ�ô�������3���л�����
	fsm_msg22Struct->currentArea = 3;
	fsm_msg22Struct->groupManageStruct[2].assignOverTime[0] = 0;
	fsm_msg22Struct->groupManageStruct[2].handOverArea = 5;
	fsm_msg22Struct->groupManageStruct[2].northEastLatitude = 50.0;
	fsm_msg22Struct->groupManageStruct[2].northEastLongitude = 116.0;
	fsm_msg22Struct->groupManageStruct[2].southWestLatitude = 48.0;
	fsm_msg22Struct->groupManageStruct[2].southWestLongitude = 114.0;
	fsm_msg22Struct->groupManageStruct[2].transChannel = CHANNELA;

	fsm_msg22Struct->lastArea = 2;
	fsm_msg22Struct->groupManageStruct[1].assignOverTime[0] = 0;
	fsm_msg22Struct->groupManageStruct[1].handOverArea = 5;
	fsm_msg22Struct->groupManageStruct[1].northEastLatitude = 50.0;
	fsm_msg22Struct->groupManageStruct[1].northEastLongitude = 118.0;
	fsm_msg22Struct->groupManageStruct[1].southWestLatitude = 48.0;
	fsm_msg22Struct->groupManageStruct[1].southWestLongitude = 116.0;
	fsm_msg22Struct->groupManageStruct[1].transChannel = CHANNELB;
	testGPS_InfoStructPara(gps_infoStruct,28980000,69550200,0,19,59);//������3��Χ�� ��γ�ȣ�48.3�� ���ȣ�115.917��
}

/************************************************************************
* Name      : testMsg22Function
* Brief     : �������е�Msg22�Ĺ��ܴ���
*
* Author    : Digital Design Team
* Param     : functionNumm- ���룬�������ò������ݣ�1-������ԣ�2-�л������ԣ�3-�ŵ�����
* Return    : void
************************************************************************/
void testMsg22Function(u8 functionNum)
{
	u8 fNum = functionNum;
	u8 newNum = 0;
	u8 overlopNum = 0;
	u8 workType = AUTO;
	if (fNum == 1)//��������
	{
		
		for (newNum = 0; newNum < 26; newNum ++)
		{
			
			if (newNum < 10)// 0~9 ����������
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,newNum,0);
			}
			else if(newNum < 18) //10~17 ��Ӧ2~9 ���ñ�������Ӧ��������
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,10,(newNum - 8));
			}
			else if(newNum < 26)//18~25 ��Ӧ2~9 �����ص����� 
			{
				overlopNum = newNum - 16;
				testMsg22(&recVDLMsg22,&gps_infoStruct,11,overlopNum);
			}
			//�ռ�����ϢID��
			collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
			//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
			//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
			updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
			
			//��⵱ǰ��վ�Ƿ�����ָ��ģʽ�����ڣ��򷵻ؾ����ģʽ״̬��Ϣ to be done
			workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
			printf("workType:  %d\n",workType);
		}
	}
	else if (fNum == 2)//�л������� ���õ�ǰMsg22��ָ������������3����һ��������2������λ�ô�������3���л�����
	{
		testMsg22Handover(&fsm_controlStruct.fsm_msg22Struct,&gps_infoStruct);
		//�ռ�����ϢID��
		collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
		//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
		//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
		updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

		//��⵱ǰ��վ�Ƿ�����ָ��ģʽ�����ڣ��򷵻ؾ����ģʽ״̬��Ϣ to be done
		workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
		//printf("workType:  %d\n",workType);
	}
	else if (fNum == 3)//�ŵ�����
	{

	}
	else if (fNum == 4)//�㲥��Ϣ22��Ѱַ��Ϣ22���ŵ��������
	{
		//�ȷ��͹㲥��Ϣ22��ָ��һ���ŵ�������ʽ���ٷ���Ѱַ��Ϣ22��ָ����һ���ŵ�������ʽ,�鿴�ŵ��仯
		//��������15�ι㲥��Ϣ22��Ӧ���趨ÿ���ӷ�һ�Σ��鿴�ŵ��仯���
		//�������Ƴ���Ѱַ�͹㲥�������õ�λ��
		for(newNum = 0; newNum < 19; newNum ++)
		{
			if (newNum == 1)//Ѱַ��Ϣ��Ӧ��MMSIΪ333333333
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,0,0);
			}
			else if (newNum == 17)//����վ��λ�üȲ���Ѱַ��Χ��Ҳ��������Χ��
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,12,0);
			}
			else//����վ������Χ��
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,1,0);
			}
			collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
			//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
			//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
			updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

			//��⵱ǰ��վ�Ƿ�����ָ��ģʽ�����ڣ��򷵻ؾ����ģʽ״̬��Ϣ to be done
			workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
		}
	}

}

/************************************************************************
* Name      : testMsg23Info
* Brief     : ����������Ϣ23�Ĳ�������
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- �����gps��Ϣ���ݽṹ��
* Param     : worktype- ���룬���ò�������
*			  1-��������Msg23��ָ������   2-������Msg23��ָ������ָ����Ч���ڼž���
*			  3-������Msg23��ָ������ָ����Ч�����ڼž���(����ΪMsg23��������)
*             (����Ϊ���Msg22����1�����ϲ���)
*			  4-������Msg23��ָ������1
*			  5-������Msg23��ָ������3
* Return    : void
************************************************************************/
void testMsg23Info(GPS_InfoStruct * gps_infoStruct,u8 worktype)
{
	initVDLMsg23Struct(&recVDLMsg23);
	isVDLMsg23NewIn = SET;
	switch(worktype)
	{
	case 1:
		{
			testGPS_InfoStructPara(gps_infoStruct,16200000,72000000,12,59,59);//����Msg23ָ�������� ��γ�ȣ�27�� ���ȣ�120�㣩
			break;
		}
	case 2:
		{
			testGPS_InfoStructPara(gps_infoStruct,17400000,71400000,12,59,59);//��Msg23ָ�������ڣ� ָ����Ч�����ڼž��ڣ�γ�ȣ�29�� ���ȣ�119�㣩
			break;
		}
	case 3:
		{
			testGPS_InfoStructPara(gps_infoStruct,17400000,71400000,0,19,59);//��Msg23ָ�������ڣ� ָ����Ч�����ڼž��ڣ�γ�ȣ�29�� ���ȣ�119�㣩
			break;
		}
	case 4:
		{
			recVDLMsg23.latitude1 = 30000;//(������γ�ȣ�50�㣬����120�㣬���Ͻ�γ��48�㣬����118��)
			recVDLMsg23.logitude1 = 72000;
			recVDLMsg23.latitude2 = 28800;
			recVDLMsg23.logitude2 = 70800;
			
			break;
		}
	case 5:
		{
			recVDLMsg23.latitude1 = 30000;//(������γ�ȣ�50�㣬����116�㣬���Ͻ�γ��48�㣬����114��)
			recVDLMsg23.logitude1 = 69600;
			recVDLMsg23.latitude2 = 28800;
			recVDLMsg23.logitude2 = 68400;
			break;
		}
	default:;

	}

}

/************************************************************************
* Name      : testMsg23Function
* Brief     : �������е�Msg22�Ĺ��ܴ���
*
* Author    : Digital Design Team
* Param     : functionNum- ���룬�������ò������ݣ�
*			  1-��������Msg23��ָ������   2-������Msg23��ָ������ָ����Ч�����ڼž���
*										  3-������Msg23��ָ������ָ����Ч�����ڼž���
* Return    : void
************************************************************************/
void testMsg23Function(u8 functionNum)
{
	u8 workType = 0;
	testMsg23Info(&gps_infoStruct,functionNum);

	//�ռ�����ϢID��
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//�����ռ����µ���ϢID�ţ�����ָ������Ϣ���������Ӧ��ָ������
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//��⵱ǰ��վ�Ƿ�����ָ��ģʽ�����ڣ��򷵻ؾ����ģʽ״̬��Ϣ to be done
	workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
	printf("workType:  %d\n",workType);
}