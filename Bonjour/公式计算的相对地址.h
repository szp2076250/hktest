#pragma once

#include <windows.h>

//【计算公式】目标地址 = 下一条指令的地址 + 有符号数偏移
class 公式计算的相对地址
{
private:
	PBYTE 目标地址 = 0;
	PBYTE 原指令地址 = 0;
	int 原指令长度 = 0;
	PBYTE 下一条指令地址 = 0;

	/*
		例如：硬编码 39 1D 0A 59 03 00
			其opcode = 39 1D
			其偏移值 = 0x0003590A
			其偏移的硬编码长度 = 4 字节
	*/
	INT64 偏移值 = 0;	//是有符号数
	PBYTE 指向偏移的指针 = 0;
	int 偏移的硬编码字节数 = 0;

public:

	BOOL IsExist = false;

	void Generate目标地址(PBYTE 原指令地址, int 原指令长度, PBYTE 指向偏移的指针, int 偏移的硬编码字节数)
	{
		/*
			我认为只有‘Jx’操作数和'Ex、Mx'(x64下的Mod==0&&RM==101)的情况下会涉及相对地址的计算
			所以一个操作数中只会计算一次相对地址
			2019.09.14
		*/
		if (IsExist) { MessageBox(0, TEXT("【错误】一个操作数中已存在一个相对跳转地址"), 0, 0); return; }


		this->原指令地址 = 原指令地址;
		this->原指令长度 = 原指令长度;
		this->指向偏移的指针 = 指向偏移的指针;
		this->偏移的硬编码字节数 = 偏移的硬编码字节数;
		switch (偏移的硬编码字节数)
		{
			case 1:this->偏移值 = *(INT8*)this->指向偏移的指针;  break;
			case 2:this->偏移值 = *(INT16*)this->指向偏移的指针; break;
			case 4:this->偏移值 = *(INT32*)this->指向偏移的指针; break;
			case 8:this->偏移值 = *(INT64*)this->指向偏移的指针; break;
			default:MessageBox(0, TEXT("传入的硬编码长度有误，请传入字节数"), 0, 0); break;
		}

		//【计算公式】目标地址 = 下一条指令的地址 + 有符号数偏移
		this->下一条指令地址 = this->原指令地址 + this->原指令长度;
		this->目标地址 = this->下一条指令地址 + this->偏移值;

		IsExist = TRUE;
		return;
	}

	PBYTE Get目标地址() { return this->目标地址; }
	INT64 Get偏移值() { return this->偏移值; }
	PBYTE Get指向偏移的指针() { return this->指向偏移的指针; }
	int Get偏移的硬编码字节数() { return this->偏移的硬编码字节数; }

};