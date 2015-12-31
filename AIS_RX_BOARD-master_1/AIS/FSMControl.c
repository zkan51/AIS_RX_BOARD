/*********************************************************************
* @file      FSMControl.c
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   12:10
********************************************************************
* @brief     状态机的控制程序,完成对串口中断接收的数据的处理，并根据相应的工作模式，组合生成相应的消息内容
*			 处理的消息包括Msg14,Msg15，Msg18,Msg19，Msg20,Msg22,Msg23,Msg24
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
**********************************************************************/
#include "FSMControl.h"
#include "testFSMControl.h"
///////////////////////////全局变量区///////////////////////////////////////
FSM_DataStruct fsm_dataStruct;
FSM_ControlStruct fsm_controlStruct;


///////////////////////////外部引用变量区//////////////////////////////////




//////////////////////////代码区/////////////////////////////////////


/************************************************************************
* Name      : FSMInit
* Brief     : 用于初始化状态机，使其一开始工作在自主模式，设定报告间隔定时器工作
*			  定时器包括：Msg18，初始计数时间间隔为3min，Msg24，初始计数时间间隔为6min
*			  设置指配消息的作用时间均处于超时状态
* Author    : Digital Design Team
* Param     : fsm_controlStruct- 状态机的控制结构体
* Return    : void
************************************************************************/
void FSMInit(FSM_ControlStruct * fsm_controlStruct)
{
	
	fsm_controlStruct->msg18Count.countNum = 180;//初始为3min， 单位：秒
	fsm_controlStruct->msg18AddCount.countNum = 0;//初始为0，仅在切换区时才使用
	fsm_controlStruct->msg24Count.countNum = 360;//初始为6min, 单位：秒
	initMsg20Struct(&fsm_controlStruct->fsm_msg20Struct);
	initMsg22Struct(&fsm_controlStruct->fsm_msg22Struct);
	initMsg23Struct(&fsm_controlStruct->fsm_msg23Struct);
	//添加启动定时器，用于消息18，24的计时
}

/************************************************************************
* Name      : generate8bitRandNum
* Brief     : 用于产生0~255之间的随机数，以供产生随机延迟的超时时间
*
* Author    : Digital Design Team
* Param     : None
* Return    : u8- 0~255之间的随机数
************************************************************************/
u8 generate8bitRandNum(void)
{
	u8 result = 0;
	srand((unsigned) time(NULL)); //用时间做种，每次产生随机数不一样
	result = rand() % 256;
	return result;
}

/************************************************************************
* Name      : collectNewMsg
* Brief     : 采集各个串口中断中接收到的新的消息ID号，并对相应的消息进行分类
*
* Author    : Digital Design Team
* Param     : ais_rcvMsgIDStruct- 输入，检测消息14新消息
* Param     : vdlMsg15- 输入，VDL消息15结构体，使用其中对具体询问消息的信息
* Param     : fsm_controlStruct- 输出，记录分类的新消息ID的结构体
* Return    : void
************************************************************************/
void collectNewMsg(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,VDLMsg15 * vdlMsg15,FSM_ControlStruct * fsm_controlStruct)
{
	if (ais_rcvMsgIDStruct->msgState == MSGNEW)//先判断了消息14
	{
		fsm_controlStruct->controlMsgType[0][fsm_controlStruct->msgTypeLen1] = 14;
		ais_rcvMsgIDStruct->msgState = MSGOLD;
		fsm_controlStruct->msgTypeLen1 ++;
	}
	if (isVDLMsg15NewIn == SET)
	{
		//to be added要提取出Msg15中询问的消息号
		if (vdlMsg15->dstMMSI1 == OWNMMSI && vdlMsg15->msgID1_1 >= 18)//第一个被询问船站,可能询问两个消息
		{
			fsm_controlStruct->controlMsgType[0][fsm_controlStruct->msgTypeLen1] = vdlMsg15->msgID1_1;
			fsm_controlStruct->msgTypeLen1 ++;
			if (vdlMsg15->msgID1_2 >= 18)//这样认为有询问目的地1的第二个消息
			{
				fsm_controlStruct->controlMsgType[0][fsm_controlStruct->msgTypeLen1] = vdlMsg15->msgID1_2;
				fsm_controlStruct->msgTypeLen1 ++;
			}

		}
		else if (vdlMsg15->dstMMSI2 == OWNMMSI && vdlMsg15->msgID2_1 >= 18)//第二个被询问船站，可能被询问一个消息
		{
			fsm_controlStruct->controlMsgType[0][fsm_controlStruct->msgTypeLen1] = vdlMsg15->msgID2_1;
			fsm_controlStruct->msgTypeLen1 ++;
		}
		isVDLMsg15NewIn = RESET;
		
	}
	if (isVDLMsg20NewIn == SET)
	{
		fsm_controlStruct->controlMsgType[1][fsm_controlStruct->msgTypeLen2] = 20;
		isVDLMsg20NewIn = RESET;
		fsm_controlStruct->msgTypeLen2 ++;
	}
	if (isVDLMsg22NewIn == SET)
	{
		fsm_controlStruct->controlMsgType[1][fsm_controlStruct->msgTypeLen2] = 22;
		isVDLMsg22NewIn = RESET;
		fsm_controlStruct->msgTypeLen2 ++;
	}
	if (isVDLMsg23NewIn == SET)
	{
		fsm_controlStruct->controlMsgType[1][fsm_controlStruct->msgTypeLen2] = 23;
		isVDLMsg23NewIn = RESET;
		fsm_controlStruct->msgTypeLen2 ++;

	}
	testCollectNewMsg(ais_rcvMsgIDStruct,isVDLMsg15NewIn,vdlMsg15,isVDLMsg20NewIn,isVDLMsg22NewIn,isVDLMsg23NewIn,fsm_controlStruct);
}

/************************************************************************
* Name      : getAlterChannel
* Brief     : 产生广播消息交替的发送信道指示
*
* Author    : Digital Design Team
* Param     : alterChannelNum- 输入，产生交替信道指示的数值
* Return    : _Bool- 当前可供发射的信道指示 0- channeA 1- channelB
************************************************************************/
_Bool getAlterChannel(u8 alterChannelNum)
{
	alterChannelNum ++;
	return (alterChannelNum % 2);
}

/************************************************************************
* Name      : getAssignAlterChannel
* Brief     : 在指配模式下，产生交替的发送信道指示
*
* Author    : Digital Design Team
* Param     : fsm_controlStruct- 输入，控制结构体，用到其中的alterChannelNum来获取交替的信道
* Return    : _Bool- 当前可供发射的信道指示 0- channeA 1- channelB
************************************************************************/
_Bool getAssignAlterChannel(FSM_ControlStruct * fsm_controlStruct)
{
	u8 i = 0;
	_Bool alterChannel = 0;
	if (MSG22 == fsm_controlStruct->fsm_msg22Struct.assignWorkType && 
	fsm_controlStruct->fsm_msg22Struct.currentArea == 9)	//寻址的Msg22
	{
		i = fsm_controlStruct->fsm_msg22Struct.addrManageStruct.transChannel;
		switch(i)
		{
		case 0:alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);break;//A.B信道交替
		case 1:alterChannel = 0;break; //指代A信道
		case 2:alterChannel = 1;break; //指代B信道
		default:break; //特殊处理
		}
	}
	else if (MSG23 == fsm_controlStruct->fsm_msg23Struct.assignWorkType)//Msg23指配了发射信道
	{
		i = fsm_controlStruct->fsm_msg23Struct.transChannel;
		switch(i)
		{
		case 0:alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);break;//A.B信道交替
		case 1:alterChannel = 0;break; //指代A信道
		case 2:alterChannel = 1;break; //指代B信道
		default:break; //特殊处理
		}
	}
	else if (MSG22 == fsm_controlStruct->fsm_msg22Struct.assignWorkType)//广播的Msg22指配了发射信息
	{
		i = (fsm_controlStruct->fsm_msg22Struct.currentArea-1);
		i = fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].transChannel;
		switch(i)
		{
		case 0:alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);break;//A.B信道交替
		case 1:alterChannel = 0;break; //指代A信道
		case 2:alterChannel = 1;break; //指代B信道
		default:break; //特殊处理
		}
	}
	else
	{
		alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);
	}
	return alterChannel;
}

/************************************************************************
* Name      : getLatiLongiMin
* Brief     : 将GPS中的经纬度按照设定的精度进行转换  ???？？这个函数在哪里调用了
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输出/输入，记录后续使用中的以1/10000min为精度的经纬度
* Param     : gps_RMCMsgStruct- 输入，记录的从GPS串口中获取的格式如（纬度xxxx.xxxxx,前两位为度，纬度yyyyy.yyyyy，前三位为度）的经纬度 
* Param     : lati- 转换后的纬度，以分为单位
* Param     : longi- 转换后的经度，以分为单位
* Param     : accurLen- 设定的取小数点后的位数,单位：分
* Return    : void
************************************************************************/
void getLatiLongiMin(GPS_InfoStruct * gps_infoStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct,u32 lati,u32 longi,u8 accurLen)
{
	u32 tmpdegree = 0;
	u32 tmpminute = 0;
	switch(accurLen)
	{
	case 1: //在GPSInfo_Struct中的以1/10000min为单位的经纬度上，获取以1/10min为单位的经纬度
		{
			lati = gps_infoStruct->latitude / 1000;
			longi = gps_infoStruct->longitude / 1000;
			break;
		}
	case 4://在原始的接收的61162格式的经纬度上，获取以1/10000min为单位的经纬度
		{
			tmpdegree = (gps_RMCMsgStruct->latitudeH >> 8) & 0x00ff;//提取度
			tmpminute = (gps_RMCMsgStruct->latitudeH) & 0x00ff;//提取分
			lati = (tmpdegree * 60 + tmpminute) * 10000 + (gps_RMCMsgStruct->latitudeL) / 10; //转化成1/10000min为精度的值
			//判断方向，若为南纬，记为1，北纬，记为0
			if (gps_RMCMsgStruct->latitudeDir == 0x4E)//北纬(0x4E)为正，南纬(0x53)为负
			{
				gps_infoStruct->latitudeDir = 0;
			}
			else
			{
				gps_infoStruct->latitudeDir = 1;
			}
			tmpdegree = (gps_RMCMsgStruct->longitudeH >> 8) & 0x00ff;//提取度
			tmpminute = (gps_RMCMsgStruct->longitudeL) & 0x00ff;//提取分
			longi = (tmpdegree * 60 + tmpminute) * 10000 + (gps_RMCMsgStruct->longitudeL) / 10; //转化成1/10000min为精度的值
			//判断方向，若为西经，记为1，东经，记为0
			if (gps_RMCMsgStruct->longitudeDir == 0x45)//东经(0x45)为正，西经(0x57)为负
			{
				gps_infoStruct->longitudeDir = 0;
			}
			else
			{
				gps_infoStruct->longitudeDir = 1;
			}
			break;
		}
		default:;
	}
}

/************************************************************************
* Name      : changeMinuteToDegree
* Brief     : 使用输入的经纬度(经纬度值的放大倍数由accurLen指定，单位为分)来计算获取实际的经纬度值（单位为度）
*
* Author    : Digital Design Team
* Param     : lat_1- 输入，转换前的纬度1，单位分，北纬为正，南纬为负，为十进制值
* Param     : lon_1- 输入，转换前的经度1，单位分，东经为正，西经为负，为十进制值
* Param     : accurLen- 输入，转换前的经纬度的放大倍数,使用accurLen*10
* Param     : lat_2- 输出，转换后的纬度2，单位度，北纬为正，南纬为负，为十进制值，为实际值
* Param     : lon_2- 输出，转换后的经度2，单位度，东经为正，西经为负，为十进制值，为实际值
* Return    : void
************************************************************************/
void changeMinuteToDegree(u32 lat_1,u32 lon_1,u8 accurLen,double * lat_2,double * lon_2)
{
	u32 tmp = 0;
	//需考虑补码的正负
	if (accurLen == 1)  //将1/10min转成度
	{
		if (lon_1 > 0x20000)//判断如果经度是负数 第18位为1 
		{
			tmp = 0x40000 - lon_1;
			* lon_2 = 0 - (tmp / 600.0);
		}
		else
		{
			* lon_2 = lon_1 / 600.0;
		}
		if (lat_1 > 0x10000)//判断如果纬度是负数，第17位为1
		{
			tmp = 0x20000 - lat_1;
			* lat_2 = 0 - (tmp / 600.0);
		}
		else
		{
			* lat_2 = lat_1 / 600.0;//分转度
		}
	}
	else if (accurLen == 4) //将1/10000min转成度
	{
		if (lon_1 > 0x8000000)//判断如果经度是负数 第28位为1 
		{
			tmp = 0x10000000 - lon_1;
			* lon_2 = 0 - (tmp / 600000.0);
		}
		else
		{
			* lon_2 = lon_1 / 600000.0;
		}
		if (lat_1 > 0x4000000)//判断如果纬度是负数，第27位为1
		{
			tmp = 0x8000000 - lat_1;
			* lat_2 = 0 - (tmp / 600000.0);
		}
		else
		{
			* lat_2 = lat_1 / 600000.0;//分转度
		}
	}

}

/************************************************************************
* Name      : calSphereDist
* Brief     : 使用输入的两组经纬度来计算两者的球面距离
*
* Author    : Digital Design Team
* Param     : lat_1- 输入，纬度1，单位度，北纬为正，南纬为负，为实际的十进制值 取大值
* Param     : lon_1- 输入，经度1，单位度，东经为正，西经为负，为实际的十进制值 取大值
* Param     : lat_2- 输入，纬度2，单位度 取小值
* Param     : lon_2- 输入，经度2，单位度 取小值
* Return    : double- 两个经纬度之间的距离，单位：海里
************************************************************************/
double calSphereDist(double lat_1,double lon_1, double lat_2, double lon_2) // lat_1的单位是度
{
	// 计算大圆航线距离：cos(delta_theta)=sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(delta_longitude), dist = R*delta_theta_(单位是m)
	double dist = 0.0; // 单位是m
	double delta_lon = lon_1 - lon_2; // 经度差

	// 计算两点之间的夹角（以地球球心为原点）
	double theta = cos((lat_1 - lat_2)*PI / 180) / 2
		- cos((lat_1 + lat_2)*PI / 180) / 2
		+ cos((delta_lon + lat_1 + lat_2)*PI / 180) / 4
		+ cos((delta_lon + lat_1 - lat_2)*PI / 180) / 4
		+ cos((delta_lon - lat_1 + lat_2)*PI / 180) / 4
		+ cos((delta_lon - lat_1 - lat_2)*PI / 180) / 4;

	// 防止由于精度问题，导致theta的绝对值超过1
	if(theta>1){
		theta=1;
	}else	if(theta<-1){
		theta=-1;
	}
	//地球半径6371KM，转换为海里为3440海里(3440*1852=6370880)
	//dist = 6371000 * acos(theta);
	dist = 3440 * acos(theta);
	return dist;
}

/************************************************************************
* Name      : calLongiDist
* Brief     : 使用输入的两组经纬度来计算平行于经线方向的球面距离
*
* Author    : Digital Design Team
* Param     : lat_1- 输入，纬度1，单位度，北纬为正，南纬为负，为实际的十进制值,取大值
* Param     : lat_2- 输入，纬度2，单位度，取小值
* Return    : double- 两个纬度之间的距离，单位：海里
************************************************************************/
double calLongiDist(double lat_1,double lat_2)
{
	double dist = 0.0;
	dist = (lat_1 - lat_2) * 60;//60 = (PI / 180) * (6371000 / 1852);
	return dist;
}

/************************************************************************
* Name      : initMsg20Struct
* Brief     : 初始化消息20的内容，设置一开始Msg20没有保留时隙
*
* Author    : Digital Design Team
* Param     : fsm_msg20Struct- 输出，控制结构体中记录Msg20的保留时隙内容的结构体
* Return    : void
************************************************************************/
void initMsg20Struct(FSM_Msg20Struct * fsm_msg20Struct)
{
	u8 i = 0;
	while(i < RESERVEDSLOTLENGTH)
	{
		fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] = 24; //超时时间的 时 设置为24，正常超时不会出现该值
		i ++;
	}
}

/************************************************************************
* Name      : updataMsg20Struct
* Brief     : 更新消息20的内容
*
* Author    : Digital Design Team
* Param     : vdlMsg20- 输入，FPGA接收的消息20结构体
* Param     : fsm_msg20Struct- 输出，GPS接收的GPS信息，此处主要使用到GPS的UTC时间和经纬度
* Param     : fsm_dataStruct- 输出，FSM的数据结构体，将新的保留时隙更新至reservedSlot数组中
* Return    : void
************************************************************************/
void updataMsg20Struct(VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct)
{
	u16 i = 0;
	u16 j = 0;
	u8 k = 0;
	u16 sub = 0;
	u8 disp = 0;
	u16 lenBias = 0;
	u16 tmpSlot = 0;
	u32 tmpTimeL = 0;
	u32 tmpTimeH = 0;
	//if (vdlMsg20->slotOffset && vdlMsg20->numSlot && vdlMsg20->overTime)//链路管理信息有用
	if (vdlMsg20->slotInfos[0].slotOffset && vdlMsg20->slotInfos[0].numSlot && vdlMsg20->slotInfos[0].overTime)//链路管理信息有用
	{
		i = 0;
		while (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] != 24)// && fsm_msg20Struct->rsvdSlotStruct[i].overTime[0])//找到最新的一个可写入的保留时隙结构体   //若超时，则将超时时间的小时字节置为24
		{
			i ++;
		}
		if(i < RESERVEDSLOTLENGTH)//最多可以保留的时隙以内
		{
			fsm_msg20Struct->rsvdSlotStruct[i].slotNum = (vdlMsg20->rcvSlot + vdlMsg20->slotInfos[0].slotOffset)%2250;
			fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum = vdlMsg20->slotInfos[0].numSlot;
			fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] = vdlMsg20->rcvUTCTime[0];	//时
			fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] = vdlMsg20->rcvUTCTime[1] + vdlMsg20->slotInfos[0].overTime;	//分
			fsm_msg20Struct->rsvdSlotStruct[i].overTime[2] = vdlMsg20->rcvUTCTime[2];	//秒
			if (fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] >= 60)
			{
				fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] -= 60;
				fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] = (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] + 1) % 24;
			}
		}
		fsm_msg20Struct->bLongitude = vdlMsg20->bLongitude;
		fsm_msg20Struct->blatitude = vdlMsg20->bLatitude;
		//保存有效的保留时隙到数据结构体的相应位置
		lenBias = fsm_msg20Struct->rsvdSlotStruct[i].slotNum;
		j = fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum;
		i = 0;
		while(i < j)
		{
			sub = (lenBias+i) / 8;;
			disp = 7 - (lenBias+i) % 8;
			fsm_dataStruct->reservedSlotByte[sub] |= (0x01 << disp);
			//fsm_dataStruct->reservedSlot[lenBias+i] = 1;
			i ++;
		}
	}
	k = 0; 
	while (k < MAXRESERVEDSLOTLEN)
	{
		//若后续还有可选的保留时隙，以增量作为判断值
		if (vdlMsg20->slotInfos[k].increment || 
			k == 0 && vdlMsg20->slotInfos[0].increment)//有后续可选的保留时隙
		{
			i = 0;
			while (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] != 24)// && fsm_msg20Struct->rsvdSlotStruct[i].overTime[0])//找到最新的一个可写入的保留时隙结构体   //若超时，则将时置为24
			{
				i ++;
			}
			if(i < RESERVEDSLOTLENGTH)//最多可以保留的时隙以内
			{
				fsm_msg20Struct->rsvdSlotStruct[i].slotNum = (vdlMsg20->rcvSlot + vdlMsg20->slotInfos[k+1].slotOffset)%2250;
				fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum = vdlMsg20->slotInfos[k+1].numSlot;
				fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] = vdlMsg20->rcvUTCTime[0];	//时
				fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] = vdlMsg20->rcvUTCTime[1] + vdlMsg20->slotInfos[k+1].overTime;	//分
				fsm_msg20Struct->rsvdSlotStruct[i].overTime[2] = vdlMsg20->rcvUTCTime[2];	//秒
				if (fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] >= 60)
				{
					fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] -= 60;
					fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] = (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] + 1) % 24;
				}
			}
			//保存有效的保留时隙到数据结构体的相应位置
			lenBias = fsm_msg20Struct->rsvdSlotStruct[i].slotNum;
			j = fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum;
			i = 0;
			while(i < j)
			{
				sub = (lenBias+i) / 8;;
				disp = 7 - (lenBias+i) % 8;
				fsm_dataStruct->reservedSlotByte[sub] |= (0x01 << disp);
				//fsm_dataStruct->reservedSlot[lenBias+i] = 1;
				i ++;
			}
			/*while(i < j)
			{
				fsm_dataStruct->reservedSlot[lenBias+i] = 1;
				i ++;
			}*/
		}
		k ++;
	}


}

/************************************************************************
* Name      : initMsg22Struct
* Brief     : 初始化消息22的内容，设置一开始Msg22没有合法的区域
*
* Author    : Digital Design Team
* Param     : fsm_msg22Struct- 输出，控制结构体中记录Msg22的区域内容的结构体
* Return    : void
************************************************************************/
void initMsg22Struct(FSM_Msg22Struct * fsm_msg22Struct)
{
	u8 i = 0;
	fsm_msg22Struct->addrManageStruct.overTime[0] = 13;
	fsm_msg22Struct->addrManageStruct.assignOverTime[0] = 24;
	while(i < 8)
	{
		fsm_msg22Struct->groupManageStruct[i].overTime[0] = 13;//将超时处的 月 部分内容设置为13，正常记录时不会出现该值
		fsm_msg22Struct->groupManageStruct[i].assignOverTime[0] = 24; //将超时处的 时 部分内容设置为24，正常记录时不会出现该值
		i ++;
	}
}

/************************************************************************
* Name      : getMsg22ChannelManagerInfo
* Brief     : 将FPGA串口上传的消息22的内容，保存至状态机中的对应的Msg22的相应位置
*
* Author    : Digital Design Team
* Param     : vdlMsg22- 输入，FPGA串口上传的Msg22的相应内容结构体
* Param     : fsm_msg22Struct- 输出，控制结构体中记录Msg22的区域内容的结构体
* Param     : biasLen- 输出，偏置，当前寻址指配Msg22,值为8，当前广播指配Msg22，值为0~7，对应要存储信息的广播地址偏置
* Return    : void
************************************************************************/
void getMsg22ChannelManagerInfo(VDLMsg22 * vdlMsg22,FSM_Msg22Struct * fsm_msg22Struct,u8 biasLen)
{
	u8 i = 0;
	u8 j = biasLen;
	u8 tmpM = 0;
	u8 tmpD = 0;
	//设置指配消息超时
	//记录指配超时时间
	if (j == 8) //保留至寻址结构体
	{
		i = generate8bitRandNum();//产生0~255范围的随机数
		fsm_msg22Struct->addrManageStruct.assignOverTime[0] = vdlMsg22->rcvTime[2]; //小时
		fsm_msg22Struct->addrManageStruct.assignOverTime[1] = (i / 60) + 4 + vdlMsg22->rcvTime[3];//分钟 待加上报告间隔的值 to be added
		fsm_msg22Struct->addrManageStruct.assignOverTime[2] = (i % 60) + vdlMsg22->rcvTime[4];//秒  带加上报告间隔的值 to be added
		if (fsm_msg22Struct->addrManageStruct.assignOverTime[2] >= 60)
		{
			fsm_msg22Struct->addrManageStruct.assignOverTime[2] -= 60;
			fsm_msg22Struct->addrManageStruct.assignOverTime[1] += 1;	
		}
		if (fsm_msg22Struct->addrManageStruct.assignOverTime[1] >= 60)
		{
			fsm_msg22Struct->addrManageStruct.assignOverTime[1] -= 60;
			fsm_msg22Struct->addrManageStruct.assignOverTime[0] = (fsm_msg22Struct->addrManageStruct.assignOverTime[0]+1) % 24; 
		}

		//记录区域超时时间
		fsm_msg22Struct->addrManageStruct.handOverArea = vdlMsg22->handOverArea;
		fsm_msg22Struct->addrManageStruct.overTime[3] = vdlMsg22->rcvTime[3];	  //分   在62287的Msg22测试中没有体现出对区域存储超时时间的测试
		fsm_msg22Struct->addrManageStruct.overTime[2] = vdlMsg22->rcvTime[2];	  //时
		//fsm_msg22Struct->addrManageStruct.overTime[1] = vdlMsg22->rcvTime[1] + 35;	  //天
		//fsm_msg22Struct->addrManageStruct.overTime[0] = vdlMsg22->rcvTime[0]; //月 设计超时为25~31天之间
		tmpM = vdlMsg22->rcvTime[0];
		tmpD = vdlMsg22->rcvTime[1] + 35;
		while ((tmpD > 28 && tmpM == 2) || 
			   (tmpD > 30 && (tmpM == 4 || tmpM == 6 || tmpM == 9 || tmpM == 11)) ||
			   (tmpD > 31 && (tmpM == 1 || tmpM == 3 || tmpM == 5 || tmpM == 7 || tmpM == 8 || tmpM == 10 || tmpM == 12)))
		{
			switch(tmpM)
			{
			case 2: tmpD -= 28;tmpM = 3;break;
			case 4:case 6:case 9:case 11:tmpD -= 30;tmpM += 1;break;
			case 1:case 3:case 5:case 7:case 8:case 10:case 12:tmpD -= 31;tmpM = (tmpM%12) + 1;break;
			default:;
			}
		}
		fsm_msg22Struct->groupManageStruct[j].overTime[1] = tmpD;//天
		fsm_msg22Struct->groupManageStruct[j].overTime[0] = tmpM;//月
		fsm_msg22Struct->addrManageStruct.transChannel = vdlMsg22->TxRxMode;
		//fsm_msg22Struct->assignValidBias = 8;//	记录当前新的指配信息内容存放在指配区域中
	}
	else//保存至广播结构体，偏移量为j = biasLen的值
	{
		i = generate8bitRandNum();//产生0~255范围的随机数
		fsm_msg22Struct->groupManageStruct[j].assignOverTime[0] = vdlMsg22->rcvTime[2]; //小时
		fsm_msg22Struct->groupManageStruct[j].assignOverTime[1] = (i / 60) + 4 + vdlMsg22->rcvTime[3];//分钟 待加上报告间隔的值 to be added
		fsm_msg22Struct->groupManageStruct[j].assignOverTime[2] = (i % 60) + vdlMsg22->rcvTime[4];//秒  带加上报告间隔的值 to be added
		if (fsm_msg22Struct->groupManageStruct[j].assignOverTime[2] >= 60)
		{
			fsm_msg22Struct->groupManageStruct[j].assignOverTime[2] -= 60;
			fsm_msg22Struct->groupManageStruct[j].assignOverTime[1] += 1;	
		}
		if (fsm_msg22Struct->groupManageStruct[j].assignOverTime[1] >= 60)
		{
			fsm_msg22Struct->groupManageStruct[j].assignOverTime[1] -= 60;
			fsm_msg22Struct->groupManageStruct[j].assignOverTime[0] = (fsm_msg22Struct->groupManageStruct[j].assignOverTime[0]+1) % 24; 
		}

		//记录区域超时时间
		fsm_msg22Struct->groupManageStruct[j].handOverArea = vdlMsg22->handOverArea;
		fsm_msg22Struct->groupManageStruct[j].overTime[3] = vdlMsg22->rcvTime[3];	  //分   在62287的Msg22测试中没有体现出对区域存储超时时间的测试
		fsm_msg22Struct->groupManageStruct[j].overTime[2] = vdlMsg22->rcvTime[2];	  //时
		//fsm_msg22Struct->groupManageStruct[j].overTime[1] = vdlMsg22->rcvTime[1] + 35;	  //天
		//fsm_msg22Struct->groupManageStruct[j].overTime[0] = vdlMsg22->rcvTime[0]; //月 
		tmpD = vdlMsg22->rcvTime[1] + 35;
		tmpM = vdlMsg22->rcvTime[0];
		while ((tmpD > 28 && tmpM == 2) || 
			(tmpD > 30 && (tmpM == 4 || tmpM == 6 || tmpM == 9 || tmpM == 11)) ||
			(tmpD > 31 && (tmpM == 1 || tmpM == 3 || tmpM == 5 || tmpM == 7 || tmpM == 8 || tmpM == 10 || tmpM == 12)))
		{
			switch(tmpM)
			{
			case 2: tmpD -= 28;tmpM = 3;break;
			case 4:case 6:case 9:case 11:tmpD -= 30;tmpM += 1;break;
			case 1:case 3:case 5:case 7:case 8:case 10:case 12:tmpD -= 31;tmpM = (tmpM%12) + 1;break;
			default:;
			}
		}
		fsm_msg22Struct->groupManageStruct[j].overTime[1] = tmpD;//天
		fsm_msg22Struct->groupManageStruct[j].overTime[0] = tmpM;//月
		fsm_msg22Struct->groupManageStruct[j].transChannel = vdlMsg22->TxRxMode;
	}
	
}

/************************************************************************
* Name      : getAreaValid
* Brief     : 检测新输入的经纬度和已存储的经纬度之间的关系
*
* Author    : Digital Design Team
* Param     : tLati1- 新输入的东北角的纬度，单位统一，均为度
* Param     : tLon1- 新输入的东北角的经度
* Param     : tLati2- 新输入的西南角的纬度
* Param     : tLon2- 新输入的西南角的经度
* Param     : sLati1- 已存储的东北角的纬度
* Param     : sLon1- 已存储的东北角的经度
* Param     : sLati2- 已存储的西南角的纬度
* Param     : sLon2- 已存储的西南角的经度
* Return    : u8，指示当前新旧区域之间的关系
*			  0-无重叠，区域有效，1-有重叠，位置有效，
*			  2-无重叠，位置无效，3-有重叠，位置无效
************************************************************************/
u8 getAreaValid(double tLati1,double tLon1,double tLati2,double tLon2,double sLati1,double sLon1,double sLati2,double sLon2)
{
	u8 result = 0;
	if ((tLati2 < sLati1 && tLon2 < sLon1 && sLon2 < tLon1)    //1 新区域与记录区域的右上角有交叉
		|| (tLati2 < sLati1 && sLon2 < tLon1 && tLon2 < sLon1)	//3 新区域与记录区域的左上角有交叉
		|| (sLati2 < tLati1 && sLon2 < tLon1 && tLon2 < sLon1)	//2 新区域与记录区域的左下角有交叉
		|| (sLati2 < tLati1 && tLon2 < sLon2 && sLon2 < tLon1)//4 新区域与记录区域的右下角有交叉
		|| (tLati1 <= sLati1 && tLon1 <= sLon1 && sLati2 <= tLati2 && sLon2 <= tLon2) //5 记录区域包含了新区域
		|| (sLati1 <= tLati1 && sLon1 <= tLon1 && tLati2 <= sLati2 && tLon2 <= sLon2) //6 新区域包含了记录区域
		)//检测是否有重叠区
	{
		result = 1;
	}
	//if ()//检测是否有不合法的位置
	//{
	//}
	return result;
}

/************************************************************************
* Name      : getHandoverAreaValid
* Brief     : 在本船已经位于Msg22指配的一个区域内部的情况下，根据上一个相邻区域的内容来计算本船是否在当前区域的切换区内
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口数据，使用其中的经纬度
* Param     : fsm_controlStruct- 输入，控制结构体，使用其中的Msg22相关的本船所在区域及上一区域结构体中的经纬度信息
* Param     : workArea-输入，用于指示本船所在区域的相关内容存储在控制结构体的Msg22相关的广播区域内的代号，0~7对应相应的广播区域
* Return    : workTypeIndicator- MSG22-普通指配模式 MSG22HANDOVER-切换区模式
************************************************************************/
workTypeIndicator getHandoverAreaValid(GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct,u8 workArea)
{
	workTypeIndicator result = MSG22;
	u8 i = workArea;
	u8 j = 0;
	double lati = 0.0;
	double lon = 0.0;
	u8 adjacentType = 0;
	double dist = 0.0;//单位：海里
	double minDist = 0.0;//单位：海里
	//先判断上一个区域是否已经超时
	j = fsm_controlStruct->fsm_msg22Struct.lastArea; 
	if (j != 0 && j != 9) //j=1~8
	{
		if (fsm_controlStruct->fsm_msg22Struct.groupManageStruct[j-1].overTime[0] == 13)//暂时判断区域的超时时间，而不是指配消息超时
		{
			j = 0;//超时了，则认为在公海
		}
	}
	//在根据上一个区域和本区域的位置来计算当前本船与边界的距离

	if (j != 0 && j < 9) //1~8对应0~7号区域
	{
		j -= 1;//直接对应0~7号区域
		if (fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLongitude == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[j].southWestLongitude)
		{
			adjacentType = 1; //本船所在区域的东北角经线和上一区域重合
		}
		else if (fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLatitude == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[j].northEastLatitude)
		{
			adjacentType = 2; //本船所在区域的西南角纬线和上一区域重合
		}
		if (fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLatitude == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[j].southWestLatitude)
		{
			adjacentType = 3; //本船所在区域的东北角纬线和上一区域重合
		}
		if (fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLongitude == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[j].northEastLongitude)
		{
			adjacentType = 4; //本船所在区域的西南角经线和上一区域重合
		}
	}
	//根据相邻的类型来计算本船里最近的边界的距离
	changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&lati,&lon);
	switch(adjacentType)
	{
	case 0://公海
		{
			dist = calSphereDist(lati,fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLongitude,lati,lon);
			minDist = calLongiDist(lati,fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLatitude);
			if (minDist < dist)
			{
				dist = minDist;
			}
			minDist = calLongiDist(fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLatitude,lati);
			if (minDist < dist)
			{
				dist = minDist;
			}
			minDist = calSphereDist(lati,lon,lati,fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLongitude);
			if (minDist < dist)
			{
				dist = minDist;
			}
			break;
		}
	case 1://用本船所在区域的东北角经度计算
		{
			dist = calSphereDist(lati,fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLongitude,lati,lon);
			break;
		}
	case 2://用本船所在区域的西南角纬度计算
		{
			dist = calLongiDist(lati,fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLatitude);
			break;
		}
	case 3://用本船所在区域的东北角纬度计算
		{
			dist = calLongiDist(fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLatitude,lati);
			break;
		}
	case 4://用本船所在区域的西南角经度计算
		{
			dist = calSphereDist(lati,lon,lati,fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLongitude);
			break;
		}
	default:;
	}
	if (dist <= fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].handOverArea)
	{
		result = MSG22HANDOVER;
	}
	return result;
}

/************************************************************************
* Name      : updataMsg22Struct
* Brief     : 更新消息22的内容,只有寻址Msg22会判断是否本船舶的指配消息，若是广播消息且有效，则不论是否本船在指配区域范围内都记录
*
* Author    : Digital Design Team
* Param     : vdlMsg22- 输入，FPGA接收的消息22结构体
* Param     : fsm_msg22Struct- 输出，FSM的消息22的结构体，将消息22的内容进行保存，以供后续使用
* Return    : void
************************************************************************/
void updataMsg22Struct(VDLMsg22 * vdlMsg22,FSM_Msg22Struct * fsm_msg22Struct)
{
	u8 i;
	u8 areaTmp = 0;
	double len1 = 0;
	double len2 = 0;
	u32 addr1 = 0;
	u32 addr2 = 0;
	double lati1 = 0.0;
	double logi1 = 0.0;
	double lati2 = 0.0;
	double logi2 = 0.0;
	
	//若是寻址的
	if(vdlMsg22->isBroadcast)//寻址消息
	{
		addr1 += (vdlMsg22->logitude1&0x0003ffff)<<12;
		addr1 += (vdlMsg22->latitude1&0x00000fff);
		addr2 += (vdlMsg22->logitude2&0x0003ffff)<<12;
		addr2 += (vdlMsg22->latitude2&0x00000fff);
		if (addr1 == OWNMMSI || addr2 == OWNMMSI)//是本站的指配消息
		{
			getMsg22ChannelManagerInfo(vdlMsg22,fsm_msg22Struct,8);
		}
	}
	else//若是广播的
	{
		//正数的补码为本身，经度、纬度为1/10min精度，则直接除以600获取得到实际的值
		//若是负数，则需要先其变化成对应的正数，则进行相应的操作
		changeMinuteToDegree(vdlMsg22->latitude1,vdlMsg22->logitude1,1,&lati1,&logi1);
		changeMinuteToDegree(vdlMsg22->latitude2,vdlMsg22->logitude2,1,&lati2,&logi2);
		len1 = calLongiDist(lati1,lati2);//获取经度方向的长度
		len2 = calSphereDist(lati1,logi1,lati1,logi2);//获取纬度方向的长度
		if (len1 > MSG22AREALENBIG || len1 < MSG22AREALENSMALL || len2 > MSG22AREALENBIG || len2 < MSG22AREALENSMALL)//区域不合法，则结束
		{
			return;
		}
		i = 0;
		while (i < 8)//对于新接收的消息22，先和已存的数组内容进行比较，若有重叠，则直接替换，否则，则替换当前操作偏置指向的操作数组
		{
			if (fsm_msg22Struct->groupManageStruct[i].overTime[0] != 13)//若有区域内容 使用区域超时做判断 
			{	
				areaTmp = getAreaValid(lati1,logi1,lati2,logi2,
									   fsm_msg22Struct->groupManageStruct[i].northEastLatitude,fsm_msg22Struct->groupManageStruct[i].northEastLongitude,
									   fsm_msg22Struct->groupManageStruct[i].southWestLatitude,fsm_msg22Struct->groupManageStruct[i].southWestLongitude);
				if (areaTmp == 1)//位置有重叠，但是有效
				{
					break;
				}
				else if (areaTmp > 1)//位置不合法
				{
					return;
				}

			}
			i ++;
		}
		if (i == 8)//指配信息为新，与上述的区域没有重叠，且位置合法
		{
			//先使用超时的区域，若没有，在使用当前区域中最先写入的区域覆盖
			i = 0;
			while(i < 8)
			{	
				if (fsm_msg22Struct->groupManageStruct[i].overTime[0] == 13)
				{
					break;
				}
				i ++ ;
			}
			if (i == 8)//区域内容都未超时
			{
				i = fsm_msg22Struct->groupManageBias;
				fsm_msg22Struct->groupManageBias = (fsm_msg22Struct->groupManageBias + 1) % 8;
			}
		}
		//记录经纬度的实际值，使用度数表示
		fsm_msg22Struct->groupManageStruct[i].northEastLatitude = lati1;//vdlMsg22->latitude1;
		fsm_msg22Struct->groupManageStruct[i].northEastLongitude = logi1;//vdlMsg22->logitude1;
		fsm_msg22Struct->groupManageStruct[i].southWestLatitude = lati2;//vdlMsg22->latitude2;
		fsm_msg22Struct->groupManageStruct[i].southWestLongitude = logi2;//vdlMsg22->logitude2;
		getMsg22ChannelManagerInfo(vdlMsg22,fsm_msg22Struct,i);
	}
}

/************************************************************************
* Name      : initMsg23Struct
* Brief     : 初始化消息23的内容，设置一开始Msg23指配消息是超时的，且寂静期也是超时的
*
* Author    : Digital Design Team
* Param     : fsm_msg23Struct- 输出，控制结构体中记录Msg23的区域内容的结构体
* Return    : void
************************************************************************/
void initMsg23Struct(FSM_Msg23Struct * fsm_msg23Struct)
{
	fsm_msg23Struct->silenceTime[0] = 24; //时部分为24
	fsm_msg23Struct->overTime[0] = 24;  //时部分为24
}

/************************************************************************
* Name      : updataMsg23Struct
* Brief     : 更新消息23的内容
*
* Author    : Digital Design Team
* Param     : vdlMsg23- 输入，FPGA接收的消息23结构体
* Param     : gps_infoStruct- 输入 GPS接收的信息结构体
* Param     : fsm_msg23Struct- 输出，FSM的消息23的结构体，将消息23的内容进行保存，以供后续使用
* Return    : void
************************************************************************/
void updataMsg23Struct(VDLMsg23 * vdlMsg23,GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct)
{
	//先判断是本船的指配消息，经纬度和船舶类型，若是再存储
	u8 i = 0;
	double gps_logi = 0.0;
	double gps_lati = 0.0;
	double msg_logi1 = 0.0;//东北角经度
	double msg_lati1 = 0.0;//东北角纬度
	double msg_logi2 = 0.0;//西南角经度
	double msg_lati2 = 0.0;//西南角纬度
	u8 shipType = 33; //测试用
	changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&gps_lati,&gps_logi);
	changeMinuteToDegree(vdlMsg23->latitude1,vdlMsg23->logitude1,1,&msg_lati1,&msg_logi1);
	changeMinuteToDegree(vdlMsg23->latitude2,vdlMsg23->logitude2,1,&msg_lati2,&msg_logi2);
	if (gps_logi > msg_logi1 || gps_logi < msg_logi2 || gps_lati > msg_lati1 || gps_lati < msg_lati2)//如不在指配的区域内，则返回
	{
		return;
	}
	if (vdlMsg23->shipType != shipType || vdlMsg23->stationType != 5)//检测是否为本船类型和台站类型(5指代B类CS)
	{
		return;
	}
	fsm_msg23Struct->northEastLongitude = msg_logi1;
	fsm_msg23Struct->northEastLatitude = msg_lati1;
	fsm_msg23Struct->southWestLongitude = msg_logi2;
	fsm_msg23Struct->southWestLatitude = msg_lati2;
	i = vdlMsg23->reportInterval;
	switch(i)
	{
		case 0:fsm_msg23Struct->reportTime = 0;break; //自主模式所给定的
		case 1:fsm_msg23Struct->reportTime = 600;break; //10min
		case 2:fsm_msg23Struct->reportTime = 360;break; //6min
		case 3:fsm_msg23Struct->reportTime = 180;break; //3min
		case 4:fsm_msg23Struct->reportTime = 60;break; //1min
		case 5:fsm_msg23Struct->reportTime = 30;break; //30s
		case 6:fsm_msg23Struct->reportTime = 15;break; //15s
		case 7:fsm_msg23Struct->reportTime = 10;break; //10s
		case 8:fsm_msg23Struct->reportTime = 5;break; //5s
		default:break;
	}
	if (vdlMsg23->silentTime)//若规定了寂静期
	{
		fsm_msg23Struct->silenceTime[0] = vdlMsg23->rcvTime[0];// 小时 将寂静期直接换算成时刻
		fsm_msg23Struct->silenceTime[1] = vdlMsg23->rcvTime[1] + vdlMsg23->silentTime;//分
		fsm_msg23Struct->silenceTime[2] = vdlMsg23->rcvTime[2];//秒
	}
	else
	{
		fsm_msg23Struct->silenceTime[0] = 24;//小时 直接认为超时
	}
	
	if (fsm_msg23Struct->silenceTime[1] >= 60)
	{
		fsm_msg23Struct->silenceTime[1] -= 60;
		fsm_msg23Struct->silenceTime[0] = (fsm_msg23Struct->silenceTime[0] + 1)% 24;
	}
	fsm_msg23Struct->transChannel = vdlMsg23->TxRxMode;
	i = generate8bitRandNum();
	fsm_msg23Struct->overTime[0] = vdlMsg23->rcvTime[0];	//小时
	fsm_msg23Struct->overTime[1] = (vdlMsg23->rcvTime[1] + 4 + i / 60);// 分 将超时时间直接换算成时刻, 待加报告间隔(暂时认为报告间隔为5分钟) to be changed 
	fsm_msg23Struct->overTime[2] = (vdlMsg23->rcvTime[2] + fsm_msg23Struct->reportTime + i % 60);//秒 
	while (fsm_msg23Struct->overTime[2] >= 60)
	{
		fsm_msg23Struct->overTime[2] -= 60;
		fsm_msg23Struct->overTime[1] += 1;
	}
	while(fsm_msg23Struct->overTime[1] >= 60)
	{
		fsm_msg23Struct->overTime[1] -= 60;
		fsm_msg23Struct->overTime[0] = (fsm_msg23Struct->overTime[0] + 1) % 24;
	}
	fsm_msg23Struct->transChannelFlag = SET;
}

/************************************************************************
* Name      : generateSafeAndInquiryContent
* Brief     : 产生安全和询问相关的消息内容
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS信息结构体，封装了GPS中的航线、航速、经纬度、UTC时间等
* Param     : ais_staticDataStruct- 输入，封装了上位机中给出的静态信息
* Param     : rcvVDLMsg15- 输入，vdl消息15的内容
* Param     : fsm_controlStruct- 输入，状态机的控制结构体
* Param     : fsm_dataStruct-输出，状态机的数据结构体
* Return    : void
************************************************************************/
void generateSafeAndInquiryContent(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * rcvVDLMsg15,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 i = 0;
	_Bool alterChannel = 0;
	//遍历消息分类数组，根据消息内容，做相应的处理
	while (i < fsm_controlStruct->msgTypeLen1)//如果有安全/询问类消息
	{
		if (14 == fsm_controlStruct->controlMsgType[0][i])
		{
			//生成消息14所需内容
			//alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);
			generateMsg14Content(&(fsm_controlStruct->ais_bbmMsgStruct),fsm_dataStruct);
		}
		else if (18 == fsm_controlStruct->controlMsgType[0][i])
		{
			//alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);
			generateMsg18Content(INTERROMSG,gps_infoStruct,fsm_dataStruct,rcvVDLMsg15,0);
		}
		else if (19 == fsm_controlStruct->controlMsgType[0][i])//消息19
		{
			//先判断响应偏置时隙是否大于10，若比10小，则不响应
			if (rcvVDLMsg15->msgID1_1 == 19 && rcvVDLMsg15->slotOffset1_1 >= 10 && rcvVDLMsg15->dstMMSI1 == OWNMMSI ||
				rcvVDLMsg15->msgID1_2 == 19 && rcvVDLMsg15->slotOffset1_2 >= 10 && rcvVDLMsg15->dstMMSI1 == OWNMMSI ||
				rcvVDLMsg15->msgID2_1 == 19 && rcvVDLMsg15->slotOffset2_1 >= 10 && rcvVDLMsg15->dstMMSI2 == OWNMMSI)
			{
				generateMsg19Content(ASSIGNMSG,ais_staticDataStruct,gps_infoStruct,fsm_dataStruct,rcvVDLMsg15,0);
			}
			
		}
		else if (24 == fsm_controlStruct->controlMsgType[0][i])//消息24
		{
			//alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);
			generateMsg24Content(INTERROMSG,ais_staticDataStruct,fsm_dataStruct,rcvVDLMsg15,0);
		}
		i ++;
		//filed = i;
	}
	fsm_controlStruct->msgTypeLen1 = 0;
}

/************************************************************************
* Name      : updataAssignContent
* Brief     : 根据新接收的指配消息更新状态机控制结构体中指配区域的内容
*
* Author    : Digital Design Team
* Param     : vdlMsg20- 输入，vdl消息20的内容
* Param     : vdlMsg22- 输入，vdl消息22的内容
* Param     : vdlMsg23- 输入，vdl消息23的内容
* Param     : gps_infoStruct- 输入，GPS信息结构体，封装了GPS中的航线、航速、经纬度、UTC时间等
* Param     : fsm_controlStruct- 输入，状态机的控制结构体
* Param     : fsm_dataStruct- 输出，状态机的数据结构体
* Return    : void
************************************************************************/
void updataAssignContent(VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 i = 0;
	while (i < fsm_controlStruct->msgTypeLen2)
	{
		if (20 == fsm_controlStruct->controlMsgType[1][i])
		{
			//保存保留时隙号
			updataMsg20Struct(vdlMsg20,&(fsm_controlStruct->fsm_msg20Struct),fsm_dataStruct);
			testUpdataMsg20Struct(vdlMsg20,&(fsm_controlStruct->fsm_msg20Struct),fsm_dataStruct);
		}
		else if (22 == fsm_controlStruct->controlMsgType[1][i])
		{
			//更新区域信息，尤其判断要不要更新Rx/Tx模式
			updataMsg22Struct(vdlMsg22,&(fsm_controlStruct->fsm_msg22Struct));
			testUpdataMsg22Struct(vdlMsg22,&(fsm_controlStruct->fsm_msg22Struct));
		}
		else if (23 == fsm_controlStruct->controlMsgType[1][i])
		{
			//判断之前的是否超时，视情况更新相应信息
			updataMsg23Struct(vdlMsg23,gps_infoStruct,&(fsm_controlStruct->fsm_msg23Struct));
			testUpdataMsg23Struct(vdlMsg23,gps_infoStruct,&(fsm_controlStruct->fsm_msg23Struct));
		}
		i ++;
		//filed = i;
	}
	fsm_controlStruct->msgTypeLen2 = 0;
}

/************************************************************************
* Name      : checkFSMMsg20Struct
* Brief     : 检测当前船站是否处于Msg20规定的指配模式
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口接收到到的实时的动态数据信息，使用其中的GPS获取的经纬度和接收的UTC时间
* Param     : fsm_msg20Struct- 输入，存储的之前接收的VDL msg20的相关内容
* Param     : fsm_dataStruct- 输出，状态机的数据结构体，主要修改reservedSlot[2250]数组对应的保留时隙使用与否标志
* Return    : u8 AUTO-不在指配模式，MSG20-保留时隙有效
************************************************************************/
u8 checkFSMMsg20Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct)
{
	u8 result = 0;
	u8 tmpSlot = 0;//用于记录相应的需要清零的超时时隙 0-没有超时  FF-在有效距离外，保留时隙都按超时处理，使用数据低四位，数据四位从低到高分别对应相应的保留组数，相应的位为1，则认为其超时
	u32 distance = 0;
	u16 sub = 0;
	u8 disp = 0;
	u8 i = 0;
	u8 j = 0;
	u16 lenBias = 0;
	u8 tmp = 0;
	//先检查是否和基站保持在120海里范围内
	//计算距离
	if (distance >= MSG20MAXDISTANCE)	//在离发射消息基站有效的距离外 ，在62287中对Msg20的测试没有体现对距离的测试，暂时不处理
	{
		tmpSlot = 0xff;
	}
	if (!tmpSlot)	//在有效的距离内，继续检查是否超时
	{
		i = 0;
		while (i < RESERVEDSLOTLENGTH)
		{	
			if (gps_infoStruct->utctime[2] == fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] &&
				gps_infoStruct->utctime[3] == fsm_msg20Struct->rsvdSlotStruct[i].overTime[1] && 
				gps_infoStruct->utctime[4] == fsm_msg20Struct->rsvdSlotStruct[i].overTime[2])//超时了 对应：时分秒
			{
				tmpSlot = (tmpSlot | (0x01<<i));
			}
			i ++;
		}
	}
	if (tmpSlot)//如果有超时,可能是部分超时，有可能是全部超时
	{
		i = 0;
		while (i < RESERVEDSLOTLENGTH)
		{
			if ((tmpSlot>>i) & 0x01)
			{
				tmp ++;
				fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] = 24;	//设置小时为24，正常计数不会出现该值
				j = 0;
				lenBias = fsm_msg20Struct->rsvdSlotStruct[i].slotNum;
				while (j < fsm_msg20Struct->rsvdSlotStruct[i].continueSlotNum)
				{
					sub = (lenBias+j) / 8;
					disp = 7-(lenBias+j) % 8;
					fsm_dataStruct->reservedSlotByte[sub] ^= (0x01<<disp);
					//fsm_dataStruct->reservedSlot[lenBias+j] = 0;
					j ++;
				}
			}
			i ++;
		}		
	}
	i = 0;
	while(i < RESERVEDSLOTLENGTH)
	{
		if (fsm_msg20Struct->rsvdSlotStruct[i].overTime[0] != 24)
		{
			result = 0x11;
			break;
		}
		i ++;
	}
	fsm_msg20Struct->assignWorkType = result;
	return result;
}

/************************************************************************
* Name      : checkFSMMsg22Struct
* Brief     : 检测当前船站是否处于Msg22规定的指配模式
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口接收到到的实时的动态数据信息，使用其中的GPS获取的经纬度和接收的UTC时间
* Param     : fsm_controlStruct-输入，存储的之前接收的VDL msg22的相关内容
* Return    : u8 AUTO-不在指配模式，MSG22-在Msg22指配区域内,MSG22HANDOVER-在Msg22切换区域内
************************************************************************/
u8 checkFSMMsg22Struct(GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct)
{
	u8 result = 0;
	u8 i = 0;
	u8 j = 0;
	double logi = 0.0;
	double lati = 0.0;
	//指配超时，影响输出，区域超时，则影响超时的区域的保存，先看区域超时，再看指配超时，再看是否在当前有效的区域位置内，并返回相应的状态值

	//看区域的信息是否还有效
	if (gps_infoStruct->utctime[0] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.overTime[0] && 
		gps_infoStruct->utctime[1] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.overTime[1] && 
		gps_infoStruct->utctime[2] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.overTime[2] && 
		gps_infoStruct->utctime[3] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.overTime[3]) //分别对应 月天时分
	{
		fsm_controlStruct->fsm_msg22Struct.addrManageStruct.overTime[0] = 13;
	}
	//看指配信息是否还有效
	if (gps_infoStruct->utctime[2] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.assignOverTime[0] &&  
		gps_infoStruct->utctime[3] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.assignOverTime[1] && 
		gps_infoStruct->utctime[4] == fsm_controlStruct->fsm_msg22Struct.addrManageStruct.assignOverTime[2] )//当前指配命令超时 分别对应 时分秒
	{
		fsm_controlStruct->fsm_msg22Struct.addrManageStruct.assignOverTime[0] = 24;
	}
	i = 0;
	while(i < 8)//检测8个区域的内容是否超时
	{
		//看区域有效性
		if (gps_infoStruct->utctime[0] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].overTime[0] &&
			gps_infoStruct->utctime[1] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].overTime[1] &&
			gps_infoStruct->utctime[2] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].overTime[2] &&
			gps_infoStruct->utctime[3] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].overTime[3]) //分别对应 月天时分
		{
			fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].overTime[0] = 13;
		}
		//看指配消息本身的有效性
		if (gps_infoStruct->utctime[2] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].assignOverTime[0] &&  
			gps_infoStruct->utctime[3] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].assignOverTime[1] && 
			gps_infoStruct->utctime[4] == fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].assignOverTime[2] )//当前指配命令超时 分别对应 时分秒
		{
			fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].assignOverTime[0] = 24;
		}
		i ++;
	}
	//若没有超时（使用指配超时标志），还需要查看本船的经纬度是否在区域内,寻址指配除外  
	//先检查寻址，再检查广播
	if (fsm_controlStruct->fsm_msg22Struct.addrManageStruct.assignOverTime[0] != 24) //寻址
	{
		result = MSG22;
		fsm_controlStruct->fsm_msg22Struct.currentArea = 9;
		i = 9;
		fsm_controlStruct->fsm_msg22Struct.addrManageStruct.assignOverTime[0] = 24; //测试用，实际时要恢复，to be changed
	}
	else//广播
	{
		changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&lati,&logi);

		//现在不确定是否Msg22可以同时多个指配区域消息有效，若是(暂时按照这种处理方法)，则需要在数组结构体内依次判断是否超时)
		//使用指配消息超时来判断是否在区域内部
		for (i = 0; i < 8; i ++)
		{
			if (fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].assignOverTime[0] != 24 &&
				lati >= fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLatitude && lati <= fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLatitude &&
				logi >= fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].southWestLongitude && logi <= fsm_controlStruct->fsm_msg22Struct.groupManageStruct[i].northEastLongitude)
			{
				result = MSG22;
				//fsm_controlStruct->fsm_msg22Struct.assignValidBias = i;
				//加判断是否更换了新的区域编号 i=0~7 对应实际储存的1~8号区域号
				if((i+1) != fsm_controlStruct->fsm_msg22Struct.currentArea)//该次计算的区域与之前存放的区域不一致
				{
					fsm_controlStruct->fsm_msg22Struct.lastArea = fsm_controlStruct->fsm_msg22Struct.currentArea;
					fsm_controlStruct->fsm_msg22Struct.currentArea = i + 1;
				}
				break;
			}
		}
		//判断是否在切换区内 跟两个区域有关系
		if ((i != 9) && (result == MSG22)) 
		{
			result = getHandoverAreaValid(gps_infoStruct,fsm_controlStruct,i);
		}
	}
	fsm_controlStruct->fsm_msg22Struct.assignWorkType = result;
	return result;
}

/************************************************************************
* Name      : checkFSMMsg23Struct
* Brief     : 检测当前船站是否处于Msg23规定的指配模式
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS串口接收到到的实时的动态数据信息，使用其中的GPS获取的经纬度和接收的UTC时间
* Param     : fsm_msg23Struct-输入，存储的之前接收的VDL msg23的相关内容
* Return    : u8 AUTO-不在指配模式，MSG23-在Msg23指配区域内，MSG23SILENCE-在Msg23的寂静期内
************************************************************************/
u8 checkFSMMsg23Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct)
{
	u8 result = 0;
	double lati = 0.0;
	double logi = 0.0;
	//检查超时
	if (gps_infoStruct->utctime[2] == fsm_msg23Struct->overTime[0] && 
		gps_infoStruct->utctime[3] == fsm_msg23Struct->overTime[1] && 
		gps_infoStruct->utctime[4] == fsm_msg23Struct->overTime[2])//如果超时 对应 时分秒
	{
		fsm_msg23Struct->overTime[0] = 24;
	}
	//若未超时，则检查是否在区域内，若在区域内，则还要检查寂静期是否结束
	if (fsm_msg23Struct->overTime[0] != 24)
	{
		changeMinuteToDegree(gps_infoStruct->latitude,gps_infoStruct->longitude,4,&lati,&logi);
		if (lati < fsm_msg23Struct->southWestLatitude || lati > fsm_msg23Struct->northEastLatitude ||
			logi < fsm_msg23Struct->southWestLatitude || logi > fsm_msg23Struct->northEastLongitude) //若不在区域内
		{
			fsm_msg23Struct->assignWorkType = result;
			return result;
		}
		result = 0x41;
		//在区域内，则判断寂静期
		if (gps_infoStruct->utctime[2] == fsm_msg23Struct->silenceTime[0] && 
			gps_infoStruct->utctime[3] == fsm_msg23Struct->silenceTime[1] && 
			gps_infoStruct->utctime[4] == fsm_msg23Struct->silenceTime[2])//如果超时 对应 时分秒
		{
			fsm_msg23Struct->silenceTime[0] = 24;
			
		}
		if (fsm_msg23Struct->silenceTime[0] != 24)//寂静期时间内
		{
			result = 0x42;
		}
	}
	fsm_msg23Struct->assignWorkType = result;
	return result;
}

/************************************************************************
* Name      : checkAssignValid
* Brief     : 检测当前船站是否处于指配模式
*
* Author    : Digital Design Team
* Param     : gps_infoStruct-输入，gps串口结构体信息，主要使用其中的UTC时间和经纬度
* Param     : fsm_controlStruct-输入，状态机控制结构体，使用其中的Msg20,22,23相关的内容
* Param     : fsm_dataStruct-输出，状态机数据结构体，用于记录Msg20作用产生的保留时隙
* Return    : u8 用于测试当前作用的指配消息的状态，无实际用途
*			  AUTO-不在指配模式，MSG20-保留时隙有效,MSG22-在Msg22指配区域内,MSG22HANDOVER-在Msg22切换区域内,
*			  MSG23-在Msg23指配区域内,MSG23SILENCE-在Msg23的寂静期内
************************************************************************/
u8 checkAssignValid(GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 result = 0;
	result = checkFSMMsg20Struct(gps_infoStruct,&(fsm_controlStruct->fsm_msg20Struct),fsm_dataStruct); //保留时隙
	testCheckFSMMsg20Struct(gps_infoStruct,&(fsm_controlStruct->fsm_msg20Struct),fsm_dataStruct);
	result = checkFSMMsg22Struct(gps_infoStruct,fsm_controlStruct);
	testCheckFSMMsg22Struct(gps_infoStruct,&(fsm_controlStruct->fsm_msg22Struct));
	result = checkFSMMsg23Struct(gps_infoStruct,&(fsm_controlStruct->fsm_msg23Struct));
	testCheckFSMMsg23Struct(gps_infoStruct,&(fsm_controlStruct->fsm_msg23Struct));
	return result;
}

/************************************************************************
* Name      : setCycleCountNum
* Brief     : 设置周期报告的值，包括消息18，24和切换区增加的周期报告消息18
*
* Author    : Digital Design Team
* Param     : fsm_controlStruct- 输入/输出，状态机的控制器结构体，使用其中和报告间隔相关的信息
* Return    : void
************************************************************************/
void setCycleCountNum(FSM_ControlStruct * fsm_controlStruct)
{

	u8 speed = 0;
	//比较顺序：先Msg22切换区，再Msg23,再航速
	if (fsm_controlStruct->fsm_msg22Struct.assignWorkType == MSG22HANDOVER)	//在Msg22设定的切换区内,先不考虑模式状态叠加的结果 to be changed
	{
		if (fsm_controlStruct->fsm_msg23Struct.assignWorkType == MSG23 && fsm_controlStruct->fsm_msg23Struct.reportTime)
		{
			fsm_controlStruct->msg18Count.countNum = (fsm_controlStruct->fsm_msg23Struct.reportTime /2);  //单位：秒
			fsm_controlStruct->msg18AddCount.countNum = (fsm_controlStruct->fsm_msg23Struct.reportTime /2); //单位：秒
		}
		else
		{
			fsm_controlStruct->msg18Count.countNum /= 2;  //单位：秒
			fsm_controlStruct->msg18AddCount.countNum = (fsm_controlStruct->msg18Count.countNum / 2); //单位：秒
		}
	}
	else if (fsm_controlStruct->fsm_msg23Struct.assignWorkType == MSG23 && fsm_controlStruct->fsm_msg23Struct.reportTime) //在Msg23指配的区域内,且修改了报告时间间隔 reportTime == 0 ，则按照自主模式给出的计数值计算
	{
		fsm_controlStruct->msg18Count.countNum = fsm_controlStruct->fsm_msg23Struct.reportTime;  //单位：秒
	}
	else	//检测航速
	{
		//计算航速   to be added
		if (speed > 2)//航速大于2
		{
			fsm_controlStruct->msg18Count.countNum = 30;	//单位：秒
		}
		else
		{
			fsm_controlStruct->msg18Count.countNum = 180;	//单位：秒
		}
	}
	//fsm_controlStruct->msg24Count.countNum = 360;	//单位：秒 消息24的报告间隔貌似没有改变过
}

/************************************************************************
* Name      : generateCycleContent
* Brief     : 用于在指配模式和自主模式下，产生周期性的报告消息，包括Msg18、Msg24
*
* Author    : Digital Design Team
* Param     : workType- 输入，用于指示当前所处的工作模式，取值为AUTO、MSG20、MSG22、MSG20HANDOVER、MSG23、MSG23SILENCE					    
* Param     : gps_infoStruct- 输入，GPS信息结构体，封装了GPS中的航线、航速、经纬度、UTC时间等
* Param     : ais_staticDataStruct- 输入，封装了上位机中给出的静态信息
* Param     : rcvVDLMsg15- 输入，vdl消息15的内容
* Param     : fsm_controlStruct- 输入，状态机的控制结构体
* Param     : fsm_dataStruct- 输出，状态机的数据结构体
* Return    : void
************************************************************************/
void generateCycleContent(workTypeIndicator workType,GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * rcvVDLMsg15,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 i = 0;
	_Bool alterChannel = 0;
	MsgTypeIndicator msgType = AUTONOMSG;
	//相应的报告间隔到达，进行相应周期消息内容合成
	if (fsm_controlStruct->msg18Count.countFlag == COUNTED)//消息18的报告间隔到
	{
		fsm_controlStruct->msg18Count.countFlag = COUNTING;
		//产生消息18的内容，分自主模式和指配模式
		if (fsm_controlStruct->fsm_msg20Struct.assignWorkType == AUTO && fsm_controlStruct->fsm_msg22Struct.assignWorkType == AUTO 
			&& fsm_controlStruct->fsm_msg23Struct.assignWorkType == AUTO)
		{
			alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);
			generateMsg18Content(msgType,gps_infoStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel);//rcvVDLMsg15结构体在自主模式下未使用
		}
		else if (MSG23SILENCE != fsm_controlStruct->fsm_msg23Struct.assignWorkType)//在指配模式但不在寂静期
		{
			if ((fsm_controlStruct->fsm_msg22Struct.assignWorkType & 0xF0) == 0x20)//Msg22指配模式
			{
				msgType = A22MSG18;
			}
			else 
			{
				msgType = ASSIGNMSG;
			}
			
			if (MSG22HANDOVER != fsm_controlStruct->fsm_msg22Struct.assignWorkType)
			{
				//如果同时Msg22和Msg23同时指配了信道，先寻址Msg22,再广播Msg23,再广播Msg22
				alterChannel = getAssignAlterChannel(fsm_controlStruct);
			}
			else
			{
				alterChannel = CHANNELA;//channelA 是主信道
			}
			
			generateMsg18Content(msgType,gps_infoStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel);//rcvVDLMsg15结构体在指配模式下未使用
		}
	}
	if (fsm_controlStruct->msg24Count.countFlag == COUNTED)//消息24的报告间隔到
	{
		fsm_controlStruct->msg24Count.countFlag = COUNTING;
		//产生消息24的内容
		if (fsm_controlStruct->fsm_msg20Struct.assignWorkType == AUTO && fsm_controlStruct->fsm_msg22Struct.assignWorkType == AUTO 
			&& fsm_controlStruct->fsm_msg23Struct.assignWorkType == AUTO)
		{
			alterChannel = getAlterChannel(fsm_controlStruct->alterChannelNum);
			generateMsg24Content(AUTONOMSG,ais_staticDataStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel);//Msg24封装内容不区分自主消息还是指配消息
		}
		else if (MSG23SILENCE != fsm_controlStruct->fsm_msg23Struct.assignWorkType)//处于指配模式
		{
			alterChannel = getAssignAlterChannel(fsm_controlStruct);
			generateMsg24Content(ASSIGNMSG,ais_staticDataStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel);//Msg24封装内容不区分自主消息还是指配消息
		}
	}
	if (fsm_controlStruct->msg18AddCount.countFlag == COUNTED)//增加的消息18的报告间隔到,应该是在指配模式的
	{
		fsm_controlStruct->msg18AddCount.countFlag = COUNTING;
		//产生增加的消息18的内容
		alterChannel = CHANNELA;//channelA为主信道
		generateMsg18Content(ASSIGNMSG,gps_infoStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel);//rcvVDLMsg15结构体在指配模式下未使用
	}
}

/************************************************************************
* Name      : rstFSMFrameStruct
* Brief     : 清零fsm_frameStruct结构体内容
*
* Author    : Digital Design Team
* Param     : fsm_frameStruct-输入/输出，用于存放待组帧封装消息内容结构体
* Return    : void
************************************************************************/
void rstFSMFrameStruct(FSM_FrameStruct * fsm_frameStruct)
{
	u8 i;
	for (i = 0; i < (fsm_frameStruct->encapDataLen/8); i ++)
	{
		fsm_frameStruct->encapDataByte[i] = 0;
	}
	fsm_frameStruct->encapDataLen = 0;
	fsm_frameStruct->broadChannel = 0;
	fsm_frameStruct->generateSlotPeriod = 0;
	fsm_frameStruct->msgState = 0;
	fsm_frameStruct->msgType = 0;
	fsm_frameStruct->rcvSlot = 0;
	
}

/************************************************************************
* Name      : rstFSMFrame19Struct
* Brief     : 清零fsm_frame19Struct结构体内容
*
* Author    : Digital Design Team
* Param     : fsm_frame19Struct-输入/输出，用于存放消息19待组帧封装消息内容结构体
* Return    : void
************************************************************************/
void rstFSMFrame19Struct(FSM_Frame19Struct * fsm_frame19Struct)
{
	u16 i;
	for (i = 0; i < (fsm_frame19Struct->encapDataLen/8); i ++)
	{
		fsm_frame19Struct->encapDataByte[i] = 0;
	}
	fsm_frame19Struct->encapDataLen = 0;
	fsm_frame19Struct->broadChannel = 0;
	fsm_frame19Struct->generateSlotPeriod = 0;
	fsm_frame19Struct->msgState = 0;
	fsm_frame19Struct->msgType = 0;
	fsm_frame19Struct->rcvSlot = 0;
	
}

/************************************************************************
* Name      : generateMsg14encapData
* Brief     : 用于生成消息14的封装数据内容
*
* Author    : Digital Design Team
* Param     : ais_bbmMsgStruct- 输入，上位机接收的安全相关的内容
* Param     : fsm_frameStruct- 输出，输出的消息14的待组帧结构体
* Return    : void
************************************************************************/
void generateMsg14encapData(AIS_BBMMsgStruct * ais_bbmMsgStruct,FSM_FrameStruct * fsm_frameStruct)
{
	u8 i,j;
	u32 tmp = 0;
	u16 lenBias = 0;
	u8 fieldNum = 1;
	u8 sub = 0;
	u8 disp = 0;
	_Bool * longTmp  = NULL;
	while(fieldNum < 6)
	{
		switch(fieldNum)
		{
			case 1: tmp = 14;i = 6;break;					//消息ID
			case 2: tmp = 0;i = 2;break;					//转发指示
			case 3: tmp = OWNMMSI; i = 30;break;				//MMSI 
			case 4: tmp = 0; i = 2;break;					//备用
			case 5: longTmp = ais_bbmMsgStruct->encapsulatedData;
				    i = ais_bbmMsgStruct->encapsulatedDataLength;break;//安全封装消息
		}
		lenBias = fsm_frameStruct->encapDataLen;
		j = 0;
		if (fieldNum != 5)
		{
			while(j < i)//对相应的长比特数据信息进行操作
			{
				sub = (lenBias+i-1-j) / 8;
				disp = 7 - (lenBias+i-1-j) % 8;
				if ((tmp>>j)&0x01)
				{
					//fsm_frameStruct->encapData[lenBias+i-1-j] = 1;
					fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
				}
				//else
					//fsm_frameStruct->encapData[lenBias+i-1-j] = 0;
				fsm_frameStruct->encapDataLen ++;
				j ++;
			}
		}
		else //长比特传输
		{
			while (j < i)  //encapData数组中低位先放数据，高位再放数据
			{
				//fsm_frameStruct->encapData[lenBias+j] = *(longTmp + j);
				sub = (lenBias+j) / 8;
				disp = 7 - (lenBias+j) % 8;
				if (*(longTmp+j))
				{
					fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
				}
				fsm_frameStruct->encapDataLen ++;
				j ++;
			}
		}
		
		fieldNum ++;
	}
}

/************************************************************************
* Name      : generateMsg14Content
* Brief     : 产生消息14的相应内容，并存放至输出结构体的相应位置
*
* Author    : Digital Design Team
* Param     : ais_bbmMsgStruct- 输入，VDL消息14的相关内容
* Param     : fsm_dataStruct- 输出，输出消息结构体的相应内容
* Return    : void
************************************************************************/
void generateMsg14Content(AIS_BBMMsgStruct * ais_bbmMsgStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 i = 0;
	u8 k = 0;
	while (fsm_dataStruct->fsm_frameStruct[i].msgState ==MSGNEW)//找到一个最新可写的结构体
	{
		i++;
	}
	if (i < FRAMELENGTH)//存在可以覆盖的组帧结构
	{
		rstFSMFrameStruct(&(fsm_dataStruct->fsm_frameStruct[i]));
		//生成Msg14的封装数据内容
		generateMsg14encapData(ais_bbmMsgStruct,&fsm_dataStruct->fsm_frameStruct[i]);
		/*while (fsm_dataStruct->fsm_frameStruct[i].encapDataLen < ais_bbmMsgStruct->encapsulatedDataLength)
		{
			fsm_dataStruct->fsm_frameStruct[i].encapData[fsm_dataStruct->fsm_frameStruct[i].encapDataLen] = ais_bbmMsgStruct->encapsulatedData[fsm_dataStruct->fsm_frameStruct[i].encapDataLen];
			fsm_dataStruct->fsm_frameStruct[i].encapDataLen ++;
		}*/
		//Msg14有可能指定了发送信道，需判断  0-channelA 1-channelB 2-channelA and channelB 3-无
		k = ais_bbmMsgStruct->broadcastChannel;
		switch(k)
		{
		case 0:fsm_dataStruct->fsm_frameStruct[i].broadChannel = CHANNELA;break;
		case 1:fsm_dataStruct->fsm_frameStruct[i].broadChannel = CHANNELB;break;
		case 2:fsm_dataStruct->fsm_frameStruct[i].broadChannel = getAlterChannel(fsm_controlStruct.alterChannelNum);break;
		default:break;
		}
		fsm_dataStruct->fsm_frameStruct[i].msgType = SAFEMSG;
		fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 4;
		fsm_dataStruct->fsm_frameStruct[i].rcvSlot = ais_bbmMsgStruct->rcvSlot;//????为什么不能赋值过去
		fsm_dataStruct->fsm_frameStruct[i].msgState = MSGNEW;
		testGenerateMsg14Content(ais_bbmMsgStruct,fsm_dataStruct,i);
	}
}

/************************************************************************
* Name      : generateMsg18encapData
* Brief     : 产生消息18的封装数据内容
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，GPS动态消息
* Param     : fsm_frameStruct- 输出，保存GPS的消息内容到相应的封装数组中，并记录比特数据的长度
* Return    : void
************************************************************************/
void generateMsg18encapData(GPS_InfoStruct * gps_infoStruct,FSM_FrameStruct * fsm_frameStruct)
{
	u8 i,j;
	u32 tmp = 0;
	u8 lenBias = 0;
	u8 fieldNum = 1;
	u8 sub = 0;
	u8 disp = 0;
	while(fieldNum < 22)
	{
		switch(fieldNum)
		{
		case 1: tmp = 18;i = 6;break;					//消息ID
		case 2: tmp = 0;i = 2;break;					//转发指示
		case 3: tmp = OWNMMSI; i = 30;break;				//MMSI 
		case 4: tmp = 0; i = 8;break;					//备用
		case 5: tmp = gps_infoStruct->SOG; i = 10;break;//地面航速
		case 6: tmp = gps_infoStruct->posAccurateFlag;i = 1;break;		//位置准确度
		case 7: tmp = gps_infoStruct->longitude; i = 28;break;			//经度
		case 8: tmp = gps_infoStruct->latitude; i = 27;break;			//纬度
		case 9: tmp = gps_infoStruct->COG; i = 12;break;				//地面航线
		case 10: tmp = gps_infoStruct->HOG; i = 9;break;				//实际航向
		case 11: tmp = gps_infoStruct->utctime[3]; i = 6;break;			//时戳 //记录秒
		case 12: tmp = 0; i = 2;break;									//备用
		case 13: tmp = 1,i = 1;break;									//0-B类SOTDMA装置，1-CS装置
		case 14: tmp = gps_infoStruct->monitorFlag; i = 1;break;		//1-装备有显示消息12和14的集成显示器，0-没有
		case 15: tmp = gps_infoStruct->DSCFlag; i = 1;break;			//1-装备了DSC功能，0-没有
		case 16: tmp = gps_infoStruct->broadBandFlag; i = 1;break;		//0-可以超出船用频带上限525KHz工作
		case 17: tmp = gps_infoStruct->msg22Flag; i = 1;break;			//1-经消息22进行频率管理
		case 18: tmp = 0; i = 1;break;									//0-台站工作在自主和连续模式，1-指配模式
		case 19: tmp = gps_infoStruct->raimFlag; i = 1;break;			//0-未使用电子定位装置，1-RAIM正使用
		case 20: tmp = 1; i = 1;break;									//1-B类“CS”通信状态选择器
		case 21: tmp = 0x60006; i = 19;break;							//通信状态：“CS”类固定为110000 000000 000011 0
		default:;
		}
		lenBias = fsm_frameStruct->encapDataLen;
		j = 0;
		while(j < i)//对相应的长比特数据信息进行操作
		{
			sub = (lenBias+i-1-j) / 8;
			disp = 7 - (lenBias+i-1-j) % 8;
			if ((tmp>>j)&0x01)
				fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
				//fsm_frameStruct->encapData[lenBias+i-1-j] = 1;
			//else
				//fsm_frameStruct->encapData[lenBias+i-1-j] = 0;
			fsm_frameStruct->encapDataLen ++;
			j ++;
		}
		fieldNum ++;

	}

}

/************************************************************************
* Name      : generateMsg18Content
* Brief     : 产生消息18的内容，并存放值输出结构体的相应位置
*
* Author    : Digital Design Team
* Param     : msgType- 输入，指示当前生成Msg18的模式，ASSIGNMSG-指配,AUTONOMSG-自主,INTERROMSG-询问,ASSIGNADDMSG18-切换区，A22MSG18-Msg22指配命令下产生Msg18
* Param     : gps_infoStruct- 输入，GPS动态消息
* Param     : fsm_dataStruct- 输出，输出结构体的相应内容
* Param     : rcvVDLMsg15- 输入，vdl 消息15的内容，当时询问模式下产生的消息18，则需要使用相关的信道和响应时隙信息
* Param     : alterChannel- 输入，用于指示在没有指定信道时，使用交替的信道作为发射信道
* Return    : void
************************************************************************/
void generateMsg18Content(MsgTypeIndicator msgType,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel)
{
	u8 i = 0;
	while (fsm_dataStruct->fsm_frameStruct[i].msgState != 0 && 
		   fsm_dataStruct->fsm_frameStruct[i].msgState != MSGOLD)//找到一个最新可写的结构体
	{
		i++;
	}
	if (i < FRAMELENGTH)//存在可以覆盖的组帧结构
	{
		rstFSMFrameStruct(&(fsm_dataStruct->fsm_frameStruct[i]));
		//添加消息18的封装消息内容
		generateMsg18encapData(gps_infoStruct,&(fsm_dataStruct->fsm_frameStruct[i])); 
		if (msgType == INTERROMSG)//询问
		{
			fsm_dataStruct->fsm_frameStruct[i].broadChannel = rcvVDLMsg15->rcvChannel;
			fsm_dataStruct->fsm_frameStruct[i].msgType = msgType;
			fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 0;
			if ((rcvVDLMsg15->slotOffset1_1 >= 10) && (rcvVDLMsg15->msgID1_1 == 18))//设计偏置至少为10
			{
				fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset1_1)%2250;
			}
			else if((rcvVDLMsg15->slotOffset1_2 >= 10) && (rcvVDLMsg15->msgID1_2 == 18))
			{
				fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset1_2)%2250;
			}
			else if ((rcvVDLMsg15->slotOffset2_1 >= 10) && (rcvVDLMsg15->msgID2_1 == 18))
			{
				fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset2_1)%2250;
			}
			else//若未指定响应时隙
			{
				fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 30;
				fsm_dataStruct->fsm_frameStruct[i].rcvSlot = rcvVDLMsg15->rcvSlot;
			}
		}
		else if (msgType == AUTONOMSG || (msgType&0xF0) == ASSIGNMSG)//指配或自主或切换区
		{
			fsm_dataStruct->fsm_frameStruct[i].broadChannel = alterChannel;//自主生成信道
			fsm_dataStruct->fsm_frameStruct[i].msgType = (msgType&0xF0);
			fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 10;//报告周期的1/3与10两者之间的小值   to be changed
			fsm_dataStruct->fsm_frameStruct[i].rcvSlot = fsm_dataStruct->realSlot;
			//若是指配模式，则还需要指定封装消息中的模式标志,若是Msg22的指配消息，还需要修改B类带宽标志和B类Msg22标志
			if (ASSIGNMSG == (msgType&0xF0))
			{
				//fsm_dataStruct->fsm_frameStruct[i].encapData[146] = 1; //修改为指配模式
				fsm_dataStruct->fsm_frameStruct[i].encapDataByte[18] |= 0x20; //修改为指配模式
			}
			if (A22MSG18 == msgType)
			{
				//fsm_dataStruct->fsm_frameStruct[i].encapData[145] = 1; //指配模式下，与Msg22相关的位，先对可能修改的位进行处理
				fsm_dataStruct->fsm_frameStruct[i].encapDataByte[18] |= 0x40; //指配模式下，与Msg22相关的位，先对可能修改的位进行处理
				//fsm_dataStruct->fsm_frameStruct[i].encapData[144] = 1;
			}
		}
		fsm_dataStruct->fsm_frameStruct[i].msgState = MSGNEW;
		//测试内容，可注解
		testGenerateMsg18Content(msgType,gps_infoStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel,i);
		
	}
}

/************************************************************************
* Name      : generateMsg19encapData
* Brief     : 用于生成消息19的封装数据内容
*
* Author    : Digital Design Team
* Param     : gps_infoStruct- 输入，gps端的动态数据内容结构体
* Param     : ais_staticDataStruct- 输入，上位机端的静态数据内容结构体
* Param     : fsm_frame19Struct- 输出，输出的消息19的待组帧结构体
* Return    : void
************************************************************************/
void generateMsg19encapData(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,FSM_Frame19Struct * fsm_frame19Struct)
{
	u8 i,j;
	u32 tmp = 0;
	u16 lenBias = 0;
	u8 fieldNum = 1;
	u8 sub = 0;
	u8 disp = 0;
	_Bool * longTmp  = NULL;
	while(fieldNum < 21)
	{
		switch(fieldNum)
		{
			case 1: tmp = 19;i = 6;break;					//消息ID
			case 2: tmp = 0;i = 2;break;					//转发指示
			case 3: tmp = OWNMMSI; i = 30;break;				//MMSI 
			case 4: tmp = 0; i = 8;break;					//备用
			case 5: tmp = gps_infoStruct->SOG; i = 10;break;//地面航速
			case 6: tmp = gps_infoStruct->posAccurateFlag;i = 1;break;		//位置准确度
			case 7: tmp = gps_infoStruct->longitude; i = 28;break;			//经度
			case 8: tmp = gps_infoStruct->latitude; i = 27;break;			//纬度
			case 9: tmp = gps_infoStruct->COG; i = 12;break;				//地面航线
			case 10: tmp = gps_infoStruct->HOG; i = 9;break;				//实际航向
			case 11: tmp = gps_infoStruct->utctime[4]; i = 6;break;			//时戳
			case 12: tmp = 0; i = 4;break;									//备用
			case 13: longTmp = ais_staticDataStruct->shipName,i = 120;break;//船名
			case 14: tmp = ais_staticDataStruct->shipType; i = 8;break;		//船舶类型或货物类型
			case 15: tmp = ais_staticDataStruct->shipSize; i = 30;break;	//船舶尺寸/参考位置
			case 16: tmp = 1; i = 4;break;									//电子定位装置类型
			case 17: tmp = gps_infoStruct->raimFlag; i = 1;;break;			//0-未使用电子定位装置，1-RAIM正使用
			case 18: tmp = 1; i = 1;break;									//DTE 数据终端就绪 0-可用，1-不可用，默认值
			case 19: tmp = 0; i = 1;break;									//0-台站工作在自主和连续模式，1-指配模式
			case 20: tmp = 0; i = 4;break;									//备用
		
		}
		lenBias = fsm_frame19Struct->encapDataLen;
		j = 0;
		if (fieldNum != 13)
		{
			while(j < i)//对相应的长比特数据信息进行操作
			{
				sub = (lenBias+i-1-j) / 8;
				disp = 7 - (lenBias+i-1-j) % 8;
				if ((tmp>>j)&0x01)
					fsm_frame19Struct->encapDataByte[sub] |= (0x01 << disp);
					/*fsm_frame19Struct->encapData[lenBias+i-1-j] = 1;
				else
					fsm_frame19Struct->encapData[lenBias+i-1-j] = 0;*/
				fsm_frame19Struct->encapDataLen ++;
				j ++;
			}
		}
		else //长比特传输
		{
			while (j < i)  //encapData数组中低位先放数据，高位再放数据
			{

				sub = (lenBias+j) / 8;
				disp = 7 - (lenBias+j) % 8;
				if (*(longTmp+j))
				{
					fsm_frame19Struct->encapDataByte[sub] |= (0x01 << disp);
				}
				//fsm_frame19Struct->encapData[lenBias+j] = *(longTmp + i-1-j);
				fsm_frame19Struct->encapDataLen ++;
				j ++;
			}
		}
		
		fieldNum ++;
	}
}

/************************************************************************
* Name      : generateMsg19Content
* Brief     : 用于生成消息19的数据内容 
*
* Author    : Digital Design Team
* Param     : msgType-输入，消息类型
* Param     : ais_staticDataStruct-输入，上位机端的静态数据内容结构体
* Param     : gps_infoStruct-输入，gps端的动态数据内容结构体
* Param     : fsm_dataStruct-输出，输出的待组帧的结构体
* Param     : vdlMsg15-输入，GPGA处接收到的询问消息结构体
* Param     : alterChannel-输入，交替发送的信道指示
* Return    : void
* Attention : CS-B类 Msg19的询问只能当基站给出了偏置（最少为10），才会响应（生成相应的内容），否则不响应
************************************************************************/
void generateMsg19Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * vdlMsg15,_Bool alterChannel)
{
	if (fsm_dataStruct->fsm_frame19Struct.msgState == 0 ||
		fsm_dataStruct->fsm_frame19Struct.msgState == MSGOLD)//之前没有存放过消息，或消息已经处理过了，询问消息响应已经发送成功了
	{
		rstFSMFrame19Struct(&(fsm_dataStruct->fsm_frame19Struct));
		generateMsg19encapData(gps_infoStruct,ais_staticDataStruct,&(fsm_dataStruct->fsm_frame19Struct));
		if (ASSIGNMSG == msgType)//询问模式下
		{
			fsm_dataStruct->fsm_frame19Struct.broadChannel = vdlMsg15->rcvChannel;
			fsm_dataStruct->fsm_frame19Struct.generateSlotPeriod = 0;//认为对消息19的询问是指定了响应时隙号的,实际时隙号=接收消息15的时隙号+消息15给出的时隙偏置
			if (vdlMsg15->msgID1_1 == 19 && OWNMMSI == vdlMsg15->dstMMSI1 && vdlMsg15->slotOffset1_1 >= 10)
				fsm_dataStruct->fsm_frame19Struct.rcvSlot = (vdlMsg15->slotOffset1_1 + vdlMsg15->rcvSlot) % 2250;
			else if(vdlMsg15->msgID1_2 == 19 && OWNMMSI == vdlMsg15->dstMMSI1 && vdlMsg15->slotOffset1_2 >= 10)
				fsm_dataStruct->fsm_frame19Struct.rcvSlot = (vdlMsg15->slotOffset1_2 + vdlMsg15->rcvSlot) % 2250;
			else if(vdlMsg15->msgID2_1 == 19 && OWNMMSI == vdlMsg15->dstMMSI2 && vdlMsg15->slotOffset2_1 >= 10)
				fsm_dataStruct->fsm_frame19Struct.rcvSlot = (vdlMsg15->slotOffset2_1 + vdlMsg15->rcvSlot) % 2250;
			fsm_dataStruct->fsm_frame19Struct.msgType = msgType;
		}
		
		//else 其他条件，待添加 自主和指配不会发送该消息

		fsm_dataStruct->fsm_frame19Struct.msgState = MSGNEW;
	}
	else if (fsm_dataStruct->fsm_frame19Struct.msgState == MSGSENDFAILD)//消息已经处理过了，询问消息响应失败，则需要重新设置相应内容，准备再次发送
	{
		//to be added
		//消息19一般指定了时隙偏置，还需要自己添加重新响应的时隙么？？？
		fsm_dataStruct->fsm_frame19Struct.msgState = MSGNEW;
	}
	//测试消息19，可注解
	testGenerateMsg19Content(msgType,ais_staticDataStruct,gps_infoStruct,fsm_dataStruct,vdlMsg15,0);
}

/************************************************************************
* Name      : generateMsg24AencapData
* Brief     : 用于生成消息24A的封装数据内容
*
* Author    : Digital Design Team
* Param     : ais_staticDataStruct- 输入，上位机串口接收到的静态数据
* Param     : fsm_frameStruct- 输出，保存静态数据内容到相应的封装数组中，并记录比特数据的长度
* Return    : void
************************************************************************/
void generateMsg24AencapData(AIS_StaticDataStruct * ais_staticDataStruct, FSM_FrameStruct * fsm_frameStruct)
{
	u8 i,j;
	u32 tmp = 0;
	u8 sub = 0;
	u8 disp = 0;
	_Bool * longTmp = NULL;
	u8 lenBias = 0;
	u8 fieldNum = 1;
	while(fieldNum < 6)
	{
		switch(fieldNum)
		{
			case 1: tmp = 24;i = 6;break;					//消息ID
			case 2: tmp = 0;i = 2;break;					//转发指示
			case 3: tmp = OWNMMSI; i = 30;break;				//MMSI 
			case 4: tmp = 0; i = 2;break;					//A部分标号 0-A部分，1-B部分
			case 5: longTmp = ais_staticDataStruct->shipName; i =120;break;	//船名 120bit，怎么分配合适了

			default:;
		}
		lenBias = fsm_frameStruct->encapDataLen;
		j = 0;
		if (fieldNum != 5)
		{
			while(j < i)//对相应的长比特数据信息进行操作
			{
				sub = (lenBias+i-1-j) / 8;
				disp = 7 - (lenBias+i-1-j) % 8;
				if ((tmp>>j)&0x01)
					fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
					/*fsm_frameStruct->encapData[lenBias+i-1-j] = 1;
				else
					fsm_frameStruct->encapData[lenBias+i-1-j] = 0;*/
				fsm_frameStruct->encapDataLen ++;
				j ++;
			}
		}
		else
		{
			while (j < i)  //encapData数组中低位先放数据，高位再放数据
			{
				sub = (lenBias+j) / 8;
				disp = 7 - (lenBias+j) % 8;
				if (*(longTmp+j))
				{
					fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
				}
				//fsm_frameStruct->encapData[lenBias+j] = *(longTmp + i-1-j);
				fsm_frameStruct->encapDataLen ++;
				j ++;
			}
		}
		
		fieldNum ++;
	}
}

/************************************************************************
* Name      : generateMsg24Content
* Brief     : 生成消息24的待组帧内容
*
* Author    : Digital Design Team
* Param     : msgType- 输入 消息类型 INTERROMSG  AUTONOMSG ASSIGNMSG
* Param     : ais_staticDataStruct- 输入 上位机静态信息结构
* Param     : fsm_dataStruct- 输出 待组帧的信息结构
* Param     : rcvVDLMsg15- 输入 FPGA接收的FPGA消息结构，在询问模式使用
* Param     : alterChannel- 输入 自主模式下交替信道指示 0-channelA 1-channelB
* Return    : void
************************************************************************/
void generateMsg24Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel)
{
	u8 i = 0;
	u8 j = 2;
	while (j>0)
	{
		j --;
		while (fsm_dataStruct->fsm_frameStruct[i].msgState != 0 && 
			fsm_dataStruct->fsm_frameStruct[i].msgState != MSGOLD)//找到一个最新可写的结构体
		{
			i++;
		}
		if (i < FRAMELENGTH)//存在可以覆盖的组帧结构
		{
			rstFSMFrameStruct(&(fsm_dataStruct->fsm_frameStruct[i]));
			if (j == 1)//生成24A部分内容
			{
				generateMsg24AencapData(ais_staticDataStruct,&(fsm_dataStruct->fsm_frameStruct[i]));
				if (msgType == INTERROMSG)//询问模式
				{
					//若指定了响应时隙
					fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 0;
					if (rcvVDLMsg15->msgID1_1 == 24 && rcvVDLMsg15->slotOffset1_1 >= 10 && rcvVDLMsg15->dstMMSI1 == OWNMMSI)
					{
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset1_1) % 2250;
					}
					else if (rcvVDLMsg15->msgID1_2 == 24 && rcvVDLMsg15->slotOffset1_2 >= 10 && rcvVDLMsg15->dstMMSI1 == OWNMMSI)
					{
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset1_2) % 2250;
					}
					else if (rcvVDLMsg15->msgID2_1 == 24 && rcvVDLMsg15->slotOffset2_1 >= 10 && rcvVDLMsg15->dstMMSI2 == OWNMMSI)
					{
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset2_1) % 2250;
					}
					else//没有指定响应时隙
					{
						fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 30;
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = rcvVDLMsg15->rcvSlot;
					}
					fsm_dataStruct->fsm_frameStruct[i].broadChannel = rcvVDLMsg15->rcvChannel;//在接收到的信道上响应
					fsm_dataStruct->fsm_frameStruct[i].msgType = msgType;  
				}
				else//自主或指配模式
				{
					fsm_dataStruct->fsm_frameStruct[i].broadChannel = alterChannel;
					fsm_dataStruct->fsm_frameStruct[i].msgType = msgType;
					fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 10;   //选择报告间隔/3和10中的小值
					fsm_dataStruct->fsm_frameStruct[i].rcvSlot =  fsm_dataStruct->realSlot;  //选择当前时隙

				}
				
			}
			else//生成24B部分内容
			{
				generateMsg24BencapData(ais_staticDataStruct,&(fsm_dataStruct->fsm_frameStruct[i]));
				if (msgType == INTERROMSG)//询问模式
				{
					//若指定了响应时隙
					fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 0;
					if (rcvVDLMsg15->msgID1_1 == 24 && rcvVDLMsg15->slotOffset1_1 >= 10 && rcvVDLMsg15->dstMMSI1 == OWNMMSI)
					{
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset1_1 + 1125) % 2250;
					}
					else if (rcvVDLMsg15->msgID1_2 == 24 && rcvVDLMsg15->slotOffset1_2 >= 10 && rcvVDLMsg15->dstMMSI1 == OWNMMSI)
					{
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset1_2 + 1125) % 2250;
					}
					else if (rcvVDLMsg15->msgID2_1 == 24 && rcvVDLMsg15->slotOffset2_1 >= 10 && rcvVDLMsg15->dstMMSI2 == OWNMMSI)
					{
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + rcvVDLMsg15->slotOffset2_1 + 1125) % 2250;
					}
					else//没有指定响应时隙
					{
						fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 30;
						fsm_dataStruct->fsm_frameStruct[i].rcvSlot = (rcvVDLMsg15->rcvSlot + (1125))%2250; //设置时隙为A部分候选时隙号之后
					}
					fsm_dataStruct->fsm_frameStruct[i].broadChannel = rcvVDLMsg15->rcvChannel;//在接收到的信道上响应
					fsm_dataStruct->fsm_frameStruct[i].msgType = msgType;
				}
				else//自主或指配模式
				{
					fsm_dataStruct->fsm_frameStruct[i].broadChannel = alterChannel;
					fsm_dataStruct->fsm_frameStruct[i].msgType = msgType;
					fsm_dataStruct->fsm_frameStruct[i].generateSlotPeriod = 10;   //选择报告间隔/3和10中的小值
					fsm_dataStruct->fsm_frameStruct[i].rcvSlot =  (fsm_dataStruct->realSlot + 375)%2250;  //设置时隙为B部分候选时隙号之后
				}
			}
			//msgType 视情况看要不要调整到一起写
			fsm_dataStruct->fsm_frameStruct[i].msgState = MSGNEW;
			//测试内容，可以注解
			//j == 1 测试A，j == 0 测试B
			testgenerateMsg24Content(msgType,ais_staticDataStruct,fsm_dataStruct,rcvVDLMsg15,alterChannel,i,j);
		}
	}
	
}

/************************************************************************
* Name      : generateMsg24BencapData
* Brief     : 用于生成消息24B的封装数据内容
*
* Author    : Digital Design Team
* Param     : ais_staticDataStruct- 输入，上位机串口接收到的静态数据
* Param     : fsm_frameStruct- 输出，保存静态数据内容到相应的封装数组中，并记录比特数据的长度
* Return    : void
************************************************************************/
void generateMsg24BencapData(AIS_StaticDataStruct * ais_staticDataStruct, FSM_FrameStruct * fsm_frameStruct)
{
	u8 i,j;
	u32 tmp = 0;
	u8 sub = 0;
	u8 disp = 0;
	_Bool * longTmp = NULL;
	u8 lenBias = 0;
	u8 fieldNum = 1;
	while(fieldNum < 10)
	{
		switch(fieldNum)
		{
		case 1: tmp = 24;i = 6;break;								//消息ID
		case 2: tmp = 0;i = 2;break;								//转发指示
		case 3: tmp = OWNMMSI; i = 30;break;							//MMSI 
		case 4: tmp = 1; i = 2;break;								//A部分标号 0-A部分，1-B部分
		case 5: tmp = ais_staticDataStruct->shipType; i =8;break;	//船舶类型和货物类型
		case 6: longTmp = ais_staticDataStruct->supplier;i = 42;break;		//供应商   
		case 7: longTmp = ais_staticDataStruct->shipCall; i = 42;break;		//呼号		
		case 8: tmp = ais_staticDataStruct->shipSize; i = 30;break;	//船舶大小/位置参考
		case 9: tmp = 0; i =6;break;								//备用
		default:;
		}
		lenBias = fsm_frameStruct->encapDataLen;
		j = 0;
		if (fieldNum != 6 && fieldNum != 7)
		{
			while(j < i)//对相应的长比特数据信息进行操作
			{
				sub = (lenBias+i-1-j) / 8;
				disp = 7 - (lenBias+i-1-j) % 8;
				if ((tmp>>j)&0x01)
					fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
					/*fsm_frameStruct->encapData[lenBias+i-1-j] = 1;
				else
					fsm_frameStruct->encapData[lenBias+i-1-j] = 0;*/
				fsm_frameStruct->encapDataLen ++;
				j ++;
			}
		}
		else
		{
			while (j < i)  //encapData数组中低位先放数据，高位再放数据
			{
				sub = (lenBias+j) / 8;
				disp = 7 - (lenBias+j) % 8;
				if (*(longTmp+j))
				{
					fsm_frameStruct->encapDataByte[sub] |= (0x01 << disp);
				}
				//fsm_frameStruct->encapData[lenBias+j] = *(longTmp + i-1-j);
				fsm_frameStruct->encapDataLen ++;
				j ++;
			}
		}
		fieldNum ++;
	}
}



////////////////////////////////////////for test////////////////////////////////////////////////////////////
//void initFsm_dataStruct(FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct)
//{
//	fsm_dataStruct->realSlot = 1125;
//
//	fsm_controlStruct->msg18Count.countFlag = COUNTED;
//
//	
//}
