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
	//初始化状态机的初始值
	FSMInit(&fsm_controlStruct);

	//测试Msg14
	//testMsg14(&(fsm_controlStruct.ais_bbmMsgStruct),&ais_rcvMsgIDStruct);

	//测试Msg18 
	//workType的取值 1-自主模式  2-询问模式 指定了响应时隙  3-询问模式 未指定响应时隙 
	//				 4-指配模式，Msg20作用  5-指配模式，Msg22作用,不在切换区  6-指配模式，Msg23未在寂静期
	//				 7-指配模式，Msg23寂静期  8-广播Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区 
	//				 9-寻址Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区
	//				 10-广播Msg22和Msg23(未在寂静期)一起作用，Msg22在切换区
	//				 11-Msg22作用,在切换区
	/*for (i = 1; i < 12; i ++)
	{
		testMsg18(i);
	}*/

	//测试Msg19
	//testMsg19(&gps_infoStruct,&ais_staticDataStruct,&recVDLMsg15,&recVDLMsg20,&recVDLMsg22,&recVDLMsg23);

	//测试Msg24
	//workType的取值 1-自主模式  2-询问模式 指定了响应时隙  3-询问模式 未指定响应时隙 
	//				 4-指配模式，Msg20作用  5-指配模式，Msg22作用  6-指配模式，Msg23未在寂静期
	//				 7-指配模式，Msg23寂静期  8-广播Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区 
	//				 9-寻址Msg22和Msg23（未在寂静期）一起作用，Msg22不在切换区

	/*for (i = 10; i < 10; i ++)
	{
		printf("the %d's test:\n",i);
		testMsg24(i); 
		
	}*/
	
	//测试Msg20
	//workType的取值 1-设置新的Msg20输入，设置4组新的保留时隙    2-Msg20的部分保留时隙超时
	//               3-Msg20的全部保留时隙超时
	testMsg20(2);

	//测试Msg22
	//testMsg22Function(2);//1-区域测试，2-切换区测试，(信道测试,已经包含在了Msg18和Msg24中,不单独测)

	//测试Msg23 
	//functionNum的取值 1-本船不在Msg23的指配区域   2-本船在Msg23的指配区域，指配有效，在寂静期
	//					3-本船在Msg23的指配区域，指配有效，不在寂静期
	//initMsg23Struct(&fsm_controlStruct.fsm_msg23Struct);
	//testMsg23Function(3);


	printf("\n");
	printf("all functions are tested over!!!!\r\n");


	while (1)
	{

	}
	getchar();
	
}