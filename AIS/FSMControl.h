/**
********************************************************************
* @file      FSMControl.h
* @author    Digital Design Team
* @version   
* @date      2015/11/23   15:58
********************************************************************
* @brief     定义状态机处理过程中需要用到的数据结构体和函数体
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef __FSMControl_H
#define __FSMControl_H

#include "myStdDef.h"
#include "AIS_PS_Interface.h"
#include "mkdData.h"
#include "gps.h"

#include <time.h>
#include <math.h> 

////////////////////////////////////define 声明区////////////////////////////////////////////////////

#define FRAMELENGTH  10   //设置输出数据结构中，最大可以储存的待组帧消息数量 
#define RESERVEDSLOTLENGTH 5	//设置保留时隙结构中，最大可以存储的保留时隙的组数 ，现在设计最大可能改动的值不要超过8！

#define MSG22AREALENSMALL	20   //设置消息22中规定的区域的边界最小值，单位：海里
#define MSG22AREALENBIG		200  //设置消息22中规定的区域的边界最大值，单位：海里
#define MSG22AREADISTANCE	8	 //设置消息22中规定的相邻区域边界的距离，单位：海里

#define MSG20MAXDISTANCE	120	 //设置消息20中规定的船站离发送消息基站的最大有效距离，单位：海里，超过了该距离，则需清空保留时隙

#define PI  3.1415926

///////////////////////////////////enum 声明区//////////////////////////////////////////////////////
typedef enum{
	//0-不在指配模式，0x11-保留时隙有效,0x21-在Msg22指配区域内,0x22-在Msg22切换区域内,0x41-在Msg23指配区域内
	//0x42-在Msg23的寂静期内
	AUTO = 0,
	MSG20 = 0x11,
	MSG22 = 0x21,
	MSG22HANDOVER = 0x22,
	MSG23 = 0x41,
	MSG23SILENCE = 0x42 
}workTypeIndicator;


//////////////////////////////////struct 声明区/////////////////////////////////////////////////////

typedef struct{
	//_Bool encapData[256];//封装内容，按照相应封装消息的格式以比特的形式存放于封装数组中
	u8 encapDataByte[32];//封装内容，按照字节的形式进行存储
						 //封装内容按照协议内容顺序，按照数组下标从低到高，字节内部从高到低的顺序依次写入
	u8 encapDataLen;//封装内容长度  比特流长度
	BchannelIndicator broadChannel;//广播信道   
	u16 rcvSlot;//接收时隙号
	u8 generateSlotPeriod;//候选时隙时间长度，单位：秒
	MsgTypeIndicator msgType;//消息类型   
	MsgHandleState msgState;//消息处理的状态  

}FSM_FrameStruct;

typedef struct{
	//_Bool encapData[312];//封装内容 //以比特的形式存放于封装数组中
	u8 encapDataByte[39];//封装内容，按照字节的形式进行存储
						 //封装内容按照协议内容顺序，按照数组下标从低到高，字节内部从高到低的顺序依次写入
	u16 encapDataLen;//封装内容长度  比特流长度
	BchannelIndicator broadChannel;//广播信道   
	u16 rcvSlot;//接收时隙号
	u8 generateSlotPeriod;//候选时隙时间长度，单位：秒
	MsgTypeIndicator msgType;//消息类型   
	MsgHandleState msgState;//广播标志   

}FSM_Frame19Struct;

typedef struct{
	u16 countNum;				//报告间隔的计数值
	//SycleCount countFlag;
	CountIndicator  countFlag;	//当前计数情况的状态
}FSM_CountStruct;				//周期消息使用的计数结构体

typedef struct{
	FSM_CountStruct fsm_bStationCount;//基站保留时隙超时


}FSM_TimeCountStruct;//时间计时结构体

typedef struct{

	u16 slotNum ;				//保留的时隙号
	u8 continueSlotNum;			//从保留时隙号起，连续的保留号数
	u8 overTime[3];				//上述连续的一串时隙超时的时刻，格式为hhmmss(时分秒)，若时隙超时，则将overTime[0]置为24

}ReservedSlotStruct;//存放计算的保留时隙和对应的超时时间

typedef struct{

	u8 reservedSlotNum;					 //记录的保留时隙的组数
	ReservedSlotStruct rsvdSlotStruct[RESERVEDSLOTLENGTH];//最多保留5组保留时隙
	u32 bLongitude;						 //发送基站的经度
	u32 blatitude;						 //发送基站的纬度
	workTypeIndicator assignWorkType;	//用于存放指配消息的有效情况，取值为 AUTO  MSG20

}FSM_Msg20Struct;//存放Msg20中给出的保留时隙号、超时

typedef struct{

	double northEastLongitude;		//东北角经度
	double northEastLatitude;		//东北角纬度
	double southWestLongitude;		//西南角经度	
	double southWestLatitude;		//西南角纬度
	u8 transChannel;			//Tx/Rx模式
	//_Bool broadbandA;			//宽带A
	///_Bool broadbandB;			//宽带B
	u8 handOverArea;			//切换区范围，单位：海里，数值为实际的，如：4，则为4海里范围内为切换区
	//_Bool power;				//功率，0-大（默认值），1-小
	u8 overTime[4];				//区域超时时刻，25~31天，记录mmddhhmm(月天时分)
	
	u8 assignOverTime[3];		//记录该指配消息超时的时间，格式为hhmmss(时分秒)，认为可以同时存在多条指配有效消息20


}ChannelManageStruct;//用于记录单个指配信息

typedef struct{
	u8 groupManageBias;							//当前可供操作的群组指配信息数组偏置，在0-7之间循环
	ChannelManageStruct groupManageStruct[8];	//记录群组指配信息
	ChannelManageStruct addrManageStruct;		//记录寻址指配信息，其中经纬度为空
	//u8 assignOverTime[3];						//记录该指配消息超时的时间，格式为hhmmss(时分秒)
	//u8 assignValidBias;							//记录新接收指配信息存放于区域的位置，0~7对应群组区域位置，8对应寻址指配区域，9超时，没有新的指配区域   
												//暂时不用了！！！,修改可以可以同时存在多个有效的指配消息，其指配超时标志及时间添加到相应的区域或寻址结构体中
	u8 lastArea;				//记录上一个所在的指配区域 0 默认值，公海区域，1~8群组指配区域对应的0~7号区域，9 寻址指配
	u8 currentArea;				//记录当前指配区域，每次计算后实时更新，0默认值，公海区域，1~8群组指配区域对应的0~7号区域，9寻址指配
	workTypeIndicator assignWorkType;	//用于存放指配消息的有效情况，取值为 AUTO MSG20 MSG20HANDOVER

}FSM_Msg22Struct;//存放消息22中给出的区域信息，及相应的指配信息，最多包含8组群组信息，1组单独信息



typedef struct{

	double northEastLongitude;		//东北角经度
	double northEastLatitude;		//东北角纬度
	double southWestLongitude;		//西南角经度	
	double southWestLatitude;		//西南角纬度
	u8 silenceTime[3];			//寂静期结束时刻，格式hhmmss（时分秒）
	u16 reportTime;				//报告间隔,单位：秒,根据接收换算成相应的值
	u16 overTime[3];			//超时时刻 ，格式hhmmss（时分秒）,在第一次发送完成后的4~8min内选择一个随机数作为超时
	u8 transChannel;			//Tx/Rx模式
	_Bool transChannelFlag;		//群组发送信道有效标志,用于和Msg22规定的信道进行区别
	workTypeIndicator assignWorkType;	//用于存放指配消息的有效情况，取值为 AUTO MSG23 MSG23SILENCE

}FSM_Msg23Struct;//存放Msg23中给出的区域、移动台类型、船只类型、寂静期、报告间隔、超时标志

typedef struct{
	
	u8 controlMsgType[2][3];						//存放消息类型，第一行存放安全/询问类消息，第二行存放指配类消息
	u8 msgTypeLen1;									//记录第一行消息个数
	u8 msgTypeLen2;									//记录第二行消息个数
	AIS_BBMMsgStruct ais_bbmMsgStruct;
	FSM_Msg20Struct fsm_msg20Struct;					//用于记录消息20的相应数据
	FSM_Msg22Struct fsm_msg22Struct;					//用于记录消息22的相应数据
	FSM_Msg23Struct fsm_msg23Struct;					//用于记录消息23的相应数据
	FSM_CountStruct msg18Count;							//用于记录消息18的周期计数
	FSM_CountStruct msg24Count;							//记录消息24的周期计数
	FSM_CountStruct msg18AddCount;						//记录切换区范围内增加的消息18的周期计数
	u8 alterChannelNum;								//用于产生交替的发射信道指示
	workTypeIndicator workType;						//用于指示当前工作的工作模式
	
}FSM_ControlStruct;//控制结构体，存放所需的输入参数

typedef struct{

	FSM_FrameStruct fsm_frameStruct[FRAMELENGTH];//按照1371-4协议封装好的消息内容，最多设置的消息个数由FRAMELENGTH确定
	FSM_Frame19Struct fsm_frame19Struct; //单独记录消息19的待组帧消息内容
	//_Bool reservedSlot[2250];	//当前有效的保留时隙 对应下标位为1，则为保留时隙号
	u8 reservedSlotByte[282];	//当前有效的保留时隙,字节保存 对应下标位为1，则为保留时隙号
								//时隙号0~2249 依次对应数组下标从低到高，字节内部从高到低的顺序依次按位递增
	//_Bool sendChannel;					//发送的信道
	u16 realSlot;						//当前时隙号
	
}FSM_DataStruct;//数据结构体，存放输出的消息相关内容

////////////////////////////////extern 声明区 ///////////////////////////////////////////////////////////////////////////////
extern FSM_DataStruct fsm_dataStruct;
extern FSM_ControlStruct fsm_controlStruct;

///////////////////////////////////////////////function 声明区///////////////////////////////////////////////////////////////////
//辅助功能函数
u8 generate8bitRandNum(void);

void rstFSMFrameStruct(FSM_FrameStruct * fsm_frameStruct);
void rstFSMFrame19Struct(FSM_Frame19Struct * fsm_frame19Struct);

_Bool getAlterChannel(u8 alterChannelNum);
_Bool getAssignAlterChannel(FSM_ControlStruct * fsm_controlStruct);

void getLatiLongiMin(GPS_InfoStruct * gps_infoStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct,u32 lati,u32 longi,u8 accurLen);
void changeMinuteToDegree(u32 lat_1,u32 lon_1,u8 accurLen,double * lat_2,double * lon_2);

double calSphereDist(double lat_1,double lon_1,double lat_2, double lon_2);
double calLongiDist(double lat_1,double lat_2);

//消息处理功能函数
void generateMsg14encapData(AIS_BBMMsgStruct * ais_bbmMsgStruct,FSM_FrameStruct * fsm_frameStruct);
void generateMsg14Content(AIS_BBMMsgStruct * ais_bbmMsgStruct,FSM_DataStruct * fsm_dataStruct);

void generateMsg18encapData(GPS_InfoStruct * gps_infoStruct,FSM_FrameStruct * fsm_frameStruct);
void generateMsg18Content(MsgTypeIndicator msgType,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel);

void generateMsg19encapData(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,FSM_Frame19Struct * fsm_frame19Struct);
void generateMsg19Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,GPS_InfoStruct * gps_infoStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * vdlMsg15,_Bool alterChannel); 

void generateMsg24AencapData(AIS_StaticDataStruct * ais_staticDataStruct, FSM_FrameStruct * fsm_frameStruct);
void generateMsg24BencapData(AIS_StaticDataStruct * ais_staticDataStruct, FSM_FrameStruct * fsm_frameStruct);
void generateMsg24Content(MsgTypeIndicator msgType,AIS_StaticDataStruct * ais_staticDataStruct,FSM_DataStruct * fsm_dataStruct,VDLMsg15 * rcvVDLMsg15,_Bool alterChannel);

void initMsg20Struct(FSM_Msg20Struct * fsm_msg20Struct);
void updataMsg20Struct(VDLMsg20 * vdlMsg20,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct);
u8 checkFSMMsg20Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg20Struct * fsm_msg20Struct,FSM_DataStruct * fsm_dataStruct);

void initMsg22Struct(FSM_Msg22Struct * fsm_msg22Struct);
void getMsg22ChannelManagerInfo(VDLMsg22 * vdlMsg22,FSM_Msg22Struct * fsm_msg22Struct,u8 biasLen);
u8 getAreaValid(double tLati1,double tLon1,double tLati2,double tLon2,double sLati1,double sLon1,double sLati2,double sLon2);
void updataMsg22Struct(VDLMsg22 * vdlMsg22,FSM_Msg22Struct * fsm_msg22Struct);
workTypeIndicator getHandoverAreaValid(GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct,u8 workArea);
u8 checkFSMMsg22Struct(GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct);

void initMsg23Struct(FSM_Msg23Struct * fsm_msg23Struct);
void updataMsg23Struct(VDLMsg23 * vdlMsg23,GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct);
u8 checkFSMMsg23Struct(GPS_InfoStruct * gps_infoStruct,FSM_Msg23Struct * fsm_msg23Struct);

//控制功能函数
void FSMInit(FSM_ControlStruct * fsm_controlStruct);
void collectNewMsg(AIS_RcvMsgIDStruct * ais_rcvMsgIDStruct,VDLMsg15 * vdlMsg15,FSM_ControlStruct * fsm_controlStruct);
void generateSafeAndInquiryContent(GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * rcvVDLMsg15,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct);
void updataAssignContent(VDLMsg20 * vdlMsg20,VDLMsg22 * vdlMsg22,VDLMsg23 * vdlMsg23,GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct);
u8 checkAssignValid(GPS_InfoStruct * gps_infoStruct,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct);//检测区域，Msg22、Msg23指配范围内的区域
void setCycleCountNum(FSM_ControlStruct * fsm_controlStruct);
void generateCycleContent(workTypeIndicator workType,GPS_InfoStruct * gps_infoStruct,AIS_StaticDataStruct * ais_staticDataStruct,VDLMsg15 * rcvVDLMsg15,FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct);


//////////////////////////////////////for test/////////////////////////////////////////

//void initFsm_dataStruct(FSM_ControlStruct * fsm_controlStruct,FSM_DataStruct * fsm_dataStruct);


#endif