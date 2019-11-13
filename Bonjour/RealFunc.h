#pragma once
#include <iostream>
#include <windows.h>

#include "Println.h"
#include "Engine.h"


#ifdef _WIN64
/*
	PUSH XXX
	MOV dword ptr ss[rsp+04h],XXX
	RET
	共14字节
*/
#define 拷贝的最小字节数_InlineHook 14 
#else
/*
	E8 XX XX XX XX    JMP XXX
	共5字节
*/
#define 拷贝的最小字节数_InlineHook 5 
#endif 
#define 拷贝的最小字节数_VEHHook 1 



class RealFunc {

public:

	int OriginFunc被覆盖的代码长度 = 0;	//至少是14字节
	int RealFunc粘贴处的代码长度 = 0;		//RealFunc粘贴处的代码长度 = OriginFunc被覆盖的代码长度(necessary) + 5字节的JMP(necessary) + 为了修正相对寻址的额外代码长度(optional)
	PBYTE RealFunc粘贴处的位置 = 0;


	//************************************
	// Method:     CopyTopOfOriginFunc 
	// Description:拷贝被挂钩函数的前几字节至前后2GB范围的位置
	// Parameter:  PBYTE pOriginFunc - 
	// Parameter:  int leastLen - 所需的最小指令长度，对于InlineHook，64位进程的最小指令长度=14，32位进程=5；对于VEH HOOK，最小值指令长度=1，因为只需拷贝首句代码；
	// Returns:    void - 被覆盖代码的粘贴位置
	//************************************
	void CopyTopOfOriginFunc(PBYTE pOriginFunc, int leastLen)
	{
		//计算被挂钩函数的前几字节代码中所用到的最前、最后地址
		PBYTE minAddr = 0;
		PBYTE maxAddr = 0;
		this->Analyze_MinMaxAddr(pOriginFunc, leastLen, OUT minAddr, OUT maxAddr);

		//计算被挂钩函数被覆盖的代码长度、被挂钩函数粘贴处所需的代码长度
		this->Analyze_CoveredLen_TotalLen(pOriginFunc, leastLen, OUT this->OriginFunc被覆盖的代码长度, OUT this->RealFunc粘贴处的代码长度);

		//申请虚拟内存，必须是距离被挂钩函数不超过前后2GB范围内
		this->RealFunc粘贴处的位置 = AllocMemIn2GB(minAddr, maxAddr, this->RealFunc粘贴处的代码长度);

		//将被挂钩函数的前几字节拷贝至新内存空间，并在末尾添加JMP指令以跳转回被挂钩函数的剩余代码，并处理被挂钩函数的前几字节中的相对地址
		HandleToNewMem(pOriginFunc, this->RealFunc粘贴处的位置, this->OriginFunc被覆盖的代码长度);

		return;
	}
private:
	//************************************
	// Method:     AllocMemIn2GB 
	// Description:申请虚拟内存，必须是距离被挂钩函数不超过前后2GB范围内
	// Parameter:  PBYTE minInstructionAddr - 被覆盖的指令集中用到的最前（最小）的地址 
	// Parameter:  PBYTE maxInstructionAddr - 被覆盖的指令集中用到的最后（最大）的地址
	// Parameter:  int needLen - 
	// Returns:    PBYTE - 
	//************************************
	PBYTE AllocMemIn2GB(PBYTE minInstructionAddr, PBYTE maxInstructionAddr, int needLen)
	{
		Println::INFO(String::Format("开始申请2GB范围内的虚拟内存空间，被挂钩函数的前几字节代码中用到的最小地址=%p  最大地址=%p", minInstructionAddr, maxInstructionAddr));

		//根据最前和最后的指令计算2GB范围内的地址
		PBYTE pFront = maxInstructionAddr - 0x7FFFFFFF;	//在2GB内能寻址的最前地址
		PBYTE pBack = minInstructionAddr + 0x7FFFFFFF;	//在2GB内能寻址的最后地址
		pFront += (0x10000 - ((UINT64)pFront % 0x10000));//最前地址修正为下个64kb对齐的地址
		PBYTE pCoverdCode = 0;
		while (true)
		{
			//申请的地址值必须是以64kb对齐(即0x10000整数倍的地址值)
			//如果传入参数不是64kb对齐的则会自动取整，例如传入0x36666时会自动修正为申请0x30000的内存区域
#ifdef _WIN64
			pCoverdCode = (PBYTE)VirtualAllocEx(GetCurrentProcess(), (LPVOID)pFront, needLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
			pCoverdCode = (PBYTE)VirtualAllocEx(GetCurrentProcess(), (LPVOID)NULL, needLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
			//申请下一个64kb对齐的地址
			pFront += 0x10000;
			if (pCoverdCode != 0)
			{
				Println::SUCCESS(String::Format("申请虚拟内存成功，起始地址=%p", pCoverdCode));
				return pCoverdCode;
			}

			if (pFront + 14 >= pBack)
			{
				Println::Error(String::Format("申请失败，被挂钩函数的前后2GB范围内找不到可用空间"));
				return 0;
			}
		};
	}




	//************************************
	// Method:     Analyze_MinMaxAddr 
	// Description:分析汇编指令中涉及到的最大(后)地址、最小(前)地址
	// Parameter:  PBYTE pDestFunc - 
	// Parameter:  OUT PBYTE & minAddr - 
	// Parameter:  OUT PBYTE & maxAddr - 
	// Returns:    void - 
	//************************************
	void Analyze_MinMaxAddr(PBYTE pDestFunc, int leastLen, PBYTE& minAddr, OUT PBYTE& maxAddr)
	{
		minAddr = pDestFunc;
		maxAddr = pDestFunc;
		int CoveredLen = 0;	//被覆盖代码的长度
		Engine disambler;
		while (true)
		{
			Instruction ins = disambler.ManuelDisassembler(pDestFunc + CoveredLen);

			CoveredLen += ins.insLength;

			if (ins.insAddr < minAddr) { minAddr = ins.insAddr; }
			if (ins.insAddr > maxAddr) { maxAddr = ins.insAddr; }
			if (ins.公式计算的相对地址.IsExist)
			{
				if (ins.公式计算的相对地址.Get目标地址() < minAddr) { minAddr = ins.公式计算的相对地址.Get目标地址(); }
				if (ins.公式计算的相对地址.Get目标地址() > maxAddr) { maxAddr = ins.公式计算的相对地址.Get目标地址(); }
			}

			//当已分析的指令长度 > 所需的最小长度时，退出循环。
			if (CoveredLen >= leastLen)
			{
				return;
			}
		}
	}

	//************************************
	// Method:     Analyze_CoveredLen_TotalLen 
	// Description:计算被挂钩函数被覆盖的代码长度、被挂钩函数粘贴处所需的代码长度
	// Parameter:  PBYTE pDestFunc - 
	// Parameter:  OUT int coveredLen - 被覆盖代码的长度
	// Parameter:  OUT int totalLen - 粘贴处所需的总长度
	// Returns:    void - 
	//************************************
	void Analyze_CoveredLen_TotalLen(PBYTE pDestFunc, int leastLen, OUT int& coveredLen, OUT int& totalLen)
	{
		//粘贴处所需的代码总长度 = 被覆盖代码的长度 + 5字节的JMP + ‘Jb’被转换为‘JMP’的长度 
		coveredLen = 0;
		totalLen = coveredLen + 5;
		Engine disambler;
		while (true)
		{
			Instruction ins = disambler.ManuelDisassembler(pDestFunc + coveredLen);
			coveredLen += ins.insLength;
			totalLen += ins.insLength;
			//暂时只有‘Jb’需要额外增加一句JMP语句，因为‘Jb’能跳转的范围太小了
			if (ins.原始字符串operand01.Equals("Jb") || ins.原始字符串operand02.Equals("Jb") || ins.原始字符串operand03.Equals("Jb"))
			{
				totalLen += 0x5;	//额外的JMP语句占5字节（E9 XX XX XX XX）
			}
			//
			if (coveredLen >= leastLen)
			{
				return;
			}
		}



	}


	//************************************
	// Method:     HandleToNewMem 
	// Description:将被挂钩函数的前几字节拷贝至新内存空间，并在末尾添加JMP指令以跳转回被挂钩函数的剩余代码，并处理被挂钩函数的前几字节中的相对地址
	// Parameter:  PBYTE pDestFunc - 
	// Parameter:  PBYTE pVirtualMem - 
	// Parameter:  int coveredCodeSize - 
	// Returns:    void - 
	//************************************
	void HandleToNewMem(PBYTE pOriginFunc, PBYTE pVirtualMem, int coveredCodeSize)
	{

		PBYTE cursor_ExtraJMP = pVirtualMem + coveredCodeSize + 0x5;	//始终指向：由‘Jb’转换成的‘JMP’指令,即指向预留给额外的新JMP指令的位置
		PBYTE cursor_CurrentWrite = pVirtualMem;						//始终指向：正在写入的地址
		Engine disambler;
		int 已写入的被覆盖代码长度 = 0;
		while (true)
		{
			//反汇编
			Instruction ins = disambler.ManuelDisassembler(pOriginFunc + 已写入的被覆盖代码长度);

			//以下两种情况，需要修正offset偏移，从而使相对地址不变
			//【1】在64位下，且Mod = 0，RM = 101时，需要用公式计算相对地址，是32位偏移
			//【2】当'Jz'时，若addressSize=32或64，同样是是32位偏移（这里暂时不考虑addressSize=16的情况，因为没遇到过）
			if ((sizeof(void*) == 8 && ins.ModRM.Mod == 0x0 && ins.ModRM.RM == 0x5)
				|| (ins.原始字符串operand01.Equals("Jz") || ins.原始字符串operand02.Equals("Jz") || ins.原始字符串operand02.Equals("Jz")))
			{
				//构建新Jz指令
				PBYTE old_Jz_DestAddr = ins.公式计算的相对地址.Get目标地址();						//原Jz指令的目标地址
				UINT32 new_Jz_Offset = old_Jz_DestAddr - (cursor_CurrentWrite + ins.insLength);	//新Jz指令偏移值 = 原Jb指令的目标地址 - 新Jz指令的下一条地址 （offset是有符号数，需转为无符号数硬编码）
				int 相对地址之前的硬编码size = ins.公式计算的相对地址.Get指向偏移的指针() - ins.insAddr;

				//写入新Jz指令
				memcpy(cursor_CurrentWrite, ins.insAddr, 相对地址之前的硬编码size);
				memcpy(cursor_CurrentWrite + 相对地址之前的硬编码size, &new_Jz_Offset, sizeof(new_Jz_Offset));
			}
			//当'Jb'时，由于可跳转的范围太小，所以需要将 jb 转为 jb + jmp 的形式
			else if (ins.原始字符串operand01.Equals("Jb") || ins.原始字符串operand02.Equals("Jb") || ins.原始字符串operand02.Equals("Jb"))
			{
				//构建新Jb指令
				UINT8 new_Jb_Offset = cursor_ExtraJMP - (cursor_CurrentWrite + 0x2);				//新偏移值 = 额外的新JMP指令地址 - 新Jb指令的下一条地址 (Jb指令占2字节) ,（offset是有符号数，需转为无符号数硬编码）
				int opcode硬编码所占字节数 = ins.公式计算的相对地址.Get指向偏移的指针() - ins.insAddr;	//计算Jb指令的opcode硬编码所占字节数

				//写入新的Jb指令
				memcpy(cursor_CurrentWrite, ins.insAddr, opcode硬编码所占字节数);								//写入新‘Jb’指令的opcode
				memcpy(cursor_CurrentWrite + opcode硬编码所占字节数, &new_Jb_Offset, sizeof(new_Jb_Offset));	//写入新‘Jb’指令的新offset

				//构建额外的新JMP指令
				PBYTE new_JMP_Dest = ins.公式计算的相对地址.Get目标地址();				//额外的新JMP指令要跳转的地址 = 原Jb指令要跳转的地址
				UINT32 new_JMP_Offset = new_JMP_Dest - (cursor_ExtraJMP + 0x5);	//额外的新JMP指令的偏移 = 额外的新JMP指令要跳转的地址 - 额外的新JMP指令的下一条指令地址(JMP指令占5字节)

				//写入新的额外JMP指令
				*cursor_ExtraJMP = 0xE9;	//0xE9 = JMP
				memcpy(cursor_ExtraJMP + 1, &new_JMP_Offset, sizeof(new_JMP_Offset));

				//预留给额外JMP指令的光标+5
				cursor_ExtraJMP += 0x5;
			}

			else
			{
				//其他汇编代码无需修改，直接拷贝粘贴
				memcpy(cursor_CurrentWrite, ins.insAddr, ins.insLength);
			}

			//移动:正在写入的光标
			cursor_CurrentWrite += ins.insLength;
			//累加:已写入的被覆盖指令长度
			已写入的被覆盖代码长度 += ins.insLength;

			//如果所有被覆盖代码已粘贴写入完毕，最后需写入一个JMP指令跳转回被Hook的函数的剩余代码
			if (已写入的被覆盖代码长度 == coveredCodeSize)
			{
				//构建跳转回原函数的JMP指令
				PBYTE new_JMP_Dest = ins.insAddr + ins.insLength;	//被hook的原函数的'剩余代码'的位置
				UINT32 new_JMP_Offset = new_JMP_Dest - (cursor_CurrentWrite + 0x5);	//新JMP指令的偏移 = 新JMP指令要跳转的地址 - 新JMP指令的下一条指令地址(JMP指令占5字节)

				//写入跳转回原函数的JMP指令
				*cursor_CurrentWrite = 0xE9;	//0xE9 = JMP
				memcpy(cursor_CurrentWrite + 1, &new_JMP_Offset, sizeof(new_JMP_Offset));

				return;
			}
		}
	}


};