/**
********************************************************************
* @file      testFSMControl.h
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   16:10
********************************************************************
* @brief     定义测试FSMControl.c中功能函数实体的对应测试函数，主要测试函数的输入和输出
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef __TESTFSMCONTROL_H
#define __TESTFSMCONTROL_H

#include "FSMControl.h"
#include "gps.h"
#include "mkdData.h"


extern u8 fillBias;//for test

////////////////////////for test //////////////////////////////////////////////////
void printfBinary(u8 num);

void testVDL_Msg15(u16 rcvSlot,u8 ID1,u16 O1,u8 ID2,u16 O2,u8 ID3,u16 O3,VDLMsg15 * vdlMsg15);

//void initMsgContent(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,GPS_InfoStruct * gps_infoStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23);

void testCollectNewMsg(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,FlagStatus isVDLMsg15NewIn,VDLMsg15 * vdlMsg15,FlagStatus isVDLMsg20NewIn,FlagStatus isVDLMsg22NewIn,FlagStatus isVDLMsg23NewIn,FSM_ControlStruct * fsm_controlStruct);

void testUpdataMsg20Struct(VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct);
void testUpdataMsg22Struct(VDLMsg22 * vdlMsg22,FSM_Msg22Struct * fsm_msg22Struct);
void testUpdataMsg23Struct(VDLMsg23 * vdlMsg23,GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct);

void testCheckFSMMsg20Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct);

void testCheckFSMMsg22Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg22Struct * fsm_msg22Strcut);

void testCheckFSMMsg23Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct);

void testGenerateMsg14Content(AIS_BBMMsgStruct * ais_bbmMsgStruct,FSM_DataStruct * fsm_dataStruct,u8 frameBias);

void testGenerateMsg18Content(MsgTypeIndicator msgType,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel,u8 frameBias);

void testGenerateMsg19Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * vdlMsg15,_Bool alterChannel); 

void testgenerateMsg24Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel,u8 frameBias,u8 part);

void testGenerateCycleContent(u8 workType,GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * rcvVDLMsg15,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct);
#endif

