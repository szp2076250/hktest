#pragma once
#include "String.h"
#include <windows.h>

/*
*	内存操作数的宽度（ 带中括号的操作数才有此属性，例如 MOV ecx,ptr byte [eax] ）
*/
class OperandWidth {
private:
	int widthValue = 0;				//宽度值:8、16、32、64.
	String widthStr="待赋值...";		//宽度字符串: word ptr = 16 , dword ptr = 32 ,  qword ptr = 64 .

public:

	//************************************
	// Method:     SetStr 
	// Description:强制设置宽度字符串（例如Bound指令需要设置其宽度为两倍：dword * 2 或 word * 2）
	// Parameter:  String widthStr - 
	// Returns:    void - 
	//************************************
	void SetStr(String widthStr)
	{
		this->widthStr = widthStr;
	}


	void SetValue(int widthValue)
	{
		this->widthValue = widthValue;
	}

	String ToString()
	{
		//如果宽度字符串尚未强制设置，则根据value设置string
		if (this->widthStr.Equals("待赋值...")) 
		{
			switch (this->widthValue)
			{
				case 0:widthStr = "";				break; //无内存操作数
				case 8:widthStr = "byte ptr ";		break;
				case 16:widthStr = "word ptr ";		break;
				case 32:widthStr = "dword ptr ";	break;
				case 64:widthStr = "qword ptr ";	break;
				default:MessageBox(0, TEXT("【error】内存操作数的宽度有误"), 0, 0); break;
			}
		}
		//如果宽度字符串已被强制设置，则直接返回字符串 (如Bound指令)
		return this->widthStr;
	}
};