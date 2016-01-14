/**
********************************************************************
* @file      test.c
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   12:16
********************************************************************
* @brief     编写Msg14、Msg18、Msg19、Msg24在各个条件下生成的内容
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
* Brief     : 测试Msg14的内容
*
* Author    : Digital Design Team
* Param     : ais_bbmMsgStruct- 上位机侧接收到的安全相关消息内容结构体
* Param     : ais_rcvMsgIDStruct- 指示当前有新的安全消息14的指示结构体
* Return    : void
************************************************************************/
void testMsg14(AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct)
{
	//testAIS_BBMMsgStruct(ais_bbmMsgStruct);
	//ais_rcvMsgIDStruct->msgState = MSGNEW;

	//收集新消息ID号
	collectNewMsg(ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);

	//根据收集的新的消息ID号，若有安全、询问类消息，则产生
	generateSafeAndInquiryContent(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
}

/************************************************************************
* Name      : testMsg18Info
* Brief     : 产生测试Msg18条件的内容
*
* Author    : Digital Design Team
* Param     : ais_rcvMsgIDStruct- 输入，用于指示当前安全消息14的新消息情况
* Param     : gps_infoStruct- 输入，GPS信息结构体
* Param     : ais_bbmMsgStruct- 输入，安全消息14的消息内容结构体
* Param     : ais_staticDataStruct- 输入，上位机处与船舶静态信息相关的结构体
* Param     : vdlMsg15- 输入，vdl消息15的内容结构体
* Param     : vdlMsg20- 输入，vdl消息20的内容结构体
* Param     : vdlMsg22- 输入，vdl消息22的内容结构体
* Param     : vdlMsg23- 输入，vdl消息23的内容结构体
* Param     : fsm_controlStruct- 输出，状态机中控制结构体，主要修改新消息的ID号
* Param     : fsm_dataStruct- 输出，状态机中的数据结构体，主要修改当前的时隙号
* Param     : workType1- 输入，根据不同的工作条件需要，选择产生不同的初始化信息
* Return    : workTypeIndicator- 输出，返回初始化的工作状态
************************************************************************/
workTypeIndicator testMsg18Info(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,GPS_InfoStruct * gps_infoStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,
			 FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct,u8 workType1)
{
	workTypeIndicator workType = AUTO;
	//u8 i = workType1;
	testGPS_InfoStruct(gps_infoStruct);//设置gps的值
	switch(workType1)
	{
	case 1://自主模式
		{
			workType = AUTO;
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 2://询问模式 指定了响应时隙
		{
			testVDL_Msg15(1120,18,10,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 3://询问模式 未指定响应时隙
		{
			testVDL_Msg15(1120,18,0,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 4://指配模式，Msg20作用,部分超时
		{
			//workType = MSG20;
			//isVDLMsg20NewIn = SET;
			testMsg20Info(gps_infoStruct, &recVDLMsg20,&fsm_controlStruct->fsm_msg20Struct,2);
			testGPS_InfoStructPara(gps_infoStruct,0,0,12,45,0);//设置gps的值 
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 5://指配模式，Msg22作用,不在切换区
		{
			workType = MSG22;
			isVDLMsg22NewIn = SET;
			testMsg22(&recVDLMsg22,gps_infoStruct,1,0);//在一号区域
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 6://指配模式，Msg23未在寂静期
		{
			workType = MSG23;
			isVDLMsg23NewIn = SET;
			testMsg23Info(gps_infoStruct,3);//指配有效，未在寂静期
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 7://指配模式，Msg23寂静期
		{
			workType = MSG23SILENCE;
			isVDLMsg23NewIn = SET;
			testMsg23Info(gps_infoStruct,2);//指配有效，在寂静期
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 8://指配模式，广播Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区
		{
			testMsg22(&recVDLMsg22,gps_infoStruct,1,0);//在一号区域
			testMsg23Info(gps_infoStruct,4);//指配有效，不在寂静期
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,0,19,59);//（纬度：48.3° 经度：119°）
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 9://指配模式，寻址Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区
		{
			testMsg22(&recVDLMsg22,gps_infoStruct,0,0);//在寻址区域
			testMsg23Info(gps_infoStruct,4);//指配有效，不在寂静期
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,0,19,59);//（纬度：48.3° 经度：119°）
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 10://指配模式，广播Msg22和Msg23(未在寂静期)一起作用，Msg22在切换区
		{
			//testMsg22(&recVDLMsg22,gps_infoStruct,10,3);//区域3 
			testMsg22Handover(&fsm_controlStruct->fsm_msg22Struct,gps_infoStruct);
			testMsg23Info(gps_infoStruct,5);//指配有效，不在寂静期
			testGPS_InfoStructPara(gps_infoStruct,28980000,69550200,0,19,59);//（纬度：48.3° 经度：115.917°）
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg18Count.countFlag = COUNTED;
			break;
		}
	case 11:////指配模式，Msg22作用,在切换区
		{
			workType = MSG22;
			//isVDLMsg22NewIn = SET;
			//testMsg22(&recVDLMsg22,gps_infoStruct,10,3);//在3号区域，上一个区域未2号
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
* Brief     : 测试消息18的内容
*
* Author    : Digital Design Team
* Param     : workType- 输入，用于设置当前产生Msg18的条件
*			  1-自主模式  2-询问模式 指定了响应时隙  3-询问模式 未指定响应时隙 
*			  4-指配模式，Msg20作用  5-指配模式，Msg22作用  6-指配模式，Msg23未在寂静期
*			  7-指配模式，Msg23寂静期  8-广播Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区 
*			  9-寻址Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区
*			  10-广播Msg22和Msg23(未在寂静期)一起作用，Msg22在切换区
*			  11-Msg22作用,在切换区
* Return    : void
************************************************************************/
void testMsg18(u8 workType)
{
	workTypeIndicator worktype = testMsg18Info(&ais_rcvMsgIDStruct,&gps_infoStruct,&ais_bbmMsgStruct,&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&fsm_controlStruct,&fsm_dataStruct,workType);
	//收集新消息ID号
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//根据收集的新的消息ID号，若有安全、询问类消息，则产生
	generateSafeAndInquiryContent(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);

	//检测当前工作的模式
	worktype = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	setCycleCountNum(&fsm_controlStruct);
	//根据报告间隔时间指示，生成相应的周期报告消息
	generateCycleContent(worktype,&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);

}

/************************************************************************
* Name      : testMsg19
* Brief     : 测试Msg19的内容
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS的信息结构体
* Param     : ais_staticDataStruct-输入，上位机处的船舶静态消息结构体
* Param     : vdlMsg15- 输入，vdl消息15的内容结构体
* Param     : vdlMsg20- 输入，vdl消息20的内容结构体，好像和本消息的联系不大
* Param     : vdlMsg22- 输入，vdl消息22的内容结构体，好像与本消息的联系不大
* Param     : vdlMsg23- 输入，vdl消息23的内容结构体，好像与本消息的联系不大
* Return    : void
************************************************************************/
void testMsg19(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23)
{
	u8 i = 0;
	testGPS_InfoStruct(gps_infoStruct);//设置gps的值
	testAIS_StaticStruct(ais_staticDataStruct);
	testVDL_Msg15(1120,19,10,0,0,0,0,vdlMsg15); 
	isVDLMsg15NewIn = SET;

	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//根据收集的新的消息ID号，若有安全、询问类消息，则产生
	generateSafeAndInquiryContent(gps_infoStruct,ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
}

/************************************************************************
* Name      : testMsg24Info
* Brief     : 产生测试Msg24的初始化内容
*
* Author    : Digital Design Team
* Param     : ais_staticDataStruct- 输入，上位机静态数据结构体
* Param     : vdlMsg15- 输出，vdl消息15内容结构体
* Param     : vdlMsg20- 输出，vdl消息20内容结构体，好像和本消息的联系不大
* Param     : vdlMsg22- 输出，vdl消息22内容结构体，好像和本消息的联系不大
* Param     : vdlMsg23- 输出，vdl消息23内容结构体，好像和本消息的联系不大
* Param     : fsm_controlStruct- 输出，用于设置在指配条件下，生成Msg24所需的初始化条件
* Param     : fsm_dataStruct- 输出，用于设置当前的时隙号
* Param     : workType1- 输出，测试用，无实际作用
* Return    : workTypeIndicator- 测试用，用于指示当前生成测试条件的状态
************************************************************************/
workTypeIndicator testMsg24Info(AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct,u8 workType1)
{
	workTypeIndicator workType = AUTO;
	u8 i = workType1;
	//testAIS_StaticStruct(ais_staticDataStruct);

	switch(i)
	{
	case 1://自主模式
		{
			workType = AUTO;
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 2://询问模式 指定了响应时隙
		{
			testVDL_Msg15(1120,24,10,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 3://询问模式 未指定响应时隙
		{
			testVDL_Msg15(1120,24,0,0,0,0,0,vdlMsg15); 
			isVDLMsg15NewIn = SET;
			break;
		}
	case 4://指配模式，Msg20作用，部分超时
		{
			//workType = MSG20;
			//isVDLMsg20NewIn = SET;
			testMsg20Info(&gps_infoStruct, &recVDLMsg20,&fsm_controlStruct->fsm_msg20Struct,2);
			testGPS_InfoStructPara(&gps_infoStruct,0,0,12,45,0);//设置gps的值 
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 5://指配模式，Msg22作用，不在切换区
		{
			workType = MSG22;
			isVDLMsg22NewIn = SET;
			testMsg22(&recVDLMsg22,&gps_infoStruct,1,0);//在一号区域
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 6://指配模式，Msg23未在寂静期
		{
			workType = MSG23;
			isVDLMsg23NewIn = SET;
			testMsg23Info(&gps_infoStruct,3);//指配有效，未在寂静期
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 7://指配模式，Msg23寂静期
		{
			workType = MSG23SILENCE;
			isVDLMsg23NewIn = SET;
			testMsg23Info(&gps_infoStruct,2);//指配有效，在寂静期
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 8://指配模式，广播Msg22和Msg23（未在寂静期）一起作用，Msg22是否在切换区不影响，但是测试为Msg22不在切换区
		{
			testMsg22(&recVDLMsg22,&gps_infoStruct,1,0);//在一号区域
			testMsg23Info(&gps_infoStruct,4);//指配有效，不在寂静期
			testGPS_InfoStructPara(&gps_infoStruct,28980000,71400000,0,19,59);//（纬度：48.3° 经度：119°）
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	case 9://指配模式，寻址Msg22和Msg23（未在寂静期）一起作用,Msg22是否在切换区不影响，但是测试为Msg22不在切换区
		{
			testMsg22(&recVDLMsg22,&gps_infoStruct,0,0);//在寻址区域
			testMsg23Info(&gps_infoStruct,4);//指配有效，不在寂静期
			testGPS_InfoStructPara(&gps_infoStruct,28980000,71400000,0,19,59);//（纬度：48.3° 经度：119°）
			fsm_dataStruct->realSlot = 1125;
			fsm_controlStruct->msg24Count.countFlag = COUNTED;
			break;
		}
	}

	return workType;
}

/************************************************************************
* Name      : testMsg24
* Brief     : 测试消息24的内容
*
* Author    : Digital Design Team
* Param     : workType- 输入，用于设置当前产生Msg24的条件
*			  1-自主模式  2-询问模式 指定了响应时隙  3-询问模式 未指定响应时隙 
*			  4-指配模式，Msg20作用  5-指配模式，Msg22作用,不在切换区  6-指配模式，Msg23未在寂静期
*			  7-指配模式，Msg23寂静期  8-广播Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区 
*			  9-寻址Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区
*			  10-广播Msg22和Msg23(未在寂静期)一起作用，Msg22在切换区
*			  11-Msg22作用,在切换区
* Return    : void
************************************************************************/
void testMsg24(u8 workType)
{
	workTypeIndicator worktype = testMsg24Info(&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&fsm_controlStruct,&fsm_dataStruct,workType);
	
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//检测当前工作的模式
	worktype = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	setCycleCountNum(&fsm_controlStruct);

	//根据收集的新的消息ID号，若有安全、询问类消息，则产生
	generateSafeAndInquiryContent(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
	
	//根据报告间隔时间指示，生成相应的周期报告消息
	generateCycleContent(worktype,&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&fsm_controlStruct,&fsm_dataStruct);
}

/************************************************************************
* Name      : testMsg20Info
* Brief     : 产生测试Msg20的初始化条件
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输出，gps信息内容结构体
* Param     : vdlMsg20- 输出，vdl消息20内容结构体
* Param     : fsm_msg20Struct- 输出，状态机中消息20相关的内容结构体，主要用于初始化消息20内容的超时无效
* Param     : worktype- 输入，工作状态测试，用于设置不同的测试条件
* Return    : void
************************************************************************/
void testMsg20Info(GPS_InfoStruct * gps_infoStruct, VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,u8 worktype)
{
	u8 i = worktype;
	initMsg20Struct(fsm_msg20Struct);
	switch(i)
	{
	case 1://设置新的Msg20输入，设置4组新的保留时隙
		{
			initVDLMsg20Struct(vdlMsg20); //设定为固定值
			isVDLMsg20NewIn = SET;
			testGPS_InfoStructPara(gps_infoStruct,0,0,14,0,0);//设置gps的值
			break;
		}
	case 2://Msg20的部分保留时隙超时
		{
			initVDLMsg20StructPara(vdlMsg20,1000,12,40,0, 10,5,5,6, 20,5,6,6, 30,5,5,6, 2000,5,6,0); 
			isVDLMsg20NewIn = SET;
			testGPS_InfoStructPara(gps_infoStruct,0,0,12,45,0);//设置gps的值 
			break;
		}
	case 3://Msg20的全部保留时隙超时
		{
			initVDLMsg20StructPara(vdlMsg20,1000,12,40,0, 10,5,5,6, 20,5,5,6, 30,5,5,6, 2000,5,5,0); 
			isVDLMsg20NewIn = SET;
			testGPS_InfoStructPara(gps_infoStruct,0,0,12,45,0);//设置gps的值 
			break;
		}
	}

}
  
/************************************************************************
* Name      : testMsg20
* Brief     : 测试消息20的内容
*
* Author    : Digital Design Team
* Param     : workType- 输入，工作条件设置
*			  1-设置新的Msg20输入，设置4组新的保留时隙    2-Msg20的部分保留时隙超时    3-Msg20的全部保留时隙超时
* Return    : void
************************************************************************/
void testMsg20(u8 workType)
{
	testMsg20Info(&gps_infoStruct, &recVDLMsg20,&fsm_controlStruct.fsm_msg20Struct,workType);
	//收集新消息ID号
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	
	//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//检测当前船站是否工作在指配模式，若在，则返回具体的模式状态信息 to be done
	workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
	printf("workType:  %d\n",workType);
}


void setMsg22RcvTime(VDLMsg22 * vdlMsg22,u8 month,u8 day,u8 hour,u8 minute,u8 second)
{
	vdlMsg22->rcvTime[0] = month;//月
	vdlMsg22->rcvTime[1] = day;//天
	vdlMsg22->rcvTime[2] = hour;//时
	vdlMsg22->rcvTime[3] = minute;//分
	vdlMsg22->rcvTime[4] = second;//秒
}
/************************************************************************
* Name      : testMsg22
* Brief     : 产生测试Msg22的初始化条件
*
* Author    : Digital Design Team
* Param     : vdlMsg22- 输出，vdl消息20的内容结构体
* Param     : gps_infoStruct- 输出，gps信息内容结构体
* Param     : newArea- 输入，设置选择当前测试的区域的号，0-寻址，1~9对应互不重叠的广播区域 >=10，开始使用overlopArea的参数中
* Param     : overlopArea- 输入，配合newArea使用，当newArea=10时，用于单独设置本船的经纬度位置正好满足在2~9号区域内部
*												  当newArea=11时，用于设置第10个区域范围正好与前面2~9个号区域进行部分重叠
* Return    : void
************************************************************************/
void testMsg22(VDLMsg22 * vdlMsg22,GPS_InfoStruct * gps_infoStruct,u8 newArea,u8 overlopArea)
{

	u8 i = newArea;
	u8 j = overlopArea;
	isVDLMsg22NewIn = SET;
	vdlMsg22->isBroadcast = 0;//0-广播 1-寻址
	//vdlMsg22->baseMMSI = 111111111;
	vdlMsg22->MMSI = 111111111;
	vdlMsg22->handOverArea = 5;
	//vdlMsg22->TxRxMode = 1;	//1-chnanelA 2-channelB 0-channelAB
	vdlMsg22->rcvTime[0] = 12;//月
	vdlMsg22->rcvTime[1] = 31;//天
	vdlMsg22->rcvTime[2] = 23;//时
	vdlMsg22->rcvTime[3] = 50;//分
	vdlMsg22->rcvTime[4] = 59;//秒
	switch(i)
	{
	case 0://寻址 对应的OWNMMSI：3333333333
		{
			vdlMsg22->isBroadcast = 1;
			vdlMsg22->latitude1 = 853;
			vdlMsg22->logitude1 = 81380; 
			vdlMsg22->latitude2 = 0;
			vdlMsg22->logitude2 = 0;
			vdlMsg22->TxRxMode = 2;//TXB RXA/RXB
			break;
		}
	case 1://区域1(东北角(纬度：50°  经度：120°)、西南角(纬度：48° 经度：118°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 72000;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 70800;
			vdlMsg22->TxRxMode = 1;//TXA RXA/RXB
			break;
		}
	case 2://区域2(东北角(纬度：50°  经度：118°)、西南角(纬度：48° 经度：116°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 70800;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 69600;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 3://区域3(东北角(纬度：50°  经度：116°)、西南角(纬度：48° 经度：114°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 69600;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 68400;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 4://区域4(东北角(纬度：50°  经度：114°)、西南角(纬度：48° 经度：112°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 68400;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 67200;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 5://区域5(东北角(纬度：50°  经度：112°)、西南角(纬度：48° 经度：110°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 67200;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 66000;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 6://区域6(东北角(纬度：50°  经度：110°)、西南角(纬度：48° 经度：108°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 66000;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 64800;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 7://区域7(东北角(纬度：50°  经度：108°)、西南角(纬度：48° 经度：106°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 64800;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 63600;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 8://区域8(东北角(纬度：50°  经度：106°)、西南角(纬度：48° 经度：104°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 63600;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 62400;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	case 9://区域9，与上述8个区域未重叠(东北角(纬度：50°  经度：104°)、西南角(纬度：48° 经度：102°))
		{
			testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
			vdlMsg22->latitude1 = 30000;
			vdlMsg22->logitude1 = 62400;
			vdlMsg22->latitude2 = 28800;
			vdlMsg22->logitude2 = 61200;
			vdlMsg22->TxRxMode = 1;
			break;
		}
	//case 12://这种本船的gps位置，不在Msg22的作用范围内
	//	{
	//		testGPS_InfoStructPara(gps_infoStruct,28980000,71400000,23,59,51);//在区域1范围内（纬度：48.3°，经度：119°）
	//		vdlMsg22->latitude1 = 30000;//(东北角(纬度：50°  经度：104°)、西南角(纬度：48° 经度：102°))
	//		vdlMsg22->logitude1 = 62400;
	//		vdlMsg22->latitude2 = 28800;
	//		vdlMsg22->logitude2 = 61200;
	//		vdlMsg22->TxRxMode = 0;//TXA RXA/RXB
	//		//setMsg22RcvTime(vdlMsg22,)
	//		break;
	//	}
	case 10://设置测试物的位置分别满足在上述的第2~第9个区域内部
		{
			isVDLMsg22NewIn = RESET;//设置没有新的Msg22，仅仅修改GPS的经纬度 ,修改效果是，在相应区域内，且不在切换区范围内
			switch(j)
			{
			case 2:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,70200000,12,59,52);//在区域2范围内（纬度：48.3°  经度：117°）
					break;
				}
			case 3:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,69000000,12,59,53);//在区域3范围内（纬度：48.3°  经度：115°）
					break;
				}
			case 4:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,67800000,12,59,54);//在区域4范围内（纬度：48.3°  经度：113°）
					break;
				}
			case 5:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,66600000,12,59,55);//在区域5范围内（纬度：48.3°  经度：111°）
					break;
				}
			case 6:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,65400000,12,59,56);//在区域6范围内（纬度：48.3°  经度：109°）
					break;
				}
			case 7:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,64200000,12,59,57);//在区域7范围内（纬度：48.3°  经度：107°）
					break;
				}
			case 8:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,63000000,12,59,58);//在区域8范围内（纬度：48.3°  经度：105°）
					break;
				}
			case 9:
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,61800000,12,59,59);//在区域9范围内（纬度：48.3°  经度：103°）
					break;
				}

			}
			break;
		}
	case 11://区域11，选择和上述各个区域发生重叠
		{
			switch(j)//应该用该部分来测试各种覆盖情况   to be changed 
			{
			
			case 2://部分重叠区域2(东北角(纬度：51°  经度：118°)、西南角(纬度：49° 经度：116°)) （纬度：48.3° 经度：117.917）
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,70750200,12,59,52);//在区域2范围内
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 70800;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 69600;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 3://部分重叠区域3(东北角(纬度：51°  经度：116°)、西南角(纬度：49° 经度：114°)) （纬度：48.3° 经度：115.917）
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,69550200,12,59,53);//在区域3范围内
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 69600;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 68400;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 4://部分重叠区域4(东北角(纬度：51°  经度：114°)、西南角(纬度：49° 经度：112°))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,68350200,12,59,54);//在区域4范围内 （纬度：48.3° 经度：113.917）
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 68400;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 67200;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 5://部分重叠区域5(东北角(纬度：51°  经度：112°)、西南角(纬度：49° 经度：110°))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,67150200,12,59,55);//在区域5范围内 （纬度：48.3° 经度：111.917） 
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 67200;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 66000;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 6://部分重叠区域6(东北角(纬度：51°  经度：110°)、西南角(纬度：49° 经度：108°))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,65950200,12,59,56);//在区域6范围内 （纬度：48.3° 经度：109.917）
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 66000;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 64800;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 7://部分重叠区域7(东北角(纬度：51°  经度：108°)、西南角(纬度：49° 经度：106°))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,64750200,12,59,57);//在区域7范围内 （纬度：48.3° 经度：107.917）
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 64800;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 63600;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 8://部分重叠区域8(东北角(纬度：51°  经度：106°)、西南角(纬度：49° 经度：104°))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,63550200,12,59,58);//在区域8范围内 （纬度：48.3° 经度：105.917）
					vdlMsg22->latitude1 = 30600;
					vdlMsg22->logitude1 = 63600;
					vdlMsg22->latitude2 = 29400;
					vdlMsg22->logitude2 = 62400;
					vdlMsg22->TxRxMode = 0;
					break;
				}
			case 9://部分重叠区域9(东北角(纬度：51°  经度：104°)、西南角(纬度：49° 经度：102°))
				{
					testGPS_InfoStructPara(gps_infoStruct,28980000,62350200,12,59,59);//在区域9范围内 （纬度：48.3° 经度：103.917）
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
* Brief     : 用于设置条件测试切换区
*
* Author    : Digital Design Team
* Param     : fsm_msg22Struct- 输出，状态机中存放消息22内容结构体
* Param     : gps_infoStruct- 输出，gps信息内容结构体，主要使用其中的经纬度和utc时间
* Return    : void
************************************************************************/
void testMsg22Handover(FSM_Msg22Struct * fsm_msg22Struct,GPS_InfoStruct * gps_infoStruct)
{
	//设置当前Msg22的指配区域是区域3，上一个区域是2，本船位置处在区域3的切换区内
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
	testGPS_InfoStructPara(gps_infoStruct,28980000,69550200,0,19,59);//在区域3范围内 （纬度：48.3° 经度：115.917）
}

/************************************************************************
* Name      : testMsg22Function
* Brief     : 测试所有的Msg22的功能代码
*
* Author    : Digital Design Team
* Param     : functionNumm- 输入，用于设置测试内容，1-区域测试，2-切换区测试，3-信道测试
* Return    : void
************************************************************************/
void testMsg22Function(u8 functionNum)
{
	u8 fNum = functionNum;
	u8 newNum = 0;
	u8 overlopNum = 0;
	u8 workType = AUTO;
	if (fNum == 1)//测试区域
	{
		
		for (newNum = 0; newNum < 26; newNum ++)
		{
			
			if (newNum < 10)// 0~9 设置新区域
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,newNum,0);
			}
			else if(newNum < 18) //10~17 对应2~9 设置本船在相应的区域内
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,10,(newNum - 8));
			}
			else if(newNum < 26)//18~25 对应2~9 设置重叠区域 
			{
				overlopNum = newNum - 16;
				testMsg22(&recVDLMsg22,&gps_infoStruct,11,overlopNum);
			}
			//收集新消息ID号
			collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
			//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
			//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
			updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
			
			//检测当前船站是否工作在指配模式，若在，则返回具体的模式状态信息 to be done
			workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
			printf("workType:  %d\n",workType);
		}
	}
	else if (fNum == 2)//切换区测试 设置当前Msg22的指配区域是区域3，上一个区域是2，本船位置处在区域3的切换区内
	{
		testMsg22Handover(&fsm_controlStruct.fsm_msg22Struct,&gps_infoStruct);
		//收集新消息ID号
		collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
		//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
		//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
		updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

		//检测当前船站是否工作在指配模式，若在，则返回具体的模式状态信息 to be done
		workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
		//printf("workType:  %d\n",workType);
	}
	else if (fNum == 3)//信道测试
	{

	}
	else if (fNum == 4)//广播消息22和寻址消息22的信道管理测试
	{
		//先发送广播消息22，指定一种信道工作方式，再发送寻址消息22，指定另一种信道工作方式,查看信道变化
		//再连续发15次广播消息22，应该设定每分钟发一次，查看信道变化情况
		//将本船移出至寻址和广播都不作用的位置
		for(newNum = 0; newNum < 19; newNum ++)
		{
			if (newNum == 1)//寻址消息对应的MMSI为333333333
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,0,0);
			}
			else if (newNum == 17)//本船站的位置既不在寻址范围，也不在区域范围内
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,12,0);
			}
			else//本船站在区域范围内
			{
				testMsg22(&recVDLMsg22,&gps_infoStruct,1,0);
			}
			collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
			//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
			//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
			updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

			//检测当前船站是否工作在指配模式，若在，则返回具体的模式状态信息 to be done
			workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
		}
	}

}

/************************************************************************
* Name      : testMsg23Info
* Brief     : 用于生成消息23的测试条件
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输出，gps信息内容结构体
* Param     : worktype- 输入，设置测试条件
*			  1-本船不在Msg23的指配区域   2-本船在Msg23的指配区域，指配有效，在寂静期
*			  3-本船在Msg23的指配区域，指配有效，不在寂静期(以上为Msg23单独测试)
*             (以下为配合Msg22区域1的联合测试)
*			  4-本船在Msg23的指配区域1
*			  5-本船在Msg23的指配区域3
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
			testGPS_InfoStructPara(gps_infoStruct,16200000,72000000,12,59,59);//不在Msg23指配区域内 （纬度：27° 经度：120°）
			break;
		}
	case 2:
		{
			testGPS_InfoStructPara(gps_infoStruct,17400000,71400000,12,59,59);//在Msg23指配区域内， 指配有效，但在寂静期（纬度：29° 经度：119°）
			break;
		}
	case 3:
		{
			testGPS_InfoStructPara(gps_infoStruct,17400000,71400000,0,19,59);//在Msg23指配区域内， 指配有效，不在寂静期（纬度：29° 经度：119°）
			break;
		}
	case 4:
		{
			recVDLMsg23.latitude1 = 30000;//(东北角纬度：50°，经度120°，西南角纬度48°，经度118°)
			recVDLMsg23.logitude1 = 72000;
			recVDLMsg23.latitude2 = 28800;
			recVDLMsg23.logitude2 = 70800;
			
			break;
		}
	case 5:
		{
			recVDLMsg23.latitude1 = 30000;//(东北角纬度：50°，经度116°，西南角纬度48°，经度114°)
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
* Brief     : 测试所有的Msg22的功能代码
*
* Author    : Digital Design Team
* Param     : functionNum- 输入，用于设置测试内容，
*			  1-本船不在Msg23的指配区域   2-本船在Msg23的指配区域，指配有效，但在寂静期
*										  3-本船在Msg23的指配区域，指配有效，不在寂静期
* Return    : void
************************************************************************/
void testMsg23Function(u8 functionNum)
{
	u8 workType = 0;
	testMsg23Info(&gps_infoStruct,functionNum);

	//收集新消息ID号
	collectNewMsg(&ais_rcvMsgIDStruct,&recVDLMsg15,&fsm_controlStruct);
	//testCollectNewMsg(&ais_rcvMsgIDStruct,isVDLMsg15NewIn,&recVDLMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,&fsm_controlStruct);
	//根据收集的新的消息ID号，若有指配类消息，则更新相应的指配内容
	updataAssignContent(&recVDLMsg20,&recVDLMsg22,&recVDLMsg23,&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);

	//检测当前船站是否工作在指配模式，若在，则返回具体的模式状态信息 to be done
	workType = checkAssignValid(&gps_infoStruct,&fsm_controlStruct,&fsm_dataStruct);
	printf("workType:  %d\n",workType);
}