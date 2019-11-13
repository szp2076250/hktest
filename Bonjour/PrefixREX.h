#pragma once
#include <Windows.h>
#include "MODRM.h"

class PrefixREX
{
private:
	BYTE REX; //Ϊ�˷�����ԣ���REX��ֵ����
public:
	/*
		REXǰ׺�ĸ���λ�̶���0100,�����塣
		REXǰ׺�ĵ���λ�ֱ���W��R��X��B
	*/
	BYTE W = 0;
	BYTE R = 0;
	BYTE X = 0;
	BYTE B = 0;

	bool IsExist = false;

	//************************************
	// Method:     SetUp 
	// Description:����REXǰ׺
	// Parameter:  BYTE REX - 
	// Returns:    void - 
	//************************************
	void SetUp(BYTE REX)
	{
		if (REX != 0)
		{
			this->IsExist = true;

			//����REXǰ׺��W��R��X��B��
			this->W = REX >> 3 & 0x1;
			this->R = REX >> 2 & 0x1;
			this->X = REX >> 1 & 0x1;
			this->B = REX & 0x1;

			this->REX = REX;//���ڲ���
		}
	}

	//************************************
	// Method:     Modify_ModRM_SIB 
	// Description:����REXǰ׺����MODRM��SIB�ֶ�
	// Parameter:  OUT MODRM & ModRM - 
	// Parameter:  OUT SIB & SIB - 
	// Returns:    void - 
	//************************************
	void Modify_ModRM_SIB(OUT MODRM& ModRM, OUT SIB& SIB)
	{
		/*
		*	�ж��Ƿ���Ҫ�޸�MODRM��SIB�ֶ�(����REXǰ׺)
		*/

		//���������REXǰ׺���������
		if (this->IsExist == false) { return; }

		//���������SIB�ֶ���mod��==11�������ͼ2-4��Figure 2-4. Memory Addressing Without an SIB Byte; REX.X Not Used��
		//���������SIB�ֶ���mod��!=11�������ͼ2-5��Figure 2-5. Register-Register Addressing (No Memory Operand); REX.X Not Used��
		if (ModRM.IsExist == true && SIB.IsExist == false)
		{
			ModRM.RegOpcode += (this->R << 3);
			ModRM.RM += (this->B << 3);
		}
		//�������SIB��mod��!=11�������ͼ2-6(Figure 2-6. Memory Addressing With a SIB Byte)
		else if (ModRM.IsExist == true && SIB.IsExist == false && ModRM.Mod != 11)
		{
			ModRM.RegOpcode += (this->R << 3);
			SIB.Index += (this->X << 3);
			SIB.Base += (this->B << 3);
		}

		//�����[opcode reg]?�������ͼ2-7(Figure 2-7. Register Operand Coded in Opcode Byte; REX.X & REX.R Not Used)
		//Ŀǰ��Ϊ��REX.R���޸�egOpcode����G���͡�R�����ܵ�Ӱ��
		//else if (ModRM.IsExist == true && SIB.IsExist == false && ModRM.Mod == 11)
		//{
		//	ModRM.RegOpcode += (this->R << 3);
		//}

		else
		{
			ModRM.RegOpcode += (this->R << 3);	//REX.R�ƺ�����Ҫ�޸�MODRM.RegOpcode��
		}
	}


	//************************************
	// Method:     ChooseREG 
	// Description:ͨ��REXǰ׺ѡ�����确rAX/r8���Ĳ������������ĸ��Ĵ���
	// Parameter:  String �Ĵ������ - 
	// Parameter:  String leftREG - 
	// Parameter:  String rightREG - 
	// Returns:    String - 
	//************************************
	String ChooseREG(String �Ĵ������, String leftREG, String rightREG)
	{
		int REG = 0;

		//�Ϸ��Լ��:ֻ������0~7�żĴ���
		if (�Ĵ������.Equals(�Ĵ���_0��)) { REG = 0; }
		else if (�Ĵ������.Equals(�Ĵ���_1��)) { REG = 1; }
		else if (�Ĵ������.Equals(�Ĵ���_2��)) { REG = 2; }
		else if (�Ĵ������.Equals(�Ĵ���_3��)) { REG = 3; }
		else if (�Ĵ������.Equals(�Ĵ���_4��)) { REG = 4; }
		else if (�Ĵ������.Equals(�Ĵ���_5��)) { REG = 5; }
		else if (�Ĵ������.Equals(�Ĵ���_6��)) { REG = 6; }
		else if (�Ĵ������.Equals(�Ĵ���_7��)) { REG = 7; }
		else { MessageBox(0, TEXT("REG��Ŵ���ֻ������0~7�żĴ���"), 0, 0); return 0; }

		//�����x64�´���REXǰ׺������ܻ�ı�REG���
		if (this->IsExist)
		{
			REG += this->B << 3;
		}

		//����REG���ӳ��Ĵ�����:���ͨ��REX�������󣬼Ĵ��������С��7��˵������ߵļĴ����������� rAX/r8����߼Ĵ�������rAX
		if (REG <= 7) { return leftREG; }
		else { return rightREG; }
	}

	//************************************
	// Method:     d64 
	// Description:����REXǰ׺���¼���Ĵ���������d64�ϱ������£�
	// Parameter:  int REG��� - 
	// Parameter:  PrefixREX ǰ׺REX - 
	// Parameter:  operandSize - 
	// Returns:    String - 
	//************************************
	String d64(String �Ĵ������, int& operandSize)
	{
		int REG = 0;
		//�Ϸ��Լ��:ֻ������0~7�żĴ���
		if (�Ĵ������.Equals(�Ĵ���_0��)) { REG = 0; }
		else if (�Ĵ������.Equals(�Ĵ���_1��)) { REG = 1; }
		else if (�Ĵ������.Equals(�Ĵ���_2��)) { REG = 2; }
		else if (�Ĵ������.Equals(�Ĵ���_3��)) { REG = 3; }
		else if (�Ĵ������.Equals(�Ĵ���_4��)) { REG = 4; }
		else if (�Ĵ������.Equals(�Ĵ���_5��)) { REG = 5; }
		else if (�Ĵ������.Equals(�Ĵ���_6��)) { REG = 6; }
		else if (�Ĵ������.Equals(�Ĵ���_7��)) { REG = 7; }
		else { MessageBox(0, TEXT("REG��Ŵ���ֻ����0~7�żĴ���"), 0, 0); return 0; }

#ifdef _WIN64

		//d64: When in 64 - bit mode, instruction defaults to 64 - bit operand size and cannot encode 32 - bit operand size.
		//�����x64�£�d64�ϱ�ὫoperandSize���Ը�Ϊ64
		operandSize = 64;

		//�����x64�´���REXǰ׺������ܻ�ı�REG���
		if (this->IsExist)
		{
			REG += this->B << 3;
		}

		//����REG���ӳ��Ĵ�����
		switch (REG)
		{
			case 0:return "rAX";
			case 1:return "rCX";
			case 2:return"rDX";
			case 3:return "rBX";
			case 4:return "rSP";
			case 5:return "rBP";
			case 6:return "rSI";
			case 7:return "rDI";
			case 8:return "r8";
			case 9:return "r9";
			case 10:return "r10";
			case 11:return "r11";
			case 12:return "r12";
			case 13:return "r13";
			case 14:return "r14";
			case 15:return "r15";
		}
#else
		//����REG���ӳ��Ĵ�����
		switch (REG)
		{
			case 0:return "eAX";
			case 1:return "eCX";
			case 2:return "eDX";
			case 3:return "eBX";
			case 4:return "eSP";
			case 5:return "eBP";
			case 6:return "eSI";
			case 7:return "eDI";
		}
#endif
	}
};



