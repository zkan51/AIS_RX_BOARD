#include "gps.h"



GPS_InfoStruct gps_infoStruct;
GPS_RMCMsgStruct gps_RMCMsgStruct;

/************************************************************************
* Name      : getGPSInfomation
* Brief     : 从一次GPS的空闲中断中识别RMC数据帧，并从RMC中提取相关的内容到gps相应的结构体中，
*			按照1371中需要的格式从RMC中提取的内容包括:
*			经、纬度及方向（提取精度为/10000min）
*			地面航速（1/10节）
*			地面航向（1/10°）
*			UTC时间（mmddhhmmss/月天时分秒）
* Author    : wqdnan
* Param     : buf--GPS一次空闲中断接收到的数据帧地址
* Param     : length--GPS一次空闲中断接收到的字节长度
* Param     : gps_infoStruct--用于存储从GPS串口中断中获取的信息 结构体
* Return    : void
************************************************************************/
void getGPSInfomation(u8 * buf,u16 length,GPS_InfoStruct * gps_infoStruct)
{
	GPS_RMCMsgStruct gps_RMCMsgStruct;
	u16 i = 0;
	u8 * p_rmc;
	u8 flag = 0;
	u16 bias = 0;
	char tmp[5] = {0};
	u8 len_rmc = 0;
	for (i = 0; i < length; i ++)
	{
		if (buf[i] == '$')//接收到起始符
		{
			//取5个字节判断是否为GPRMC
			bias = i + 5;
			while (i < bias)
			{
				i ++;
				tmp[bias-i] = buf[i];
			}
			//if (strcmp(&tmp[0],"CMRPG") == 0)//是GPRMC
			if (tmp[0] =='C' && tmp[1] =='M' && tmp[2] =='R' && tmp[3] =='P' && tmp[4] =='G')
			{
				flag = 1;
				p_rmc = (buf+i);
			}
		}
		else if (flag == 1 && buf[i] == 0x0A && buf[i-1] == 0x0D)//接收到结束符
		{
			flag = 2;
			break;
		}
		else if (flag == 1)//在接收到RMC帧头后开始计数RMC的帧长
		{
			len_rmc ++;
		}

	}
	if (flag == 2)//接收到RMC帧后开始提取一帧的数据内容
	{
		Get_GPS_RMCMsg(p_rmc,&gps_RMCMsgStruct,len_rmc);
		updataGPSInfoStruct(gps_infoStruct,&gps_RMCMsgStruct);
	}
}

/************************************************************************
* Name      : Get_GPS_RMCMsg
* Brief     : 将接收的一帧RMC格式数据保存到RMC结构体中
*			RMC格式为：$GPRMC,hhmmss.ss,A,IIII.II,a,yyyyy,yy,a,x.x,x.x,xxxxxx,x.x,a,a*hh<CR><LF>
*							  UTC时间|状态|纬度|纬度方向|经度|经度方向|地面航速|地面航向|日期ddmmyy|磁变化|模式指示
* Author    : Digital Design Team && Embedded Team
* Param     : buff            -GPS串口接收到的一帧数据结构体
* Param     : gps_RMCMsgStruct-RMC数据结构体
* Return    : void
************************************************************************/
void Get_GPS_RMCMsg(u8 *buff,GPS_RMCMsgStruct *gps_RMCMsgStruct,u16 length)
{
	u8 fieldTmp[20];
	u8 fieldTmpNum = 0;
	u8 Comma_Num = 0;
	u32 dataTmp = 0;
	u16 i;
	u8 j = 0;
	for(i = 0; i < length; i ++)
	{
		if(buff[i] == 0x2C || buff[i] == 0x2A)//检测到字段分隔符或校验和标志符
		{
			switch(Comma_Num)//根据字段号进行相应的操作
			{
			case 1://utc时间 hhmmss.ss
				{
					gps_RMCMsgStruct->utcTime = (((fieldTmp[0]-0x30)*10+(fieldTmp[1]-0x30))<<24)+
						(((fieldTmp[2]-0x30)*10+(fieldTmp[3]-0x30))<<16)+
						(((fieldTmp[4]-0x30)*10+(fieldTmp[5]-0x30))<<8)+
						((fieldTmp[7]-0x30)*10+(fieldTmp[8]-0x30));
					break;
				}
			case 2://状态 A-data valid V-navigation receiver warning导航接收机报警
				{
					gps_RMCMsgStruct->status = fieldTmp[0];
					break;
				}
			case 3://纬度
				{
					gps_RMCMsgStruct->latitudeL = (fieldTmp[5]-0x30) * 1000 +
						(fieldTmp[6]-0x30) * 100 +
						(fieldTmp[7]-0x30) * 10 +
						(fieldTmp[8]-0x30); //小数分 只取4位
					gps_RMCMsgStruct->latitudeH = (fieldTmp[0]-0x30) * 600 +
						(fieldTmp[1]-0x30) * 60 +
						(fieldTmp[2]-0x30) * 10 +
						(fieldTmp[3]-0x30);  //全部改成min为单位，需要扩大100000倍
					break;
				}
			case 4://纬度方向
				{
					gps_RMCMsgStruct->latitudeDir = fieldTmp[0]; //存的8bit ASCII码字符
					break;
				}
			case 5://经度
				{
					gps_RMCMsgStruct->longitudeL = (fieldTmp[6]-0x30) * 1000 +
						(fieldTmp[7]-0x30) * 100 +
						(fieldTmp[8]-0x30) * 10 +
						(fieldTmp[9]-0x30);//小数分 只取4位
					gps_RMCMsgStruct->longitudeH = (fieldTmp[0]-0x30)*6000 +
						(fieldTmp[1]-0x30)*600 +
						(fieldTmp[2]-0x30)*60 +
						(fieldTmp[3]-0x30)*10 +
						(fieldTmp[4]-0x30);//全部改成min为单位，需要扩大100000倍
					break;
				}
			case 6://经度方向
				{
					gps_RMCMsgStruct->longitudeDir = fieldTmp[0];
					break;
				}
			case 7://地面航速
				{
					//只取小数点后一位，记录值为1/10节
					j = 0;
					while(fieldTmp[j] != '.' && j < 4)//找小数点  最大值为102.2
					{
						j ++;
					}
					if(j < 4)//找到小数点
					{
						switch(j)//根据小数点的位置，取可变长度内容数值
						{
						case 1: gps_RMCMsgStruct->sog = (fieldTmp[0]-0x30) * 10 +
									(fieldTmp[2]-0x30);
							break;
						case 2:gps_RMCMsgStruct->sog = (fieldTmp[0]-0x30) * 100 +
								   (fieldTmp[1]-0x30) * 10 +
								   (fieldTmp[3]-0x30);
							break;
						case 3:gps_RMCMsgStruct->sog = (fieldTmp[0]-0x30) * 1000 +
								   (fieldTmp[1]-0x30) * 100 +
								   (fieldTmp[2]-0x30) * 10 +
								   (fieldTmp[4]-0x30);
							break;
						default:;
						}

					}
					break;
				}
			case 8://地面航线
				{
					//只取小数点后一位，记录值为1/10°
					j = 0;
					while(fieldTmp[j] != '.' && j < 4)//找小数点  最大值为359.9
					{
						j ++;
					}
					if(j < 4)//找到小数点
					{
						switch(j)
						{
						case 1: gps_RMCMsgStruct->cog = (fieldTmp[0]-0x30) * 10 +
									(fieldTmp[2]-0x30);
							break;
						case 2:gps_RMCMsgStruct->cog = (fieldTmp[0]-0x30) * 100 +
								   (fieldTmp[1]-0x30) * 10 +
								   (fieldTmp[3]-0x30);
							break;
						case 3:gps_RMCMsgStruct->cog = (fieldTmp[0]-0x30) * 1000 +
								   (fieldTmp[1]-0x30) * 100 +
								   (fieldTmp[2]-0x30) * 10 +
								   (fieldTmp[4]-0x30);
							break;
						default:;
						}

					}
					else
					{
						gps_RMCMsgStruct->cog = 0;
					}
					break;
				}
			case 9://日期 ddmmyy
				{
					gps_RMCMsgStruct->date = (((fieldTmp[0]-0x30)*10 +
						(fieldTmp[1]-0x30))<<16)+

						(((fieldTmp[2]-0x30)*10 +
						(fieldTmp[3]-0x30))<<8)+

						((fieldTmp[4]-0x30)*10 +
						(fieldTmp[5]-0x30));
					break;
				}
			case 10://磁变化
				{
					break;
				}
			case 11://磁变化方向
				{
					gps_RMCMsgStruct->magneticVarDir = fieldTmp[0];
					break;
				}
			case 12://模式指示器
				{
					gps_RMCMsgStruct->modeIndicator = fieldTmp[0];//感觉用不上
					break;
				}
			default :;
			}
			while((fieldTmpNum) != 0)
			{
				fieldTmp[fieldTmpNum] = 0;
				fieldTmpNum --;
			}
			fieldTmp[0] = 0;
			Comma_Num ++;
		}
		else//接收字段数据内容
		{
			fieldTmp[fieldTmpNum++] = buff[i];
		}
	}
}
/************************************************************************
* Name      : updataGPSInfoStruct
* Brief     : 在每次中断的结束位置更新GPS信息结构体的内容
*			按照1371中需要的格式提取的内容包括:
*			经、纬度及方向（提取精度为/10000min）
*			地面航速（1/10节）
*			地面航向（1/10°）
*			UTC时间（mmddhhmmss）
* Author    : Digital Design Team
* Param     : gps_RMCMsgStruct- GPS一帧数据内容的结构体
* Param     : gps_infoStruct- GPS一帧数据内容的结构体
* Return    : void
************************************************************************/
void updataGPSInfoStruct(GPS_InfoStruct * gps_infoStruct,GPS_RMCMsgStruct * gps_RMCMsgStruct)
{
	//使用RMC更新gps信息的经纬度（及方向）、cog、sog、utc时间（mmddhhmmss(月天时分秒)）
	//1/10000min为单位，E-0，W-1，N-0，S-1
	gps_infoStruct->COG = gps_RMCMsgStruct->cog;
	gps_infoStruct->SOG = gps_RMCMsgStruct->sog;
	gps_infoStruct->latitude = gps_RMCMsgStruct->latitudeH * 10000 + gps_RMCMsgStruct->latitudeL;
	gps_infoStruct->latitudeDir = gps_RMCMsgStruct->latitudeDir;
	gps_infoStruct->longitude = gps_RMCMsgStruct->longitudeH * 10000 + gps_RMCMsgStruct->longitudeL;
	gps_infoStruct->longitudeDir = gps_RMCMsgStruct->longitudeDir;
	gps_infoStruct->utctime[0] = (gps_RMCMsgStruct->date&0xff00)>>8;//月
	gps_infoStruct->utctime[1] = (gps_RMCMsgStruct->date&0xff0000)>>16;//天
	gps_infoStruct->utctime[2] = (gps_RMCMsgStruct->utcTime&0xff000000)>>24;//时
	gps_infoStruct->utctime[3] = (gps_RMCMsgStruct->utcTime&0x00ff0000)>>16;//分
	gps_infoStruct->utctime[4] = (gps_RMCMsgStruct->utcTime&0xff00)>>8;//秒
	//其他的值
}


/////////////for test///////////////

void testGPS_InfoStruct(GPS_InfoStruct * gps_infoStruct)
{
	gps_infoStruct->broadBandFlag = 0;
	gps_infoStruct->COG = 2750;
	gps_infoStruct->commuFlag = 1;
	gps_infoStruct->deviceFlag = 1;
	gps_infoStruct->DSCFlag = 0;
	gps_infoStruct->HOG = 84;  //测试值
	gps_infoStruct->latitude = 1900;//0x21A31D8;纬度
	gps_infoStruct->longitude = 900;//0xD926F7D;经度
	gps_infoStruct->modeFlag = 0;
	gps_infoStruct->monitorFlag = 1;
	gps_infoStruct->msg22Flag = 0;
	gps_infoStruct->posAccurateFlag = 1;
	gps_infoStruct->raimFlag = 1;
	gps_infoStruct->SOG = 0;
	gps_infoStruct->utcTime = 50;
	gps_infoStruct->utctime[0] = 12;//月
	gps_infoStruct->utctime[1] = 31;//天
	gps_infoStruct->utctime[2] = 0;//12;//时 当前时间为12：50：00
	gps_infoStruct->utctime[3] = 19;//50;//分
	gps_infoStruct->utctime[4] = 59;//0;秒


}

void testGPS_InfoStructPara(GPS_InfoStruct * gps_infoStruct,u32 lati,u32 longi,u8 utchh,u8 utcmm,u8 utcss)
{
	gps_infoStruct->broadBandFlag = 0;
	gps_infoStruct->COG = 2750;
	gps_infoStruct->commuFlag = 1;
	gps_infoStruct->deviceFlag = 1;
	gps_infoStruct->DSCFlag = 0;
	gps_infoStruct->HOG = 84;  //测试值
	gps_infoStruct->latitude = lati;//0x21A31D8;纬度
	gps_infoStruct->longitude = longi;//0xD926F7D;经度
	gps_infoStruct->modeFlag = 0;
	gps_infoStruct->monitorFlag = 1;
	gps_infoStruct->msg22Flag = 0;
	gps_infoStruct->posAccurateFlag = 1;
	gps_infoStruct->raimFlag = 1;
	gps_infoStruct->SOG = 0;
	gps_infoStruct->utcTime = 50;
	gps_infoStruct->utctime[0] = 12;//月
	gps_infoStruct->utctime[1] = 31;//天
	gps_infoStruct->utctime[2] = utchh;//0;//12;//时 当前时间为12：50：00
	gps_infoStruct->utctime[3] = utcmm;//19;//50;//分
	gps_infoStruct->utctime[4] = utcss;//59;//0;秒
}
