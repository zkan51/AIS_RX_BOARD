#include "mkdData.h"


//MKD_FrameStruct mkd_frameStruct;
AIS_RcvMsgIDStruct ais_rcvMsgIDStruct;
AIS_StaticDataStruct ais_staticDataStruct;
AIS_BBMMsgStruct ais_bbmMsgStruct;
AIS_RcvMsgIDStruct ais_msgIDStruct;


//VSD
u8 mkd_data1[] = "UU$AIVSD,33,25,100,beijing,125959.59,31,12,14,15*12\r\n";
//SSD
u8 mkd_data2[] = "kdkdk$AISSD,ABC1234,@@@@@@@@@@@@@@@@@@@A,050,511,06,63,1,A*12\r\n";
//BBM
u8 mkd_data3[] = "kaddva!AIBBM,1,1,2,2,14,0000,0*12\r\n";

/************************************************************************
* Name      : getVSDStaticInfo
* Brief     : 将接收的一帧VSD格式数据中的船类型，并保存到staticData结构体中
*			VSD格式：$AIVSD,x.x,x.x,x.x,c--c,hhmmss.ss,xx,xx,x.x,x.x*hh<CR><LF>
*							船类型(第一个字节，后续内容暂不做提取)
* Author    : Digital Design Team
* Param     : buf-MKD串口接收到的一帧VSD数据的指针地址
* Param     : ais_staticDataStruct-静态数据相关的数据结构体
* Return    : void
************************************************************************/
void getVSDStaticInfo(u8 * buf,AIS_StaticDataStruct * ais_staticDataStruct)
{
	u8 fieldTmp[5];
	u8 fieldTmpNum = 0;
	u8 i = 0;
	//fieldTmpNum = 0;
	//i = 0;
	while(buf[i] != 0x2C)//找","
	{
		i ++;
	}
	i ++;
	while(buf[i] != 0x2C)//找","
	{
		fieldTmp[fieldTmpNum] = buf[i];//取第一个字段的船舶类型
		fieldTmpNum ++;
		i ++;
	}
	switch(fieldTmpNum)
	{
		case 1: ais_staticDataStruct->shipType = (fieldTmp[0]-0x30);break;
		case 2: ais_staticDataStruct->shipType = (fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30);break;
		case 3: ais_staticDataStruct->shipType = (fieldTmp[0]-0x30)*100+(fieldTmp[1]-0x30)*10+(fieldTmp[2]-0x30);break;
		default: ais_staticDataStruct->shipType = 0;//不可用或无船舶
	}
}

/************************************************************************
* Name      : setSupplierInfo
* Brief     : 设置供应商的值，供应商的值一般由本地程序中给出
*			(根据定义的宏SUPPLIER来获取比特流)  该函数应该在程序初始化时调用
* Author    : Digital Design Team
* Param     : ais_staicDataStruct- 设定结构体中的supplier数组的值
* Return    : void
************************************************************************/
void setSupplierInfo(AIS_StaticDataStruct * ais_staicDataStruct)
{
	u32 tmp = 0;
	u8 i = 0;
	tmp = (SUPPLIER & 0x3FFFFFFFC00)>>10;	//取高32位
	while(i < 32)
	{
		if ((tmp >> i)&0x01)
		{
			ais_staicDataStruct->supplier[i+10] = 1;
		}
		else
		{
			ais_staicDataStruct->supplier[i+10] = 0;
		}
		i ++;
	}
	tmp = SUPPLIER & 0x3FF;			//取低10位
	i = 0;
	while(i < 10)
	{
		if ((tmp >> i)&0x01)
		{
			ais_staicDataStruct->supplier[i] = 1;
		}
		else
		{
			ais_staicDataStruct->supplier[i] = 0;
		}
		i ++;
	}
}

/************************************************************************
* Name      : change8bitASCIItoBit
* Brief     : 将接收的61162中的标准8bitASCII码转换成1371中规定的6bitASCII码的二进制比特流
*			标准ASCII码和1371中6bitASCII码的对应关系为：
*			    32~63(标准ASCII码十进制) ->	32~63（1371中6bitASCII码的十进制）
*				64~95					 ->	 0~31
*			存放对应关系：
*			地址（字节） 0  1   2    ……  len-1
*			*sour：     7~0（高字节） 7~0 7~0  …… 7~0（低字节）
*			地址（比特）6*len-1  ……      76543210
*			*dest:      543210（高字节对应bit）	 543210   …… 543210(低字节对应bit)
* Author    : wqdnan
* Param     : len- 输入，8bitASCII码字节数据的长度
* Param     : sour- 输入，8bitASCII码字节存放的地址
* Param     : dest- 输出，转换的13716bit数据比特流的输出地址
* Return    : void
************************************************************************/
void change8bitASCIItoBit(u8 len,u8 * sour,_Bool * dest)
{
	u8 i = 0;
	u8 j = 0;
	u8 tmp = 0;
	u8 bias = 0;
	while (i < len)
	{
		tmp = *(sour + i);
		if (tmp == 94)//保留字符, 则连续取后续的两个字符的数值(先低位，再高位)，组成一个字符信息
		{
			i ++;
			tmp = *(sour + i);
			i ++;
			tmp += (*(sour + i)<<4);
		}
		if (tmp >= 32) // 这个时候若还出现94，也认为是有效值
		{
			if (tmp >= 64)
			{
				tmp -= 64;
			}
			//取低六位的值并存放到目标地址中
			j = 0;
			//* dest = (* dest << (i * 6));
			bias = (len-1) * 6 - i * 6;
			while(j < 6)
			{
				if ((tmp >> j) &0x01)
				{
					* (dest + j + bias) = 1;
				}
				else
				{
					* (dest + j + bias) = 0;
				}
				j ++;
			}
		}
		else	// 不应该出现的数据，若出现了怎么处理
		{
			// to be added
		}
		i ++;

	}
}

/************************************************************************
* Name      : changeMKD6bitASCIItoBit
* Brief     : 将接收的61162中的6bitASCII码，按照每3个字节中完成4个6bitASCII码的提取出封装字节中的实际内容
*		      并将其转换成1371中规定的6bitASCII码的二进制比特流
*			  转化中先将61162中的6bitASCII码转换为标准的8bitASCII码，再将标准ASCII码转换为1371中的6bitASCII码
*			  611626bitASCII码转化为标准ASCII码：
*			  若待转换值小于101000，则加上00110000，转换为标准ASCII码
*			  若待转换值大于等于101000，则加上00111000，转换为标准ASCII码
*			  标准ASCII码转换为1371中的6bitACII码：
*			    32~63(标准ASCII码十进制) ->	32~63（1371中6bitASCII码的十进制）
*				64~95					 ->	 0~31
* Author    : Digital Design Team
* Param     : len- 输入，6bitASCII码字节数据的长度
* Param     : sour- 输入，6bitASCII码字节存放的地址
* Param     : dest- 输出，转换的13716bit数据比特流的输出地址
* Return    : void
************************************************************************/
void changeMKD6bitASCIItoBit(u8 len,u8 *sour,_Bool * dest)
{
	u8 tmp[20] = {0};//对于B-CS安全类命令只占一个时隙长度，安全文本最大为16个6bit字符
	u8 i = 0;
	u8 j = 0;
	u8 tmp6bit[4] = {0};
	u8 tmp6bitLen = 0;
	u8 bias = 0;
	//先将输入的字段按照6bit分别取出，每次取3个字节，24个bit，可以组成4个6bit数
	for(j = 0; j < len; j += 3)
	{
		if(j+2 > len)//取的字节不是3的整数
		{
			if((len-j)==1)//若正好多1个字节,那么最后4bit是填充的
			{
				tmp6bitLen = 2;
				tmp6bit[0] = (*(sour+j)&0xFC)>>2;
				tmp6bit[1] = (*(sour+j)&0x03)<<4;;
			}
			else //若正好多2个字节,那么最后2bit是填充的
			{
				tmp6bitLen = 3;
				tmp6bit[0] = (*(sour+j)&0xFC)>>2;
				tmp6bit[1] = (*(sour+j)&0x03)<<4;
				tmp6bit[1] += (*(sour+j+1)&0xF0)>>4;
				tmp6bit[2] = (*(sour+j+1)&0x0F)<<2;
			}
		}
		else//正好可以取3个字节
		{
			tmp6bitLen = 4;
			tmp6bit[0] = (*(sour+j)&0xFC)>>2;
			tmp6bit[1] = (*(sour+j)&0x03)<<4;
			tmp6bit[1] += (*(sour+j+1)&0xF0)>>4;
			tmp6bit[2] = (*(sour+j+1)&0x0F)<<2;
			tmp6bit[2] += (*(sour+j+2)&0xC0)>>6;
			tmp6bit[3] = (*(sour+j+2)&0x3F);
		}
		//先将61162中的6bitASCII码转换成标准8bitASCII码
		for(i = 0; i < tmp6bitLen; i ++)
		{
			if(tmp6bit[i] < 40)//值小于101000
			{
				tmp[i+bias] = tmp6bit[i] + 48;//加上00110000
			}
			else
			{
				tmp[i+bias] = tmp6bit[i] + 56;//加上00111000
			}
		}
		bias += tmp6bitLen;

	}
	//bias += 1;
	change8bitASCIItoBit(bias,tmp,dest);
}
/************************************************************************
* Name      : getMKDAISInfo   （与外部的接口函数，在串口任务中调用以提取相应的内容）
* Brief     : 从上位机串口一次接收中断识别VSD、SSD、BBM消息，并进行相应的操作如下：
*			  若是VSD，则提取船类型到静态信息结构体中保存
*			  若是SSD，则提取船的呼号，船的尺寸，DTE指示标志位到静态信息结构体中保存
*			  若是BBM，则提取安全相关的文本信息，并将文本格式按照1371中的格式进行转换，并保存至安全信息结构体中
* Author    : Digital Design Team
* Param     : mkd_frameStruct-mkd上位机接收的一帧数据结构体
* Param     : ais_staticDataStruct-ais相关的静态数据结构体
* Param     : ais_bbmMsgStruct-ais消息中的BBM数据结构体
* Param     : ais_msgIDStruct-记录消息ID号的结构体
* Return    : void
************************************************************************/
void getMKDAISInfo(u8 * buf,u16 length,AIS_StaticDataStruct * ais_staticDataStruct,AIS_BBMMsgStruct * ais_bbmMsgStruct,AIS_RcvMsgIDStruct * ais_msgIDStruct)
{
	u16 i = 0;
	u8 * p_data;
	u8 flag = 0;
	u16 bias = 0;
	char tmp[5] = {0};
	u8 len_data = 0;
	for (i = 0; i < length; i ++)
	{
		if (buf[i] == '$' || buf[i] == '!')//接收到起始符
		{
			//取5个字节判断是否为GPRMC
			bias = i + 5;
			while (i < bias)
			{
				i ++;
				tmp[bias-i] = buf[i];
			}
			//if (strcmp(&tmp[0],"CMRPG") == 0)//是GPVSD GPSSD GPBBM 中的一种
			if (tmp[4] =='A' && tmp[3] =='I' && tmp[2] =='V' && tmp[1] =='S' && tmp[0] =='D' ||
				tmp[4] =='A' && tmp[3] =='I' && tmp[2] =='S' && tmp[1] =='S' && tmp[0] =='D' ||
				tmp[4] =='A' && tmp[3] =='I' && tmp[2] =='B' && tmp[1] =='B' && tmp[0] =='M')
			{
				flag = 1;
				p_data = (buf+i);
			}
		}
		else if (flag == 1 && buf[i] == 0x0A && buf[i-1] == 0x0D)//接收到结束符
		{
			flag = 2;
			break;
		}
		else if (flag == 1)//在接收到RMC帧头后开始计数RMC的帧长
		{
			len_data ++;
		}

	}
	if (flag == 2)//正确接收到三种数据帧中的一种
	{
		//if(strcmp(datatmp,"aivsd")==0)//静态消息
		if (tmp[4] =='A' && tmp[3] =='I' && tmp[2] =='V' && tmp[1] =='S' && tmp[0] =='D')
		{
			getVSDStaticInfo(p_data,ais_staticDataStruct);
		}
		//else if(strcmp(datatmp,"aissd")==0)//静态消息
		else if (tmp[4] =='A' && tmp[3] =='I' && tmp[2] =='S' && tmp[1] =='S' && tmp[0] =='D')
		{
			getSSDStaticInfo(p_data,len_data,ais_staticDataStruct);
		}
		//else if(strcmp(datatmp,"aibbm")==0)//报警消息
		else if (tmp[4] =='A' && tmp[3] =='I' && tmp[2] =='B' && tmp[1] =='B' && tmp[0] =='M')
		{
			getBBMMsgInfo(p_data,len_data,ais_bbmMsgStruct);
			if (ais_msgIDStruct->msgState != MSGNEW && ais_bbmMsgStruct->rcvStateFlag == RECEIVED)//bbm消息接收完成，添加新的消息
			{
				ais_msgIDStruct->msgID = 14;
				ais_msgIDStruct->msgState = MSGNEW;
			}
		}
	}
}
/************************************************************************
* Name      : getSSDStaticInfo
* Brief     : 将接收的一帧SSD格式数据保存到staticData结构体中
*			SSD格式：$AISSD,c--c,c--c,xxx,xxx,xx,xx,c,aa*hh<CR><LF>
*			对应数据		船呼号|船名|A|B|C|D|DTE标志位|信息源指示器
*			对其中的船呼号,船名，船的尺寸ABCD的信息进行提取
* Author    : Digital Design Team
* Param     : buf-MKD串口接收到的一帧SSD数据数据的地址指针
* Param		: length-SSD一帧数据长度
* Param     : ais_staticDataStruct-静态数据相关的数据结构体
* Return    : void
************************************************************************/
void getSSDStaticInfo(u8 * buf,u16 length,AIS_StaticDataStruct * ais_staticDataStruct)
{
	u8 fieldTmp[22];
	u8 fieldTmpNum = 0;
	u8 i=0,j=0;
	u32 dataTmp = 0;
	u8 fieldNum = 0;
	//fieldTmpNum = 0;
	//fieldNum = 0;
	for(i = 0; i < length; i ++)
	{
		if(buf[i] == 0x2C)//找到了“,”
		{
			switch(fieldNum)
			{
				case 1://呼号,按照8bit ASCII码存放，高位存在低地址 最多可能存放42bit的数据
				{
					change8bitASCIItoBit(fieldTmpNum,&fieldTmp[0],ais_staticDataStruct->shipCall);
					break;
				}
				case 2://船名,按照8bit ASCII码存放，高位存在低地址，最多可能存放120bit
				{
					change8bitASCIItoBit(fieldTmpNum,&fieldTmp[0],ais_staticDataStruct->shipName);
					break;
				}
				case 3://位置A 固定三位数，放在shipSize[29:21]
				{
					dataTmp = ((fieldTmp[0]-0x30)*100 + (fieldTmp[1]-0x30)*10 + (fieldTmp[2]-0x30))&0x000001FF;
					ais_staticDataStruct->shipSize =0;//先清零，再准备重新赋值 ABCD尺寸仿真一个32位的空间内
					ais_staticDataStruct->shipSize |= (dataTmp << 21);
					break;
				}
				case 4://位置B 固定三位数，放在shipSize[21:12]
				{
					dataTmp = ((fieldTmp[0]-0x30)*100 + (fieldTmp[1]-0x30)*10 + (fieldTmp[2]-0x30))&0x000001FF;
					ais_staticDataStruct->shipSize |= (dataTmp << 12);
					break;
				}
				case 5://位置C 固定两位数，放在shipSize[11:6]
				{
					dataTmp = ((fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30))&0x000000FF;
					ais_staticDataStruct->shipSize |= (dataTmp << 6);
					break;
				}
				case 6://位置D 固定两位数，放在shipSize[5:0]
				{
					dataTmp = ((fieldTmp[0]-0x30)*10 + (fieldTmp[1]-0x30))&0x0000000FF;
					ais_staticDataStruct->shipSize |= dataTmp;
					break;
				}
				case 7://数据终端设置指示标志位
				{
					if(!(fieldTmp[0]-0x30))//0为可用
						ais_staticDataStruct->DTEIndicatorFlag = 1;
					else
						ais_staticDataStruct->DTEIndicatorFlag = 0;
					break;
				}
				default:;//其他不需要的内容
			}
			while(fieldTmpNum != 0)
			{
				fieldTmpNum --;
				fieldTmp[fieldTmpNum] = 0;
			}
			fieldNum ++;
		}
		else//存放字段信息
		{
			fieldTmp[fieldTmpNum] = buf[i];
			fieldTmpNum ++;
		}
	}

}

/************************************************************************
* Name      : getBBMMsgInfo
* Brief     : 将接收的一帧BBM格式数据保存到BBM结构体中
*			BBM格式：!AIBBM,x,x,x,x,x.x,s--s,x*hh<CR><LF>
*			对应内容		分段总数量|分段顺序号|序列号|广播信道|安全消息ID|封装数据|填充比特
*			安全消息针对A类（消息8）、B类（消息14），暂时只考虑消息14情况，分段总数为1（占用一个时隙）
*			广播信道（0-无 1-A 2-B 3-同时在A和B上广播）
* Author    : wqdnan
* Param     : mkd_frameStruct-MKD串口接收到的一帧数据数据结构体
* Param     : ais_bbmMsgStruct-BBM报警消息相关的数据结构体
* Return    : void
************************************************************************/
void getBBMMsgInfo(u8 * buf,u16 length,AIS_BBMMsgStruct * ais_bbmMsgStruct)
{
	u8 fieldTmp[62];
	u8 fieldTmpNum = 0;
	u8 fieldNum = 0;
	u8 dataTmp = 0;
	u8 i,j;
	fieldTmpNum = 0;
	fieldNum = 0;
	for(i = 0; i < length; i ++)
	{
		if(buf[i] == 0x2C)//找到","
		{
			switch(fieldNum)
			{
				case 1://分段总数
				{
					ais_bbmMsgStruct->sentenceTotalNum = (fieldTmp[0]-0x30);
					break;
				}
				case 2://分段顺序号
				{
					dataTmp = (fieldTmp[0]-0x30);
					ais_bbmMsgStruct->sentenceOrderNum = dataTmp;
					if((dataTmp > 1 && dataTmp != (ais_bbmMsgStruct->sentenceOrderNum+1)) || (dataTmp > ais_bbmMsgStruct->sentenceTotalNum))
						ais_bbmMsgStruct->rcvStateFlag = RECEIVEWRONG;
					break;
				}
				case 3://顺序消息识别符
				{
					dataTmp = (fieldTmp[0]-0x30);
					if(ais_bbmMsgStruct->sentenceTotalNum == ais_bbmMsgStruct->sentenceOrderNum)//接收完成
					{
						if(ais_bbmMsgStruct->sentenceTotalNum == 1 || (ais_bbmMsgStruct->sentenceTotalNum > 1 && dataTmp == ais_bbmMsgStruct->sequentialMessageIdentifier))
						{
							ais_bbmMsgStruct->rcvStateFlag = RECEIVED;
							ais_bbmMsgStruct->sequentialMessageIdentifier = dataTmp;
							//给处增加接收的时隙号
						}
						else
						{
							ais_bbmMsgStruct->rcvStateFlag = RECEIVEWRONG;
							break;
						}

					}
					else if(ais_bbmMsgStruct->sentenceTotalNum > ais_bbmMsgStruct->sentenceOrderNum)//接收分段中
					{
						if(ais_bbmMsgStruct->sentenceOrderNum == 1 || (dataTmp == ais_bbmMsgStruct->sequentialMessageIdentifier))
						{
							ais_bbmMsgStruct->rcvStateFlag = RECEIVING;
							ais_bbmMsgStruct->sequentialMessageIdentifier = dataTmp;
						}
						else
						{
							ais_bbmMsgStruct->rcvStateFlag = RECEIVEWRONG;
							break;
						}
					}
					break;
				}
				case 4://广播通道
				{
					dataTmp = (fieldTmp[0]-0x30);
					if (dataTmp > 0)
					{
						ais_bbmMsgStruct->broadcastChannel = dataTmp - 1;
					}
					else
					{
						ais_bbmMsgStruct->broadcastChannel = 4;//未使用
					}
					break;
				}
				case 5://消息ID
				{
					if(fieldTmpNum == 2)//消息14
					{
						ais_bbmMsgStruct->msgID = (fieldTmp[0]-0x30)*10+(fieldTmp[1]-0x30);
					}
					else if(fieldTmpNum == 1)//消息8
					{
						ais_bbmMsgStruct->msgID = (fieldTmp[0]-0x30);
					}
					break;
				}
				case 6://封装消息内容
				{
					if(ais_bbmMsgStruct->sentenceOrderNum == 1 && ais_bbmMsgStruct->msgOldFlag == MSGOLD)//如果收到新的消息帧，且旧消息已经处理过了，则先清空数组，再重新赋值
					{
						while(ais_bbmMsgStruct->encapsulatedDataLength != 0)
						{
							ais_bbmMsgStruct->encapsulatedDataLength --;
							ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = 0;
						}
						ais_bbmMsgStruct->msgOldFlag = MSGNEW;
					}
					if(ais_bbmMsgStruct->rcvStateFlag != RECEIVEWRONG)//接收正常的情况下，保存数据到数组中
					{
						//此处将61162规定的6bitASCII码转换成1371规定的6bitASCII码对应的二进制流
						changeMKD6bitASCIItoBit(fieldTmpNum,fieldTmp,&ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength]);
						ais_bbmMsgStruct->encapsulatedDataLength += ((fieldTmpNum*8)/6)*6;
						if ((fieldTmpNum * 8)%6)//有余数
						{
							ais_bbmMsgStruct->encapsulatedDataLength += 6;
						}
					}
					//if(ais_bbmMsgStruct->sentenceTotalNum == ais_bbmMsgStruct->sentenceOrderNum)//测试用，当接收完成，则可以重新接收新的一帧数据
					//	ais_bbmMsgStruct->msgOldFlag = MSGOLD;
					break;
				}
				default:;
			}
			while(fieldTmpNum != 0)//清空数据缓存中的信息
			{
				fieldTmpNum --;
				fieldTmp[fieldTmpNum] = 0;
			}
			fieldNum ++;
		}
		else
		{
			fieldTmp[fieldTmpNum] = buf[i];
			fieldTmpNum ++;
		}
	}
}

////////////////for test ////////////////////////////////
void testAIS_StaticStruct(AIS_StaticDataStruct * ais_staticDataStruct)
{
	u8 i,j;
	ais_staticDataStruct->DTEIndicatorFlag = 1;
	i= 0;
	j = 42;
	while (i < j)
	{
		ais_staticDataStruct->shipCall[i] = 1;
		i ++;
	}
	i = 0;
	j = 120;
	while (i < j)
	{
		ais_staticDataStruct->shipName[i] = 1;
		i ++;
	}
	ais_staticDataStruct->shipSize = 2048;//
	ais_staticDataStruct->shipType = 99;
	setSupplierInfo(ais_staticDataStruct);

}

void testAIS_BBMMsgStruct(AIS_BBMMsgStruct * ais_bbmMsgStruct)
{
	u8 lenTmp = 96;
	ais_bbmMsgStruct->msgID = 14;
	ais_bbmMsgStruct->rcvSlot = 1250;
	ais_bbmMsgStruct->broadcastChannel = CHANNELA;
	while (ais_bbmMsgStruct->encapsulatedDataLength < lenTmp)
	{
		if (ais_bbmMsgStruct->encapsulatedDataLength % 4 == 0)
		{
			ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = 0;
		}
		else
		{
			ais_bbmMsgStruct->encapsulatedData[ais_bbmMsgStruct->encapsulatedDataLength] = 1;
		}
		ais_bbmMsgStruct->encapsulatedDataLength ++;
	}


}
