#ifndef __temp_H
#define __temp_h

#include "myStdDef.h"

	typedef struct
	{
		u32 longitude;						//γ�ȣ�ʵ�ʳ���28bit,1/10000minΪ��λ
		u32 latitude;							//���ȣ�ʵ�ʳ���27bit,1/10000minΪ��λ
		u32  utcTime;							//utcʱ�䣬��ʽΪhhmmssss��ʽ
		u16 SOG;									//���溽�٣�ʵ�ʳ���10bit,1/10Ϊ��������Ч��Χ0-102.2��1023�����ã�1022=102.2�ڻ����
		u16 COG;									//���溽�ߣ�ʵ�ʳ���12bit,1/10��Ϊ��������Ч��Χ0-3599��3600=e10h�����ã�3601~4095Ӧ������
		_Bool raimFlag;						//���Ӷ�λװ�ã�0-δʹ�ã�1-����ʹ��
		
		
	}GPS_InfoStruct;//���ڴ�ž����㷨�����ľ�γ�ȡ�����ǵ�ֵ

#endif