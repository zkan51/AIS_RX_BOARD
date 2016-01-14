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
#include <stdlib.h>
#include "AISMissionGeneratorTest.h"
#include "AISMissionGenerator.h"


void testBitStuffing(){
	u8 a[] = {255, 254, 20, 127, 229 , 33, 45, 67, 90};
	u8 b[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int dstNum;
	int i;
	int offset = 8;
	dstNum = bitStuff(a, 72, b, offset);
	printf("\n 去比特填充后的比特数为 ----     %d\n\n\n", dstNum);
	for(i = 0; i < dstNum+offset; i++){
		printf("%d, ", getBitValue(b, i));
	}

	printf("\n\n");
}

void testCrcCalculator(){
	u8 a[] = {245, 247, 40, 248, 245, 124, 204, 36, 107, 234, 202, 245, 167, 9, 217, 239, 173, 193, 190, 100};
	u8 crc[] = {0x03, 0x57};
	crcCalculator(a, 20, crc);
	printf("CRC 结果为 ---- \n\n");
	printf("           %d,    %d\n\n", crc[0], crc[1]);
}

void testAISFraming(){
	u8 msg[100];
	char *filePath = "testFile.txt";
	int numSrcBytes;
	int numDstBytes;
	int i;
	numSrcBytes = readDataFromFile(filePath, msg);
	initMissionDatas();
	numDstBytes = aisFraming(msg, numSrcBytes, missionDatas);
	printf("\n    组帧后的帧长为   ---  %d\n\n", numDstBytes);
	for (i = 0; i < numDstBytes; i++)
	{
		printf("%d, ", missionDatas[0].frameData[i]);
	}
	printf("\n\n");
}

/************************************************************************
* Name      : testMissionGen
* Brief     : 
*		测试任务的生成过程
* Author    : ZuoDahua
* Return    : void
************************************************************************/
void testMissionGen(){

}

