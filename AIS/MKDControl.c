/**
********************************************************************
* @file      MKDControl.c
* @author    Digital Design Team
* @version   
* @date      2015/12/2   20:17
********************************************************************
* @brief     建立MCU和上位机之间的交互功能，包括：
*			 1、本船和它船的获取的原始数据的封装和按照61162格式的打包，并上传给上位机
*			 2、上位机安全类数据的处理和重新打包，发送给MCU，并确认反馈信息
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "MKDControl.h"
#include "testMKDControl.h"
#include "APP.h"

////////////////////////////////////变量定义区////////////////////////////////////////////////////

MKD_ControlStruct mkd_controlStruct;
MKD_DataStruct mkd_dataStruct;

/************************************************************************
* Name      : rstMKDFramedStruct
* Brief     : 清空mkd_framedStruct的内容，用于存放新的信息
*
* Author    : Digital Design Team
* Param     : mkd_framedStruct-组帧结构体，用于存放61162封装好的消息内容及其辅助信息
* Return    : void
************************************************************************/
void rstMKDFramedStruct(MKD_FramedStruct * mkd_framedStruct)
{
	u8 i = 0;
	while (i < mkd_framedStruct->mkd_encapDataByteLen)
	{
		mkd_framedStruct->mkd_encapDataByte[i] = 0;
	}
	mkd_framedStruct->mkd_encapDataByteLen = 0;
	mkd_framedStruct->mkd_broadChannel = 0;
	mkd_framedStruct->mkd_msgState = 0;
	mkd_framedStruct->mkd_msgType = 0;
}

/************************************************************************
* Name      : rstMKDContentToBeFramedStruct
* Brief     : 清空mkd_toBeframeStruct的内容，用于存放新的信息
*
* Author    : Digital Design Team
* Param     : mkd_toBeframeStruct-待组帧结构体，用于暂时存放组帧前所需的所有信息
* Return    : void
************************************************************************/
void rstMKDContentToBeFramedStruct(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct)
{
	u8 i = 0;
	while (i < mkd_toBeframeStruct->frameEncapContentByteLen)
	{
		mkd_toBeframeStruct->frameEncapContentByte[i] = 0;
		i ++;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = 0;
	mkd_toBeframeStruct->fillBits = 0;
	mkd_toBeframeStruct->aisChannel = 0;
	i = 0;
	while(i < 5)
	{
		mkd_toBeframeStruct->frameHead[i] = 0;
		i ++;
	}
	mkd_toBeframeStruct->frameType = 0;
	mkd_toBeframeStruct->sentenceNum = 0;
	mkd_toBeframeStruct->totalNum = 0;
	mkd_toBeframeStruct->sequenceNum = 0;
}

/************************************************************************
* Name      : chnage611626bitASCIIto8bitASCII
* Brief     : 按照61162协议，将6bit的ASCIIm码转换为8bit的ASCII码
*
* Author    : Digital Design Team
* Param     : len-6bitASCII码的长度
* Param     : sour-61162中的6bitASCII码的存放地址
* Param     : dest-61162中的8bitASCII码的存放地址
* Return    : void
************************************************************************/
void chnage611626bitASCIIto8bitASCII(u8 len,u8 * sour,u8 * dest)
{
	u8 i = 0;
	//先将61162中的6bitASCII码转换成标准8bitASCII码
	for (i = 0; i < len; i ++)
	{
		if (*(sour+i) < 40) //值小于101000
		{
			*(dest+i) = *(sour+i) + 48;//加上00110000
		}
		else//值大于等于101000
		{
			*(dest+i) = *(sour+i) + 56;//加上00111000
		}
	}
}

/************************************************************************
* Name      : change1371ContentsTo61162
* Brief     : 将1371协议的比特内容，按照对应关系转换为61162协议中的6比特ASCII码封装内容
*
* Author    : Digital Design Team
* Param     : fsm_frameStruct-1371中消息封装内容
* Param     : mkd_toBeframeStruct-61162中组帧所需的消息内容，未完成组帧
* Return    : void
************************************************************************/
void change1371ContentsTo61162(FSM_FrameStruct * fsm_frameStruct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct)
{
	u8 tmp13716bit[4] = {0};
	u8 tmp13716bitLen = 0;
	//u32 tmpThr8bits = 0;
	u8 tmp611626bit = 0;
	u8 i = 0;
	u8 bias = 0;
	//需要先清除mkd_toBeframeStruct 中的值
	rstMKDContentToBeFramedStruct(mkd_toBeframeStruct);
	for (i = 0; i < (fsm_frameStruct->encapDataLen/8); i += 3)
	{
		//先将1371协议内容，按照3字节取出，若不足3字节，需要保证取出的内容为6的整数倍，若不满足，则在末尾补0
		//字节按照下标从低到高取出，并依次按照从低到高存放到tmp13716bit中
		if (i+2 > fsm_frameStruct->encapDataLen/8)//1371中的封装消息字节数不是正好为3的倍数
		{
			//若多余1个字节,补4bit 存放为
			if ((fsm_frameStruct->encapDataLen/8 - i) == 1)
			{
				//tmpThr8bits = fsm_frameStruct->encapDataByte[i]<<4;   
				mkd_toBeframeStruct->fillBits = 4;
				tmp13716bit[0] = (fsm_frameStruct->encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (fsm_frameStruct->encapDataByte[i] & 0x03)<<4;
				tmp13716bitLen = 2;

			}
			else//若多余2个字节,补2bit (fsm_frameStruct->encapDataLen/8 - i) == 2
			{
				//tmpThr8bits = fsm_frameStruct->encapDataByte[i]<<10
				//			+ fsm_frameStruct->encapDataByte[i+1]<<2;
				mkd_toBeframeStruct->fillBits = 2;
				tmp13716bit[0] = (fsm_frameStruct->encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (fsm_frameStruct->encapDataByte [i] & 0x03)<< 4 + (fsm_frameStruct->encapDataByte[i+1] & 0xF0)>>4;
				tmp13716bit[2] = (fsm_frameStruct->encapDataByte[i+1] & 0x0F) << 2;
				tmp13716bitLen = 3;
			}
		}
		else//正好取出三个字节
		{
			//tmpThr8bits = fsm_frameStruct->encapDataByte[i] << 16
			//			 + fsm_frameStruct->encapDataByte[i+1] << 8
			//			 + fsm_frameStruct->encapDataByte[i+2];
			tmp13716bit[0] = (fsm_frameStruct->encapDataByte[i] & 0xFC)>>2;
			tmp13716bit[1] = (fsm_frameStruct->encapDataByte [i] & 0x03)<< 4 + (fsm_frameStruct->encapDataByte[i+1] & 0xF0)>>4;
			tmp13716bit[2] = (fsm_frameStruct->encapDataByte[i+1] & 0x0F) << 2 + (fsm_frameStruct->encapDataByte[i+2] & 0xC0)>>6;
			tmp13716bit[3] = (fsm_frameStruct->encapDataByte[i+2] & 0x3F);
			tmp13716bitLen = 4;

		}
		//将1371中的二进制码，直接组合成61162中的6bit(暂时这样处理，如果不对，再改),再转换为61162中的8bitASCII码
		chnage611626bitASCIIto8bitASCII(tmp13716bitLen,tmp13716bit,&mkd_toBeframeStruct->frameEncapContentByte[bias]);
		bias += tmp13716bitLen;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = bias;
}

/************************************************************************
* Name      : changeOtherShipContentTo61162
* Brief     : 将FPGA串口上报的vdl消息中的8字节内容，转换为61162协议中的6bitASCII码的形式
*
* Author    : Digital Design Team
* Param     : otherShipMsg-FPGA串口接收到的它船的原始数据内容，主要使用其中的封装数据信息
* Param     : mkd_toBeframeStruct-61162待组帧信息，主要使用其中存放将1371二进制数据修改为61162对应的6bit ASCII码的封装内容
* Return    : void
************************************************************************/
void changeOtherShipContentTo61162(FPGAVDLData * otherShipMsg,MKD_ContentToBeFramedStruct *mkd_toBeframeStruct)
{
	u8 tmp13716bit[4] = {0};
	u8 tmp13716bitLen = 0;
	//u32 tmpThr8bits = 0;
	u8 tmp611626bit = 0;
	u8 i = 0;
	u8 bias = 0;
	//需要先清除mkd_toBeframeStruct 中的值
	rstMKDContentToBeFramedStruct(mkd_toBeframeStruct);
	for (i = 0; i < otherShipMsg->frameBytes; i += 3)
	{
		//先将1371协议内容，按照3字节取出，若不足3字节，需要保证取出的内容为6的整数倍，若不满足，则在末尾补0
		//字节按照下标从低到高取出，并依次按照从低到高存放到tmp13716bit中
		if (i+3 > otherShipMsg->frameBytes)//1371中的封装消息字节数不是正好为3的倍数
		{
			//若多余1个字节,补4bit 存放为
			if ((otherShipMsg->frameBytes - i) == 1)
			{  
				mkd_toBeframeStruct->fillBits = 4;
				tmp13716bit[0] = (otherShipMsg->frameData[i] & 0xFC)>>2;
				tmp13716bit[1] = (otherShipMsg->frameData[i] & 0x03)<<4;
				tmp13716bitLen = 2;

			}
			else//若多余2个字节,补2bit (fsm_frameStruct->encapDataLen/8 - i) == 2
			{

				mkd_toBeframeStruct->fillBits = 2;
				tmp13716bit[0] = (otherShipMsg->frameData[i] & 0xFC)>>2;
				tmp13716bit[1] = (otherShipMsg->frameData[i] & 0x03)<< 4;
				tmp13716bit[1] += (otherShipMsg->frameData[i+1] & 0xF0)>>4;
				tmp13716bit[2] = (otherShipMsg->frameData[i+1] & 0x0F) << 2;
				tmp13716bitLen = 3;
			}
		}
		else//正好取出三个字节
		{
			tmp13716bit[0] = (otherShipMsg->frameData[i] & 0xFC)>>2;
			tmp13716bit[1] = (otherShipMsg->frameData[i] & 0x03)<< 4;
			tmp13716bit[1] += (otherShipMsg->frameData[i+1] & 0xF0)>>4;
			tmp13716bit[2] = (otherShipMsg->frameData[i+1] & 0x0F) << 2;
			tmp13716bit[2] += (otherShipMsg->frameData[i+2] & 0xC0)>>6;
			tmp13716bit[3] = (otherShipMsg->frameData[i+2] & 0x3F);
			tmp13716bitLen = 4;

		}
		//将1371中的二进制码，直接组合成61162中的6bit(暂时这样处理，如果不对，再改),再转换为61162中的8bitASCII码
		chnage611626bitASCIIto8bitASCII(tmp13716bitLen,tmp13716bit,&mkd_toBeframeStruct->frameEncapContentByte[bias]);
		bias += tmp13716bitLen;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = bias;
	// 测试代码，可以注解
	//testChangeOtherShipContentTo61162(otherShipMsg,mkd_toBeframeStruct);
}
/************************************************************************
* Name      : changeOwnShipContentTo61162
* Brief     : 将本船生成的vdl消息中的8字节内容，转换为61162协议中的6bitASCII码的形式
*
* Author    : Digital Design Team
* Param     : ownShipInfo_struct- 本船中产生的即将发送到VDL链路上的1371的信息内容，主要记录待发送帧的封装数据内容，长度和信道
* Param     : mkd_toBeframeStruct- 61162待组帧信息，主要使用其中存放将1371二进制数据修改为61162对应的6bit ASCII码的封装内容
* Return    : void
************************************************************************/
void changeOwnShipContentTo61162(OwnShipInfo_Struct * ownShipInfo_struct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct)
{
	u8 tmp13716bit[4] = {0};
	u8 tmp13716bitLen = 0;
	//u32 tmpThr8bits = 0;
	u8 tmp611626bit = 0;
	u8 i = 0;
	u8 bias = 0;
	//需要先清除mkd_toBeframeStruct 中的值
	rstMKDContentToBeFramedStruct(mkd_toBeframeStruct);
	for (i = 0; i < ownShipInfo_struct->vdl_encapDataByteLen; i += 3)
	{
		//先将1371协议内容，按照3字节取出，若不足3字节，需要保证取出的内容为6的整数倍，若不满足，则在末尾补0
		//字节按照下标从低到高取出，并依次按照从低到高存放到tmp13716bit中
		if (i+3 > ownShipInfo_struct->vdl_encapDataByteLen)//1371中的封装消息字节数不是正好为3的倍数
		{
			//若多余1个字节,补4bit 存放为
			if ((ownShipInfo_struct->vdl_encapDataByteLen - i) == 1)
			{
				mkd_toBeframeStruct->fillBits = 4;
				tmp13716bit[0] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0x03)<<4;
				tmp13716bitLen = 2;

			}
			else//若多余2个字节,补2bit (fsm_frameStruct->encapDataLen/8 - i) == 2
			{

				mkd_toBeframeStruct->fillBits = 2;
				tmp13716bit[0] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0x03)<< 4;
				tmp13716bit[1] += (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0xF0)>>4;
				tmp13716bit[2] = (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0x0F) << 2;
				tmp13716bitLen = 3;
			}
		}
		else//正好取出三个字节
		{
			tmp13716bit[0] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0xFC)>>2;
			tmp13716bit[1] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0x03)<< 4;
			tmp13716bit[1] += (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0xF0)>>4;
			tmp13716bit[2] = (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0x0F) << 2;
			tmp13716bit[2] += (ownShipInfo_struct->vdl_encapDataByte[i+2] & 0xC0)>>6;
			tmp13716bit[3] = (ownShipInfo_struct->vdl_encapDataByte[i+2] & 0x3F);
			tmp13716bitLen = 4;

		}
		//将1371中的二进制码，直接组合成61162中的6bit(暂时这样处理，如果不对，再改),再转换为61162中的8bitASCII码
		chnage611626bitASCIIto8bitASCII(tmp13716bitLen,tmp13716bit,&mkd_toBeframeStruct->frameEncapContentByte[bias]);
		bias += tmp13716bitLen;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = bias;
	mkd_toBeframeStruct->aisChannel = ownShipInfo_struct->vdl_channel;//
	// 测试代码，可以注解
	//testChangeOwnShipContentTo61162(ownShipInfo_struct,mkd_toBeframeStruct);
}
/************************************************************************
* Name      : updataToBeFrameInfo
* Brief     : 根据FPGA串口接收的vdl消息，准备61162协议格式所需要的帧头和帧尾数据内容
*
* Author    : Digital Design Team
* Param     : mkd_toBeframeStruct- 需要封装成61162协议格式的准备消息内容信息，使用除了封装消息内容外的其他的数据内容
* Param     : id-用于指示当前需要生成的消息ID号，
* Return    : void
************************************************************************/
void updataToBeFrameInfo(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_IdIndicator id)
{
	u8 i = 0;
	if (id == VDM || id == VDO)
	{
		mkd_toBeframeStruct->frameType = '!';//‘!’符号
	}
	else //ABK TXT ALR
	{
		mkd_toBeframeStruct->frameType = '$';//‘$’符号
	}
	mkd_toBeframeStruct->frameHead[0] = 'A';
	mkd_toBeframeStruct->frameHead[1] = 'I';
	switch(id)
	{
	case VDM:
		mkd_toBeframeStruct->frameHead[2] = 'V';
		mkd_toBeframeStruct->frameHead[3] = 'D';
		mkd_toBeframeStruct->frameHead[4] = 'M';
		break;
	case VDO:
		mkd_toBeframeStruct->frameHead[2] = 'V';
		mkd_toBeframeStruct->frameHead[3] = 'D';
		mkd_toBeframeStruct->frameHead[4] = 'O';
		break;
	case ABK:
		mkd_toBeframeStruct->frameHead[2] = 'A';
		mkd_toBeframeStruct->frameHead[3] = 'B';
		mkd_toBeframeStruct->frameHead[4] = 'K';
		break;
	case TXT:
		mkd_toBeframeStruct->frameHead[2] = 'T';
		mkd_toBeframeStruct->frameHead[3] = 'X';
		mkd_toBeframeStruct->frameHead[4] = 'T';
		break;
	case ALR:
		mkd_toBeframeStruct->frameHead[2] = 'A';
		mkd_toBeframeStruct->frameHead[3] = 'L';
		mkd_toBeframeStruct->frameHead[4] = 'R';
		break;
	default:
		break;
	}
	if (mkd_toBeframeStruct->frameEncapContentByteLen < 60)//一个分段完成
	{
		mkd_toBeframeStruct->totalNum = 1;
	}
	else if (mkd_toBeframeStruct->frameEncapContentByteLen < 120)//两个分段完成
	{
		mkd_toBeframeStruct->totalNum = 2;
	}
	else//现在认为分段数不会超过3个
	{
		mkd_toBeframeStruct->totalNum = 3;
	}
	mkd_toBeframeStruct->sequenceNum = 0;//(mkd_toBeframeStruct->sequenceNum + 1)%10;要上一个消息的格式和该消息格式一致，才用加1，否则不操作
}
/************************************************************************
* Name      : updataVDMorVDOFrame
* Brief     : 将61162按照内容分开存放的待组帧信息，组合在一起完成组帧，并计算校验位添加相应位
*
* Author    : Digital Design Team
* Param     : mkd_toBeframeStruct- 待组帧的信息内容
* Param     : mkd_dataStruct-上位机的数据结构体，用于存放组帧完成后的相应消息内容
* Return    : void
************************************************************************/
void updataVDMorVDOFrame(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_DataStruct * mkd_dataStruct)
{
	u8 bias = 0;
	u8 biasSub = 0;
	u8 j = 0;
	u8 i = 0;
	u8 tmpCheck = 0;
	u8 tmp = 0;
  	u8 subSection = 0;
	u8 subSectionLen = 0;
	static u8 MsgQueueBias;
	while (subSection < mkd_toBeframeStruct->totalNum)//subSection 对应相应的分段顺序号
	{
		//清零mkd_frameStruct结构体
		subSection ++;
		//先找到一个可供写入的新的结构体
		i = 0;
		while (mkd_dataStruct->mkd_framedStruct[i].mkd_msgState != MSGOLD)
		{
			i++;
		}
		if (i < MAXMKDFRAMELEN)//还有可供写入的结构体空间
		{
			rstMKDFramedStruct(&mkd_dataStruct->mkd_framedStruct[i]);
			bias = 0;
			if (subSection == 1 && mkd_toBeframeStruct->frameEncapContentByteLen > 60)//这个时候需要分成多个分段  暂时认为没有超过3个时隙的长度
			{
				subSectionLen = 60;
			}
			else if (subSection == 2 && mkd_toBeframeStruct->frameEncapContentByteLen > 120)
			{
				subSectionLen = 60;
			}
			else //第一个第二个或者第三个分段不足60的整数倍的情况
			{
				subSectionLen = mkd_toBeframeStruct->frameEncapContentByteLen - (subSection-1)*60;
			}
			while(bias < 15+subSectionLen) //to be changed
			{
				switch (bias)
				{
				case 0: //！ 消息类型1
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->frameType;
						bias = 1;
						break;
					}
				case 1: //aaccc 消息类型2
					{
						bias = 6;
						while(mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen < bias)
							mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->frameHead[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen -1];
						break;
					}
				case 7: //总分段数 1~9
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->totalNum+0x30;//变化为对应点ASCII码
						bias = 8;
						break;
					}
				case 9: //分段顺序号 1~9
					{
						//可能有多个分段
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = subSection+0x30;
						bias = 10;
						break;
					}
				case 11://消息序列号 0~9
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = mkd_toBeframeStruct->sequenceNum+0x30;
						bias = 12;
						break;
					}
				case 13: //AIS发送信道
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->aisChannel+0x41;//转换为A、B
						bias = 14;
						break;
					}
				case 15://封装的1371的消息   //如果封装信息长度超过了60，则需要分段成多个
					{
						bias = 15+subSectionLen;
						biasSub = (subSection-1)*60;
						while(mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen < bias)
							mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++]
							= mkd_toBeframeStruct->frameEncapContentByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen+biasSub-15];
						break;
					}
				case 6:case 8:case 10:case 12:case 14: //分割用的','
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0x2C;
						bias += 1;
						break;
					}
				default:break;//不该出现的情况
				}
			}
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0x2C;//封装1371消息内容后的','
			//填充应该是在最后一个分段中添加，其他的整数分段中，不需要
			if (subSection == mkd_toBeframeStruct->totalNum)//已经是最后一个分段了
			{
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->fillBits+0x30;//比特填充
			}
			else//不是最后一个分段
			{
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0+0x30;//比特填充
			}
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0x2A;//校验和前的'*'符号
			j = 1;
			tmpCheck = 0;
			while (j < mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen-1)//计算除了！ * 这两个符号，且在这两个符号之间的封装信息的异或结果
			{
				tmpCheck ^=  mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[j];
				j ++;
			}
			tmp = (tmpCheck &0xF0)>>4;
			if(tmp < 10)//0~9
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x30;//0~9加48（0x30)对应为0x30~0x39
			else
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x37;//A~F加55（0x37）对应为0x65~0x70
			tmp = tmpCheck &0x0F;
			if(tmp < 10)//0~9
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x30;//0~9加48（0x30)对应为0x30~0x39
			else
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x37;//A~F加55（0x37）对应为0x65~0x70
			//mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmpCheck;//存放校验和
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = 0x0D;//存放结束符
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = 0x0A;
			mkd_dataStruct->mkd_framedStruct[i].mkd_msgState = MSGNEW;
			mkd_dataStruct->mkd_framedStruct[i].mkd_broadChannel = mkd_toBeframeStruct->aisChannel;
			//生成好的它船或自船消息字节流以FIFO方式压入消息队列开始
			MsgQ_Mkd_Buf[MsgQueueBias][0] = 0;//第一个字节记录消息内容的长度
			MsgQ_Mkd_Buf[MsgQueueBias][1] = mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen;//第一个字节记录消息内容的长度
			for(j = 0; j < mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen; j ++)
			{
				//将封装消息存放到二维数组中
				MsgQ_Mkd_Buf[MsgQueueBias][j+2] = mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[j];
			}
			OSQPost(Q_mkd_ais,&MsgQ_Mkd_Buf[MsgQueueBias]);
			MsgQueueBias ++;
			MsgQueueBias %= MSG_QUEUE_MKD_NUM;
			//压入消息队列结束
			//测试用，可以注解掉
			//testUpdataVDMorVDOFrame(mkd_toBeframeStruct,&mkd_dataStruct->mkd_framedStruct[i]);
		}
	}
}

/************************************************************************
* Name      : MKDInit
* Brief     : 初始化上位机的控制功能状态，设定最开始没有新的消息，相应的消息状态全部设定为MSGOLD等
*
* Author    : Digital Design Team
* Param     : mkd_controlStruct-上位机控制功能的控制结构体
* Param     : mkd_dataStruct-上位机的数据结构体，用于存放向上位机上报的新消息内容
* Return    : void
************************************************************************/
void MKDInit(MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct)
{
	u8 i = 0;
	for (i = 0; i < MAXOWNINFOLEN; i ++)
	{
		mkd_controlStruct->ownShipInfoStruct[i].vdlMsgState = MSGOLD;
	}
	for (i = 0; i < MAXMKDFRAMELEN; i ++)
	{
		mkd_dataStruct->mkd_framedStruct[i].mkd_msgState = MSGOLD;
	}
}

/************************************************************************
* Name      : getOwn61162ToBeFramedInfo
* Brief     : 从状态机处获取最新的要发送到VDL链路层的新的封装消息内容，并将其存放到上位机控制结构中的相应位置，以供后续生成相应的VDO消息
*
* Author    : Digital Design Team
* Param     : mkd_controlStruct- 上位机的控制结构体，使用其中的存放1371封装消息ownShipInfoStruct结构体数组
* Param     : fsm_dataStruct- 状态机的输出数据结构体，使用其中的新产生的待发送的1371封装消息结构体
* Return    : void
************************************************************************/
void getOwn61162ToBeFramedInfo(MKD_ControlStruct * mkd_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	while(i < FRAMELENGTH)//判断一个时隙的消息
	{
		if (fsm_dataStruct->fsm_frameStruct[i].msgState == MSGNEW)// 有新的FSM新消息
		{
			//将封装的内容，长度，信道赋给上位机的缓冲区
			j = 0;
			while (mkd_controlStruct->ownShipInfoStruct[j].vdlMsgState != MSGOLD)
			{
				j ++;
				if (j >= MAXOWNINFOLEN)
					break;
			}
			if (j < MAXOWNINFOLEN)//找到一个新的可以覆盖的结构体
			{
				for (k = 0; k < fsm_dataStruct->fsm_frameStruct[j].encapDataLen/8; k ++)
				{
					mkd_controlStruct->ownShipInfoStruct[j].vdl_encapDataByte[k] = fsm_dataStruct->fsm_frameStruct[i].encapDataByte[k];
				}
				mkd_controlStruct->ownShipInfoStruct[j].vdl_encapDataByteLen = k;
				mkd_controlStruct->ownShipInfoStruct[j].vdl_channel = fsm_dataStruct->fsm_frameStruct[i].broadChannel;
				mkd_controlStruct->ownShipInfoStruct[j].vdlMsgState = MSGNEW;
			}

		}
		i ++;
	}
	if (fsm_dataStruct->fsm_frame19Struct.msgState == MSGNEW)//有消息19的新消息
	{
		//将封装的内容，长度，信道赋给上位机的缓冲区
		j = 0;
		while (mkd_controlStruct->ownShipInfoStruct[j].vdlMsgState != MSGOLD)
		{
			j ++;
			if (j >= MAXOWNINFOLEN)
				break;
		}
		if (j < MAXOWNINFOLEN)//找到一个新的可以覆盖的结构体
		{
			for (k = 0; k < fsm_dataStruct->fsm_frame19Struct.encapDataLen/8; k ++)
			{
				mkd_controlStruct->ownShipInfoStruct[j].vdl_encapDataByte[k] = fsm_dataStruct->fsm_frame19Struct.encapDataByte[k];
			}
			mkd_controlStruct->ownShipInfoStruct[j].vdl_encapDataByteLen = k;
			mkd_controlStruct->ownShipInfoStruct[j].vdl_channel = fsm_dataStruct->fsm_frame19Struct.broadChannel;
			mkd_controlStruct->ownShipInfoStruct[j].vdlMsgState = MSGNEW;
		}
	}
}
/************************************************************************
* Name      : mkd_collectNewMsg
* Brief     : 收集它船消息，若有，则将其转化为符合61162协议格式的数据内容
*
* Author    : Digital Design Team
* Param     : vdlOtherData-接收的它船的完整消息的结构体，存放组成61162协议消息所需的全部原始数据信息
* Param     : mkd_controlStruct-上位机控制结构体，用于存放中间变量信息
* Param     : mkd_dataStruct-上位机数据结构体，用于存放按照61162协议组帧完成的分段消息内容
* Param		: id- 用于指示生成61162消息的标识
* Return    : void
************************************************************************/
void mkd_collecVDLShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,MKD_IdIndicator id)
{
	//当存在它船消息时
	if (id == VDM)
	{
		changeOtherShipContentTo61162(vdlOtherData,&mkd_controlStruct->mkd_toBeFrameStruct);

	}
	else if (id == VDO)
	{
		changeOwnShipContentTo61162(&mkd_controlStruct->ownShipInfoStruct[mkd_controlStruct->ownShipInfoBias],&mkd_controlStruct->mkd_toBeFrameStruct);
	}
	updataToBeFrameInfo(&mkd_controlStruct->mkd_toBeFrameStruct,id);
	updataVDMorVDOFrame(&mkd_controlStruct->mkd_toBeFrameStruct,mkd_dataStruct);

}


/************************************************************************
* Name      : mkd_collectNewMsg
* Brief     : 收集上位机新接收的新接收的相应消息，并将消息按照61162的协议规定按照格式进行封装
*
* Author    : Digital Design Team
* Param     : isOtherShipMsgNewIn- FPGA串口接收的它船vdl新消息标志
* Param     : vdlOtherData- FPGA串口接收的它船vdl新消息的具体内容
* Param     : mkd_controlStruct- 上位机的控制结构体，用于存放中间数据内容
* Param     : mkd_dataStruct- 上位机的数据结构体，用于存放按照61162协议组帧好的消息内容
* Return    : void
************************************************************************/
void mkd_collectNewMsg(FlagStatus * isOtherShipMsgNewIn,FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct)
{
	u8 i = 0;
	if (* isOtherShipMsgNewIn == SET)//有新的他船消息接收到 VDM
	{
		mkd_collecVDLShipMsg(vdlOtherData,mkd_controlStruct,mkd_dataStruct,VDM);
		* isOtherShipMsgNewIn = RESET;
	}
	while(i < MAXOWNINFOLEN)
	{
		if (mkd_controlStruct->ownShipInfoStruct[i].vdlMsgState == MSGNEW)//有新的自船消息到 VDO
		{
			mkd_controlStruct->ownShipInfoBias = i;
			mkd_collecVDLShipMsg(vdlOtherData,mkd_controlStruct,mkd_dataStruct,VDO);
			mkd_controlStruct->ownShipInfoStruct[i].vdlMsgState = MSGOLD;
		}
		i ++;
	}
	//else if ()//有新的安全确认消息 ABK
	//{
	//}
	//else if ()//有新的报警消息TXT ALR
	//{
	//}
}

