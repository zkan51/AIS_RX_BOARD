/**
********************************************************************
* @file      testMKDControl.h
* @author    Digital Design Team
* @version   
* @date      2015/12/8   17:10
********************************************************************
* @brief     编写MKDControl.c中主要功能函数的测试函数的接口声明
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef __TESTMKDCONTROL_H
#define __TESTMKDCONTROL_H

#include "MKDControl.h"
#include "FSMControl.h"


void initOtherDataStruct(FPGAVDLData * vdlOtherData,u8 len);
void initOtherDataFixedStruct(MKD_ContentToBeFramedStruct * toBeFramedStruct,u8 fillBias);
void testChangeOtherShipContentTo61162(FPGAVDLData * otherShipMsg,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);
void testChangeOwnShipContentTo61162(OwnShipInfo_Struct * ownShipInfo_struct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);
void testMkd_collectOtherShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,u8 i);

void testUpdataVDMorVDOFrame(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_FramedStruct * mkd_framedStruct);
void mkd_collecVDLShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,MKD_IdIndicator id);
#endif

