/*********************************************************************
* @file      UART2.h
* @author    Embedded Team
* @version   V0.0.0
* @date      2015/12/21   13:14
********************************************************************
* @brief     定义gps串口和状态机之间的数据结构体和功能函数的接口
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
**********************************************************************/
#ifndef  UART2_H
#define  UART2_H
#include "stm32f4xx.h" 
#include "stdio.h"
#include "myStdDef.h"
#include "AIS_PS_Struct.h"

#define Uart2_GPS1_LEN     1024 
#define Uart2_GPS2_LEN     1024 
extern uint8_t Uart2_GPS1[Uart2_GPS1_LEN];//UART1����
extern uint8_t Uart2_GPS2[Uart2_GPS2_LEN];//UART1ޓ˕

//u8 Flag_Uart_Send = 0;


/**********************************************************************/
#define MAXRCVBUFFERLENGTH 			 150
/******************************struct define**********************************/
typedef struct
{
	
	u8 receivingFlag;					//开始接收串行数据标志位
	u8 receivedFlag;					//接收完成一帧数据标志位
	u8 rcvBuffer[150];				//接收数据缓存
	u8 rcvBufferLength;				//接收数据长度
	u8 receiveWrong;					//接收数据出错，现在判断的一个方式是接收数据长度超过最大值
	
}GPS_FrameStruct;

typedef struct
{
	u32 longitude;						//经度，实际长度28bit,1/10000min为单位 ，与获取的 
	_Bool longitudeDir;					//经度方向，E-0，W-1
	u32 latitude;						//纬度，实际长度27bit,1/10000min为单位
	_Bool latitudeDir;					//纬度方向，N-0，S-1
	u32  utcTime;							//utc时间，格式为hhmmssss形式
	u8 utctime[5];							//utc时间，格式为mmddhhmmss(月天时分秒)
	u16 SOG;									//地面航速，实际长度10bit,1/10为步长，有效范围0-102.2节1023不可用，1022=102.2节或更快
	u16 COG;									//地面航线，实际长度12bit,1/10°为步长，有效范围0-3599，3600=e10h不可用，3601~4095应不采用
	u16 HOG;								//实际航向，实际长度为9bit,GPS 中没有提供，需要自己计算获得
	_Bool raimFlag;						//电子定位装置，0-未使用，1-正在使用
	_Bool deviceFlag;						//装置标志位   0-B类SOTDMA装置，1-CS装置;
	_Bool monitorFlag;						//显示器标志位  1-装备有显示消息12和14的集成显示器，0-没有
	_Bool broadBandFlag;					//宽带标志位  0-可以超出船用频带上限525KHz工作
	_Bool DSCFlag;							//DSC标志位   1-装备了DSC功能，0-没有
	_Bool msg22Flag;						//消息22进行频率管理标志位  1-经消息22进行频率管理
	_Bool modeFlag;							//模式标志位 0-台站工作在自主和连续模式，1-指配模式
	_Bool commuFlag;						//通信选择  1-B类“CS”
	_Bool posAccurateFlag;					//位置准确度 1-高，0-低

}GPS_InfoStruct;//用于存放经过算法修正的经纬度、航向角等值

typedef struct
{
	u32 utcTime;							//utc时间，格式为hhmmssss形式
	u8 status;								//状态，V-导航接收机报警，A-数据可用
	u16 latitudeH;						//纬度高位，1/10000min为单位
	u32 latitudeL;						//纬度低位,不固定长度为，最大为5位
	u8 latitudeDir;						//纬度方向，N-0/S-1
	u16 longitudeH;						//经度高位，1/10000min为单位
	u32 longitudeL;						//经度低位
	u8 longitudeDir;					//经度方向，E-0/W-1
	u16 sog;									//地面航速，实际长度10bit,1/10为步长，有效范围0-102.2节1023不可用，1022=102.2节或更快
	u16 cog;									//地面航线，实际长度12bit,1/10°为步长，有效范围0-3599，3600=e10h不可用，3601~4095应不采用
	u32 date;									//日期，格式为dd/mm/yy
	u8 modeIndicator;					//模式指示器，A-自主，D-差分，E-估计，M-人工输入，S-仿真，N-数据不可用
	u16 magneticVar;					//磁变化
	u8 magneticVarDir;				//磁变化方向，E/W
	
}GPS_RMCMsgStruct;

typedef struct
{
	u16 COGT;									//真实角度的地面航线，以1/10º为单位=（0-3 599）。3 600 (E10h)= 不可用=默认值
	u16 COGM;									//磁角度的地面航线，以1/10º为单位=（0-3 599）。3 600 (E10h)= 不可用=默认值
	u16 SOGN;									//节速度的地面航速，步长为1/10 节（0-102.2节）1 023= 不可用，1 022=102.2节或更快
	u16 SOGK;									//km/h速度的地面航速，步长为1/10 节（0-102.2节）1 023= 不可用，1 022=102.2节或更快
	u8 modeIndicator;					//模式指示，A-自主 D-差分 E-估计 M-人工输入 S-仿真 N-数据不可用
}GPS_VTGMsgStruct;

typedef struct
{
	u32 utcTime;							//utc时间，格式为hhmmssss形式
	u16 latitudeH;						//纬度高位，1/10000min为单位
	u32 latitudeL;						//纬度低位
	u8 latitudeDir;						//纬度方向，N/S
	u16 longitudeH;						//经度高位，1/10000min为单位
	u32 longitudeL;						//经度低位
	u8 longitudeDir;					//经度方向，E/W
	// to be added
}GPS_GGAMsgStruct;
/*
typedef struct
{
	
}GPS_GSAMsgStruct;

typedef struct
{
	
}GPS_GSVMsgStruct;

typedef struct
{
	
}GPS_GLLMsgStruct;
*/

/////////////////////////////////////////外部接口申明区//////////////////////////////////////////////////////
//extern GPS_FrameStruct gps_frameStruct;
extern GPS_InfoStruct gps_infoStruct;
extern GPS_RMCMsgStruct gps_RMCMsgStruct;
//extern GPS_VTGMsgStruct gps_VTGMsgStruct;

////////////////////////////////fuction 定义区 /////////////////////////////////////////////////////
//void rstGPSFrameStruct(GPS_FrameStruct * gps_frameStruct);
void rstGPSInfoStruct(GPS_InfoStruct *gps_infoStruct);
//void getGPSInfo(GPS_FrameStruct * gps_frameStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct,GPS_VTGMsgStruct * gps_VTGMsgStruct);
//u8 changeASCToDigital(u8 data);
//void getGPSRMCMsgInfo(GPS_FrameStruct * gps_frameStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct);
//void getGPSVTGMsgInfo(GPS_FrameStruct * gps_frameStruct,GPS_VTGMsgStruct * gps_VTGMsgStruct);
//void getGPSGGAMsgInfo(GPS_FrameStruct * gps_frameStruct,GPS_GGAMsgStruct * gps_GGAMsgStruct);

void updataGPSInfoStruct(GPS_InfoStruct * gps_infoStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct);
void Get_GPS_RMCMsg(u8 * buff,GPS_RMCMsgStruct * gps_RMCMsgStruct,u16 length);
void getGPSInfomation(u8 * buff,u16 length, GPS_InfoStruct * gps_infoStruct);

/////////////for test///////////////
void testGPS_InfoStruct(GPS_InfoStruct * gps_infoStruct);
void testGPS_InfoStructPara(GPS_InfoStruct * gps_infoStruct,u32 lati,u32 longi,u8 utchh,u8 utcmm,u8 utcss);
void UART2_Config(u32 bound);
void  Putc_UART2(u8 ch);


#endif
