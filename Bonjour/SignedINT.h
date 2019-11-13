#pragma once
#include <Windows.h>
class SignedINT {

public:
	char symbol = 0;	//�з������������Ų���
	DWORD64 digit = 0;	//�з����������ֲ���

	//************************************
	// Method:     GetInt32 
	// Description:��32λ�޷�����תΪ32λ�з�����
	// Parameter:  UINT32 uInt32 - 
	// Returns:    void - 
	//************************************
	static SignedINT GetInt32(UINT32 uInt32)
	{
		SignedINT signedInt32;
		//�õ��з������������Ų��֣�ȡ��λ
		signedInt32.symbol = uInt32 & 0x80000000 ? '-' : '+';
		//�õ��з����������ֲ��֣�(����)�����һȡ����ȥ������λ��
		signedInt32.digit = (signedInt32.symbol == '-') ? ~(uInt32 - 1) & 0x7FFFFFFF : uInt32;
		return signedInt32;
	}
	//************************************
	// Method:     GetInt8 
	// Description:��8λ�޷�����תΪ8λ�з�����
	// Parameter:  UINT8 uInt8 - 
	// Returns:    void - 
	//************************************
	static SignedINT GetInt8(UINT8 uInt8)
	{
		SignedINT signedInt8;
		//�õ��з������������Ų��֣�ȡ��λ
		signedInt8.symbol = uInt8 & 0x80 ? '-' : '+';
		//�õ��з����������ֲ��֣�(����)�����һȡ����ȥ������λ��
		signedInt8.digit = (signedInt8.symbol == '-') ? ~(uInt8 - 1) & 0x7F : uInt8;
		return signedInt8;
	}
};