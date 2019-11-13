#pragma once
#include "String.h"
#include "MODRM.h"
#include "PrefixREX.h"
#include "PrefixSEG.h"
#include "OperandWidth.h"
#include "Operand.h"
#include "��ʽ�������Ե�ַ.h"
class Instruction
{
private:

	String ��������;


	int OperandSize = 0;
	int AddressSize = 0;

public:
	String ԭʼ�ַ���operand01;
	String ԭʼ�ַ���operand02;
	String ԭʼ�ַ���operand03;

	String opcode;
	Operand operand01;
	Operand operand02;
	Operand operand03;	//IMULָ��(0x69)ӵ��3��������

	int insLength = 0;		//��ǰָ����ռ�ֽڳ���
	PBYTE insAddr = 0;		//��ǰָ��ĵ�ֵַ

	��ʽ�������Ե�ַ ��ʽ�������Ե�ַ;

	/*
		��װ OperandSize �� AddressSize
	*/
	bool ����ǰ׺�޸�OperandSize = true;	//�Ƿ��������޸�OperandSize
	bool ����ǰ׺�޸�AddressSize = true;	//�Ƿ��������޸�AddressSize
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

	PrefixREX ǰ׺REX;			//REXǰ׺:0x41~0x4F
	PrefixSEG ǰ׺SEG;			//��ǰ׺
	BYTE ǰ׺OperandSize = 0;	//OperandSize Overrideǰ׺: 0x66
	BYTE ǰ׺AddressSize = 0;	//AddressSiz Overrideǰ׺: 0x67
	BYTE ǰ׺LOCK = 0;				//LOCKǰ׺
	BYTE ǰ׺REPE = 0;				//REPEǰ׺
	BYTE ǰ׺REPNE = 0;				//REPNEǰ׺



	MODRM ModRM;

	//************************************
	// Method:     GetPrefixNum 
	// Description:�õ�ǰ׺�ĸ���
	// Returns:    int - 
	//************************************
	int GetPrefixNum()
	{
		int num = 0;
		if (this->ǰ׺REX.IsExist)num++;
		if (this->ǰ׺SEG.IsExist)num++;
		if (this->ǰ׺OperandSize != 0)num++;
		if (this->ǰ׺AddressSize != 0)num++;
		return num;
	}


	//************************************
	// Method:     Generate�������� 
	// Description:��ָ��ƴ��ΪString��ʽ
	// Returns:    void - 
	//************************************
	void Generate��������()
	{
		//�����1��������Ϊ�գ���˵����operand��ֻ���ӡopcode
		if (operand01.��������ȵĲ������ַ��� == NULL
			|| operand01.��������ȵĲ������ַ���.Equals(""))
		{
			this->�������� = String::Format("%-10s", this->opcode.GetPChar());
		}
		//�����2��������Ϊ�գ���˵����1��operand��ֻ���ӡopcode�͵�1��operand
		else if (operand02.��������ȵĲ������ַ��� == NULL
			|| operand02.��������ȵĲ������ַ���.Equals(""))
		{
			this->�������� = String::Format("%-10s %s", this->opcode.GetPChar(), this->operand01.ToString(this->ǰ׺SEG));
		}
		//�����3��������Ϊ�գ���˵����2��operand��ֻ���ӡopcode�͵�1��2��operand
		else if (operand03.��������ȵĲ������ַ��� == NULL
			|| operand03.��������ȵĲ������ַ���.Equals(""))
		{
			this->�������� = String::Format("%-10s %s , %s", this->opcode.GetPChar(), this->operand01.ToString(this->ǰ׺SEG), this->operand02.ToString(this->ǰ׺SEG));
		}
		//�����˵������3��operand��Ҫ��ӡopcode�͵�1��2��3��operand
		else
		{
			this->�������� = String::Format("%-10s %s , %s , %s", this->opcode.GetPChar(), this->operand01.ToString(this->ǰ׺SEG), this->operand02.ToString(this->ǰ׺SEG), this->operand03.ToString(this->ǰ׺SEG));
		}
	}

	String GetDisassemblerCode() { return this->��������; }

	//************************************
	// Method:     GetHardCode 
	// Description:
	// Returns:    String - 
	//************************************
	String GetHardCode()
	{

		String res;

		//��ӡ������
		for (int i = 0; i < this->insLength; i++)
		{
			res = res + String::Format(" %02X", *(this->insAddr + i));
			//��ÿ��ǰ׺����ӡһ��ð��
			if ((this->ǰ׺SEG.IsExist
				|| this->ǰ׺REX.IsExist
				|| this->ǰ׺AddressSize
				|| this->ǰ׺OperandSize
				|| this->ǰ׺LOCK
				|| this->ǰ׺REPNE
				|| this->ǰ׺REPE)
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
	// Method:     ���������ؼ����� 
	// Description: ����operand size ��Addresss size���ԣ�����ǰ׺��
	//				���AddressSize��OperandSize��������ֵ���������d64��f64��ǰ׺�ĸ�ֵ
	// Returns:    void - 
	//************************************
	void ����OperandSize��AddresssSize()
	{
		//��ʼ�������ؼ�����
		if (this->OperandSize == 0) { this->OperandSize = 32; }					//����64λ����32λ����oprand Size��ʼ����32				
		if (this->AddressSize == 0) { this->AddressSize = sizeof(void*) * 8; }	//��64λ������addressSize=64����32λ������addressSize=32			

		//66ǰ׺:operand size��Ϊԭ����1/2������Ϊ16 bit��
		if (this->ǰ׺OperandSize == 0x66 && ����ǰ׺�޸�OperandSize)
		{
			this->OperandSize = this->OperandSize / 2;	//16
		}

		//67ǰ׺:Address Size��Ϊԭ����1/2
		if (this->ǰ׺AddressSize == 0x67 && ����ǰ׺�޸�AddressSize)
		{
			this->AddressSize = this->AddressSize / 2;
		}

		//REXǰ׺����REX.W==1ʱ��64 Bit Operand Size
		if (this->ǰ׺REX.W == 1 && ����ǰ׺�޸�OperandSize)
		{
			this->OperandSize = 64;
		}
		//REXǰ׺����REX.W==0ʱ��Operand size determined by CS.D
		else if (this->ǰ׺REX.W == 0 && ����ǰ׺�޸�OperandSize)
		{

		}
		return;

	}

};

