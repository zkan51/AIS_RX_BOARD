/**
********************************************************************
* @file      PS_FPGA_InterfaceTest.h
* @author    Digital Design Team
* @version   
* @date      2016/1/13   11:52
********************************************************************
* @brief     
*		测试PS_FPGA_Interface中的各个功能模块
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/

#ifndef PS_FPGA_InterfaceTest
#define PS_FPGA_InterfaceTest

#include "AIS_PS_Struct.h"

void printMsg15Info(VDLMsg15 msg15);
void testGetMMSI();
void testGetMsgInfo();
void testGetInfoForMsg15();
void testGetReserveSlotInfo();
void printMsg20Info();
void testGetInfoForMsg20();
void testGetLongitudeFromBuffer();
void testGetLatitudeFromBuffer();
void testGetInfoForMsg22();
void testGetInfoForMsg23();


#endif