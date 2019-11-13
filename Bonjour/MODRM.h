#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include "String.h"
#include "SIB.h"
#include "OperandWidth.h"
#include "Operand.h"
#include "SignedINT.h"
#include "公式计算的相对地址.h"
class MODRM
{

	/*
	*	字符串宏定义，最后可以用正则表达式替换其为正确的寄存器名,例如 "寄存器_0号" -> "RAX"
	*/
#define 寄存器_0号	"寄存器_0号"
#define 寄存器_1号	"寄存器_1号"
#define 寄存器_2号	"寄存器_2号"
#define 寄存器_3号	"寄存器_3号"
#define 寄存器_4号	"寄存器_4号"
#define 寄存器_5号	"寄存器_5号"
#define 寄存器_6号	"寄存器_6号"
#define 寄存器_7号	"寄存器_7号"
#define 寄存器_8号	"寄存器_8号"
#define 寄存器_9号	"寄存器_9号"
#define 寄存器_10号	"寄存器_10号"
#define 寄存器_11号	"寄存器_11号"
#define 寄存器_12号	"寄存器_12号"
#define 寄存器_13号	"寄存器_13号"
#define 寄存器_14号	"寄存器_14号"
#define 寄存器_15号	"寄存器_15号"

private:

	/*
	*	Ex可能是带中括号的操作数，也可能是不带中括号的操作数
	*	Mx一定带中括号
	*	Rx一定不带中括号
	*	Gx一定不带中括号
	*/
	String E_根据RM;			//根据MODRM.RM字段，查到的memory或register字符串，E=M+R
	String M_根据RM;			//根据MODRM.RM字段，查到的memory字符串(MODRM.Mod!=11)
	String R_根据RM;			//根据MODRM.RM字段，查到的register字符串(MODRM.Mod==11)
	String G_根据RegOpcode;	//根据MODRM.RegOpcode字段，查到的register字符串
	OperandWidth 内存操作数宽度;		//带中括号的操作数的宽度，即【M】的宽度



	//************************************
	// Method:     AnalyzeMemWidth 
	// Description:根据传入的‘z’和Operand Size属性，分析带中括号的操作数的宽度（即分析【M】的操作数宽度）
	// Parameter:  int operandSize - 
	// Parameter:  String opcodeStr - 
	// Parameter:  String Zz_z - 
	// Returns:    void - 
	//************************************
	void AnalyzeMemWidth(int operandSize, String opcodeStr, String Zz_z)
	{
		//如果'Zz'中的'z'=='v'，则根据Operand Size属性决定可变宽度'v'的具体宽度
		if (Zz_z.Equals("v"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于8
				case 16:Zz_z = "w"; break;	//通过66前缀可以变为16位
				case 32:Zz_z = "d"; break;	//OperandSize默认是32位
				case 64:Zz_z = "q"; break;	//通过REX前缀可以变为64位
				default:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;
			}
		}

		//如果'Zz'中的'z'=='a'，则根据Operand Size属性决定宽度值，但宽度字符串需要显示为两倍形式
		//宽度值=16时，宽度字符串="word * 2 ptr "
		//宽度值=32时，宽度字符串="dword * 2 ptr "
		if (Zz_z.Equals("a"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于8
				case 16:this->内存操作数宽度.SetValue(16); this->内存操作数宽度.SetStr("word * 2 ptr "); break;
				case 32:this->内存操作数宽度.SetValue(16); this->内存操作数宽度.SetStr("dword * 2 ptr ");  break;
				case 64:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于64
				default:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;
			}
		}

		//如果'Zz'中的'z'=='p'
		//p: 32-bit, 48-bit, or 80-bit pointer, depending on operand-size attribute.
		else if (Zz_z.Equals("p"))
		{
			//当Operand Size = 16 时，【p】表示4字节 dword  (32 bit)
			//当Operand Size = 32 时，【p】表示6字节 fword  (48 bit) 
			//当Operand Size = 64 时，【p】表示10字节 tword  (80 bit) 
			if (operandSize == 16)
			{
				this->内存操作数宽度.SetValue(32);
				this->内存操作数宽度.SetStr("dword ptr ");
			}
			else if (operandSize == 32)
			{
				this->内存操作数宽度.SetValue(48);
				this->内存操作数宽度.SetStr("fword ptr ");
			}
			else if (operandSize == 64)
			{
				this->内存操作数宽度.SetValue(80);
				this->内存操作数宽度.SetStr("tword ptr ");
			}


		}
		//如果'Zz'中的'z'=='s'
		//s: 6-byte or 10-byte pseudo-descriptor.
		else if (Zz_z.Equals("s"))
		{
			//目前已知：32位程序中所有涉及【Ms】的指令都是fword(6字节，48 bit)
			//目前已知：64位程序中所有涉及【Ms】的指令都是tword(10字节，80 bit)
			if (sizeof(void*) == 4) {
				this->内存操作数宽度.SetValue(48);
				this->内存操作数宽度.SetStr("fword ptr ");
			}
			else if (sizeof(void*) == 8) {
				this->内存操作数宽度.SetValue(80);
				this->内存操作数宽度.SetStr("tword ptr ");
			}
		}
		//如果opcode是‘MOVSXD带符号扩展传送指令’，则根据Operand Size属性决定宽度值，但宽度字符串需要显示为二分之一形式
		//宽度值=64时，宽度字符串="qword / 2 ptr "
		if (opcodeStr.Equals("MOVSXD"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于8  ??
				case 16:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于16 ??
				case 32:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于32 ??
				case 64:this->内存操作数宽度.SetValue(64); this->内存操作数宽度.SetStr("qword / 2 ptr ");  break;
				default:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;
			}
		}


		//如果'Zz'中的'z'=='b'，则强制设置内存操作数宽度值=8
		if (Zz_z.Equals("b"))
		{
			this->内存操作数宽度.SetValue(8);
		}
		//如果'Zz'中的'z'=='w'，则强制设置内存操作数宽度值=16
		else if (Zz_z.Equals("w"))
		{
			this->内存操作数宽度.SetValue(16);
		}
		//如果'Zz'中的'z'=='d'，则强制设置内存操作数宽度值=32
		else if (Zz_z.Equals("d"))
		{
			this->内存操作数宽度.SetValue(32);
		}
		//如果'Zz'中的'z'=='q'，则强制设置内存操作数宽度值=64
		else if (Zz_z.Equals("q"))
		{
			this->内存操作数宽度.SetValue(64);
		}
	}
	//************************************
	// Method:     Replace_M 
	// Description:将【M】中的寄存器序号替换为具体的寄存器名称
	// Parameter:  int addressSize - 
	// Parameter:  OUT String & dest - 
	// Returns:    void - 
	//************************************
	void Replace_M(int addressSize, OUT String& dest)
	{
		//当AddressSize==16时，带中括号的寄存器是16位寄存器
		if (addressSize == 16)
		{
			//16位寄存器在Win32汇编中似乎不支持
		}
		//当AddressSize==32时，带中括号的寄存器是32位寄存器
		else if (addressSize == 32)
		{
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_0号), "EAX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_1号), "ECX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_2号), "EDX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_3号), "EBX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_4号), "ESP").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_5号), "EBP").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_6号), "ESI").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_7号), "EDI").data();

		}
		//当AddressSize==64时，带中括号的寄存器是64位寄存器
		else if (addressSize == 64)
		{
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_0号), "RAX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_1号), "RCX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_2号), "RDX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_3号), "RBX").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_4号), "RSP").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_5号), "RBP").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_6号), "RSI").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_7号), "RDI").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_8号), "R8").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_9号), "R9").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_10号), "R10").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_11号), "R11").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_12号), "R12").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_13号), "R13").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_14号), "R14").data();
			this->M_根据RM = std::regex_replace(this->M_根据RM.GetPChar(), std::regex(寄存器_15号), "R15").data();
		}
	}
	//************************************
	// Method:     Replace_R_G 
	// Description:将【R或G】中的寄存器序号替换为具体的寄存器名称
	// Parameter:  int operandSize - 
	// Parameter:  String Zz_z - 
	// Parameter:  OUT String & dest - 
	// Returns:    void - 
	//************************************
	void Replace_R_G(int operandSize, String Zz_z, OUT String& dest)
	{
		//如果'Zz'中的'z'=='v'，则根据Operand Size属性决定可变宽度'v'的具体宽度
		if (Zz_z.Equals("v"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于8
				case 16:Zz_z = "w"; break;	//通过66前缀可以变为16位
				case 32:Zz_z = "d"; break;	//OperandSize默认是32位
				case 64:Zz_z = "q"; break;	//通过REX前缀可以变为64位
				default:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;
			}
		}

		//如果'Zz'中的'z'=='z'，则根据Operand Size属性决定可变宽度'z'的具体宽度
		//z:Word for 16-bit operand-size or doubleword for 32 or 64-bit operand-size.
		//当operandSize=16时，是16位寄存器.
		//当operandSize=32或64时，是32位寄存器.
		if (Zz_z.Equals("z"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;	//OperandSize 不可能等于8
				case 16:Zz_z = "w"; break;	//通过66前缀可以变为16位
				case 32:Zz_z = "d"; break;	//OperandSize默认是32位
				case 64:Zz_z = "d"; break;	//通过REX前缀可以变为64位
				default:MessageBox(0, TEXT("Operand Size 属性有错误"), 0, 0); break;
			}
		}

		/*
		*	根据WidthString，将G_根据RegOpcode中的寄存器序号替换为对应的寄存器名称
		*/

		//如果'Zz'中的'z'=='b'，表示8位寄存器
		if (Zz_z.Equals("b"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_0号), "AL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_1号), "CL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_2号), "DL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_3号), "BL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_4号), "AH").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_5号), "CH").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_6号), "DH").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_7号), "BH").data();
		}
		//如果'Zz'中的'z'=='w'，表示16位寄存器
		else if (Zz_z.Equals("w"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_0号), "AX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_1号), "CX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_2号), "DX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_3号), "BX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_4号), "SP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_5号), "BP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_6号), "SI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_7号), "DI").data();

			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_8号), "R8W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_9号), "R9W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_10号), "R10W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_11号), "R11W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_12号), "R12W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_13号), "R13W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_14号), "R14W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_15号), "R15W").data();
		}
		//如果'Zz'中的'z'=='d'，表示32位寄存器
		else if (Zz_z.Equals("d"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_0号), "EAX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_1号), "ECX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_2号), "EDX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_3号), "EBX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_4号), "ESP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_5号), "EBP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_6号), "ESI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_7号), "EDI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_8号), "R8D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_9号), "R9D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_10号), "R10D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_11号), "R11D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_12号), "R12D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_13号), "R13D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_14号), "R14D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_15号), "R15D").data();
		}
		//如果'Zz'中的'z'=='q'，表示64位寄存器
		else if (Zz_z.Equals("q"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_0号), "RAX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_1号), "RCX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_2号), "RDX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_3号), "RBX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_4号), "RSP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_5号), "RBP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_6号), "RSI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_7号), "RDI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_8号), "R8").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_9号), "R9").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_10号), "R10").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_11号), "R11").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_12号), "R12").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_13号), "R13").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_14号), "R14").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(寄存器_15号), "R15").data();
		}
	}

public:

	BYTE Mod = 0;			//MODRM字段的Mod域
	BYTE RegOpcode = 0;		//MODRM字段的RegOpcode域
	BYTE RM = 0;			//MODRM字段的RM域

	SIB SIB;				//SIB字段

	bool IsExist = false;	//是否存在MODRM字段



	//************************************
	// Method:     Map_M_R_G 
	// Description:分析指令的寄存器序号和偏移，映射为【M】【R】【G】
	// Parameter:  PBYTE pModRM - 
	// Parameter:  OUT int & insLen - 
	// Parameter:  PBYTE insAddr - 指令的起始地址
	// Returns:    void - 
	//************************************
	void Map_M_R_G(PBYTE pModRM, OUT int& insLen, PBYTE insAddr, OUT 公式计算的相对地址& 公式计算的相对地址)
	{
		//读取MODRM字段，指令长度+1
		insLen++;

		//1.判断是否存在SIB字段
		char sibStr[40] = { 0 };
		if (this->SIB.IsExist)
		{
			//读取SIB字段，长度加1
			insLen++;
			PBYTE pSIB = pModRM + 0x1;

			//取偏移disp（无符号数）
			UINT32 disp32 = *(UINT32*)(pSIB + 0x1);
			UINT8 disp8 = *(UINT8*)(pSIB + 0x1);
			//取偏移disp（有符号数，偏移都是有符号数）
			char symbol8 = disp8 & 0x80 ? '-' : '+';			//得到有符号数的正负号部分。
			char symbol32 = disp32 & 0x80000000 ? '-' : '+';	//得到有符号数的正负号部分。
			INT32 disp32_signed = (symbol32 == '-') ? ~(disp32 - 1) & 0x7FFFFFFF : disp32;	//得到有符号数的数字部分：(负数)补码减一取反，去掉符号位。
			INT8 disp8_signed = (symbol8 == '-') ? ~(disp8 - 1) & 0x7F : disp8;				//得到有符号数的数字部分：(负数)补码减一取反，去掉符号位。

			//根据Base值得到第一个字符串
			String baseStr = { 0 };
			switch (this->SIB.Base)
			{
				case 0:baseStr = 寄存器_0号; break;
				case 1:baseStr = 寄存器_1号; break;
				case 2:baseStr = 寄存器_2号; break;
				case 3:baseStr = 寄存器_3号; break;
				case 4:baseStr = 寄存器_4号; break;
					//case 5:baseStr = 寄存器_5号; break; //当Base=101时:[*]
				case 6:baseStr = 寄存器_6号; break;
				case 7:baseStr = 寄存器_7号; break;
				case 8:baseStr = 寄存器_8号; break;
				case 9:baseStr = 寄存器_9号; break;
				case 10:baseStr = 寄存器_10号; break;
				case 11:baseStr = 寄存器_11号; break;
				case 12:baseStr = 寄存器_12号; break;
				case 13:baseStr = 寄存器_13号; break;
				case 14:baseStr = 寄存器_14号; break;
				case 15:baseStr = 寄存器_15号; break;
			}

			//根据Index和Scale得到另一个字符串
			String ScaleIndexStr = { 0 };
			String scaleStr = { 0 };
			scaleStr = String::Format("%d", (int)pow(2, this->SIB.Scale));
			switch (this->SIB.Index)
			{
				case 0:ScaleIndexStr = String::Format("%s * %s", 寄存器_0号, scaleStr); break;
				case 1:ScaleIndexStr = String::Format("%s * %s", 寄存器_1号, scaleStr); break;
				case 2:ScaleIndexStr = String::Format("%s * %s", 寄存器_2号, scaleStr); break;
				case 3:ScaleIndexStr = String::Format("%s * %s", 寄存器_3号, scaleStr); break;
				case 4:ScaleIndexStr = ""; break;//当Index=100时，是none
				case 5:ScaleIndexStr = String::Format("%s * %s", 寄存器_5号, scaleStr); break;
				case 6:ScaleIndexStr = String::Format("%s * %s", 寄存器_6号, scaleStr); break;
				case 7:ScaleIndexStr = String::Format("%s * %s", 寄存器_7号, scaleStr); break;
				case 8:ScaleIndexStr = String::Format("%s * %s", 寄存器_8号, scaleStr); break;
				case 9:ScaleIndexStr = String::Format("%s * %s", 寄存器_9号, scaleStr); break;
				case 10:ScaleIndexStr = String::Format("%s * %s", 寄存器_10号, scaleStr); break;
				case 11:ScaleIndexStr = String::Format("%s * %s", 寄存器_11号, scaleStr); break;
				case 12:ScaleIndexStr = String::Format("%s * %s", 寄存器_12号, scaleStr); break;
				case 13:ScaleIndexStr = String::Format("%s * %s", 寄存器_13号, scaleStr); break;
				case 14:ScaleIndexStr = String::Format("%s * %s", 寄存器_14号, scaleStr); break;
				case 15:ScaleIndexStr = String::Format("%s * %s", 寄存器_15号, scaleStr); break;
			}


			//拼接SIB字符串：当 base==101 时
			if (this->SIB.Base == 0x5)
			{
				switch (this->Mod)
				{
					case 0x0: sprintf(sibStr, "%s %c %08X", ScaleIndexStr, symbol32, disp32_signed); insLen += 4; break;			//多读取4字节偏移
					case 0x1:sprintf(sibStr, "%s %c %02X + ebp", ScaleIndexStr, symbol8, disp8_signed); insLen += 1; break;		//多读取1字节偏移
					case 0x2:sprintf(sibStr, "%s %c %08X + ebp", ScaleIndexStr, symbol32, disp32_signed); insLen += 4; break;	//多读取4字节偏移
				}
			}
			//拼接SIB字符串：当 base!=101 时
			else
			{
				switch (this->Mod)
				{
					case 0x0:sprintf(sibStr, "%s + %s", baseStr, ScaleIndexStr);  break;
					case 0x1:sprintf(sibStr, "%s + %s %c %02X", baseStr, ScaleIndexStr, symbol8, disp8_signed); insLen += 1; break;	//多读取1字节偏移
					case 0x2:sprintf(sibStr, "%s + %s %c %08X", baseStr, ScaleIndexStr, symbol32, disp32_signed); insLen += 4; break;//多读取4字节偏移
				}
			}
		}

		//2.若无SIB，则只需分析ModRM
		//2.1 根据Mod、RM分析【M】和【R】
		if (this->Mod == 0x0)
		{

			switch (this->RM)
			{
				case 0:	this->M_根据RM = String::Format("[%s]", 寄存器_0号); break;
				case 1:	this->M_根据RM = String::Format("[%s]", 寄存器_1号); break;
				case 2:	this->M_根据RM = String::Format("[%s]", 寄存器_2号); break;
				case 3:	this->M_根据RM = String::Format("[%s]", 寄存器_3号); break;
				case 4:	this->M_根据RM = String::Format("[%s]", sibStr); break;//存在SIB字段
				case 5:
				{
					//向后读取4字节
					UINT32 uInt32 = *(UINT32*)(pModRM + 1);
					insLen += 4;

					//在x64下，MODRM.Mod==00B && MODRM.RM==101B时: 中括号里的地址值 = 下一条指令的地址 + disp32（是有符号数）
					if (sizeof(void*) == 8)
					{

						公式计算的相对地址.Generate目标地址(insAddr, insLen, pModRM + 1, 4);
						this->M_根据RM = String::Format("[%016llX]", 公式计算的相对地址.Get目标地址());
					}
					//在x86下，MODRM.Mod==00B && MODRM.RM==101B时: 中括号里的地址值 =  一个32位偏移数（OD上看，disp32被显示为无符号数，按理说是有符号数才对吧？）
					else
					{
						this->M_根据RM = String::Format("[%08X]", uInt32);
					}
					break;
				}
				case 6:	this->M_根据RM = String::Format("[%s]", 寄存器_6号); break;
				case 7:	this->M_根据RM = String::Format("[%s]", 寄存器_7号); break;
				case 8:	this->M_根据RM = String::Format("[%s]", 寄存器_8号); break;
				case 9:	this->M_根据RM = String::Format("[%s]", 寄存器_9号); break;
				case 10:this->M_根据RM = String::Format("[%s]", 寄存器_10号); break;
				case 11:this->M_根据RM = String::Format("[%s]", 寄存器_11号); break;
				case 12:this->M_根据RM = String::Format("[%s]", 寄存器_12号); break;
				case 13:this->M_根据RM = String::Format("[%s]", 寄存器_13号); break;
				case 14:this->M_根据RM = String::Format("[%s]", 寄存器_14号); break;
				case 15:this->M_根据RM = String::Format("[%s]", 寄存器_15号); break;
			}
		}
		else if (this->Mod == 0x1)
		{
			//得到8位偏移(无符号)
			BYTE disp8 = *(BYTE*)(pModRM + 0x1);
			//将偏移转为16进制显示的有符号数
			char symbol = disp8 & 0x80 ? '-' : '+';		//得到有符号数的正负号部分。
			INT8 disp8_signed = (symbol == '-') ? ~(disp8 - 1) & 0x7F : disp8;	//得到有符号数的数字部分：(负数)补码减一取反，去掉符号位

			switch (this->RM)
			{
				case 0:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_0号, symbol, disp8_signed);  insLen += 1; break;
				case 1:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_1号, symbol, disp8_signed);  insLen += 1; break;
				case 2:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_2号, symbol, disp8_signed);  insLen += 1; break;
				case 3:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_3号, symbol, disp8_signed);  insLen += 1; break;
				case 4:this->M_根据RM = String::Format("[%s]", sibStr);   break;	//存在SIB字段
				case 5:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_5号, symbol, disp8_signed);  insLen += 1; break;
				case 6:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_6号, symbol, disp8_signed);  insLen += 1; break;
				case 7:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_7号, symbol, disp8_signed);  insLen += 1; break;
				case 8:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_8号, symbol, disp8_signed);  insLen += 1; break;
				case 9:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_9号, symbol, disp8_signed);  insLen += 1; break;
				case 10:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_10号, symbol, disp8_signed);  insLen += 1; break;
				case 11:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_11号, symbol, disp8_signed);  insLen += 1; break;
				case 12:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_12号, symbol, disp8_signed);  insLen += 1; break;
				case 13:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_13号, symbol, disp8_signed);  insLen += 1; break;
				case 14:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_14号, symbol, disp8_signed);  insLen += 1; break;
				case 15:this->M_根据RM = String::Format("[%s %c %02X]", 寄存器_15号, symbol, disp8_signed);  insLen += 1; break;

			}
		}
		else if (this->Mod == 0x2)
		{
			//得到32位偏移(无符号)
			UINT32 disp32 = *(UINT32*)(pModRM + 0x1);
			//将偏移转为16进制显示的有符号数
			char symbol = disp32 & 0x80000000 ? '-' : '+';		//得到有符号数的正负号部分。
			INT32 disp32_signed = (symbol == '-') ? ~(disp32 - 1) & 0x7FFFFFFF : disp32;//得到有符号数的数字部分：(负数)补码减一取反，去掉符号位

			switch (this->RM)
			{

				case 0:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_0号, symbol, disp32_signed); insLen += 4; break;
				case 1:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_1号, symbol, disp32_signed); insLen += 4; break;
				case 2:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_2号, symbol, disp32_signed); insLen += 4; break;
				case 3:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_3号, symbol, disp32_signed); insLen += 4; break;
				case 4:	this->M_根据RM = String::Format("[%s]", sibStr); break;	//存在SIB字段
				case 5:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_5号, symbol, disp32_signed); insLen += 4; break;
				case 6:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_6号, symbol, disp32_signed); insLen += 4; break;
				case 7:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_7号, symbol, disp32_signed); insLen += 4; break;
				case 8:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_8号, symbol, disp32_signed); insLen += 4; break;
				case 9:	this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_9号, symbol, disp32_signed); insLen += 4; break;
				case 10:this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_10号, symbol, disp32_signed); insLen += 4; break;
				case 11:this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_11号, symbol, disp32_signed); insLen += 4; break;
				case 12:this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_12号, symbol, disp32_signed); insLen += 4; break;
				case 13:this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_13号, symbol, disp32_signed); insLen += 4; break;
				case 14:this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_14号, symbol, disp32_signed); insLen += 4; break;
				case 15:this->M_根据RM = String::Format("[%s %c %08X]", 寄存器_15号, symbol, disp32_signed); insLen += 4; break;
			}
		}
		else if (this->Mod == 0x3)
		{
			switch (this->RM)
			{
				//非寻址寄存器
				case 0:	this->R_根据RM = 寄存器_0号; break;
				case 1:	this->R_根据RM = 寄存器_1号; break;
				case 2:	this->R_根据RM = 寄存器_2号; break;
				case 3:	this->R_根据RM = 寄存器_3号; break;
				case 4:	this->R_根据RM = 寄存器_4号; break;
				case 5:	this->R_根据RM = 寄存器_5号; break;
				case 6:	this->R_根据RM = 寄存器_6号; break;
				case 7:	this->R_根据RM = 寄存器_7号; break;
				case 8:	this->R_根据RM = 寄存器_8号; break;
				case 9:	this->R_根据RM = 寄存器_9号; break;
				case 10:this->R_根据RM = 寄存器_10号; break;
				case 11:this->R_根据RM = 寄存器_11号; break;
				case 12:this->R_根据RM = 寄存器_12号; break;
				case 13:this->R_根据RM = 寄存器_13号; break;
				case 14:this->R_根据RM = 寄存器_14号; break;
				case 15:this->R_根据RM = 寄存器_15号; break;
			}
		}

		//2.2 根据RegOpcode分析【G】
		switch (this->RegOpcode)
		{
			//非寻址寄存器
			case 0:	this->G_根据RegOpcode = 寄存器_0号; break;
			case 1:	this->G_根据RegOpcode = 寄存器_1号; break;
			case 2:	this->G_根据RegOpcode = 寄存器_2号; break;
			case 3:	this->G_根据RegOpcode = 寄存器_3号; break;
			case 4:	this->G_根据RegOpcode = 寄存器_4号; break;
			case 5:	this->G_根据RegOpcode = 寄存器_5号; break;
			case 6:	this->G_根据RegOpcode = 寄存器_6号; break;
			case 7:	this->G_根据RegOpcode = 寄存器_7号; break;
			case 8:	this->G_根据RegOpcode = 寄存器_8号; break;
			case 9:	this->G_根据RegOpcode = 寄存器_9号; break;
			case 10:this->G_根据RegOpcode = 寄存器_10号; break;
			case 11:this->G_根据RegOpcode = 寄存器_11号; break;
			case 12:this->G_根据RegOpcode = 寄存器_12号; break;
			case 13:this->G_根据RegOpcode = 寄存器_13号; break;
			case 14:this->G_根据RegOpcode = 寄存器_14号; break;
			case 15:this->G_根据RegOpcode = 寄存器_15号; break;
			default:MessageBox(0, TEXT("MOD.RegOpcode域的值有误"), 0, 0); this->G_根据RegOpcode = "MOD.RegOpcode域的值有误"; break;
		}
	}



	String GetE(int addressSize, int operandSize, String Zz_z, String opcodeStr)
	{
		if (this->Mod != 0x3)
		{
			return GetM(addressSize, operandSize, Zz_z, opcodeStr);
		}
		else
		{
			return GetR(addressSize, operandSize, Zz_z, opcodeStr);
		}
	}
	String GetM(int addressSize, int operandSize, String Zz_z, String opcodeStr)
	{
		//将M中的寄存器序号替换为具体的寄存器名
		Replace_M(addressSize, this->M_根据RM);
		return this->M_根据RM;
	}
	String GetR(int addressSize, int operandSize, String Zz_z, String opcodeStr)
	{
		//将R中的寄存器序号替换为具体的寄存器名
		Replace_R_G(operandSize, Zz_z, this->R_根据RM);
		return this->R_根据RM;
	}
	String GetG(int addressSize, int operandSize, String Zz_z, String opcodeStr)
	{
		//将G中的寄存器序号替换为具体的寄存器名
		Replace_R_G(operandSize, Zz_z, this->G_根据RegOpcode);
		return this->G_根据RegOpcode;
	}

	//************************************
	// Method:     GetS 
	// Description:通过MOD.reg域计算段寄存器
	// Returns:    String - 
	//************************************
	String GetS()
	{
		switch (this->RegOpcode)
		{
			case 0:return "ES";
			case 1:return "CS";
			case 2:return "SS";
			case 3:return "DS";
			case 4:return "FS";
			case 5:return "GS";
			default:return "错误!段寄存器只有6个";
		}
	}

	//************************************
	// Method:     Get内存操作数的宽度
	// Description:获取ModRM涉及的内存操作数的宽度，只有带中括号的操作数才需要宽度，即【M】的宽度
	// Parameter:  int operandSize - 
	// Parameter:  String Zz_z - 
	// Parameter:  String opcodeStr - 
	// Returns:    OperandWidth - 
	//************************************
	OperandWidth Get内存操作数的宽度(int operandSize, String Zz_z, String opcodeStr)
	{
		//分析M的内存操作数宽度
		AnalyzeMemWidth(operandSize, opcodeStr, Zz_z);

		return this->内存操作数宽度;
	}
	String GetE的操作数类型()
	{
		//当MORRM.Mod==00、01、10时，都是【M】memory操作数，都有中括号
		if (this->Mod != 0x3)
		{
			return 操作数类型_带中括号的操作数;
		}
		//当MORRM.Mod==11时，是【R】register操作数，没有中括号
		else
		{
			return 操作数类型_不带中括号的操作数;
		}
	}
	String GetM的操作数类型()
	{
		return 操作数类型_带中括号的操作数;
	}
	String GetR的操作数类型()
	{
		//【R】都是带中括号的操作数
		return 操作数类型_不带中括号的操作数;
	}
	String GetG的操作数类型()
	{
		//【G】都是没有中括号的操作数
		return 操作数类型_不带中括号的操作数;
	}
	String GetS的操作数类型()
	{
		return 操作数类型_不带中括号的操作数;
	}
	//************************************
	// Method:     Map_ModRM_SIB
	// Description:分析Mod字段和SIB字段的各域
	// Parameter:  PBYTE pModRM - 
	// Parameter:  PrefixREX 前缀REX - 
	// Returns:    void - 
	//************************************
	void Map_ModRM_SIB(PBYTE pModRM)
	{
		/*
		* 初步分析【ModR/M】字段
		* 第6~7位是"Mod"
		* 第3~5位是"Reg/Opcode"
		* 第0~2位是"r/m"
		*/
		this->IsExist = true;
		BYTE ModRM = *pModRM;
		this->Mod = ModRM >> 0x6;					//右移6位	
		this->RegOpcode = (ModRM >> 0x3) & (0x7);	//右移2位,按位与0000 0111
		this->RM = ModRM & 0x7;						//按位与0000 0011

		/*
			* 初步分析【SIB】字段
			* 第6~7位是"Scale"
			* 第3~5位是"Index"
			* 第0~2位是"Base"
			* 【Base+Index*2^Scale】
		*/
		if (this->RM == 0x4 && (this->Mod == 0x0 || this->Mod == 0x1 || this->Mod == 0x2))
		{//当Mod域=00 01 10 , r/m域=100时，存在SIB字段

			this->SIB.IsExist = true;
			BYTE SIB_Byte = *(pModRM + 1);
			this->SIB.Scale = SIB_Byte >> 0x6;
			this->SIB.Index = (SIB_Byte >> 0x3) & (0x7);
			this->SIB.Base = SIB_Byte & 0x7;
		}
	}

};

