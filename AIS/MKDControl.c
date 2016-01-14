/**
********************************************************************
* @file      MKDControl.c
* @author    Digital Design Team
* @version   
* @date      2015/12/2   20:17
********************************************************************
* @brief     ����MCU����λ��֮��Ľ������ܣ�������
*			 1�������������Ļ�ȡ��ԭʼ���ݵķ�װ�Ͱ���61162��ʽ�Ĵ�������ϴ�����λ��
*			 2����λ����ȫ�����ݵĴ�������´�������͸�MCU����ȷ�Ϸ�����Ϣ
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "MKDControl.h"
#include "testMKDControl.h"

////////////////////////////////////����������////////////////////////////////////////////////////

MKD_ControlStruct mkd_controlStruct;
MKD_DataStruct mkd_dataStruct;

/************************************************************************
* Name      : rstMKDFramedStruct
* Brief     : ���mkd_framedStruct�����ݣ����ڴ���µ���Ϣ
*
* Author    : Digital Design Team
* Param     : mkd_framedStruct-��֡�ṹ�壬���ڴ��61162��װ�õ���Ϣ���ݼ��丨����Ϣ
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
* Brief     : ���mkd_toBeframeStruct�����ݣ����ڴ���µ���Ϣ
*
* Author    : Digital Design Team
* Param     : mkd_toBeframeStruct-����֡�ṹ�壬������ʱ�����֡ǰ�����������Ϣ
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
* Brief     : ����61162Э�飬��6bit��ASCIIm��ת��Ϊ8bit��ASCII��
*
* Author    : Digital Design Team
* Param     : len-6bitASCII��ĳ���
* Param     : sour-61162�е�6bitASCII��Ĵ�ŵ�ַ
* Param     : dest-61162�е�8bitASCII��Ĵ�ŵ�ַ
* Return    : void
************************************************************************/
void chnage611626bitASCIIto8bitASCII(u8 len,u8 * sour,u8 * dest)
{
	u8 i = 0;
	//�Ƚ�61162�е�6bitASCII��ת���ɱ�׼8bitASCII��
	for (i = 0; i < len; i ++)
	{
		if (*(sour+i) < 40) //ֵС��101000
		{
			*(dest+i) = *(sour+i) + 48;//����00110000 
		}
		else//ֵ���ڵ���101000
		{
			*(dest+i) = *(sour+i) + 56;//����00111000
		}
	}
}

/************************************************************************
* Name      : change1371ContentsTo61162
* Brief     : ��1371Э��ı������ݣ����ն�Ӧ��ϵת��Ϊ61162Э���е�6����ASCII���װ����
*
* Author    : Digital Design Team
* Param     : fsm_frameStruct-1371����Ϣ��װ����
* Param     : mkd_toBeframeStruct-61162����֡�������Ϣ���ݣ�δ�����֡
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
	//��Ҫ�����mkd_toBeframeStruct �е�ֵ
	rstMKDContentToBeFramedStruct(mkd_toBeframeStruct);
	for (i = 0; i < (fsm_frameStruct->encapDataLen/8); i += 3)
	{
		//�Ƚ�1371Э�����ݣ�����3�ֽ�ȡ����������3�ֽڣ���Ҫ��֤ȡ��������Ϊ6�����������������㣬����ĩβ��0
		//�ֽڰ����±�ӵ͵���ȡ���������ΰ��մӵ͵��ߴ�ŵ�tmp13716bit��
		if (i+2 > fsm_frameStruct->encapDataLen/8)//1371�еķ�װ��Ϣ�ֽ�����������Ϊ3�ı���
		{
			//������1���ֽ�,��4bit ���Ϊ
			if ((fsm_frameStruct->encapDataLen/8 - i) == 1)
			{
				//tmpThr8bits = fsm_frameStruct->encapDataByte[i]<<4;   
				mkd_toBeframeStruct->fillBits = 4;
				tmp13716bit[0] = (fsm_frameStruct->encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (fsm_frameStruct->encapDataByte[i] & 0x03)<<4;
				tmp13716bitLen = 2;

			}
			else//������2���ֽ�,��2bit (fsm_frameStruct->encapDataLen/8 - i) == 2
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
		else//����ȡ�������ֽ�
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
		//��1371�еĶ������룬ֱ����ϳ�61162�е�6bit(��ʱ��������������ԣ��ٸ�),��ת��Ϊ61162�е�8bitASCII��
		chnage611626bitASCIIto8bitASCII(tmp13716bitLen,tmp13716bit,&mkd_toBeframeStruct->frameEncapContentByte[bias]);
		bias += tmp13716bitLen;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = bias;
}

/************************************************************************
* Name      : changeOtherShipContentTo61162
* Brief     : ��FPGA�����ϱ���vdl��Ϣ�е�8�ֽ����ݣ�ת��Ϊ61162Э���е�6bitASCII�����ʽ
*
* Author    : Digital Design Team
* Param     : otherShipMsg-FPGA���ڽ��յ���������ԭʼ�������ݣ���Ҫʹ�����еķ�װ������Ϣ
* Param     : mkd_toBeframeStruct-61162����֡��Ϣ����Ҫʹ�����д�Ž�1371�����������޸�Ϊ61162��Ӧ��6bit ASCII��ķ�װ����
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
	//��Ҫ�����mkd_toBeframeStruct �е�ֵ
	rstMKDContentToBeFramedStruct(mkd_toBeframeStruct);
	for (i = 0; i < otherShipMsg->frameBytes; i += 3)
	{
		//�Ƚ�1371Э�����ݣ�����3�ֽ�ȡ����������3�ֽڣ���Ҫ��֤ȡ��������Ϊ6�����������������㣬����ĩβ��0
		//�ֽڰ����±�ӵ͵���ȡ���������ΰ��մӵ͵��ߴ�ŵ�tmp13716bit��
		if (i+3 > otherShipMsg->frameBytes)//1371�еķ�װ��Ϣ�ֽ�����������Ϊ3�ı���
		{
			//������1���ֽ�,��4bit ���Ϊ
			if ((otherShipMsg->frameBytes - i) == 1)
			{  
				mkd_toBeframeStruct->fillBits = 4;
				tmp13716bit[0] = (otherShipMsg->frameData[i] & 0xFC)>>2;
				tmp13716bit[1] = (otherShipMsg->frameData[i] & 0x03)<<4;
				tmp13716bitLen = 2;

			}
			else//������2���ֽ�,��2bit (fsm_frameStruct->encapDataLen/8 - i) == 2
			{

				mkd_toBeframeStruct->fillBits = 2;
				tmp13716bit[0] = (otherShipMsg->frameData[i] & 0xFC)>>2;
				tmp13716bit[1] = (otherShipMsg->frameData[i] & 0x03)<< 4;
				tmp13716bit[1] += (otherShipMsg->frameData[i+1] & 0xF0)>>4;
				tmp13716bit[2] = (otherShipMsg->frameData[i+1] & 0x0F) << 2;
				tmp13716bitLen = 3;
			}
		}
		else//����ȡ�������ֽ�
		{
			tmp13716bit[0] = (otherShipMsg->frameData[i] & 0xFC)>>2;
			tmp13716bit[1] = (otherShipMsg->frameData[i] & 0x03)<< 4;
			tmp13716bit[1] += (otherShipMsg->frameData[i+1] & 0xF0)>>4;
			tmp13716bit[2] = (otherShipMsg->frameData[i+1] & 0x0F) << 2;
			tmp13716bit[2] += (otherShipMsg->frameData[i+2] & 0xC0)>>6;
			tmp13716bit[3] = (otherShipMsg->frameData[i+2] & 0x3F);
			tmp13716bitLen = 4;

		}
		//��1371�еĶ������룬ֱ����ϳ�61162�е�6bit(��ʱ��������������ԣ��ٸ�),��ת��Ϊ61162�е�8bitASCII��
		chnage611626bitASCIIto8bitASCII(tmp13716bitLen,tmp13716bit,&mkd_toBeframeStruct->frameEncapContentByte[bias]);
		bias += tmp13716bitLen;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = bias;
	// ���Դ��룬����ע��
	testChangeOtherShipContentTo61162(otherShipMsg,mkd_toBeframeStruct);
}
/************************************************************************
* Name      : changeOwnShipContentTo61162
* Brief     : ���������ɵ�vdl��Ϣ�е�8�ֽ����ݣ�ת��Ϊ61162Э���е�6bitASCII�����ʽ
*
* Author    : Digital Design Team
* Param     : ownShipInfo_struct- �����в����ļ������͵�VDL��·�ϵ�1371����Ϣ���ݣ���Ҫ��¼������֡�ķ�װ�������ݣ����Ⱥ��ŵ�
* Param     : mkd_toBeframeStruct- 61162����֡��Ϣ����Ҫʹ�����д�Ž�1371�����������޸�Ϊ61162��Ӧ��6bit ASCII��ķ�װ����
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
	//��Ҫ�����mkd_toBeframeStruct �е�ֵ
	rstMKDContentToBeFramedStruct(mkd_toBeframeStruct);
	for (i = 0; i < ownShipInfo_struct->vdl_encapDataByteLen; i += 3)
	{
		//�Ƚ�1371Э�����ݣ�����3�ֽ�ȡ����������3�ֽڣ���Ҫ��֤ȡ��������Ϊ6�����������������㣬����ĩβ��0
		//�ֽڰ����±�ӵ͵���ȡ���������ΰ��մӵ͵��ߴ�ŵ�tmp13716bit��
		if (i+3 > ownShipInfo_struct->vdl_encapDataByteLen)//1371�еķ�װ��Ϣ�ֽ�����������Ϊ3�ı���
		{
			//������1���ֽ�,��4bit ���Ϊ
			if ((ownShipInfo_struct->vdl_encapDataByteLen - i) == 1)
			{  
				mkd_toBeframeStruct->fillBits = 4;
				tmp13716bit[0] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0x03)<<4;
				tmp13716bitLen = 2;

			}
			else//������2���ֽ�,��2bit (fsm_frameStruct->encapDataLen/8 - i) == 2
			{

				mkd_toBeframeStruct->fillBits = 2;
				tmp13716bit[0] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0xFC)>>2;
				tmp13716bit[1] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0x03)<< 4;
				tmp13716bit[1] += (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0xF0)>>4;
				tmp13716bit[2] = (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0x0F) << 2;
				tmp13716bitLen = 3;
			}
		}
		else//����ȡ�������ֽ�
		{
			tmp13716bit[0] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0xFC)>>2;
			tmp13716bit[1] = (ownShipInfo_struct->vdl_encapDataByte[i] & 0x03)<< 4;
			tmp13716bit[1] += (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0xF0)>>4;
			tmp13716bit[2] = (ownShipInfo_struct->vdl_encapDataByte[i+1] & 0x0F) << 2;
			tmp13716bit[2] += (ownShipInfo_struct->vdl_encapDataByte[i+2] & 0xC0)>>6;
			tmp13716bit[3] = (ownShipInfo_struct->vdl_encapDataByte[i+2] & 0x3F);
			tmp13716bitLen = 4;

		}
		//��1371�еĶ������룬ֱ����ϳ�61162�е�6bit(��ʱ��������������ԣ��ٸ�),��ת��Ϊ61162�е�8bitASCII��
		chnage611626bitASCIIto8bitASCII(tmp13716bitLen,tmp13716bit,&mkd_toBeframeStruct->frameEncapContentByte[bias]);
		bias += tmp13716bitLen;
	}
	mkd_toBeframeStruct->frameEncapContentByteLen = bias;
	mkd_toBeframeStruct->aisChannel = ownShipInfo_struct->vdl_channel;//
	// ���Դ��룬����ע�� 
	//testChangeOwnShipContentTo61162(ownShipInfo_struct,mkd_toBeframeStruct);
}
/************************************************************************
* Name      : updataToBeFrameInfo
* Brief     : ����FPGA���ڽ��յ�vdl��Ϣ��׼��61162Э���ʽ����Ҫ��֡ͷ��֡β��������
*
* Author    : Digital Design Team
* Param     : mkd_toBeframeStruct- ��Ҫ��װ��61162Э���ʽ��׼����Ϣ������Ϣ��ʹ�ó��˷�װ��Ϣ���������������������
* Param     : id-����ָʾ��ǰ��Ҫ���ɵ���ϢID�ţ� 
* Return    : void
************************************************************************/
void updataToBeFrameInfo(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_IdIndicator id)
{
	u8 i = 0;
	if (id == VDM || id == VDO)
	{
		mkd_toBeframeStruct->frameType = '!';//��!������
	}
	else //ABK TXT ALR
	{
		mkd_toBeframeStruct->frameType = '$';//��$������
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
	if (mkd_toBeframeStruct->frameEncapContentByteLen < 60)//һ���ֶ����
	{
		mkd_toBeframeStruct->totalNum = 1;
	}
	else if (mkd_toBeframeStruct->frameEncapContentByteLen < 120)//�����ֶ����
	{
		mkd_toBeframeStruct->totalNum = 2;
	}
	else//������Ϊ�ֶ������ᳬ��3��
	{
		mkd_toBeframeStruct->totalNum = 3;
	}
	mkd_toBeframeStruct->sequenceNum = 0;//(mkd_toBeframeStruct->sequenceNum + 1)%10;Ҫ��һ����Ϣ�ĸ�ʽ�͸���Ϣ��ʽһ�£����ü�1�����򲻲���
}
/************************************************************************
* Name      : updataVDMorVDOFrame
* Brief     : ��61162�������ݷֿ���ŵĴ���֡��Ϣ�������һ�������֡��������У��λ�����Ӧλ
*
* Author    : Digital Design Team
* Param     : mkd_toBeframeStruct- ����֡����Ϣ����
* Param     : mkd_dataStruct-��λ�������ݽṹ�壬���ڴ����֡��ɺ����Ӧ��Ϣ����
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
	while (subSection < mkd_toBeframeStruct->totalNum)//subSection ��Ӧ��Ӧ�ķֶ�˳���
	{
		//����mkd_frameStruct�ṹ��
		subSection ++;
		//���ҵ�һ���ɹ�д����µĽṹ��
		i = 0;
		while (mkd_dataStruct->mkd_framedStruct[i].mkd_msgState != MSGOLD)
		{
			i++;
		}
		if (i < MAXMKDFRAMELEN)//���пɹ�д��Ľṹ��ռ�
		{
			rstMKDFramedStruct(&mkd_dataStruct->mkd_framedStruct[i]);
			bias = 0;
			if (subSection == 1 && mkd_toBeframeStruct->frameEncapContentByteLen > 60)//���ʱ����Ҫ�ֳɶ���ֶ�  ��ʱ��Ϊû�г���3��ʱ϶�ĳ���
			{
				subSectionLen = 60;
			}
			else if (subSection == 2 && mkd_toBeframeStruct->frameEncapContentByteLen > 120)
			{
				subSectionLen = 60;
			}
			else //��һ���ڶ������ߵ������ֶβ���60�������������
			{
				subSectionLen = mkd_toBeframeStruct->frameEncapContentByteLen - (subSection-1)*60;
			}
			while(bias < 15+subSectionLen) //to be changed
			{
				switch (bias)
				{
				case 0: //�� ��Ϣ����1
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->frameType;
						bias = 1;
						break;
					}
				case 1: //aaccc ��Ϣ����2
					{
						bias = 6;
						while(mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen < bias)
							mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->frameHead[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen -1];
						break;
					}
				case 7: //�ֶܷ��� 1~9
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->totalNum+0x30;//�仯Ϊ��Ӧ��ASCII��
						bias = 8;
						break;
					}
				case 9: //�ֶ�˳��� 1~9
					{
						//�����ж���ֶ�
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = subSection+0x30;
						bias = 10;
						break;
					}
				case 11://��Ϣ���к� 0~9 
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = mkd_toBeframeStruct->sequenceNum+0x30;
						bias = 12;
						break;
					}
				case 13: //AIS�����ŵ�
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->aisChannel+0x41;//ת��ΪA��B
						bias = 14;
						break;
					}
				case 15://��װ��1371����Ϣ   //�����װ��Ϣ���ȳ�����60������Ҫ�ֶγɶ��
					{
						bias = 15+subSectionLen;
						biasSub = (subSection-1)*60;
						while(mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen < bias)
							mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++]
							= mkd_toBeframeStruct->frameEncapContentByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen+biasSub-15];
						break;
					}
				case 6:case 8:case 10:case 12:case 14: //�ָ��õ�','
					{
						mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0x2C;
						bias += 1;
						break;
					}
				default:break;//���ó��ֵ����
				}
			}
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0x2C;//��װ1371��Ϣ���ݺ��','
			//���Ӧ���������һ���ֶ�����ӣ������������ֶ��У�����Ҫ
			if (subSection == mkd_toBeframeStruct->totalNum)//�Ѿ������һ���ֶ���
			{
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = mkd_toBeframeStruct->fillBits+0x30;//�������
			}
			else//�������һ���ֶ�
			{
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0+0x30;//�������
			}
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen++] = 0x2A;//У���ǰ��'*'����
			j = 1;
			tmpCheck = 0;
			while (j < mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen-1)//������ˣ� * ���������ţ���������������֮��ķ�װ��Ϣ�������
			{
				tmpCheck ^=  mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[j];
				j ++;
			}
			tmp = (tmpCheck &0xF0)>>4;
			if(tmp < 10)//0~9
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x30;//0~9��48��0x30)��ӦΪ0x30~0x39
			else
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x37;//A~F��55��0x37����ӦΪ0x65~0x70
			tmp = tmpCheck &0x0F;
			if(tmp < 10)//0~9
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x30;//0~9��48��0x30)��ӦΪ0x30~0x39
			else
				mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmp + 0x37;//A~F��55��0x37����ӦΪ0x65~0x70
			//mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = tmpCheck;//���У���
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = 0x0D;//��Ž�����
			mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByte[mkd_dataStruct->mkd_framedStruct[i].mkd_encapDataByteLen ++] = 0x0A;
			mkd_dataStruct->mkd_framedStruct[i].mkd_msgState = MSGNEW;
			mkd_dataStruct->mkd_framedStruct[i].mkd_broadChannel = mkd_toBeframeStruct->aisChannel;
			//�����ã�����ע���
			testUpdataVDMorVDOFrame(mkd_toBeframeStruct,&mkd_dataStruct->mkd_framedStruct[i]);
		}		
	}
}

/************************************************************************
* Name      : MKDInit
* Brief     : ��ʼ����λ���Ŀ��ƹ���״̬���趨�ʼû���µ���Ϣ����Ӧ����Ϣ״̬ȫ���趨ΪMSGOLD��
*
* Author    : Digital Design Team
* Param     : mkd_controlStruct-��λ�����ƹ��ܵĿ��ƽṹ��
* Param     : mkd_dataStruct-��λ�������ݽṹ�壬���ڴ������λ���ϱ�������Ϣ����
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
* Brief     : ��״̬������ȡ���µ�Ҫ���͵�VDL��·����µķ�װ��Ϣ���ݣ��������ŵ���λ�����ƽṹ�е���Ӧλ�ã��Թ�����������Ӧ��VDO��Ϣ
*
* Author    : Digital Design Team
* Param     : mkd_controlStruct- ��λ���Ŀ��ƽṹ�壬ʹ�����еĴ��1371��װ��ϢownShipInfoStruct�ṹ������ 
* Param     : fsm_dataStruct- ״̬����������ݽṹ�壬ʹ�����е��²����Ĵ����͵�1371��װ��Ϣ�ṹ��
* Return    : void
************************************************************************/
void getOwn61162ToBeFramedInfo(MKD_ControlStruct * mkd_controlStruct,FSM_DataStruct * fsm_dataStruct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	while(i < FRAMELENGTH)//�ж�һ��ʱ϶����Ϣ
	{
		if (fsm_dataStruct->fsm_frameStruct[i].msgState == MSGNEW)// ���µ�FSM����Ϣ
		{
			//����װ�����ݣ����ȣ��ŵ�������λ���Ļ�����
			j = 0;
			while (mkd_controlStruct->ownShipInfoStruct[j].vdlMsgState != MSGOLD)
			{
				j ++;
				if (j >= MAXOWNINFOLEN)
					break;
			}
			if (j < MAXOWNINFOLEN)//�ҵ�һ���µĿ��Ը��ǵĽṹ��
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
	if (fsm_dataStruct->fsm_frame19Struct.msgState == MSGNEW)//����Ϣ19������Ϣ
	{
		//����װ�����ݣ����ȣ��ŵ�������λ���Ļ�����
		j = 0;
		while (mkd_controlStruct->ownShipInfoStruct[j].vdlMsgState != MSGOLD)
		{
			j ++;
			if (j >= MAXOWNINFOLEN)
				break;
		}
		if (j < MAXOWNINFOLEN)//�ҵ�һ���µĿ��Ը��ǵĽṹ��
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
* Brief     : �ռ�������Ϣ�����У�����ת��Ϊ����61162Э���ʽ����������
*
* Author    : Digital Design Team
* Param     : vdlOtherData-���յ�������������Ϣ�Ľṹ�壬������61162Э����Ϣ�����ȫ��ԭʼ������Ϣ
* Param     : mkd_controlStruct-��λ�����ƽṹ�壬���ڴ���м������Ϣ
* Param     : mkd_dataStruct-��λ�����ݽṹ�壬���ڴ�Ű���61162Э����֡��ɵķֶ���Ϣ����
* Param		: id- ����ָʾ����61162��Ϣ�ı�ʶ
* Return    : void
************************************************************************/
void mkd_collecVDLShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,MKD_IdIndicator id)
{
	//������������Ϣʱ
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
* Brief     : �ռ���λ���½��յ��½��յ���Ӧ��Ϣ��������Ϣ����61162��Э��涨���ո�ʽ���з�װ
*
* Author    : Digital Design Team
* Param     : isOtherShipMsgNewIn- FPGA���ڽ��յ�����vdl����Ϣ��־
* Param     : vdlOtherData- FPGA���ڽ��յ�����vdl����Ϣ�ľ�������
* Param     : mkd_controlStruct- ��λ���Ŀ��ƽṹ�壬���ڴ���м���������
* Param     : mkd_dataStruct- ��λ�������ݽṹ�壬���ڴ�Ű���61162Э����֡�õ���Ϣ����
* Return    : void
************************************************************************/
void mkd_collectNewMsg(FlagStatus * isOtherShipMsgNewIn,FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct)
{
	u8 i = 0;
	if (* isOtherShipMsgNewIn == SET)//���µ�������Ϣ���յ� VDM
	{
		mkd_collecVDLShipMsg(vdlOtherData,mkd_controlStruct,mkd_dataStruct,VDM);
		* isOtherShipMsgNewIn = RESET;
	}
	while(i < MAXOWNINFOLEN)
	{
		if (mkd_controlStruct->ownShipInfoStruct[i].vdlMsgState == MSGNEW)//���µ��Դ���Ϣ�� VDO
		{
			mkd_controlStruct->ownShipInfoBias = i;
			mkd_collecVDLShipMsg(vdlOtherData,mkd_controlStruct,mkd_dataStruct,VDO);
			mkd_controlStruct->ownShipInfoStruct[i].vdlMsgState = MSGOLD;
		}
		i ++;
	}
	//else if ()//���µİ�ȫȷ����Ϣ ABK
	//{
	//}
	//else if ()//���µı�����ϢTXT ALR
	//{
	//}
}

