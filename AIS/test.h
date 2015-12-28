/**
********************************************************************
* @file      test.h
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   16:05
********************************************************************
* @brief     编写各个消息在各种情况下的测试例程，包括Msg14,Msg18,Msg19,Msg24
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef __TEST_H
#define __TEST_H

#include "UART2.h"
#include "UART3.h"
#include "myStdDef.h"
#include "AIS_PS_Interface.h"
#include "AIS_PS_Struct.h"
#include "FSMControl.h"
#include "testFSMControl.h"



void testMsg14(AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct);

workTypeIndicator testMsg18Info(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,GPS_InfoStruct * gps_infoStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,
			 FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct,u8 workType1);
void testMsg18(u8 workType);

void testMsg19(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23);

workTypeIndicator testMsg24Info(AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * vdlMsg15,VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct,u8 workType1);
void testMsg24(u8 workType);

void testMsg20Info(GPS_InfoStruct * gps_infoStruct, VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,u8 worktype);
void testMsg20(u8 workType);

void testMsg22(VDLMsg22 * vdlMsg22,GPS_InfoStruct * gps_infoStruct,u8 newArea,u8 overlopArea);
void testMsg22Handover(FSM_Msg22Struct * fsm_msg22Struct,GPS_InfoStruct * gps_infoStruct);
void testMsg22Function(u8 functionNum);
void testMsg23Info(GPS_InfoStruct * gps_infoStruct,u8 worktype);
void testMsg23Function(u8 functionNum);
#endif
