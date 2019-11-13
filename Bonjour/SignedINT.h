#pragma once
#include <Windows.h>
class SignedINT {

public:
	char symbol = 0;	//有符号数的正负号部分
	DWORD64 digit = 0;	//有符号数的数字部分

	//************************************
	// Method:     GetInt32 
	// Description:将32位无符号数转为32位有符号数
	// Parameter:  UINT32 uInt32 - 
	// Returns:    void - 
	//************************************
	static SignedINT GetInt32(UINT32 uInt32)
	{
		SignedINT signedInt32;
		//得到有符号数的正负号部分：取首位
		signedInt32.symbol = uInt32 & 0x80000000 ? '-' : '+';
		//得到有符号数的数字部分：(负数)补码减一取反，去掉符号位。
		signedInt32.digit = (signedInt32.symbol == '-') ? ~(uInt32 - 1) & 0x7FFFFFFF : uInt32;
		return signedInt32;
	}
	//************************************
	// Method:     GetInt8 
	// Description:将8位无符号数转为8位有符号数
	// Parameter:  UINT8 uInt8 - 
	// Returns:    void - 
	//************************************
	static SignedINT GetInt8(UINT8 uInt8)
	{
		SignedINT signedInt8;
		//得到有符号数的正负号部分：取首位
		signedInt8.symbol = uInt8 & 0x80 ? '-' : '+';
		//得到有符号数的数字部分：(负数)补码减一取反，去掉符号位。
		signedInt8.digit = (signedInt8.symbol == '-') ? ~(uInt8 - 1) & 0x7F : uInt8;
		return signedInt8;
	}
};