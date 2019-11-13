#pragma once
#include "String.h"
#include "OperandWidth.h"
#include "Segment.h"
#include "PrefixSEG.h"

#define 操作数类型_带中括号的操作数 "操作数类型_带中括号的操作数"		// 内存寻址的操作数，都是带中括号的操作数，例如：MOV ecx , ptr dword [ecx]
#define 操作数类型_不带中括号的操作数 "操作数类型_不带中括号的操作数"		//

/*
* 操作数
*/
class Operand {
public:
	String 不包含宽度的操作数字符串;
	String 操作数类型;					//操作数类型: 带中括号的操作数 = 0 ; 不带中括号的操作数 = 1 .
	OperandWidth 带中括号的操作数的宽度;	//带中括号的操作数的宽度：word ptr =16 , dword ptr = 32 , qword ptr = 64.
	Segment 段;

	String ToString(PrefixSEG SEG)
	{
		if (操作数类型.Equals(操作数类型_带中括号的操作数))
		{
			return 带中括号的操作数的宽度.ToString() + 段.GetString(SEG) + 不包含宽度的操作数字符串;
		}
		else if (操作数类型.Equals(操作数类型_不带中括号的操作数))
		{
			return 不包含宽度的操作数字符串;
		}

		//return 带中括号的操作数的宽度.ToString() + 段.GetString(SEG) + 不包含宽度的操作数字符串;
	}
};
