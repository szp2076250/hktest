#pragma once
#include "String.h"
#include "MODRM.h"
#include "PrefixREX.h"
#include "PrefixSEG.h"
#include "OperandWidth.h"
#include "Operand.h"
#include "公式计算的相对地址.h"
class Instruction
{
private:

	String 反汇编代码;


	int OperandSize = 0;
	int AddressSize = 0;

public:
	String 原始字符串operand01;
	String 原始字符串operand02;
	String 原始字符串operand03;

	String opcode;
	Operand operand01;
	Operand operand02;
	Operand operand03;	//IMUL指令(0x69)拥有3个操作数

	int insLength = 0;		//当前指令所占字节长度
	PBYTE insAddr = 0;		//当前指令的地址值

	公式计算的相对地址 公式计算的相对地址;

	/*
		封装 OperandSize 和 AddressSize
	*/
	bool 允许前缀修改OperandSize = true;	//是否再允许修改OperandSize
	bool 允许前缀修改AddressSize = true;	//是否再允许修改AddressSize
	int GetOperandSize() { return this->OperandSize; }
	int GetAddressSize() { return this->AddressSize; }
	void SetOperandSize(int OperandSize)
	{
		this->OperandSize = OperandSize;

	}
	void SetAddressSize(int AddressSize)
	{
		this->AddressSize = AddressSize;
	}

	PrefixREX 前缀REX;			//REX前缀:0x41~0x4F
	PrefixSEG 前缀SEG;			//段前缀
	BYTE 前缀OperandSize = 0;	//OperandSize Override前缀: 0x66
	BYTE 前缀AddressSize = 0;	//AddressSiz Override前缀: 0x67
	BYTE 前缀LOCK = 0;				//LOCK前缀
	BYTE 前缀REPE = 0;				//REPE前缀
	BYTE 前缀REPNE = 0;				//REPNE前缀



	MODRM ModRM;

	//************************************
	// Method:     GetPrefixNum 
	// Description:得到前缀的个数
	// Returns:    int - 
	//************************************
	int GetPrefixNum()
	{
		int num = 0;
		if (this->前缀REX.IsExist)num++;
		if (this->前缀SEG.IsExist)num++;
		if (this->前缀OperandSize != 0)num++;
		if (this->前缀AddressSize != 0)num++;
		return num;
	}


	//************************************
	// Method:     Generate反汇编代码 
	// Description:将指令拼接为String格式
	// Returns:    void - 
	//************************************
	void Generate反汇编代码()
	{
		//如果第1个操作数为空，则说明无operand，只需打印opcode
		if (operand01.不包含宽度的操作数字符串 == NULL
			|| operand01.不包含宽度的操作数字符串.Equals(""))
		{
			this->反汇编代码 = String::Format("%-10s", this->opcode.GetPChar());
		}
		//如果第2个操作数为空，则说明有1个operand，只需打印opcode和第1个operand
		else if (operand02.不包含宽度的操作数字符串 == NULL
			|| operand02.不包含宽度的操作数字符串.Equals(""))
		{
			this->反汇编代码 = String::Format("%-10s %s", this->opcode.GetPChar(), this->operand01.ToString(this->前缀SEG));
		}
		//如果第3个操作数为空，则说明有2个operand，只需打印opcode和第1、2个operand
		else if (operand03.不包含宽度的操作数字符串 == NULL
			|| operand03.不包含宽度的操作数字符串.Equals(""))
		{
			this->反汇编代码 = String::Format("%-10s %s , %s", this->opcode.GetPChar(), this->operand01.ToString(this->前缀SEG), this->operand02.ToString(this->前缀SEG));
		}
		//否则就说明共有3个operand，要打印opcode和第1、2、3个operand
		else
		{
			this->反汇编代码 = String::Format("%-10s %s , %s , %s", this->opcode.GetPChar(), this->operand01.ToString(this->前缀SEG), this->operand02.ToString(this->前缀SEG), this->operand03.ToString(this->前缀SEG));
		}
	}

	String GetDisassemblerCode() { return this->反汇编代码; }

	//************************************
	// Method:     GetHardCode 
	// Description:
	// Returns:    String - 
	//************************************
	String GetHardCode()
	{

		String res;

		//打印机器码
		for (int i = 0; i < this->insLength; i++)
		{
			res = res + String::Format(" %02X", *(this->insAddr + i));
			//给每个前缀都打印一个冒号
			if ((this->前缀SEG.IsExist
				|| this->前缀REX.IsExist
				|| this->前缀AddressSize
				|| this->前缀OperandSize
				|| this->前缀LOCK
				|| this->前缀REPNE
				|| this->前缀REPE)
				&& i < this->GetPrefixNum()
				)
			{
				res = res + ":";
				//printf(":");
			}
		}

		return res;
	}

	//************************************
	// Method:     处理两个关键属性 
	// Description: 处理operand size 和Addresss size属性（根据前缀）
	//				如果AddressSize或OperandSize已有其他值，则可能是d64、f64等前缀的赋值
	// Returns:    void - 
	//************************************
	void 处理OperandSize和AddresssSize()
	{
		//初始化两个关键属性
		if (this->OperandSize == 0) { this->OperandSize = 32; }					//不论64位还是32位程序，oprand Size初始都是32				
		if (this->AddressSize == 0) { this->AddressSize = sizeof(void*) * 8; }	//在64位程序中addressSize=64，在32位程序中addressSize=32			

		//66前缀:operand size改为原来的1/2（即改为16 bit）
		if (this->前缀OperandSize == 0x66 && 允许前缀修改OperandSize)
		{
			this->OperandSize = this->OperandSize / 2;	//16
		}

		//67前缀:Address Size改为原来的1/2
		if (this->前缀AddressSize == 0x67 && 允许前缀修改AddressSize)
		{
			this->AddressSize = this->AddressSize / 2;
		}

		//REX前缀：当REX.W==1时，64 Bit Operand Size
		if (this->前缀REX.W == 1 && 允许前缀修改OperandSize)
		{
			this->OperandSize = 64;
		}
		//REX前缀：当REX.W==0时，Operand size determined by CS.D
		else if (this->前缀REX.W == 0 && 允许前缀修改OperandSize)
		{

		}
		return;

	}

};

