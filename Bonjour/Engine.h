#pragma once
#include "stdafx.h"
#include "Instruction.h"
#include "String.h"
#include "ModRM.h"
#include "Operand.h"
#include "OpcodeExtensions.h"
#include <list>

class Engine
{


private:


	//************************************
	// Method:     HandleZz 
	// Description:处理单个操作数（格式为'Zz'）
	// Parameter:  PBYTE pOpcode - 
	// Parameter:  String 不包含宽度的操作数字符串 - 
	// Parameter:  Instruction ins - 
	// Parameter:  OUT String & destOperand - 
	// Parameter:  OUT int & insLen - 
	// Returns:    void - 
	//************************************
	void HandleZz(PBYTE pOpcode, String Zz, Instruction& ins, OUT Operand& destOperand, OUT int& insLen)
	{
		if (Zz.Equals("")) { return; }

		/*
		*	如果oprand是写死的寄存器名
		*/
		std::regex reg_写死的REG("(AL|CL|DL|BL|AH|CH|DH|BH|AX|CX|DX|BX|SP|BP|SI|DI|R8L|R9L|R10L|R11L|R12L|R13L|R14L|R15L|CS|DS|ES|SS|FS|GS|EAX|ECX|EDX|EBX|ESP|EBP|ESI|EDI|RAX|RCX|RDX|RBX|RSP|RBP|RSI|RDI|R8|R9|R10|R11|R12|R13|R14|R15)");
		if (std::regex_match(Zz.GetPChar(), reg_写死的REG))
		{
			destOperand.不包含宽度的操作数字符串 = Zz;
			destOperand.操作数类型 = 操作数类型_不带中括号的操作数;
		}

		/*
		*	如果oprand是'rXX'或'eXX'
		*/
		//rXX:rXX is used when 16, 32, or 64-bit sizes are possible, depends on the operand-size attribute
		//eXX:eXX is used when 16 or 32 - bit sizes are possible, depends on the operand-size attribute
		std::regex reg_erXX("[er]{1}[ACDBS189]{1}[XPI012345]{0,1}");	//创建正则表达式
		if (std::regex_match(Zz.GetPChar(), reg_erXX))			//正则表达式的匹配。匹配成功返回true
		{
			if (ins.GetOperandSize() == 64) { destOperand.不包含宽度的操作数字符串 = "r" + Zz.SubString(1); }
			else if (ins.GetOperandSize() == 32)
			{
				//如果operand是R8、R9这样的64位【新增寄存器】，那么其32位的形式是R8D、R9D
				if (std::regex_match(Zz.GetPChar(), std::regex("(r8|r9|r10|r11|r12|r13|r14|r15)")))
				{
					destOperand.不包含宽度的操作数字符串 = "r" + Zz.SubString(1) + "d";
				}
				//如果operand是RAX、RCX这样的64位【拓展寄存器】，那么其32位的形式是EAX、ECX
				else { destOperand.不包含宽度的操作数字符串 = "e" + Zz.SubString(1); }
			}
			else if (ins.GetOperandSize() == 16)
			{
				destOperand.不包含宽度的操作数字符串 = Zz.SubString(1);
			}
			destOperand.不包含宽度的操作数字符串 = destOperand.不包含宽度的操作数字符串.ToUpper();	//转为大写
			destOperand.操作数类型 = 操作数类型_不带中括号的操作数;
		}

		/*
		*	如果oprand是立即数'Ix'
		*/
		//I:Immediate data，暂时看都是'无符号数'
		std::regex regIx("[I]{1}[bzwv]{1}");	//Iv、Ib
		if (std::regex_match(Zz.GetPChar(), regIx))
		{
			destOperand.操作数类型 = 操作数类型_不带中括号的操作数;

			//获取宽度Zz的z
			String Zz_z = Zz.SubString(1);  //截取'Ix'的宽度'x'

			//根据立即数的宽度获取立即数
			//b:Byte, regardless of operand-size attribute.
			if (Zz_z.Equals("b"))
			{
				UINT8 uImm8 = 0;
				//如果指令涉及ModRM，则该指令的Ix应该从ModRM(包括ModRM所读取的立即数)之后读取。
				if (ins.ModRM.IsExist) { uImm8 = *(UINT8*)(pOpcode + insLen - ins.GetPrefixNum()); }
				else { uImm8 = *(UINT8*)(pOpcode + 1); }
				destOperand.不包含宽度的操作数字符串 = String::Format("%02X", uImm8);
				insLen += 1;
			}
			else if (Zz_z.Equals("w"))
			{
				UINT16 uImm16 = 0;
				//如果指令涉及ModRM，则该指令的Ix应该从ModRM(包括ModRM所读取的立即数)之后读取。
				if (ins.ModRM.IsExist) { uImm16 = *(UINT16*)(pOpcode + insLen - ins.GetPrefixNum()); }
				else { uImm16 = *(UINT16*)(pOpcode + 1); }
				destOperand.不包含宽度的操作数字符串 = String::Format("%04X", uImm16);
				insLen += 2;
			}
			//z:Word for 16-bit operand-size or doubleword for 32 or 64-bit operand-size.
			else if (Zz_z.Equals("z"))
			{
				if (ins.GetOperandSize() == 16) {}
				else if (ins.GetOperandSize() == 32 || ins.GetOperandSize() == 64)
				{
					UINT32 uImm32 = 0;
					//如果指令涉及ModRM，则该指令的Ix应该从ModRM(包括ModRM所读取的立即数)之后读取。
					if (ins.ModRM.IsExist) { uImm32 = *(UINT32*)(pOpcode + insLen - ins.GetPrefixNum()); }
					else { uImm32 = *(UINT32*)(pOpcode + 1); }
					destOperand.不包含宽度的操作数字符串 = String::Format("%08X", uImm32);
					insLen += 4;
				}
			}
			//v:Word, doubleword or quadword (in 64-bit mode), depending on operand-size attribute.
			else if (Zz_z.Equals("v"))
			{
				if (ins.GetOperandSize() == 16)
				{
					UINT16 uImm16 = 0;
					//如果指令涉及ModRM，则该指令的Ix应该从ModRM(包括ModRM所读取的立即数)之后读取。
					if (ins.ModRM.IsExist) { uImm16 = *(UINT16*)(pOpcode + insLen - ins.GetPrefixNum()); }
					else { uImm16 = *(UINT16*)(pOpcode + 1); }
					destOperand.不包含宽度的操作数字符串 = String::Format("%04X", uImm16);
					insLen += 2;
				}
				else if (ins.GetOperandSize() == 32)
				{
					UINT32 uImm32 = 0;
					//如果指令涉及ModRM，则该指令的Ix应该从ModRM(包括ModRM所读取的立即数)之后读取。
					if (ins.ModRM.IsExist) { uImm32 = *(UINT32*)(pOpcode + insLen - ins.GetPrefixNum()); }
					else { uImm32 = *(UINT32*)(pOpcode + 1); }
					destOperand.不包含宽度的操作数字符串 = String::Format("%08X", uImm32);
					insLen += 4;
				}
				else if (ins.GetOperandSize() == 64)
				{
					UINT64 uImm64 = 0;
					//如果指令涉及ModRM，则该指令的Ix应该从ModRM(包括ModRM所读取的立即数)之后读取。
					if (ins.ModRM.IsExist) { uImm64 = *(UINT64*)(pOpcode + insLen - ins.GetPrefixNum()); }
					else { uImm64 = *(UINT64*)(pOpcode + 1); }
					destOperand.不包含宽度的操作数字符串 = String::Format("%016llX", uImm64);
					insLen += 8;

				}
			}
		}

		/*
		*	如果oprand是写死的立即数，例如 ‘1’
		*/
		if (std::regex_match(Zz.GetPChar(), std::regex("[1]{1}")))
		{
			destOperand.操作数类型 = 操作数类型_不带中括号的操作数;
			destOperand.不包含宽度的操作数字符串 = String::Format("%X", 1);
		}


		/*
		*	如果oprand是'Jx'
		*/
		//J:The instruction contains a relative offset to be added to the instruction pointer register (for example, JMP(0E9), LOOP).
		//	【operand 是基于 rip 的 offset（偏移量），是 signed（有符号数）】
		//	【公式：JCC要跳转的地址 = 当前指令下一行地址 + operand】
		std::regex regJx("[J]{1}[bz]{1}");	//【{n}】表示该字符允许出现任意n次，【[bv]】表示允许出现的字符有b、v
		if (std::regex_match(Zz.GetPChar(), regJx))
		{
			destOperand.操作数类型 = 操作数类型_不带中括号的操作数;

			//获取宽度Zz的z
			String Zz_z = Zz.SubString(1);

			//b:Byte, regardless of operand-size attribute.
			if (Zz_z.Equals("b"))
			{
				insLen += 1;


				//INT8 operand = *(INT8*)(pOpcode + 1);	//JCC的operand宽度是byte，且是有符号数(signed)
				//PVOID destAddr = (pOpcode + insLen) + operand;
				//destOperand.不包含宽度的操作数字符串 = String::Format("%p", destAddr);

				ins.公式计算的相对地址.Generate目标地址(ins.insAddr, insLen, pOpcode + 1, sizeof(INT8));//JCC的operand宽度是byte，且是有符号数(signed)
				destOperand.不包含宽度的操作数字符串 = String::Format("%p", ins.公式计算的相对地址.Get目标地址());
			}
			//z:Word for 16-bit operand-size or doubleword for 32 or 64-bit operand-size.
			else if (Zz_z.Equals("z"))
			{
				if (ins.GetOperandSize() == 16) {}
				else if (ins.GetOperandSize() == 32 || ins.GetOperandSize() == 64)
				{
					insLen += 4;	//向后读4字节

					//INT32 operand = *(INT32*)(pOpcode + 1);		//JCC的operand宽度是dword，且是有符号数(signed)
					//PVOID destAddr = (pOpcode + insLen) + operand;
					//destOperand.不包含宽度的操作数字符串 = String::Format("%p", destAddr);

					ins.公式计算的相对地址.Generate目标地址(ins.insAddr, insLen, pOpcode + 1, sizeof(INT32));//JCC的operand宽度是dword，且是有符号数(signed)
					destOperand.不包含宽度的操作数字符串 = String::Format("%p", ins.公式计算的相对地址.Get目标地址());
				}
			}
		}

		/*
		*	如果oprand是'Ax'
		*/
		//A：Direct address: the instruction has no ModR/M byte; the address of the operand is encoded in the instruction.No base register, index register, or scaling factor can be applied(for example, far JMP(EA)).
		//【CALL或JMP跳转的地址是直接从后面硬编码中读取的，不需要用公式计算地址。地址写为selector:offset的形式。】
		if (std::regex_match(Zz.GetPChar(), std::regex("[A]{1}[p]{1}")))	//Ap、
		{
			//Ax是地址值，所以操作数类型是不带中括号的
			destOperand.操作数类型 = 操作数类型_不带中括号的操作数;

			//获取宽度Zz的z
			String Zz_z = Zz.SubString(1);

			//【Ax】的值直接从后面的硬编码中读取，写为 selector:offset 的形式，offset是高位读取，selector是低位读取
			//p:32-bit, 48-bit, or 80-bit pointer, depending on operand-size attribute.
			if (Zz_z.Equals("p"))
			{
				//当Operand Size = 16 时，【p】表示4字节 dword  (32 bit)
				//当Operand Size = 32 时，【p】表示6字节 fword  (48 bit) 
				//当Operand Size = 64 时，【p】表示10字节 tword  (80 bit) 
				if (ins.GetOperandSize() == 16)
				{
					//Ap共4字节，其中selector二字节:offset二字节
					insLen += 4;
					UINT16 offset = *(UINT16*)(pOpcode + 1);
					UINT16 selector = *(UINT16*)(pOpcode + 1 + 2);
					destOperand.不包含宽度的操作数字符串 = String::Format("%04X:%04X", selector, offset);

				}
				else if (ins.GetOperandSize() == 32)
				{
					//Ap共6字节，其中selector二字节:offset四字节
					insLen += 6;
					UINT32 offset = *(UINT32*)(pOpcode + 1);
					UINT16 selector = *(UINT16*)(pOpcode + 1 + 4);
					destOperand.不包含宽度的操作数字符串 = String::Format("%04X:%08X", selector, offset);
				}
			}

		}


		/*
		*	如果oprand是'Ox'
		*/
		//O：The instruction has no ModR / M byte.The offset of the operand is coded as a word or double word
		//(depending on address size attribute) in the instruction.No base register, index register, or scaling factor
		//	can be applied(for example, MOV(A0–A3)).
		//【 operand 是 memory offset，直接提供绝对地址。 无需 modrm 和 SIB 寻址,寻址的地址值直接从后面的硬编码中读取。】
		if (std::regex_match(Zz.GetPChar(), std::regex("[O]{1}[bv]{1}")))
		{
			//Ox都是带中括号的操作数
			destOperand.操作数类型 = 操作数类型_带中括号的操作数;
			PVOID operand = *(PVOID*)(pOpcode + 1);				//32位下是DWORD，64位下是QWORD
			destOperand.不包含宽度的操作数字符串 = String::Format("[%p]", operand);

			//指令长度增加：一个指针的长度
			insLen += sizeof(void*);

			//获取宽度Zz的z
			String Zz_z = Zz.SubString(1);

			//b:Byte, regardless of operand-size attribute.
			if (Zz_z.Equals("b"))
			{
				destOperand.带中括号的操作数的宽度.SetValue(8);
			}

			//v:Word, doubleword or quadword (in 64-bit mode), depending on operand-size attribute.
			else if (Zz_z.Equals("v"))
			{
				destOperand.带中括号的操作数的宽度.SetValue(ins.GetOperandSize());
			}
		}

		/*
		*	如果oprand是'Xx'
		*	如果oprand是'Yx'
		*/
		//X: Memory addressed by the DS : rSI register pair(for example, MOVS, CMPS, OUTS, or LODS).
		//Y：Memory addressed by the ES:rDI register pair (for example, MOVS, CMPS, INS, STOS, or SCAS).
		//【X：operand是由'rSI'提供寻址】
		//【Y：operand是由'rDI'提供寻址】
		//rXX:rXX is used when 16, 32, or 64-bit sizes are possible, depends on the operand-size attribute
		if (std::regex_match(Zz.GetPChar(), std::regex("[XY]{1}[bvz]{1}")))	//Yz、Yb
		{
			//获取宽度Zz
			String Zz_Z = Zz.SubString(0, 1);
			String Zz_z = Zz.SubString(1);

			destOperand.操作数类型 = 操作数类型_带中括号的操作数;

			//X是rSI，Y是rDI
			if (ins.GetOperandSize() == 16) { Zz_Z.Equals("X") ? destOperand.不包含宽度的操作数字符串 = "[ESI]" : destOperand.不包含宽度的操作数字符串 = "[EDI]"; }
			else if (ins.GetOperandSize() == 32) { Zz_Z.Equals("X") ? destOperand.不包含宽度的操作数字符串 = "[ESI]" : destOperand.不包含宽度的操作数字符串 = "[EDI]"; }
			else if (ins.GetOperandSize() == 64) { Zz_Z.Equals("X") ? destOperand.不包含宽度的操作数字符串 = "[RSI]" : destOperand.不包含宽度的操作数字符串 = "[RDI]"; }

			//b:Byte, regardless of operand-size attribute.
			if (Zz_z.Equals("b"))
			{
				destOperand.带中括号的操作数的宽度.SetValue(8);
			}
			//v:Word, doubleword or quadword (in 64-bit mode), depending on operand-size attribute.
			else if (Zz_z.Equals("v"))
			{
				destOperand.带中括号的操作数的宽度.SetValue(ins.GetOperandSize());
			}
			//z:Word for 16-bit operand-size or doubleword for 32 or 64-bit operand-size.
			else if (Zz_z.Equals("z"))
			{
				if (ins.GetOperandSize() == 16) { destOperand.带中括号的操作数的宽度.SetValue(16); }
				else if (ins.GetOperandSize() == 32 || 64) { destOperand.带中括号的操作数的宽度.SetValue(32); }
			}
		}

		/*
		*	如果oprand是'Sx'
		*/
		//S:The reg field of the ModR/M byte selects a segment register.
		//【通过MODRM.reg域决定的段寄存器】
		if (std::regex_match(Zz.GetPChar(), std::regex("[S]{1}[w]{1}")))	//Sw
		{
			//截取'Zz'的'z'
			String Zz_z = Zz.SubString(1);

			//
			destOperand.不包含宽度的操作数字符串 = ins.ModRM.GetS();

			//根据Operand Size属性和Zz，分析带中括号的操作数的宽度值，返回宽度值
			destOperand.带中括号的操作数的宽度 = ins.ModRM.Get内存操作数的宽度(ins.GetOperandSize(), Zz_z, ins.opcode);

			//得到操作数的类型（即操作数是否带括号）
			destOperand.操作数类型 = ins.ModRM.GetS的操作数类型();
		}

		/*
		*	如果oprand是'Ex'
		*/
		//E: A ModR/M byte follows the opcode and specifies the operand. The operand is either a general-purpose
		//	 register or a memory address.If it is a memory address, the address is computed from a segment register
		//	 and any of the following values : a base register, an index register, a scaling factor, a displacement.
		//	 【operand 是一个 register 或者 memory, 由 modrm.r/m 提供寻址。】
		if (std::regex_match(Zz.GetPChar(), std::regex("[E]{1}[bvwp]{1}")))	//Eb、Ev、Ew、Ep
		{
			//截取'Zz'的'z'
			String Zz_z = Zz.SubString(1);

			//将E中的寄存器序号替换为寄存器名
			destOperand.不包含宽度的操作数字符串 = ins.ModRM.GetE(ins.GetAddressSize(), ins.GetOperandSize(), Zz_z, ins.opcode);

			//根据Operand Size属性和Zz，分析带中括号的操作数的宽度值，返回宽度值
			destOperand.带中括号的操作数的宽度 = ins.ModRM.Get内存操作数的宽度(ins.GetOperandSize(), Zz_z, ins.opcode);

			//得到操作数的类型（即操作数是否带括号）
			destOperand.操作数类型 = ins.ModRM.GetE的操作数类型();
		}

		/*
		*	如果oprand是'Mx'
		*/
		//M: The ModR/M byte may refer only to memory (for example, BOUND, LES, LDS, LSS, LFS, LGS,CMPXCHG8B).
		//	 【operand 是 memory 操作数, 由 modrm.r/m 提供寻址，其中 modrm.mod != 11（因为它不是memory）】		
		std::regex regEx("[M]{1}[bwvaps]{1}");		//Ms、Mp
		if (std::regex_match(Zz.GetPChar(), regEx))
		{
			//截取'Zz'的'z'
			String Zz_z = Zz.SubString(1);

			//将M中的寄存器序号替换为寄存器名
			destOperand.不包含宽度的操作数字符串 = ins.ModRM.GetM(ins.GetAddressSize(), ins.GetOperandSize(), Zz_z, ins.opcode);

			//根据Operand Size属性和Zz，分析带中括号的操作数的宽度值，返回宽度值
			destOperand.带中括号的操作数的宽度 = ins.ModRM.Get内存操作数的宽度(ins.GetOperandSize(), Zz_z, ins.opcode);

			//得到操作数的类型（即操作数是否带括号）
			destOperand.操作数类型 = ins.ModRM.GetM的操作数类型();
		}

		/*
		*	如果oprand是'Rx'
		*/
		//R:The R/M field of the ModR/M byte may refer only to a general register (for example, MOV (0F20-0F23)).
		//【opernad 是一个通用寄存器,	 由 modrm.r/m 提供寻址，其中 modrm.mod = 11】
		if (std::regex_match(Zz.GetPChar(), std::regex("[R]{1}[v]{1}")))	//Rv
		{
			//截取'Zz'的'z'
			String Zz_z = Zz.SubString(1);

			//将R中的寄存器序号替换为寄存器名
			destOperand.不包含宽度的操作数字符串 = ins.ModRM.GetR(ins.GetAddressSize(), ins.GetOperandSize(), Zz_z, ins.opcode);

			//根据Operand Size属性和Zz，分析带中括号的操作数的宽度值，返回宽度值
			destOperand.带中括号的操作数的宽度 = ins.ModRM.Get内存操作数的宽度(ins.GetOperandSize(), Zz_z, ins.opcode);

			//得到操作数的类型（即操作数是否带括号）
			destOperand.操作数类型 = ins.ModRM.GetR的操作数类型();
		}

		/*
		*	如果oprand是'Gx'
		*/
		//G:The reg field of the ModR/M byte selects a general register (for example, AX (000)).
		//	【operand 是一个通用寄存器 （rax ~ r15）, 由 modrm.reg 提供寻址。】
		std::regex regGx("G[bwvz]{1}");	//【{n}】表示该字符允许出现任意n次，【[bv]】表示允许出现的字符有b、v
		if (std::regex_match(Zz.GetPChar(), regGx))
		{
			//截取'Zz'的'z'
			String Zz_z = Zz.SubString(1);

			//将G中的寄存器序号替换为寄存器名
			destOperand.不包含宽度的操作数字符串 = ins.ModRM.GetG(ins.GetAddressSize(), ins.GetOperandSize(), Zz_z, ins.opcode);

			//根据Operand Size属性和Zz，分析带中括号的操作数的宽度值，返回宽度值
			destOperand.带中括号的操作数的宽度 = ins.ModRM.Get内存操作数的宽度(ins.GetOperandSize(), Zz_z, ins.opcode);	//其实这里不需要获取宽度

			//得到操作数的类型（即操作数是否带括号）
			destOperand.操作数类型 = ins.ModRM.GetG的操作数类型();
		}


	}

	void PreHandle_OpcodeStr(Instruction& ins)
	{
		//处理操作码opcode
		if (std::regex_match(ins.opcode.GetPChar(), std::regex("JrCXZ")))
		{//处理操作码opcode：专门处理0xE3（JrCXZ）
			//J'rCX'Z中的rCX取决于operandSize，operandSize=32时rCX=ECX，operandSize=64时rCX=RCX.
			if (ins.GetOperandSize() == 64)
			{
				ins.opcode = std::regex_replace(ins.opcode.GetPChar(), std::regex("JrCXZ"), "JRCXZ").data();
			}
			else if (ins.GetOperandSize() == 32)
			{
				ins.opcode = std::regex_replace(ins.opcode.GetPChar(), std::regex("JrCXZ"), "JECXZ").data();
			}
		}
		else if (ins.opcode.Equals("CBW/CWDE/CDQE"))
		{//处理操作码opcode：专门处理0x98 CBW/CWDE/CDQE
			//operandSize=16 -> CBW
			//operandSize=32 -> CWDE
			//operandSize=64 -> CDQE
			switch (ins.GetOperandSize())
			{
				case 16:ins.opcode = "CBW"; break;
				case 32:ins.opcode = "CWDE"; break;
				case 64:ins.opcode = "CDQE"; break;
			}
		}
		else if (ins.opcode.Equals("CWD/CDQ/CQO"))
		{//处理操作码opcode：专门处理0x99 CWD/CDQ/CQO
			//operandSize=16 -> CWD
			//operandSize=32 -> CDQ
			//operandSize=64 -> CQO
			switch (ins.GetOperandSize())
			{
				case 16:ins.opcode = "CWD"; break;
				case 32:ins.opcode = "CDQ"; break;
				case 64:ins.opcode = "CQO"; break;
			}
		}
		else if (ins.opcode.Equals("PUSHF/D/Q"))
		{//处理操作码opcode：专门处理0x9C PUSHF/D/Q
			switch (ins.GetOperandSize())
			{
				case 16:ins.opcode = "PUSHF"; break;	//将的16位标志寄存器EFLAGS压入堆栈
				case 32:ins.opcode = "PUSHFD"; break;	//将的32位标志寄存器EFLAGS压入堆栈
				case 64:ins.opcode = "PUSHFQ"; break;	//将的64位标志寄存器RFLAGS压入堆栈
			}
		}
		else if (ins.opcode.Equals("POPF/D/Q"))
		{//处理操作码opcode：专门处理0x9D POPF/D/Q 
			switch (ins.GetOperandSize())
			{
				case 16:ins.opcode = "POPF"; break;		//将的16位标志寄存器EFLAGS取出堆栈
				case 32:ins.opcode = "POPFD"; break;	//将的32位标志寄存器EFLAGS取出堆栈
				case 64:ins.opcode = "POPFQ"; break;	//将的64位标志寄存器RFLAGS取出堆栈
			}
		}
		else if (ins.opcode.Equals("IRET/D/Q"))
		{//处理操作码opcode：专门处理0xCF IRET/D/Q
			switch (ins.GetOperandSize())
			{
				case 16:ins.opcode = "IRET"; break;
				case 32:ins.opcode = "IRETD"; break;
				case 64:ins.opcode = "IRETQ"; break;
			}
		}

		//处理操作码opcode：判断是否要加REP/REPNE前缀
		if (ins.前缀REPNE == 0xF2) { ins.opcode = String::Format("REPNE %s", ins.opcode); }
		if (ins.前缀REPE == 0xF3) { ins.opcode = String::Format("REPE %s", ins.opcode); }
		//处理操作码opcode：最后判断是否要加LOCK前缀
		if (ins.前缀LOCK == 0xF0) { ins.opcode = String::Format("LOCK %s", ins.opcode); }
	}

	//预处理
	void PreHandle(PBYTE pOpcode, String opcode, String operand01, String operand02, String superscript, OUT Instruction& ins, OUT int& insLen, String operand03 = "")
	{
		//合法性检查
		if (operand01 == NULL || operand02 == NULL || operand03 == NULL) { MessageBox(0, TEXT("operand不允许传入空值"), 0, 0); return; }


		//如果涉及MODRM字段，则在这里先映射MODRM和SIB的各域（注意，指令长度会被修改）
		std::regex regModRM("([EGMS]{1}[bwvazp]{1})");	//Ex、Gx、Mx、Sx都涉及MODRM字段
		if (std::regex_match(operand01.GetPChar(), regModRM) ||
			std::regex_match(operand02.GetPChar(), regModRM) ||
			opcode.Equals(GROUP4) ||	//GROUP4 都涉及MODRM字段
			opcode.Equals(GROUP5) ||	//GROUP5 都涉及MODRM字段
			opcode.Equals(GROUP6) ||	//GROUP6 都涉及MODRM字段
			opcode.Equals(GROUP7)		//GROUP7 都涉及MODRM字段
			)
		{
			//分析ModRM和SIB的各域
			ins.ModRM.Map_ModRM_SIB(pOpcode + 1);
			//根据REX前缀修改MODRM和SIB
			ins.前缀REX.Modify_ModRM_SIB(ins.ModRM, ins.ModRM.SIB);
			//分析指令的寄存器序号和偏移，映射为【M】、【R】、【G】字符串
			ins.ModRM.Map_M_R_G(pOpcode + 1, OUT insLen, ins.insAddr, OUT ins.公式计算的相对地址);
		}

		//预处理：处理操作码opcode和操作数operand：从GROUP表映射
		if (OpcodeExtensions::IsOpcodeExtensions(opcode))
		{
			OpcodeExtensions::Map_TableA6(opcode, ins.ModRM, pOpcode, OUT superscript, OUT ins.opcode, OUT operand01, OUT operand02);
		}
		else
		{
			ins.opcode = opcode;
		}

		//预处理：操作数的上标
		if (superscript != NULL && !superscript.Equals(""))
		{
			//d64: When in 64 - bit mode, instruction defaults to 64 - bit operand size and cannot encode 32 - bit operand size.
			//【d64上标：在64位模式下，默认设置operand size属性为64，但允许前缀对operandSize的修改】
			if (superscript.Equals("d64"))
			{
#ifdef _WIN64
				ins.SetOperandSize(64);
#endif 
			}
			//f64:The operand size is forced to a 64-bit operand size when in 64-bit mode (prefixes that change operand size are ignored for this instruction in 64 - bit mode)
			//【f64上标：在64位模式下，强制设置operand size属性为64，并且无视任何前缀对operandSize的修改】
			else if (superscript.Equals("f64"))
			{
#ifdef _WIN64
				ins.SetOperandSize(64);
				ins.允许前缀修改OperandSize = false; //不允许再通过前缀修改OperandSize属性
#endif 
			}
			else { MessageBox(0, TEXT("传入的上标错误"), 0, 0); }
		}

		//预处理：处理operand size 和Addresss size属性（根据其他前缀）
		ins.处理OperandSize和AddresssSize();

		//预处理：操作码字符串
		PreHandle_OpcodeStr(OUT ins);

		//预处理：处理操作数operand（专门处理'Rv/Mw', 如果Mod域==11，则表示是‘Rx’,否则是‘Mx’）
		if (operand01.Equals("Rv/Mw")) ins.ModRM.Mod == 0x3 ? operand01 = "Rv" : operand01 = "Mw";
		if (operand02.Equals("Rv/Mw")) ins.ModRM.Mod == 0x3 ? operand02 = "Rv" : operand02 = "Mw";

		//正式处理操作数operand：映射Zz
		ins.原始字符串operand01 = operand01;
		ins.原始字符串operand02 = operand02;
		ins.原始字符串operand03 = operand03;
		HandleZz(pOpcode, operand01, ins, OUT ins.operand01, insLen); //处理第1个操作数
		HandleZz(pOpcode, operand02, ins, OUT ins.operand02, insLen); //处理第2个操作数
		HandleZz(pOpcode, operand03, ins, OUT ins.operand03, insLen); //处理第3个操作数

		return;
	}

	//************************************
	// Method:     Map_TableA2 
	// Description:映射表2，单字节opcode的映射：Table A-2. One-byte Opcode Map 
	// Parameter:  PBYTE pOpcode - 
	// Parameter:  OUT int & insLen - 
	// Parameter:  OUT Instruction & ins - 
	// Returns:    BOOL - 
	//************************************
	BOOL Map_TableA2(PBYTE pOpcode, OUT int& insLen, OUT Instruction& ins)
	{
		switch (*pOpcode)
		{
			case 0x00: {PreHandle(pOpcode, "ADD", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }		//ADD Eb,Gb
			case 0x01: {PreHandle(pOpcode, "ADD", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }		//ADD Ev,Gv
			case 0x02: {PreHandle(pOpcode, "ADD", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }		//ADD Gb,Eb
			case 0x03: {PreHandle(pOpcode, "ADD", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }		//ADD Gv,Ev
			case 0x04: {PreHandle(pOpcode, "ADD", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }		//ADD AL,Ib
			case 0x05: {PreHandle(pOpcode, "ADD", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }		//ADD rAX,Iz
#ifndef _WIN64
			case 0x06: {PreHandle(pOpcode, "PUSH", "ES", "", NULL, ins, ++insLen); return TRUE; }		//PUSH ES (i64) 
			case 0x07: {PreHandle(pOpcode, "POP", "ES", "", NULL, ins, ++insLen); return TRUE; }		//POP ES (i64) 
#endif	
			case 0x08: {PreHandle(pOpcode, "OR", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }		//OR Eb,Gb
			case 0x09: {PreHandle(pOpcode, "OR", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }		//OR Ev,Gv
			case 0x0A: {PreHandle(pOpcode, "OR", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }		//OR Gb,Eb
			case 0x0B: {PreHandle(pOpcode, "OR", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }		//OR Gv,Ev
			case 0x0C: {PreHandle(pOpcode, "OR", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }		//OR AL,Ib
			case 0x0D: {PreHandle(pOpcode, "OR", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }		//OR rAX,Iz
#ifndef _WIN64
			case 0x0E: {PreHandle(pOpcode, "PUSH", "CS", "", NULL, ins, ++insLen); return TRUE; }		//PUSH CS (i64)
#endif	
			case 0x0F: {return Map_TableA3(pOpcode + 1, OUT ++insLen, OUT  ins); }						//2-byte escape (Table A-3)

			case 0x10: {PreHandle(pOpcode, "ADC", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }		//ADC Eb,Gb
			case 0x11: {PreHandle(pOpcode, "ADC", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }		//ADC Ev,Gv
			case 0x12: {PreHandle(pOpcode, "ADC", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }		//ADC Gb,Eb
			case 0x13: {PreHandle(pOpcode, "ADC", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }		//ADC Gv,Ev
			case 0x14: {PreHandle(pOpcode, "ADC", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }		//ADC AL,Ib
			case 0x15: {PreHandle(pOpcode, "ADC", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }		//ADC rAX,Iz
#ifndef _WIN64
			case 0x16: {PreHandle(pOpcode, "PUSH", "SS", "", NULL, ins, ++insLen); return TRUE; }		//PUSH SS (i64) 
			case 0x17: {PreHandle(pOpcode, "POP", "SS", "", NULL, ins, ++insLen); return TRUE; }		//POP SS (i64) 
#endif
			case 0x18: {PreHandle(pOpcode, "SBB", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }
			case 0x19: {PreHandle(pOpcode, "SBB", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }
			case 0x1A: {PreHandle(pOpcode, "SBB", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }
			case 0x1B: {PreHandle(pOpcode, "SBB", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }
			case 0x1C: {PreHandle(pOpcode, "SBB", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }
			case 0x1D: {PreHandle(pOpcode, "SBB", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }
#ifndef _WIN64
			case 0x1E: {PreHandle(pOpcode, "PUSH", "DS", "", NULL, ins, ++insLen); return TRUE; }		//PUSH DS (i64) 
			case 0x1F: {PreHandle(pOpcode, "POP", "DS", "", NULL, ins, ++insLen); return TRUE; }		//POP DS (i64) 
#endif	

			case 0x20: {PreHandle(pOpcode, "AND", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }		//AND Eb,Gb
			case 0x21: {PreHandle(pOpcode, "AND", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }		//AND Ev,Gv
			case 0x22: {PreHandle(pOpcode, "AND", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }		//AND Gb,Eb
			case 0x23: {PreHandle(pOpcode, "AND", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }		//AND Gv,Ev
			case 0x24: {PreHandle(pOpcode, "AND", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }		//AND AL,Ib
			case 0x25: {PreHandle(pOpcode, "AND", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }		//AND rAX,Iz
			case 0x26: {ins.前缀SEG.SetValue(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//SEG=ES (Prefix)
#ifndef _WIN64
			case 0x27: {PreHandle(pOpcode, "DAA", "", "", NULL, ins, ++insLen); return TRUE; }			//DAA (i64) 
#endif
			case 0x28: {PreHandle(pOpcode, "SUB", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }
			case 0x29: {PreHandle(pOpcode, "SUB", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }
			case 0x2A: {PreHandle(pOpcode, "SUB", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }
			case 0x2B: {PreHandle(pOpcode, "SUB", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }
			case 0x2C: {PreHandle(pOpcode, "SUB", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }
			case 0x2D: {PreHandle(pOpcode, "SUB", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }
			case 0x2E: {ins.前缀SEG.SetValue(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//SEG=CS (Prefix)
#ifndef _WIN64
			case 0x2F: {PreHandle(pOpcode, "DSA", "", "", NULL, ins, ++insLen); return TRUE; }			//DSA (i64) 
#endif
			case 0x30: {PreHandle(pOpcode, "XOR", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }		//XOR Eb,Gb
			case 0x31: {PreHandle(pOpcode, "XOR", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }		//XOR Ev,Gv
			case 0x32: {PreHandle(pOpcode, "XOR", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }		//XOR Gb,Eb
			case 0x33: {PreHandle(pOpcode, "XOR", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }		//XOR Gv,Ev
			case 0x34: {PreHandle(pOpcode, "XOR", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }		//XOR AL,Ib
			case 0x35: {PreHandle(pOpcode, "XOR", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }		//XOR rAX,Iz
			case 0x36: {ins.前缀SEG.SetValue(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//SEG=SS (Prefix)
#ifndef _WIN64
			case 0x37: {PreHandle(pOpcode, "AAA", "", "", NULL, ins, ++insLen); return TRUE; }		//AAA (i64) 
#endif
			case 0x38: {PreHandle(pOpcode, "CMP", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }
			case 0x39: {PreHandle(pOpcode, "CMP", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }
			case 0x3A: {PreHandle(pOpcode, "CMP", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }
			case 0x3B: {PreHandle(pOpcode, "CMP", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }
			case 0x3C: {PreHandle(pOpcode, "CMP", "AL", "Ib", NULL, ins, ++insLen); return TRUE; }
			case 0x3D: {PreHandle(pOpcode, "CMP", "rAX", "Iz", NULL, ins, ++insLen); return TRUE; }
			case 0x3E: {ins.前缀SEG.SetValue(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//SEG=DS (Prefix)
#ifndef _WIN64
			case 0x3F: {PreHandle(pOpcode, "AAS", "", "", NULL, ins, ++insLen); return TRUE; }		//AAS (i64) 
			case 0x40: {PreHandle(pOpcode, "INC", "eAX", "", NULL, ins, ++insLen); return TRUE; }	//INC eAX (i64) 
			case 0x41: {PreHandle(pOpcode, "INC", "eCX", "", NULL, ins, ++insLen); return TRUE; }	//INC eCX (i64) 
			case 0x42: {PreHandle(pOpcode, "INC", "eDX", "", NULL, ins, ++insLen); return TRUE; }	//INC eDX (i64) 
			case 0x43: {PreHandle(pOpcode, "INC", "eBX", "", NULL, ins, ++insLen); return TRUE; }	//INC eBX (i64) 
			case 0x44: {PreHandle(pOpcode, "INC", "eSP", "", NULL, ins, ++insLen); return TRUE; }	//INC eSP (i64) 
			case 0x45: {PreHandle(pOpcode, "INC", "eBP", "", NULL, ins, ++insLen); return TRUE; }	//INC eBP (i64) 
			case 0x46: {PreHandle(pOpcode, "INC", "eSI", "", NULL, ins, ++insLen); return TRUE; }	//INC eSI (i64) 
			case 0x47: {PreHandle(pOpcode, "INC", "eDI", "", NULL, ins, ++insLen); return TRUE; }	//INC eDI (i64) 

			case 0x48: {PreHandle(pOpcode, "DEC", "eAX", "", NULL, ins, ++insLen); return TRUE; }	//DEC eAX (i64) 
			case 0x49: {PreHandle(pOpcode, "DEC", "eCX", "", NULL, ins, ++insLen); return TRUE; }	//DEC eCX (i64) 
			case 0x4A: {PreHandle(pOpcode, "DEC", "eDX", "", NULL, ins, ++insLen); return TRUE; }	//DEC eDX (i64) 
			case 0x4B: {PreHandle(pOpcode, "DEC", "eBX", "", NULL, ins, ++insLen); return TRUE; }	//DEC eBX (i64) 
			case 0x4C: {PreHandle(pOpcode, "DEC", "eSP", "", NULL, ins, ++insLen); return TRUE; }	//DEC eSP (i64) 
			case 0x4D: {PreHandle(pOpcode, "DEC", "eBP", "", NULL, ins, ++insLen); return TRUE; }	//DEC eBP (i64) 
			case 0x4E: {PreHandle(pOpcode, "DEC", "eSI", "", NULL, ins, ++insLen); return TRUE; }	//DEC eSI (i64) 
			case 0x4F: {PreHandle(pOpcode, "DEC", "eDI", "", NULL, ins, ++insLen); return TRUE; }	//DEC eDI (i64) 
#endif

#ifdef _WIN64
			case 0x40: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX (Prefix)(o64)
			case 0x41: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.B (Prefix)(o64)
			case 0x42: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.X (Prefix)(o64)
			case 0x43: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.XB (Prefix)(o64)
			case 0x44: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.R (Prefix)(o64)
			case 0x45: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.RB (Prefix)(o64)
			case 0x46: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.RX (Prefix)(o64)
			case 0x47: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.RXB (Prefix)(o64)
			case 0x48: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.W (Prefix)(o64)
			case 0x49: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WB (Prefix)(o64)
			case 0x4A: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WX (Prefix)(o64)
			case 0x4B: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WXB (Prefix)(o64)
			case 0x4C: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WR (Prefix)(o64)
			case 0x4D: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WRB (Prefix)(o64)
			case 0x4E: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WRX (Prefix)(o64)
			case 0x4F: {ins.前缀REX.SetUp(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	 //REX.WRXB (Prefix)(o64)
#endif
			case 0x50: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_0号, "rAX", "r8"), "", "d64", ins, ++insLen); return TRUE; }		//PUSH general register(d64)	
			case 0x51: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_1号, "rCX", "r9"), "", "d64", ins, ++insLen); return TRUE; }		//PUSH general register(d64)	
			case 0x52: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_2号, "rDX", "r10"), "", "d64", ins, ++insLen); return TRUE; }	//PUSH general register(d64)	
			case 0x53: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_3号, "rBX", "r11"), "", "d64", ins, ++insLen); return TRUE; }	//PUSH general register(d64)	
			case 0x54: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_4号, "rSP", "r12"), "", "d64", ins, ++insLen); return TRUE; }	//PUSH general register(d64)
			case 0x55: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_5号, "rBP", "r13"), "", "d64", ins, ++insLen); return TRUE; }	//PUSH general register(d64)	
			case 0x56: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_6号, "rSI", "r14"), "", "d64", ins, ++insLen); return TRUE; }	//PUSH general register(d64)	
			case 0x57: { PreHandle(pOpcode, "PUSH", ins.前缀REX.ChooseREG(寄存器_7号, "rDI", "r15"), "", "d64", ins, ++insLen); return TRUE; }	//PUSH general register(d64)	
			case 0x58: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rAX", "r8"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)
			case 0x59: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rCX", "r9"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)
			case 0x5A: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rDX", "r10"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)	
			case 0x5B: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rBX", "r11"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)	
			case 0x5C: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rSP", "r12"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)	
			case 0x5D: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rBP", "r13"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)
			case 0x5E: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rSI", "r14"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)
			case 0x5F: { PreHandle(pOpcode, "POP", ins.前缀REX.ChooseREG(寄存器_0号, "rDI", "r15"), "", "d64", ins, ++insLen); return TRUE; }		//POP general register(d64)
#ifndef _WIN64
			case 0x60: {PreHandle(pOpcode, "PUSHAD", "", "", NULL, ins, ++insLen); return TRUE; }		//PUSHA/PUSHAD(i64)
			case 0x61: {PreHandle(pOpcode, "POPAD", "", "", NULL, ins, ++insLen); return TRUE; }		//POPA/POPAD(i64)
			case 0x62: {PreHandle(pOpcode, "BOUND", "Gv", "Ma", NULL, ins, ++insLen); return TRUE; }	//BOUND Gv,Ma(i64)
			case 0x63: {PreHandle(pOpcode, "ARPL", "Ew", "Gw", NULL, ins, ++insLen); return TRUE; }		//ARPL Ew, Gw(i64)
#endif
#ifdef _WIN64
			case 0x63: {PreHandle(pOpcode, "MOVSXD", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }	//MOVSXD Gv, Ev(o64)
#endif
			case 0x64: {ins.前缀SEG.SetValue(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//SEG=FS (Prefix)
			case 0x65: {ins.前缀SEG.SetValue(*pOpcode); return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//SEG=GS (Prefix)
			case 0x66: {ins.前缀OperandSize = *pOpcode; return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//Operand Size(Prefix)
			case 0x67: {ins.前缀AddressSize = *pOpcode; return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//Address Size(Prefix)
			case 0x68: {PreHandle(pOpcode, "PUSH", "Iz", "", "d64", ins, ++insLen);			return TRUE; }		//PUSH Iz (d64)
			case 0x69: {PreHandle(pOpcode, "IMUL", "Gv", "Ev", "d64", ins, ++insLen, "Iz"); return TRUE; }		//IMUL Gv, Ev, Iz
			case 0x6A: {PreHandle(pOpcode, "PUSH", "Ib", "", "d64", ins, ++insLen);			return TRUE; }		//PUSH Ib (d64)
			case 0x6B: {PreHandle(pOpcode, "IMUL", "Gv", "Ev", "d64", ins, ++insLen, "Ib"); return TRUE; }		//IMUL Gv, Ev, Ib
			case 0x6C: {PreHandle(pOpcode, "INS", "Yb", "DX", "", ins, ++insLen);	return TRUE; }	//INS Yb,Dx
			case 0x6D: {PreHandle(pOpcode, "INS", "Yz", "DX", "", ins, ++insLen);	return TRUE; }	//INS Yz,Dx
			case 0x6E: {PreHandle(pOpcode, "OUTS", "DX", "Xb", "", ins, ++insLen);	return TRUE; }	//OUTS Dx,Xb
			case 0x6F: {PreHandle(pOpcode, "OUTS", "DX", "Xz", "", ins, ++insLen);	return TRUE; }	//OUTS Dx,Xz
			case 0x70: {PreHandle(pOpcode, "JO", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64) 
			case 0x71: {PreHandle(pOpcode, "JNO", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x72: {PreHandle(pOpcode, "JB", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x73: {PreHandle(pOpcode, "JNB", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x74: {PreHandle(pOpcode, "JE", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x75: {PreHandle(pOpcode, "JNE", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x76: {PreHandle(pOpcode, "JBE", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x77: {PreHandle(pOpcode, "JNBE", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x78: {PreHandle(pOpcode, "JS", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64) 
			case 0x79: {PreHandle(pOpcode, "JNS", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x7A: {PreHandle(pOpcode, "JP", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x7B: {PreHandle(pOpcode, "JNP", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x7C: {PreHandle(pOpcode, "JL", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x7D: {PreHandle(pOpcode, "JNL", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x7E: {PreHandle(pOpcode, "JLE", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x7F: {PreHandle(pOpcode, "JNLE", "Jb", "", "f64", ins, ++insLen);	return TRUE; }	//Jcc, Jb - Short - displacement jump on condition (f64)
			case 0x80: {PreHandle(pOpcode, GROUP1, "Eb", "Ib", NULL, ins, ++insLen); return TRUE; }	//(Immediate Grp 1) Eb, Ib （1A）
			case 0x81: {PreHandle(pOpcode, GROUP1, "Ev", "Iz", NULL, ins, ++insLen); return TRUE; }	//(Immediate Grp 1) Ev, Iz （1A）
#ifndef _WIN64
			case 0x82: {PreHandle(pOpcode, GROUP1, "Eb", "Ib", NULL, ins, ++insLen); return TRUE; }	//(Immediate Grp 1) Eb, Ib （1A） (i64)
#endif
			case 0x83: {PreHandle(pOpcode, GROUP1, "Ev", "Ib", NULL, ins, ++insLen); return TRUE; }	//(Immediate Grp 1 1A) Ev, Ib （1A）
			case 0x84: {PreHandle(pOpcode, "TEST", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }	//TEST Eb, Gb 
			case 0x85: {PreHandle(pOpcode, "TEST", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }	//TEST Ev, Gv 
			case 0x86: {PreHandle(pOpcode, "XCHG", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }	//XCHG Eb, Gb 
			case 0x87: {PreHandle(pOpcode, "XCHG", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }	//XCHG Ev, Gv
			case 0x88: {PreHandle(pOpcode, "MOV", "Eb", "Gb", NULL, ins, ++insLen); return TRUE; }	//MOV Eb,Gb
			case 0x89: {PreHandle(pOpcode, "MOV", "Ev", "Gv", NULL, ins, ++insLen); return TRUE; }	//MOV Ev,Gv
			case 0x8A: {PreHandle(pOpcode, "MOV", "Gb", "Eb", NULL, ins, ++insLen); return TRUE; }	//MOV Gb,Eb
			case 0x8B: {PreHandle(pOpcode, "MOV", "Gv", "Ev", NULL, ins, ++insLen); return TRUE; }	//MOV Gv,Ev
			case 0x8C: {PreHandle(pOpcode, "MOV", "Ev", "Sw", NULL, ins, ++insLen); return TRUE; }	//MOV Ev,Sw
			case 0x8D: {PreHandle(pOpcode, "LEA", "Gv", "Mv", NULL, ins, ++insLen); return TRUE; }	//LEA Gv,M		/* 这里我将M等同于Mv */
			case 0x8E: {PreHandle(pOpcode, "MOV", "Sw", "Ew", NULL, ins, ++insLen); return TRUE; }	//MOV Ev,Sw
			case 0x8F: {PreHandle(pOpcode, GROUP1A, "Ev", "", "d64", ins, ++insLen); return TRUE; }	//(Grp 1A) POP Ev (d64)
			case 0x90:
			{
				if (ins.前缀REX.B == 0) { PreHandle(pOpcode, "NOP", "", "", NULL, ins, ++insLen); return TRUE; }
				else { PreHandle(pOpcode, "XCHG", "r8", "rAX", NULL, ins, ++insLen); return TRUE; }
			}
			case 0x91:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_1号, "rCX", "r9"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x92:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_2号, "rDX", "r10"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x93:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_3号, "rBX", "r11"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x94:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_4号, "rSP", "r12"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x95:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_5号, "rBP", "r13"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x96:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_6号, "rSI", "r14"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x97:PreHandle(pOpcode, "XCHG", ins.前缀REX.ChooseREG(寄存器_7号, "rDI", "r15"), "rAX", NULL, ins, ++insLen); return TRUE;
			case 0x98:PreHandle(pOpcode, "CBW/CWDE/CDQE", "", "", NULL, ins, ++insLen); return TRUE;	//CBW/CWDE/CDQE -> operandsize=16/32/64 bit
			case 0x99:PreHandle(pOpcode, "CWD/CDQ/CQO", "", "", NULL, ins, ++insLen); return TRUE;		//CWD/CDQ/CQO	-> operandsize=16/32/64 bit
#ifndef _WIN64
			case 0x9A:PreHandle(pOpcode, "CALL", "Ap", "", NULL, ins, ++insLen); return TRUE;		//far CALL Ap (i64)
#endif
			case 0x9B:PreHandle(pOpcode, "WAIT", "", "", NULL, ins, ++insLen); return TRUE;			//WAIT
			case 0x9C:PreHandle(pOpcode, "PUSHF/D/Q", "", "", "d64", ins, ++insLen); return TRUE;	//PUSHF/D/Q (d64)
			case 0x9D:PreHandle(pOpcode, "POPF/D/Q", "", "", "d64", ins, ++insLen); return TRUE;	//POPF/D/Q (d64)
			case 0x9E:PreHandle(pOpcode, "SAHF", "", "", "", ins, ++insLen); return TRUE;			//SAHF
			case 0x9F:PreHandle(pOpcode, "LAHF", "", "", "", ins, ++insLen); return TRUE;			//LAHF
			case 0xA0: PreHandle(pOpcode, "MOV", "AL", "Ob", NULL, ins, ++insLen); return TRUE;		//MOV AL, Ob
			case 0xA1: PreHandle(pOpcode, "MOV", "rAX", "Ov", NULL, ins, ++insLen); return TRUE;	//MOV rAX, Ov
			case 0xA2: PreHandle(pOpcode, "MOV", "Ob", "AL", NULL, ins, ++insLen); return TRUE;		//MOV Ob, AL 
			case 0xA3: PreHandle(pOpcode, "MOV", "Ov", "rAX", NULL, ins, ++insLen); return TRUE;	//MOV Ov, rAX
			case 0xA4: PreHandle(pOpcode, "MOVS", "Yb", "Xb", NULL, ins, ++insLen); return TRUE;	//MOVS Yb, Xb (MOVSB)
			case 0xA5: PreHandle(pOpcode, "MOVS", "Yv", "Xv", NULL, ins, ++insLen); return TRUE;	//MOVS Yv, Xv (MOVSW、MOVSD、MOVSQ)
			case 0xA6: PreHandle(pOpcode, "CMPS", "Xb", "Yb", NULL, ins, ++insLen); return TRUE;	//CMPS Xb, Yb (CMPSB)
			case 0xA7: PreHandle(pOpcode, "CMPS", "Xv", "Yv", NULL, ins, ++insLen); return TRUE;	//CMPS Xv, Yv (CMPSW、CMPSD)
			case 0xA8: PreHandle(pOpcode, "TEST", "AL", "Ib", NULL, ins, ++insLen); return TRUE;	//TEST AL, Ib
			case 0xA9: PreHandle(pOpcode, "TEST", "rAX", "Iz", NULL, ins, ++insLen); return TRUE;	//TEST rAX, Iz
			case 0xAA: PreHandle(pOpcode, "STOS", "Yb", "AL", NULL, ins, ++insLen); return TRUE;	//STOS Yb, AL
			case 0xAB: PreHandle(pOpcode, "STOS", "Yv", "rAX", NULL, ins, ++insLen); return TRUE;	//STOS Yv, rAX
			case 0xAC: PreHandle(pOpcode, "LODS", "AL", "Xb", NULL, ins, ++insLen); return TRUE;	//LODS AL, Xb
			case 0xAD: PreHandle(pOpcode, "LODS", "rAX", "Xv", NULL, ins, ++insLen); return TRUE;	//LODS rAX, Xv
			case 0xAE: PreHandle(pOpcode, "SCAS", "AL", "Yb", NULL, ins, ++insLen); return TRUE;	//SCAS AL, Yb
			case 0xAF: PreHandle(pOpcode, "SCAS", "rAX", "Yv", NULL, ins, ++insLen); return TRUE;	//SCAS rAX, Yv
			case 0xB0:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_0号, "AL", "R8L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB1:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_1号, "CL", "R9L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB2:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_2号, "DL", "R10L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB3:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_3号, "BL", "R11L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB4:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_4号, "AH", "R12L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB5:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_5号, "CH", "R13L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB6:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_6号, "DH", "R14L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB7:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_7号, "BH", "R15L"), "Ib", NULL, ins, ++insLen); return TRUE;
			case 0xB8:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_0号, "rAX", "r8"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xB9:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_1号, "rBX", "r9"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xBA:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_2号, "rDX", "r10"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xBB:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_3号, "rBX", "r11"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xBC:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_4号, "rSP", "r12"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xBD:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_5号, "rBP", "r13"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xBE:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_6号, "rSI", "r14"), "Iv", NULL, ins, ++insLen); return TRUE;
			case 0xBF:PreHandle(pOpcode, "MOV", ins.前缀REX.ChooseREG(寄存器_7号, "rDI", "r15"), "Iv", NULL, ins, ++insLen); return TRUE;

			case 0xC0: PreHandle(pOpcode, GROUP2, "Eb", "Ib", NULL, ins, ++insLen); return TRUE; 	//(Shift Grp 2) Eb, Ib （1A）
			case 0xC1: PreHandle(pOpcode, GROUP2, "Ev", "Ib", NULL, ins, ++insLen); return TRUE; 	//(Shift Grp 2) Ev, Ib （1A）
#ifndef _WIN64
			case 0xC4: PreHandle(pOpcode, "LES", "Gz", "Mp", NULL, ins, ++insLen); return TRUE;		//LES Gz,Mp (i64)
			case 0xC5: PreHandle(pOpcode, "LDS", "Gz", "Mp", NULL, ins, ++insLen); return TRUE;		//LDS Gz,Mp (i64)
#endif
			case 0xC6: PreHandle(pOpcode, GROUP11, "Eb", "Ib", NULL, ins, ++insLen); return TRUE;	//(Grp 11) MOV Eb, Ib (1A)
			case 0xC7: PreHandle(pOpcode, GROUP11, "Ev", "Iz", NULL, ins, ++insLen); return TRUE;	//(Grp 11) MOV Ev, Iz (1A)
			case 0xC8: PreHandle(pOpcode, "ENTER", "Iw", "Ib", NULL, ins, ++insLen); return TRUE;	//ENTER Iw, Ib 
			case 0xC9: PreHandle(pOpcode, "LEAVE", "", "", "d64", ins, ++insLen);	return TRUE;	//LEAVE	(d64)
			case 0xCC: PreHandle(pOpcode, "INT 3", "", "", NULL, ins, ++insLen);	return TRUE;	//INT 3
			case 0xCD: PreHandle(pOpcode, "INT", "Ib", "", NULL, ins, ++insLen);	return TRUE;	//INT Ib
#ifndef _WIN64
			case 0xCE: PreHandle(pOpcode, "INTO", "", "", NULL, ins, ++insLen);		return TRUE;	//INTO (i64)
#endif
			case 0xCF: PreHandle(pOpcode, "IRET/D/Q", "", "", NULL, ins, ++insLen);	return TRUE;	//IRET/D/Q

			case 0xD0:PreHandle(pOpcode, GROUP2, "Eb", "1", NULL, ins, ++insLen); return TRUE;		//(Shift Grp 2) Eb,  1（1A）
			case 0xD1:PreHandle(pOpcode, GROUP2, "Ev", "1", NULL, ins, ++insLen); return TRUE;		//(Shift Grp 2) Ev,  1（1A）
			case 0xD2:PreHandle(pOpcode, GROUP2, "Eb", "CL", NULL, ins, ++insLen); return TRUE;		//(Shift Grp 2) Eb, CL（1A）
			case 0xD3:PreHandle(pOpcode, GROUP2, "Ev", "CL", NULL, ins, ++insLen); return TRUE;		//(Shift Grp 2) Ev, CL（1A）
#ifndef _WIN64
			case 0xD4:PreHandle(pOpcode, "AAM", "Ib", "", NULL, ins, ++insLen); return TRUE;		//AAM Ib（i64）
			case 0xD5:PreHandle(pOpcode, "AAD", "Ib", "", NULL, ins, ++insLen); return TRUE;		//AAD Ib（i64）
#endif
			case 0xD7:PreHandle(pOpcode, "XLAT", "", "", NULL, ins, ++insLen); return TRUE;			// XLAT/XLATB

			case 0xE0:PreHandle(pOpcode, "LOOPNE", "Jb", "", "f64", ins, ++insLen); return TRUE;	// LOOPNE Jb (f64)
			case 0xE1:PreHandle(pOpcode, "LOOPE", "Jb", "", "f64", ins, ++insLen); return TRUE;		// LOOPE  Jb (f64)
			case 0xE2:PreHandle(pOpcode, "LOOP", "Jb", "", "f64", ins, ++insLen); return TRUE;		// LOOP   Jb (f64)
			case 0xE3:PreHandle(pOpcode, "JrCXZ", "Jb", "", "f64", ins, ++insLen); return TRUE;		// JrCXZ  Jb (f64)  rCX表示寄存器(rCX=0则跳转);
			case 0xE4:PreHandle(pOpcode, "IN", "AL", "Ib", NULL, ins, ++insLen); return TRUE;		// IN	AL,Ib 
			case 0xE5:PreHandle(pOpcode, "IN", "eAX", "Ib", NULL, ins, ++insLen); return TRUE;		// IN	eAX,Ib 
			case 0xE6:PreHandle(pOpcode, "OUT", "Ib", "AL", NULL, ins, ++insLen); return TRUE;		// OUT	Ib,AL
			case 0xE7:PreHandle(pOpcode, "OUT", "Ib", "eAX", NULL, ins, ++insLen); return TRUE;		// OUT	Ib,eAX
			case 0xE8:PreHandle(pOpcode, "CALL", "Jz", "", "f64", ins, ++insLen); return TRUE;		//near CALL	Jz (f64)
			case 0xE9:PreHandle(pOpcode, "JMP", "Jz", "", "f64", ins, ++insLen); return TRUE;		//JMP near Jz (f64)
#ifndef _WIN64
			case 0xEA:PreHandle(pOpcode, "JMP", "Ap", "", NULL, ins, ++insLen); return TRUE;		//JMP far Ap (i64)
#endif
			case 0xEB:PreHandle(pOpcode, "JMP", "Jb", "", "f64", ins, ++insLen); return TRUE;		//JMP short Jb (f64)
			case 0xEC:PreHandle(pOpcode, "IN", "AL", "DX", NULL, ins, ++insLen); return TRUE;		//IN AL, DX
			case 0xED:PreHandle(pOpcode, "IN", "eAX", "DX", NULL, ins, ++insLen); return TRUE;		//IN eAX, DX
			case 0xEE:PreHandle(pOpcode, "OUT", "AL", "DX", NULL, ins, ++insLen); return TRUE;		//OUT AL, DX
			case 0xEF:PreHandle(pOpcode, "OUT", "eAX", "DX", NULL, ins, ++insLen); return TRUE;		//OUT eAX, DX
			case 0xF0: {ins.前缀LOCK = *pOpcode; return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//LOCK (Prefix)
			case 0xF2: {ins.前缀REPNE = *pOpcode; return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//REPNE (Prefix)
			case 0xF3: {ins.前缀REPE = *pOpcode; return Map_TableA2(pOpcode + 1, OUT ++insLen, OUT  ins); }	//REPE (Prefix)
			case 0xF4:PreHandle(pOpcode, "HLT", "", "", NULL, ins, ++insLen); return TRUE;		// HLT
			case 0xF5:PreHandle(pOpcode, "CMC", "", "", NULL, ins, ++insLen); return TRUE;		// CMC
			case 0xF6:PreHandle(pOpcode, GROUP3, "Eb", "", NULL, ins, ++insLen); return TRUE;	// Unary Grp 3 Eb (1A)
			case 0xF7:PreHandle(pOpcode, GROUP3, "Ev", "", NULL, ins, ++insLen); return TRUE;	// Unary Grp 3 Ev (1A)
			case 0xF8:PreHandle(pOpcode, "CLC", "", "", NULL, ins, ++insLen); return TRUE;		// CLC
			case 0xF9:PreHandle(pOpcode, "STC", "", "", NULL, ins, ++insLen); return TRUE;		// STC
			case 0xFA:PreHandle(pOpcode, "CLI", "", "", NULL, ins, ++insLen); return TRUE;		// CLI
			case 0xFB:PreHandle(pOpcode, "STI", "", "", NULL, ins, ++insLen); return TRUE;		// STI
			case 0xFC:PreHandle(pOpcode, "CLD", "", "", NULL, ins, ++insLen); return TRUE;		// CLD
			case 0xFD:PreHandle(pOpcode, "STD", "", "", NULL, ins, ++insLen); return TRUE;		// STD
			case 0xFE:PreHandle(pOpcode, GROUP4, "", "", NULL, ins, ++insLen); return TRUE;		// INC/DEC (Grp4)(1A)
			case 0xFF:PreHandle(pOpcode, GROUP5, "", "", NULL, ins, ++insLen); return TRUE;		// INC/DEC (Grp5)(1A)


		}
		return FALSE;
	}

	//************************************
	// Method:     Map_TableA3 
	// Description:映射表3：二字节的操作数
	// Parameter:  PBYTE pOpcode - 
	// Parameter:  OUT int & insLen - 
	// Parameter:  OUT Instruction & ins - 
	// Returns:    bool - 
	//************************************
	bool Map_TableA3(PBYTE pOpcode, OUT int& insLen, OUT Instruction& ins)
	{
		switch (*pOpcode)
		{
			case 0x00:PreHandle(pOpcode, GROUP6, "", "", NULL, ins, ++insLen);			return TRUE;	//Grp 6 (1A)
			case 0x01:PreHandle(pOpcode, GROUP7, "", "", NULL, ins, ++insLen);			return TRUE;	//Grp 7 (1A)
			case 0x02:PreHandle(pOpcode, "LAR", "Gv", "Ew", NULL, ins, ++insLen);		return TRUE;	//LAR Gv, Ew
			case 0x03:PreHandle(pOpcode, "LSL", "Gv", "Ew", NULL, ins, ++insLen);		return TRUE;	//LSL Gv, Ew
#ifdef _WIN64
			case 0x05:PreHandle(pOpcode, "SYSCALL", "", "", NULL, ins, ++insLen);		return TRUE;	//SYSCALL (o64)
#endif
			case 0x06:PreHandle(pOpcode, "CLTS", "", "", NULL, ins, ++insLen);			return TRUE;	//CLTS
#ifdef _WIN64
			case 0x07:PreHandle(pOpcode, "SYSRET", "", "", NULL, ins, ++insLen);		return TRUE;	//SYSRET (o64)
#endif
			case 0x40:PreHandle(pOpcode, "CMOVO", "Gv", "Ev", NULL, ins, ++insLen);		return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x41:PreHandle(pOpcode, "CMOVNO", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x42:PreHandle(pOpcode, "CMOVB", "Gv", "Ev", NULL, ins, ++insLen);		return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x43:PreHandle(pOpcode, "CMOVNB", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x44:PreHandle(pOpcode, "CMOVE", "Gv", "Ev", NULL, ins, ++insLen);		return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x45:PreHandle(pOpcode, "CMOVNE", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x46:PreHandle(pOpcode, "CMOVBE", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x47:PreHandle(pOpcode, "CMOVNBE", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move

			case 0x48:PreHandle(pOpcode, "CMOVS", "Gv", "Ev", NULL, ins, ++insLen);		return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x49:PreHandle(pOpcode, "CMOVNS", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x4A:PreHandle(pOpcode, "CMOVP", "Gv", "Ev", NULL, ins, ++insLen);		return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x4B:PreHandle(pOpcode, "CMOVNP", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x4C:PreHandle(pOpcode, "CMOVL", "Gv", "Ev", NULL, ins, ++insLen);		return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x4D:PreHandle(pOpcode, "CMOVNL", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x4E:PreHandle(pOpcode, "CMOVLE", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move
			case 0x4F:PreHandle(pOpcode, "CMOVNLE", "Gv", "Ev", NULL, ins, ++insLen);	return TRUE;	//CMOVcc, (Gv, Ev) - Conditional Move



			case 0x80:PreHandle(pOpcode, "JO", "Jz", "", "f64", ins, ++insLen);		return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x81:PreHandle(pOpcode, "JNO", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x82:PreHandle(pOpcode, "JB", "Jz", "", "f64", ins, ++insLen);		return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x83:PreHandle(pOpcode, "JNB", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x84:PreHandle(pOpcode, "JE", "Jz", "", "f64", ins, ++insLen);		return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x85:PreHandle(pOpcode, "JNE", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x86:PreHandle(pOpcode, "JBE", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x87:PreHandle(pOpcode, "JNBE", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x88:PreHandle(pOpcode, "JS", "Jz", "", "f64", ins, ++insLen);		return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x89:PreHandle(pOpcode, "JNS", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x8A:PreHandle(pOpcode, "JP", "Jz", "", "f64", ins, ++insLen);		return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x8B:PreHandle(pOpcode, "JNP", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x8C:PreHandle(pOpcode, "JL", "Jz", "", "f64", ins, ++insLen);		return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x8D:PreHandle(pOpcode, "JNL", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x8E:PreHandle(pOpcode, "JLE", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)
			case 0x8F:PreHandle(pOpcode, "JNLE", "Jz", "", "f64", ins, ++insLen);	return TRUE;	//Jcc, Jz - Long-displacement jump on condition (f64)

			case 0xB0:PreHandle(pOpcode, "CMPXCHG", "Eb", "Gb", NULL, ins, ++insLen); return TRUE;	//CMPXCHG 
			case 0xB1:PreHandle(pOpcode, "CMPXCHG", "Ev", "Gv", NULL, ins, ++insLen); return TRUE;	//CMPXCHG 
			case 0xB2:PreHandle(pOpcode, "LSS", "Gv", "Mp", NULL, ins, ++insLen); return TRUE;		//LSS Gv, Mp
			case 0xB3:PreHandle(pOpcode, "BTR", "Ev", "Gv", NULL, ins, ++insLen); return TRUE;		//BTR Ev, Gv
			case 0xB4:PreHandle(pOpcode, "LFS", "Gv", "Mp", NULL, ins, ++insLen); return TRUE;		//LFS Gv, Mp
			case 0xB5:PreHandle(pOpcode, "LGS", "Gv", "Mp", NULL, ins, ++insLen); return TRUE;		//LGS Gv, Mp
			case 0xB6:PreHandle(pOpcode, "MOVZX", "Gv", "Eb", NULL, ins, ++insLen); return TRUE;	//MOVZX Gv, Eb
			case 0xB7:PreHandle(pOpcode, "MOVZX", "Gv", "Ew", NULL, ins, ++insLen); return TRUE;	//MOVZX Gv, Ew

		}
	}

	//************************************
	// Method:     Map_RET 
	// Description:映射函数结束语句
	// Parameter:  PBYTE pOpcode - 
	// Parameter:  OUT int & insLen - 
	// Parameter:  OUT Instruction & ins - 
	// Returns:    BOOL - 
	//************************************
	BOOL Map_RET(PBYTE pOpcode, OUT int& insLen, OUT Instruction& ins)
	{
		switch (*pOpcode)
		{
			case 0xC2: {insLen++; PreHandle(pOpcode, "RET", "Iw", "", NULL, ins, insLen); return TRUE; }	//RET Iw
			case 0xC3: {insLen++; PreHandle(pOpcode, "RET", "", "", NULL, ins, insLen); return TRUE; }		//RET 
			case 0xCA: {insLen++; PreHandle(pOpcode, "RETF", "Iw", "", NULL, ins, insLen); return TRUE; }	//RETF Iw
			case 0xCB: {insLen++; PreHandle(pOpcode, "RETF", "", "", NULL, ins, insLen); return TRUE; }	//RETF 
			default: {insLen++; ins.opcode = String::Format("[未知指令]");  return FALSE; }
		}
	}

public:

	//************************************
	// Method:     PrintCode 
	// Description:打印
	// Parameter:  Instruction ins - 
	// Returns:    void - 
	//************************************
	void PrintCode(Instruction ins)
	{
		//打印地址
		printf("0x%p | ", ins.insAddr);

		//打印反汇编代码
		printf("%-60s |", ins.GetDisassemblerCode());  //-号表示左对齐

		//打印机器码
		printf("%s", ins.GetHardCode());
		printf("\n");
	}

	//************************************
	// Method:     AutoDisassembler 
	// Description:自动反汇编至函数末尾
	// Parameter:  PBYTE pFunc - 
	// Returns:    std::list<Instruction> - 返回链表
	//************************************
	std::list<Instruction> AutoDisassembler(PBYTE pFunc)
	{
		//声明list容器
		std::list<Instruction> list;

		//指令长度
		int insLen = 0;

		//当前游标
		PBYTE cursor = pFunc;
		BOOL ISDONE = FALSE;

		while (true)
		{
			//初始化
			cursor += insLen;
			insLen = 0;
			Instruction ins;
			ins.insAddr = cursor;

			//disassembler
			if (FALSE == Map_TableA2(cursor, OUT insLen, OUT ins))
				if (TRUE == Map_RET(cursor, OUT insLen, OUT ins)) { ISDONE = TRUE; }
			ins.insLength = insLen;
			ins.Generate反汇编代码();

			//存入list容器
			list.push_back(ins);

			//判断结束
			if (ISDONE) { return  list; }
			Sleep(1);
		}
	}

	Instruction ManuelDisassembler(PBYTE cursor)
	{
		Instruction ins;
		ins.insAddr = cursor;
		if (FALSE == Map_TableA2(cursor, OUT ins.insLength, OUT ins))
			Map_RET(cursor, OUT ins.insLength, OUT ins);

		ins.Generate反汇编代码();

		return ins;
	}

};

