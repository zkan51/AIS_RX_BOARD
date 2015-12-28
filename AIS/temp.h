#ifndef __temp_H
#define __temp_h

#include "myStdDef.h"

	typedef struct
	{
		u32 longitude;						//纬度，实际长度28bit,1/10000min为单位
		u32 latitude;							//经度，实际长度27bit,1/10000min为单位
		u32  utcTime;							//utc时间，格式为hhmmssss形式
		u16 SOG;									//地面航速，实际长度10bit,1/10为步长，有效范围0-102.2节1023不可用，1022=102.2节或更快
		u16 COG;									//地面航线，实际长度12bit,1/10°为步长，有效范围0-3599，3600=e10h不可用，3601~4095应不采用
		_Bool raimFlag;						//电子定位装置，0-未使用，1-正在使用
		
		
	}GPS_InfoStruct;//用于存放经过算法修正的经纬度、航向角等值

#endif