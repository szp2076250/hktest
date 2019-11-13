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
	��14�ֽ�
*/
#define ��������С�ֽ���_InlineHook 14 
#else
/*
	E8 XX XX XX XX    JMP XXX
	��5�ֽ�
*/
#define ��������С�ֽ���_InlineHook 5 
#endif 
#define ��������С�ֽ���_VEHHook 1 



class RealFunc {

public:

	int OriginFunc�����ǵĴ��볤�� = 0;	//������14�ֽ�
	int RealFuncճ�����Ĵ��볤�� = 0;		//RealFuncճ�����Ĵ��볤�� = OriginFunc�����ǵĴ��볤��(necessary) + 5�ֽڵ�JMP(necessary) + Ϊ���������Ѱַ�Ķ�����볤��(optional)
	PBYTE RealFuncճ������λ�� = 0;


	//************************************
	// Method:     CopyTopOfOriginFunc 
	// Description:�������ҹ�������ǰ���ֽ���ǰ��2GB��Χ��λ��
	// Parameter:  PBYTE pOriginFunc - 
	// Parameter:  int leastLen - �������Сָ��ȣ�����InlineHook��64λ���̵���Сָ���=14��32λ����=5������VEH HOOK����Сֵָ���=1����Ϊֻ�追���׾���룻
	// Returns:    void - �����Ǵ����ճ��λ��
	//************************************
	void CopyTopOfOriginFunc(PBYTE pOriginFunc, int leastLen)
	{
		//���㱻�ҹ�������ǰ���ֽڴ��������õ�����ǰ������ַ
		PBYTE minAddr = 0;
		PBYTE maxAddr = 0;
		this->Analyze_MinMaxAddr(pOriginFunc, leastLen, OUT minAddr, OUT maxAddr);

		//���㱻�ҹ����������ǵĴ��볤�ȡ����ҹ�����ճ��������Ĵ��볤��
		this->Analyze_CoveredLen_TotalLen(pOriginFunc, leastLen, OUT this->OriginFunc�����ǵĴ��볤��, OUT this->RealFuncճ�����Ĵ��볤��);

		//���������ڴ棬�����Ǿ��뱻�ҹ�����������ǰ��2GB��Χ��
		this->RealFuncճ������λ�� = AllocMemIn2GB(minAddr, maxAddr, this->RealFuncճ�����Ĵ��볤��);

		//�����ҹ�������ǰ���ֽڿ��������ڴ�ռ䣬����ĩβ���JMPָ������ת�ر��ҹ�������ʣ����룬�������ҹ�������ǰ���ֽ��е���Ե�ַ
		HandleToNewMem(pOriginFunc, this->RealFuncճ������λ��, this->OriginFunc�����ǵĴ��볤��);

		return;
	}
private:
	//************************************
	// Method:     AllocMemIn2GB 
	// Description:���������ڴ棬�����Ǿ��뱻�ҹ�����������ǰ��2GB��Χ��
	// Parameter:  PBYTE minInstructionAddr - �����ǵ�ָ����õ�����ǰ����С���ĵ�ַ 
	// Parameter:  PBYTE maxInstructionAddr - �����ǵ�ָ����õ��������󣩵ĵ�ַ
	// Parameter:  int needLen - 
	// Returns:    PBYTE - 
	//************************************
	PBYTE AllocMemIn2GB(PBYTE minInstructionAddr, PBYTE maxInstructionAddr, int needLen)
	{
		Println::INFO(String::Format("��ʼ����2GB��Χ�ڵ������ڴ�ռ䣬���ҹ�������ǰ���ֽڴ������õ�����С��ַ=%p  ����ַ=%p", minInstructionAddr, maxInstructionAddr));

		//������ǰ������ָ�����2GB��Χ�ڵĵ�ַ
		PBYTE pFront = maxInstructionAddr - 0x7FFFFFFF;	//��2GB����Ѱַ����ǰ��ַ
		PBYTE pBack = minInstructionAddr + 0x7FFFFFFF;	//��2GB����Ѱַ������ַ
		pFront += (0x10000 - ((UINT64)pFront % 0x10000));//��ǰ��ַ����Ϊ�¸�64kb����ĵ�ַ
		PBYTE pCoverdCode = 0;
		while (true)
		{
			//����ĵ�ֵַ��������64kb����(��0x10000�������ĵ�ֵַ)
			//��������������64kb���������Զ�ȡ�������紫��0x36666ʱ���Զ�����Ϊ����0x30000���ڴ�����
#ifdef _WIN64
			pCoverdCode = (PBYTE)VirtualAllocEx(GetCurrentProcess(), (LPVOID)pFront, needLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
			pCoverdCode = (PBYTE)VirtualAllocEx(GetCurrentProcess(), (LPVOID)NULL, needLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
			//������һ��64kb����ĵ�ַ
			pFront += 0x10000;
			if (pCoverdCode != 0)
			{
				Println::SUCCESS(String::Format("���������ڴ�ɹ�����ʼ��ַ=%p", pCoverdCode));
				return pCoverdCode;
			}

			if (pFront + 14 >= pBack)
			{
				Println::Error(String::Format("����ʧ�ܣ����ҹ�������ǰ��2GB��Χ���Ҳ������ÿռ�"));
				return 0;
			}
		};
	}




	//************************************
	// Method:     Analyze_MinMaxAddr 
	// Description:�������ָ�����漰�������(��)��ַ����С(ǰ)��ַ
	// Parameter:  PBYTE pDestFunc - 
	// Parameter:  OUT PBYTE & minAddr - 
	// Parameter:  OUT PBYTE & maxAddr - 
	// Returns:    void - 
	//************************************
	void Analyze_MinMaxAddr(PBYTE pDestFunc, int leastLen, PBYTE& minAddr, OUT PBYTE& maxAddr)
	{
		minAddr = pDestFunc;
		maxAddr = pDestFunc;
		int CoveredLen = 0;	//�����Ǵ���ĳ���
		Engine disambler;
		while (true)
		{
			Instruction ins = disambler.ManuelDisassembler(pDestFunc + CoveredLen);

			CoveredLen += ins.insLength;

			if (ins.insAddr < minAddr) { minAddr = ins.insAddr; }
			if (ins.insAddr > maxAddr) { maxAddr = ins.insAddr; }
			if (ins.��ʽ�������Ե�ַ.IsExist)
			{
				if (ins.��ʽ�������Ե�ַ.GetĿ���ַ() < minAddr) { minAddr = ins.��ʽ�������Ե�ַ.GetĿ���ַ(); }
				if (ins.��ʽ�������Ե�ַ.GetĿ���ַ() > maxAddr) { maxAddr = ins.��ʽ�������Ե�ַ.GetĿ���ַ(); }
			}

			//���ѷ�����ָ��� > �������С����ʱ���˳�ѭ����
			if (CoveredLen >= leastLen)
			{
				return;
			}
		}
	}

	//************************************
	// Method:     Analyze_CoveredLen_TotalLen 
	// Description:���㱻�ҹ����������ǵĴ��볤�ȡ����ҹ�����ճ��������Ĵ��볤��
	// Parameter:  PBYTE pDestFunc - 
	// Parameter:  OUT int coveredLen - �����Ǵ���ĳ���
	// Parameter:  OUT int totalLen - ճ����������ܳ���
	// Returns:    void - 
	//************************************
	void Analyze_CoveredLen_TotalLen(PBYTE pDestFunc, int leastLen, OUT int& coveredLen, OUT int& totalLen)
	{
		//ճ��������Ĵ����ܳ��� = �����Ǵ���ĳ��� + 5�ֽڵ�JMP + ��Jb����ת��Ϊ��JMP���ĳ��� 
		coveredLen = 0;
		totalLen = coveredLen + 5;
		Engine disambler;
		while (true)
		{
			Instruction ins = disambler.ManuelDisassembler(pDestFunc + coveredLen);
			coveredLen += ins.insLength;
			totalLen += ins.insLength;
			//��ʱֻ�С�Jb����Ҫ��������һ��JMP��䣬��Ϊ��Jb������ת�ķ�Χ̫С��
			if (ins.ԭʼ�ַ���operand01.Equals("Jb") || ins.ԭʼ�ַ���operand02.Equals("Jb") || ins.ԭʼ�ַ���operand03.Equals("Jb"))
			{
				totalLen += 0x5;	//�����JMP���ռ5�ֽڣ�E9 XX XX XX XX��
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
	// Description:�����ҹ�������ǰ���ֽڿ��������ڴ�ռ䣬����ĩβ���JMPָ������ת�ر��ҹ�������ʣ����룬�������ҹ�������ǰ���ֽ��е���Ե�ַ
	// Parameter:  PBYTE pDestFunc - 
	// Parameter:  PBYTE pVirtualMem - 
	// Parameter:  int coveredCodeSize - 
	// Returns:    void - 
	//************************************
	void HandleToNewMem(PBYTE pOriginFunc, PBYTE pVirtualMem, int coveredCodeSize)
	{

		PBYTE cursor_ExtraJMP = pVirtualMem + coveredCodeSize + 0x5;	//ʼ��ָ���ɡ�Jb��ת���ɵġ�JMP��ָ��,��ָ��Ԥ�����������JMPָ���λ��
		PBYTE cursor_CurrentWrite = pVirtualMem;						//ʼ��ָ������д��ĵ�ַ
		Engine disambler;
		int ��д��ı����Ǵ��볤�� = 0;
		while (true)
		{
			//�����
			Instruction ins = disambler.ManuelDisassembler(pOriginFunc + ��д��ı����Ǵ��볤��);

			//���������������Ҫ����offsetƫ�ƣ��Ӷ�ʹ��Ե�ַ����
			//��1����64λ�£���Mod = 0��RM = 101ʱ����Ҫ�ù�ʽ������Ե�ַ����32λƫ��
			//��2����'Jz'ʱ����addressSize=32��64��ͬ������32λƫ�ƣ�������ʱ������addressSize=16���������Ϊû��������
			if ((sizeof(void*) == 8 && ins.ModRM.Mod == 0x0 && ins.ModRM.RM == 0x5)
				|| (ins.ԭʼ�ַ���operand01.Equals("Jz") || ins.ԭʼ�ַ���operand02.Equals("Jz") || ins.ԭʼ�ַ���operand02.Equals("Jz")))
			{
				//������Jzָ��
				PBYTE old_Jz_DestAddr = ins.��ʽ�������Ե�ַ.GetĿ���ַ();						//ԭJzָ���Ŀ���ַ
				UINT32 new_Jz_Offset = old_Jz_DestAddr - (cursor_CurrentWrite + ins.insLength);	//��Jzָ��ƫ��ֵ = ԭJbָ���Ŀ���ַ - ��Jzָ�����һ����ַ ��offset���з���������תΪ�޷�����Ӳ���룩
				int ��Ե�ַ֮ǰ��Ӳ����size = ins.��ʽ�������Ե�ַ.Getָ��ƫ�Ƶ�ָ��() - ins.insAddr;

				//д����Jzָ��
				memcpy(cursor_CurrentWrite, ins.insAddr, ��Ե�ַ֮ǰ��Ӳ����size);
				memcpy(cursor_CurrentWrite + ��Ե�ַ֮ǰ��Ӳ����size, &new_Jz_Offset, sizeof(new_Jz_Offset));
			}
			//��'Jb'ʱ�����ڿ���ת�ķ�Χ̫С��������Ҫ�� jb תΪ jb + jmp ����ʽ
			else if (ins.ԭʼ�ַ���operand01.Equals("Jb") || ins.ԭʼ�ַ���operand02.Equals("Jb") || ins.ԭʼ�ַ���operand02.Equals("Jb"))
			{
				//������Jbָ��
				UINT8 new_Jb_Offset = cursor_ExtraJMP - (cursor_CurrentWrite + 0x2);				//��ƫ��ֵ = �������JMPָ���ַ - ��Jbָ�����һ����ַ (Jbָ��ռ2�ֽ�) ,��offset���з���������תΪ�޷�����Ӳ���룩
				int opcodeӲ������ռ�ֽ��� = ins.��ʽ�������Ե�ַ.Getָ��ƫ�Ƶ�ָ��() - ins.insAddr;	//����Jbָ���opcodeӲ������ռ�ֽ���

				//д���µ�Jbָ��
				memcpy(cursor_CurrentWrite, ins.insAddr, opcodeӲ������ռ�ֽ���);								//д���¡�Jb��ָ���opcode
				memcpy(cursor_CurrentWrite + opcodeӲ������ռ�ֽ���, &new_Jb_Offset, sizeof(new_Jb_Offset));	//д���¡�Jb��ָ�����offset

				//�����������JMPָ��
				PBYTE new_JMP_Dest = ins.��ʽ�������Ե�ַ.GetĿ���ַ();				//�������JMPָ��Ҫ��ת�ĵ�ַ = ԭJbָ��Ҫ��ת�ĵ�ַ
				UINT32 new_JMP_Offset = new_JMP_Dest - (cursor_ExtraJMP + 0x5);	//�������JMPָ���ƫ�� = �������JMPָ��Ҫ��ת�ĵ�ַ - �������JMPָ�����һ��ָ���ַ(JMPָ��ռ5�ֽ�)

				//д���µĶ���JMPָ��
				*cursor_ExtraJMP = 0xE9;	//0xE9 = JMP
				memcpy(cursor_ExtraJMP + 1, &new_JMP_Offset, sizeof(new_JMP_Offset));

				//Ԥ��������JMPָ��Ĺ��+5
				cursor_ExtraJMP += 0x5;
			}

			else
			{
				//���������������޸ģ�ֱ�ӿ���ճ��
				memcpy(cursor_CurrentWrite, ins.insAddr, ins.insLength);
			}

			//�ƶ�:����д��Ĺ��
			cursor_CurrentWrite += ins.insLength;
			//�ۼ�:��д��ı�����ָ���
			��д��ı����Ǵ��볤�� += ins.insLength;

			//������б����Ǵ�����ճ��д����ϣ������д��һ��JMPָ����ת�ر�Hook�ĺ�����ʣ�����
			if (��д��ı����Ǵ��볤�� == coveredCodeSize)
			{
				//������ת��ԭ������JMPָ��
				PBYTE new_JMP_Dest = ins.insAddr + ins.insLength;	//��hook��ԭ������'ʣ�����'��λ��
				UINT32 new_JMP_Offset = new_JMP_Dest - (cursor_CurrentWrite + 0x5);	//��JMPָ���ƫ�� = ��JMPָ��Ҫ��ת�ĵ�ַ - ��JMPָ�����һ��ָ���ַ(JMPָ��ռ5�ֽ�)

				//д����ת��ԭ������JMPָ��
				*cursor_CurrentWrite = 0xE9;	//0xE9 = JMP
				memcpy(cursor_CurrentWrite + 1, &new_JMP_Offset, sizeof(new_JMP_Offset));

				return;
			}
		}
	}


};