/**
********************************************************************
* @file      main.c
* @author    Digital Design Team
* @version   V3.5.0
* @date      2015/11/23   16:07
********************************************************************
* @brief     
*
* Copyright (C) NanJing Sandemarine Electric Co., Ltd
* All Rights Reserved
*********************************************************************
*/
#include "stdio.h"
#include "FSMControl.h"
#include "testFSMControl.h"
#include "test.h"





void main()
{
	u8 i = 0;
	//��ʼ��״̬���ĳ�ʼֵ
	FSMInit(&fsm_controlStruct);

	//����Msg14
	//testMsg14(&(fsm_controlStruct.ais_bbmMsgStruct),&ais_rcvMsgIDStruct);

	//����Msg18 
	//workType��ȡֵ 1-����ģʽ  2-ѯ��ģʽ ָ������Ӧʱ϶  3-ѯ��ģʽ δָ����Ӧʱ϶ 
	//				 4-ָ��ģʽ��Msg20����  5-ָ��ģʽ��Msg22����,�����л���  6-ָ��ģʽ��Msg23δ�ڼž���
	//				 7-ָ��ģʽ��Msg23�ž���  8-�㲥Msg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л��� 
	//				 9-ѰַMsg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л���
	//				 10-�㲥Msg22��Msg23(δ�ڼž���)һ�����ã�Msg22���л���
	//				 11-Msg22����,���л���
	/*for (i = 1; i < 12; i ++)
	{
		testMsg18(i);
	}*/

	//����Msg19
	//testMsg19(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23);

	//����Msg24
	//workType��ȡֵ 1-����ģʽ  2-ѯ��ģʽ ָ������Ӧʱ϶  3-ѯ��ģʽ δָ����Ӧʱ϶ 
	//				 4-ָ��ģʽ��Msg20����  5-ָ��ģʽ��Msg22����  6-ָ��ģʽ��Msg23δ�ڼž���
	//				 7-ָ��ģʽ��Msg23�ž���  8-�㲥Msg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л��� 
	//				 9-ѰַMsg22��Msg23��δ�ڼž��ڣ�һ�����ã�Msg22�����л���

	/*for (i = 10; i < 10; i ++)
	{
		printf("the %d's test:\n",i);
		testMsg24(i); 
		
	}*/
	
	//����Msg20
	//workType��ȡֵ 1-�����µ�Msg20���룬����4���µı���ʱ϶    2-Msg20�Ĳ��ֱ���ʱ϶��ʱ
	//               3-Msg20��ȫ������ʱ϶��ʱ
	testMsg20(2);

	//����Msg22
	//testMsg22Function(2);//1-������ԣ�2-�л������ԣ�(�ŵ�����,�Ѿ���������Msg18��Msg24��,��������)

	//����Msg23 
	//functionNum��ȡֵ 1-��������Msg23��ָ������   2-������Msg23��ָ������ָ����Ч���ڼž���
	//					3-������Msg23��ָ������ָ����Ч�����ڼž���
	//initMsg23Struct(&fsm_controlStruct.fsm_msg23Struct);
	//testMsg23Function(3);


	printf("\n");
	printf("all functions are tested over!!!!\r\n");


	while (1)
	{

	}
	getchar();
	
}