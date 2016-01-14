/**
********************************************************************
* @file      MKDControl.h
* @author    Digital Design Team
* @version   
* @date      2015/12/2   19:32
********************************************************************
* @brief     ����MKDControl.c��ʹ�õĽṹ��ͺ����ӿ�
*			
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#ifndef _MKDControl_H
#define _MKDControl_H


#include "myStdDef.h"
#include "AIS_PS_Interface.h"
#include "mkdData.h"
#include "gps.h"


#include "FSMControl.h"

////////////////////////////////////define ������////////////////////////////////////////////////////
#define MAXMKDFRAMELEN 10//һ����λ��ִ�м�����Ի������������λ���ϱ�����Ϣ��
#define MAXOWNINFOLEN 3	 //һ����λ��ִ�м���У����Ի������������λ���ϱ��ı�������Ϣ��

////////////////////////////////////enum ������////////////////////////////////////////////////////

typedef enum{

	safeCfmMsg = 0x11,//��ȫ��Ϣ��ȷ����Ϣ


}MKD_MsgTypeIndicator;

typedef enum{
	VDM = 1,
	VDO = 2,
	ABK = 3,
	ALR = 4,
	TXT = 5
}MKD_IdIndicator;

////////////////////////////////////struct ������////////////////////////////////////////////////////
typedef struct{
	u8 frameType;//�������� �̶���ʽ$�������Ƿǹ̶���ʽ!
	u8 frameHead[5];//���ڴ����Ϣ���ͺ�
	u8 totalNum;//�ֶܷ�����ȡֵ1~9
	u8 sentenceNum;//���ֶܷ����е�˳��ţ�ȡֵ1~9
	u8 aisChannel;//��AIS�ŵ��Ϸ��͵�ָʾ����A��B��ָ��
	u8 sequenceNum;//�������ֲ�ͬ��Ϣ�����кţ���0~9֮��ѭ��
	u8 frameEncapContentByte[120];//���1371����61162Э���ʽ��Ӧ����Ϣ���� ,���ܻᳬ��1��ʱ϶���� 60+60
	u8 frameEncapContentByteLen;//���1371��Ϣת��Ϊ61162Э���װ��Ϣ��6���صĳ���
	u8 fillBits;			//Ϊ��֤61162�з�װ��1371��Ϣ����Ϊ6����������ȡֵΪ0~5
}MKD_ContentToBeFramedStruct;

typedef struct{

	u8 mkd_encapDataByte[82];//61162Э�����ݣ������ֽڵ���ʽ���д洢 
	//��װ���ݰ���Э������˳�򣬰��������±�ӵ͵��ߣ�˳������д��
	u8 mkd_encapDataByteLen;//��װ���ݳ���  �ֽڳ���
	BchannelIndicator mkd_broadChannel;//�㲥�ŵ�   
	MKD_MsgTypeIndicator mkd_msgType;//��Ϣ����   �������ְ�ȫ����Ϣ��һ����Ϣ������ǰ�ȫ����Ϣ��ȷ����Ϣ��������Ҫ�����ϴ�����λ�������ȴ�   
	MsgHandleState mkd_msgState;//��Ϣ�����״̬  ʹ��MSGNEW-��������Ϣ��û���ϴ�����λ�� MSGOLD-�Ѿ��ϴ�����λ����������Ϣ���Ը��Ǹýṹ�� 

}MKD_FramedStruct;// ��λ���ж���������ͱ������ϱ���ʽ��Ϣ�������־������ݣ����ڶ���Ϣ��һ֡����һ���ֶΣ��е���Ϣ�����ж���ֶ�


//typedef struct{


//}MKD_LFrameStruct;//��λ���ж�����Ҫ��һ����Ϣ�ֳɶ�����鷢�͵���Ϣ����ʱû���뵽��Ӧ���ô�



typedef struct  
{
	u8 vdl_encapDataByte[40];//FPGA���ڽ��յ�������vdl��·��������Ϣ�����ǵ�1371�е���Ϣ19����ռ��2��ʱ϶����󳤶���312/8=39
	u8 vdl_encapDataByteLen;//FPGA���ڽ��յ�����vdl��·��Ϣ���ֽڳ���
	u8 vdl_channel;//��Ϣ�Ľ����ŵ���A��B
	MsgHandleState vdlMsgState;// ������Ϣ�Ĵ���״̬��MSGNEW ���� MSGOLD

}OwnShipInfo_Struct;//���յ��Դ���vdl��װ��Ϣ

typedef struct{

	MKD_ContentToBeFramedStruct mkd_toBeFrameStruct;//��Ŵ���֡��ͨ����Ϣ�ṹ��
	OwnShipInfo_Struct ownShipInfoStruct[MAXOWNINFOLEN];//�Դ���ŵ�vdl��װ��Ϣ�ṹ������
	u8 ownShipInfoBias;//�Դ�vdl��Ϣ�ṹ������Ϣ�����ƫ�ã�ȡֵΪ0~MAXOWNINFOLEN-1

}MKD_ControlStruct;

typedef struct{

	MKD_FramedStruct mkd_framedStruct[MAXMKDFRAMELEN];//��Ŵ���õ�����λ���㱨������61162��ʽ����Ϣ

}MKD_DataStruct;//��λ���д�ŵ����ں���λ����������Ϣ���丨������




////////////////////////////////////�����ӿ�����////////////////////////////////////////////////////

extern MKD_ControlStruct mkd_controlStruct;
extern MKD_DataStruct mkd_dataStruct;



////////////////////////////////////function ������////////////////////////////////////////////////////
void rstMKDFramedStruct(MKD_FramedStruct * mkd_framedStruct);
void rstMKDContentToBeFramedStruct(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);

void chnage611626bitASCIIto8bitASCII(u8 len,u8 * sour,u8 * dest);
void change1371ContentsT61162(FSM_FrameStruct * fsm_frameStruct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);
void changeOtherShipCntentTo61162(FPGAVDLData * otherShipMsg,MKD_ContentToBeFramedStruct *mkd_toBeframeStruct);
void changeOwnShipContentTo61162(OwnShipInfo_Struct * ownShipInfo_struct,MKD_ContentToBeFramedStruct * mkd_toBeframeStruct);
void updataToBeFrameInfo(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_IdIndicator id);

void updataVDMorVDOFrame(MKD_ContentToBeFramedStruct * mkd_toBeframeStruct,MKD_DataStruct * mkd_dataStruct);

void MKDInit(MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct);
void getOwn61162ToBeFramedInfo(MKD_ControlStruct * mkd_controlStruct,FSM_DataStruct * fsm_dataStruct);
//void mkd_collectOtherShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct);
void mkd_collecVDLShipMsg(FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct,MKD_IdIndicator id);
void mkd_collectNewMsg(FlagStatus * isOtherShipMsgNewIn,FPGAVDLData * vdlOtherData,MKD_ControlStruct * mkd_controlStruct,MKD_DataStruct * mkd_dataStruct);


////////////////////////////////////others////////////////////////////////////////////////////
#endif
