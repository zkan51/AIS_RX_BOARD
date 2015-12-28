#include "AIS_PS_Struct.h"

//flagstatus isfpgavdldatanewin;
//
///* 接收的消息15、20、消息22、消息23 */
//vdlmsg15 recvdlmsg15;
//flagstatus isvdlmsg15newin;                   // set - 旧数据尚未处理，  reset - 旧数据已经处理，可以覆盖
//
//vdlmsg20 recvdlmsg20;
//flagstatus isvdlmsg20newin;                   // set - 旧数据尚未处理，  reset - 旧数据已经处理，可以覆盖
//
//vdlmsg22 recvdlmsg22;
//flagstatus isvdlmsg22newin;                   // set - 旧数据尚未处理，  reset - 旧数据已经处理，可以覆盖
//
//vdlmsg23 recvdlmsg23;
//flagstatus isvdlmsg23newin;                   // set - 旧数据尚未处理，  reset - 旧数据已经处理，可以覆盖



void initVDLMsg20Struct(VDLMsg20 * vdlMsg20)
{
	//vdlMsg20->baseMMSI = 333333333;
	vdlMsg20->MMSI = 333333333;
	vdlMsg20->bLatitude = 900;
	vdlMsg20->bLongitude = 1000;
	vdlMsg20->rcvSlot = 1000;
	vdlMsg20->rcvUTCTime[0] = 12; //12:40:00  
	vdlMsg20->rcvUTCTime[1] = 40;
	vdlMsg20->rcvUTCTime[2] = 00;
	//vdlMsg20->reservedSlotInfo[0].slotOffset = 10;
	//vdlMsg20->reservedSlotInfo[0].numSlot = 5;
	//vdlMsg20->reservedSlotInfo[0].overTime = 3;	//超时时间为12：43：00
	//vdlMsg20->reservedSlotInfo[0].increment = 3;

	//vdlMsg20->reservedSlotInfo[1].slotOffset = 20;
	//vdlMsg20->reservedSlotInfo[1].numSlot = 5;
	//vdlMsg20->reservedSlotInfo[1].overTime = 4;	//超时时间为12：44：00
	//vdlMsg20->reservedSlotInfo[1].increment = 3;

	//vdlMsg20->reservedSlotInfo[2].slotOffset = 30;
	//vdlMsg20->reservedSlotInfo[2].numSlot = 5;
	//vdlMsg20->reservedSlotInfo[2].overTime = 5;	//超时时间为12：45：00
	//vdlMsg20->reservedSlotInfo[2].increment = 3;

	//vdlMsg20->reservedSlotInfo[3].slotOffset = 2000;
	//vdlMsg20->reservedSlotInfo[3].numSlot = 5;
	//vdlMsg20->reservedSlotInfo[3].overTime = 6;	//超时时间为12：46：00
	//vdlMsg20->reservedSlotInfo[3].increment = 0;
	vdlMsg20->slotInfos[0].slotOffset = 10;
	vdlMsg20->slotInfos[0].numSlot = 5;
	vdlMsg20->slotInfos[0].overTime = 3;	//超时时间为12：43：00
	vdlMsg20->slotInfos[0].increment = 3;

	vdlMsg20->slotInfos[1].slotOffset = 20;
	vdlMsg20->slotInfos[1].numSlot = 5;
	vdlMsg20->slotInfos[1].overTime = 4;	//超时时间为12：44：00
	vdlMsg20->slotInfos[1].increment = 3;

	vdlMsg20->slotInfos[2].slotOffset = 30;
	vdlMsg20->slotInfos[2].numSlot = 5;
	vdlMsg20->slotInfos[2].overTime = 5;	//超时时间为12：45：00
	vdlMsg20->slotInfos[2].increment = 3;

	vdlMsg20->slotInfos[3].slotOffset = 2000;
	vdlMsg20->slotInfos[3].numSlot = 5;
	vdlMsg20->slotInfos[3].overTime = 6;	//超时时间为12：46：00
	vdlMsg20->slotInfos[3].increment = 0;
	
}

void initVDLMsg20StructPara(VDLMsg20 * vdlMsg20,u16 rcvSlot,u8 rcvhh,u8 rcvmm,u8 rcvss,u16 Offer1,u8 num1,u8 over1,u8 incre1,u16 Offer2,u8 num2,u8 over2,u8 incre2,u16 Offer3,u8 num3,u8 over3,u8 incre3,u16 Offer4,u8 num4,u8 over4,u8 incre4)
{
	//vdlMsg20->baseMMSI = 333333333;
	vdlMsg20->MMSI = 333333333;
	vdlMsg20->bLatitude = 37369287;
	vdlMsg20->bLongitude = 41184142;
	vdlMsg20->rcvSlot = rcvSlot;
	vdlMsg20->rcvUTCTime[0] = rcvhh; 
	vdlMsg20->rcvUTCTime[1] = rcvmm;
	vdlMsg20->rcvUTCTime[2] = rcvss;
	/*vdlMsg20->reservedSlotInfo[0].slotOffset = Offer1;
	vdlMsg20->reservedSlotInfo[0].numSlot = num1;
	vdlMsg20->reservedSlotInfo[0].overTime = over1;	
	vdlMsg20->reservedSlotInfo[0].increment = incre1;

	vdlMsg20->reservedSlotInfo[1].slotOffset = Offer2;
	vdlMsg20->reservedSlotInfo[1].numSlot = num2;
	vdlMsg20->reservedSlotInfo[1].overTime = over2;	
	vdlMsg20->reservedSlotInfo[1].increment = incre2;

	vdlMsg20->reservedSlotInfo[2].slotOffset = Offer3;
	vdlMsg20->reservedSlotInfo[2].numSlot = num3;
	vdlMsg20->reservedSlotInfo[2].overTime = over3;	
	vdlMsg20->reservedSlotInfo[2].increment = incre3;

	vdlMsg20->reservedSlotInfo[3].slotOffset = Offer4;
	vdlMsg20->reservedSlotInfo[3].numSlot = num4;
	vdlMsg20->reservedSlotInfo[3].overTime = over4;	
	vdlMsg20->reservedSlotInfo[3].increment = incre4;*/
	vdlMsg20->slotInfos[0].slotOffset = Offer1;
	vdlMsg20->slotInfos[0].numSlot = num1;
	vdlMsg20->slotInfos[0].overTime = over1;	
	vdlMsg20->slotInfos[0].increment = incre1;

	vdlMsg20->slotInfos[1].slotOffset = Offer2;
	vdlMsg20->slotInfos[1].numSlot = num2;
	vdlMsg20->slotInfos[1].overTime = over2;	
	vdlMsg20->slotInfos[1].increment = incre2;

	vdlMsg20->slotInfos[2].slotOffset = Offer3;
	vdlMsg20->slotInfos[2].numSlot = num3;
	vdlMsg20->slotInfos[2].overTime = over3;	
	vdlMsg20->slotInfos[2].increment = incre3;

	vdlMsg20->slotInfos[3].slotOffset = Offer4;
	vdlMsg20->slotInfos[3].numSlot = num4;
	vdlMsg20->slotInfos[3].overTime = over4;	
	vdlMsg20->slotInfos[3].increment = incre4;
}


void initVDLMsg23Struct(VDLMsg23 * vdlMsg23)
{
	//vdlMsg23->baseMMSI = 111111111;
	vdlMsg23->MMSI = 111111111;
	vdlMsg23->latitude1 = 18000;//(东北角纬度：30°，经度120°，西南角纬度28°，经度118°)
	vdlMsg23->logitude1 = 72000;
	vdlMsg23->latitude2 = 16800;
	vdlMsg23->logitude2 = 70800;
	vdlMsg23->rcvTime[0] = 23; //设置为23：59：59
	vdlMsg23->rcvTime[1] = 59;
	vdlMsg23->rcvTime[2] = 59;
	vdlMsg23->reportInterval = 1; //为10分钟 
	vdlMsg23->silentTime = 20; //寂静结束时刻：0：19：59
	vdlMsg23->shipType = 33;
	vdlMsg23->stationType = 5;
	vdlMsg23->TxRxMode = 1;


}