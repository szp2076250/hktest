#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include "String.h"
#include "SIB.h"
#include "OperandWidth.h"
#include "Operand.h"
#include "SignedINT.h"
#include "��ʽ�������Ե�ַ.h"
class MODRM
{

	/*
	*	�ַ����궨�壬��������������ʽ�滻��Ϊ��ȷ�ļĴ�����,���� "�Ĵ���_0��" -> "RAX"
	*/
#define �Ĵ���_0��	"�Ĵ���_0��"
#define �Ĵ���_1��	"�Ĵ���_1��"
#define �Ĵ���_2��	"�Ĵ���_2��"
#define �Ĵ���_3��	"�Ĵ���_3��"
#define �Ĵ���_4��	"�Ĵ���_4��"
#define �Ĵ���_5��	"�Ĵ���_5��"
#define �Ĵ���_6��	"�Ĵ���_6��"
#define �Ĵ���_7��	"�Ĵ���_7��"
#define �Ĵ���_8��	"�Ĵ���_8��"
#define �Ĵ���_9��	"�Ĵ���_9��"
#define �Ĵ���_10��	"�Ĵ���_10��"
#define �Ĵ���_11��	"�Ĵ���_11��"
#define �Ĵ���_12��	"�Ĵ���_12��"
#define �Ĵ���_13��	"�Ĵ���_13��"
#define �Ĵ���_14��	"�Ĵ���_14��"
#define �Ĵ���_15��	"�Ĵ���_15��"

private:

	/*
	*	Ex�����Ǵ������ŵĲ�������Ҳ�����ǲ��������ŵĲ�����
	*	Mxһ����������
	*	Rxһ������������
	*	Gxһ������������
	*/
	String E_����RM;			//����MODRM.RM�ֶΣ��鵽��memory��register�ַ�����E=M+R
	String M_����RM;			//����MODRM.RM�ֶΣ��鵽��memory�ַ���(MODRM.Mod!=11)
	String R_����RM;			//����MODRM.RM�ֶΣ��鵽��register�ַ���(MODRM.Mod==11)
	String G_����RegOpcode;	//����MODRM.RegOpcode�ֶΣ��鵽��register�ַ���
	OperandWidth �ڴ���������;		//�������ŵĲ������Ŀ�ȣ�����M���Ŀ��



	//************************************
	// Method:     AnalyzeMemWidth 
	// Description:���ݴ���ġ�z����Operand Size���ԣ������������ŵĲ������Ŀ�ȣ���������M���Ĳ�������ȣ�
	// Parameter:  int operandSize - 
	// Parameter:  String opcodeStr - 
	// Parameter:  String Zz_z - 
	// Returns:    void - 
	//************************************
	void AnalyzeMemWidth(int operandSize, String opcodeStr, String Zz_z)
	{
		//���'Zz'�е�'z'=='v'�������Operand Size���Ծ����ɱ���'v'�ľ�����
		if (Zz_z.Equals("v"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���8
				case 16:Zz_z = "w"; break;	//ͨ��66ǰ׺���Ա�Ϊ16λ
				case 32:Zz_z = "d"; break;	//OperandSizeĬ����32λ
				case 64:Zz_z = "q"; break;	//ͨ��REXǰ׺���Ա�Ϊ64λ
				default:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;
			}
		}

		//���'Zz'�е�'z'=='a'�������Operand Size���Ծ������ֵ��������ַ�����Ҫ��ʾΪ������ʽ
		//���ֵ=16ʱ������ַ���="word * 2 ptr "
		//���ֵ=32ʱ������ַ���="dword * 2 ptr "
		if (Zz_z.Equals("a"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���8
				case 16:this->�ڴ���������.SetValue(16); this->�ڴ���������.SetStr("word * 2 ptr "); break;
				case 32:this->�ڴ���������.SetValue(16); this->�ڴ���������.SetStr("dword * 2 ptr ");  break;
				case 64:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���64
				default:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;
			}
		}

		//���'Zz'�е�'z'=='p'
		//p: 32-bit, 48-bit, or 80-bit pointer, depending on operand-size attribute.
		else if (Zz_z.Equals("p"))
		{
			//��Operand Size = 16 ʱ����p����ʾ4�ֽ� dword  (32 bit)
			//��Operand Size = 32 ʱ����p����ʾ6�ֽ� fword  (48 bit) 
			//��Operand Size = 64 ʱ����p����ʾ10�ֽ� tword  (80 bit) 
			if (operandSize == 16)
			{
				this->�ڴ���������.SetValue(32);
				this->�ڴ���������.SetStr("dword ptr ");
			}
			else if (operandSize == 32)
			{
				this->�ڴ���������.SetValue(48);
				this->�ڴ���������.SetStr("fword ptr ");
			}
			else if (operandSize == 64)
			{
				this->�ڴ���������.SetValue(80);
				this->�ڴ���������.SetStr("tword ptr ");
			}


		}
		//���'Zz'�е�'z'=='s'
		//s: 6-byte or 10-byte pseudo-descriptor.
		else if (Zz_z.Equals("s"))
		{
			//Ŀǰ��֪��32λ�����������漰��Ms����ָ���fword(6�ֽڣ�48 bit)
			//Ŀǰ��֪��64λ�����������漰��Ms����ָ���tword(10�ֽڣ�80 bit)
			if (sizeof(void*) == 4) {
				this->�ڴ���������.SetValue(48);
				this->�ڴ���������.SetStr("fword ptr ");
			}
			else if (sizeof(void*) == 8) {
				this->�ڴ���������.SetValue(80);
				this->�ڴ���������.SetStr("tword ptr ");
			}
		}
		//���opcode�ǡ�MOVSXD��������չ����ָ��������Operand Size���Ծ������ֵ��������ַ�����Ҫ��ʾΪ����֮һ��ʽ
		//���ֵ=64ʱ������ַ���="qword / 2 ptr "
		if (opcodeStr.Equals("MOVSXD"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���8  ??
				case 16:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���16 ??
				case 32:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���32 ??
				case 64:this->�ڴ���������.SetValue(64); this->�ڴ���������.SetStr("qword / 2 ptr ");  break;
				default:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;
			}
		}


		//���'Zz'�е�'z'=='b'����ǿ�������ڴ���������ֵ=8
		if (Zz_z.Equals("b"))
		{
			this->�ڴ���������.SetValue(8);
		}
		//���'Zz'�е�'z'=='w'����ǿ�������ڴ���������ֵ=16
		else if (Zz_z.Equals("w"))
		{
			this->�ڴ���������.SetValue(16);
		}
		//���'Zz'�е�'z'=='d'����ǿ�������ڴ���������ֵ=32
		else if (Zz_z.Equals("d"))
		{
			this->�ڴ���������.SetValue(32);
		}
		//���'Zz'�е�'z'=='q'����ǿ�������ڴ���������ֵ=64
		else if (Zz_z.Equals("q"))
		{
			this->�ڴ���������.SetValue(64);
		}
	}
	//************************************
	// Method:     Replace_M 
	// Description:����M���еļĴ�������滻Ϊ����ļĴ�������
	// Parameter:  int addressSize - 
	// Parameter:  OUT String & dest - 
	// Returns:    void - 
	//************************************
	void Replace_M(int addressSize, OUT String& dest)
	{
		//��AddressSize==16ʱ���������ŵļĴ�����16λ�Ĵ���
		if (addressSize == 16)
		{
			//16λ�Ĵ�����Win32������ƺ���֧��
		}
		//��AddressSize==32ʱ���������ŵļĴ�����32λ�Ĵ���
		else if (addressSize == 32)
		{
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_0��), "EAX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_1��), "ECX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_2��), "EDX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_3��), "EBX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_4��), "ESP").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_5��), "EBP").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_6��), "ESI").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_7��), "EDI").data();

		}
		//��AddressSize==64ʱ���������ŵļĴ�����64λ�Ĵ���
		else if (addressSize == 64)
		{
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_0��), "RAX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_1��), "RCX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_2��), "RDX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_3��), "RBX").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_4��), "RSP").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_5��), "RBP").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_6��), "RSI").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_7��), "RDI").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_8��), "R8").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_9��), "R9").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_10��), "R10").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_11��), "R11").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_12��), "R12").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_13��), "R13").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_14��), "R14").data();
			this->M_����RM = std::regex_replace(this->M_����RM.GetPChar(), std::regex(�Ĵ���_15��), "R15").data();
		}
	}
	//************************************
	// Method:     Replace_R_G 
	// Description:����R��G���еļĴ�������滻Ϊ����ļĴ�������
	// Parameter:  int operandSize - 
	// Parameter:  String Zz_z - 
	// Parameter:  OUT String & dest - 
	// Returns:    void - 
	//************************************
	void Replace_R_G(int operandSize, String Zz_z, OUT String& dest)
	{
		//���'Zz'�е�'z'=='v'�������Operand Size���Ծ����ɱ���'v'�ľ�����
		if (Zz_z.Equals("v"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���8
				case 16:Zz_z = "w"; break;	//ͨ��66ǰ׺���Ա�Ϊ16λ
				case 32:Zz_z = "d"; break;	//OperandSizeĬ����32λ
				case 64:Zz_z = "q"; break;	//ͨ��REXǰ׺���Ա�Ϊ64λ
				default:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;
			}
		}

		//���'Zz'�е�'z'=='z'�������Operand Size���Ծ����ɱ���'z'�ľ�����
		//z:Word for 16-bit operand-size or doubleword for 32 or 64-bit operand-size.
		//��operandSize=16ʱ����16λ�Ĵ���.
		//��operandSize=32��64ʱ����32λ�Ĵ���.
		if (Zz_z.Equals("z"))
		{
			switch (operandSize)
			{
				case 8: MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;	//OperandSize �����ܵ���8
				case 16:Zz_z = "w"; break;	//ͨ��66ǰ׺���Ա�Ϊ16λ
				case 32:Zz_z = "d"; break;	//OperandSizeĬ����32λ
				case 64:Zz_z = "d"; break;	//ͨ��REXǰ׺���Ա�Ϊ64λ
				default:MessageBox(0, TEXT("Operand Size �����д���"), 0, 0); break;
			}
		}

		/*
		*	����WidthString����G_����RegOpcode�еļĴ�������滻Ϊ��Ӧ�ļĴ�������
		*/

		//���'Zz'�е�'z'=='b'����ʾ8λ�Ĵ���
		if (Zz_z.Equals("b"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_0��), "AL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_1��), "CL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_2��), "DL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_3��), "BL").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_4��), "AH").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_5��), "CH").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_6��), "DH").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_7��), "BH").data();
		}
		//���'Zz'�е�'z'=='w'����ʾ16λ�Ĵ���
		else if (Zz_z.Equals("w"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_0��), "AX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_1��), "CX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_2��), "DX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_3��), "BX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_4��), "SP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_5��), "BP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_6��), "SI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_7��), "DI").data();

			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_8��), "R8W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_9��), "R9W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_10��), "R10W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_11��), "R11W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_12��), "R12W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_13��), "R13W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_14��), "R14W").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_15��), "R15W").data();
		}
		//���'Zz'�е�'z'=='d'����ʾ32λ�Ĵ���
		else if (Zz_z.Equals("d"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_0��), "EAX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_1��), "ECX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_2��), "EDX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_3��), "EBX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_4��), "ESP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_5��), "EBP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_6��), "ESI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_7��), "EDI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_8��), "R8D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_9��), "R9D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_10��), "R10D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_11��), "R11D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_12��), "R12D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_13��), "R13D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_14��), "R14D").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_15��), "R15D").data();
		}
		//���'Zz'�е�'z'=='q'����ʾ64λ�Ĵ���
		else if (Zz_z.Equals("q"))
		{
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_0��), "RAX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_1��), "RCX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_2��), "RDX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_3��), "RBX").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_4��), "RSP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_5��), "RBP").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_6��), "RSI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_7��), "RDI").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_8��), "R8").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_9��), "R9").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_10��), "R10").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_11��), "R11").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_12��), "R12").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_13��), "R13").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_14��), "R14").data();
			dest = std::regex_replace(dest.GetPChar(), std::regex(�Ĵ���_15��), "R15").data();
		}
	}

public:

	BYTE Mod = 0;			//MODRM�ֶε�Mod��
	BYTE RegOpcode = 0;		//MODRM�ֶε�RegOpcode��
	BYTE RM = 0;			//MODRM�ֶε�RM��

	SIB SIB;				//SIB�ֶ�

	bool IsExist = false;	//�Ƿ����MODRM�ֶ�



	//************************************
	// Method:     Map_M_R_G 
	// Description:����ָ��ļĴ�����ź�ƫ�ƣ�ӳ��Ϊ��M����R����G��
	// Parameter:  PBYTE pModRM - 
	// Parameter:  OUT int & insLen - 
	// Parameter:  PBYTE insAddr - ָ�����ʼ��ַ
	// Returns:    void - 
	//************************************
	void Map_M_R_G(PBYTE pModRM, OUT int& insLen, PBYTE insAddr, OUT ��ʽ�������Ե�ַ& ��ʽ�������Ե�ַ)
	{
		//��ȡMODRM�ֶΣ�ָ���+1
		insLen++;

		//1.�ж��Ƿ����SIB�ֶ�
		char sibStr[40] = { 0 };
		if (this->SIB.IsExist)
		{
			//��ȡSIB�ֶΣ����ȼ�1
			insLen++;
			PBYTE pSIB = pModRM + 0x1;

			//ȡƫ��disp���޷�������
			UINT32 disp32 = *(UINT32*)(pSIB + 0x1);
			UINT8 disp8 = *(UINT8*)(pSIB + 0x1);
			//ȡƫ��disp���з�������ƫ�ƶ����з�������
			char symbol8 = disp8 & 0x80 ? '-' : '+';			//�õ��з������������Ų��֡�
			char symbol32 = disp32 & 0x80000000 ? '-' : '+';	//�õ��з������������Ų��֡�
			INT32 disp32_signed = (symbol32 == '-') ? ~(disp32 - 1) & 0x7FFFFFFF : disp32;	//�õ��з����������ֲ��֣�(����)�����һȡ����ȥ������λ��
			INT8 disp8_signed = (symbol8 == '-') ? ~(disp8 - 1) & 0x7F : disp8;				//�õ��з����������ֲ��֣�(����)�����һȡ����ȥ������λ��

			//����Baseֵ�õ���һ���ַ���
			String baseStr = { 0 };
			switch (this->SIB.Base)
			{
				case 0:baseStr = �Ĵ���_0��; break;
				case 1:baseStr = �Ĵ���_1��; break;
				case 2:baseStr = �Ĵ���_2��; break;
				case 3:baseStr = �Ĵ���_3��; break;
				case 4:baseStr = �Ĵ���_4��; break;
					//case 5:baseStr = �Ĵ���_5��; break; //��Base=101ʱ:[*]
				case 6:baseStr = �Ĵ���_6��; break;
				case 7:baseStr = �Ĵ���_7��; break;
				case 8:baseStr = �Ĵ���_8��; break;
				case 9:baseStr = �Ĵ���_9��; break;
				case 10:baseStr = �Ĵ���_10��; break;
				case 11:baseStr = �Ĵ���_11��; break;
				case 12:baseStr = �Ĵ���_12��; break;
				case 13:baseStr = �Ĵ���_13��; break;
				case 14:baseStr = �Ĵ���_14��; break;
				case 15:baseStr = �Ĵ���_15��; break;
			}

			//����Index��Scale�õ���һ���ַ���
			String ScaleIndexStr = { 0 };
			String scaleStr = { 0 };
			scaleStr = String::Format("%d", (int)pow(2, this->SIB.Scale));
			switch (this->SIB.Index)
			{
				case 0:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_0��, scaleStr); break;
				case 1:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_1��, scaleStr); break;
				case 2:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_2��, scaleStr); break;
				case 3:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_3��, scaleStr); break;
				case 4:ScaleIndexStr = ""; break;//��Index=100ʱ����none
				case 5:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_5��, scaleStr); break;
				case 6:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_6��, scaleStr); break;
				case 7:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_7��, scaleStr); break;
				case 8:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_8��, scaleStr); break;
				case 9:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_9��, scaleStr); break;
				case 10:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_10��, scaleStr); break;
				case 11:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_11��, scaleStr); break;
				case 12:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_12��, scaleStr); break;
				case 13:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_13��, scaleStr); break;
				case 14:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_14��, scaleStr); break;
				case 15:ScaleIndexStr = String::Format("%s * %s", �Ĵ���_15��, scaleStr); break;
			}


			//ƴ��SIB�ַ������� base==101 ʱ
			if (this->SIB.Base == 0x5)
			{
				switch (this->Mod)
				{
					case 0x0: sprintf(sibStr, "%s %c %08X", ScaleIndexStr, symbol32, disp32_signed); insLen += 4; break;			//���ȡ4�ֽ�ƫ��
					case 0x1:sprintf(sibStr, "%s %c %02X + ebp", ScaleIndexStr, symbol8, disp8_signed); insLen += 1; break;		//���ȡ1�ֽ�ƫ��
					case 0x2:sprintf(sibStr, "%s %c %08X + ebp", ScaleIndexStr, symbol32, disp32_signed); insLen += 4; break;	//���ȡ4�ֽ�ƫ��
				}
			}
			//ƴ��SIB�ַ������� base!=101 ʱ
			else
			{
				switch (this->Mod)
				{
					case 0x0:sprintf(sibStr, "%s + %s", baseStr, ScaleIndexStr);  break;
					case 0x1:sprintf(sibStr, "%s + %s %c %02X", baseStr, ScaleIndexStr, symbol8, disp8_signed); insLen += 1; break;	//���ȡ1�ֽ�ƫ��
					case 0x2:sprintf(sibStr, "%s + %s %c %08X", baseStr, ScaleIndexStr, symbol32, disp32_signed); insLen += 4; break;//���ȡ4�ֽ�ƫ��
				}
			}
		}

		//2.����SIB����ֻ�����ModRM
		//2.1 ����Mod��RM������M���͡�R��
		if (this->Mod == 0x0)
		{

			switch (this->RM)
			{
				case 0:	this->M_����RM = String::Format("[%s]", �Ĵ���_0��); break;
				case 1:	this->M_����RM = String::Format("[%s]", �Ĵ���_1��); break;
				case 2:	this->M_����RM = String::Format("[%s]", �Ĵ���_2��); break;
				case 3:	this->M_����RM = String::Format("[%s]", �Ĵ���_3��); break;
				case 4:	this->M_����RM = String::Format("[%s]", sibStr); break;//����SIB�ֶ�
				case 5:
				{
					//����ȡ4�ֽ�
					UINT32 uInt32 = *(UINT32*)(pModRM + 1);
					insLen += 4;

					//��x64�£�MODRM.Mod==00B && MODRM.RM==101Bʱ: ��������ĵ�ֵַ = ��һ��ָ��ĵ�ַ + disp32�����з�������
					if (sizeof(void*) == 8)
					{

						��ʽ�������Ե�ַ.GenerateĿ���ַ(insAddr, insLen, pModRM + 1, 4);
						this->M_����RM = String::Format("[%016llX]", ��ʽ�������Ե�ַ.GetĿ���ַ());
					}
					//��x86�£�MODRM.Mod==00B && MODRM.RM==101Bʱ: ��������ĵ�ֵַ =  һ��32λƫ������OD�Ͽ���disp32����ʾΪ�޷�����������˵���з������Ŷ԰ɣ���
					else
					{
						this->M_����RM = String::Format("[%08X]", uInt32);
					}
					break;
				}
				case 6:	this->M_����RM = String::Format("[%s]", �Ĵ���_6��); break;
				case 7:	this->M_����RM = String::Format("[%s]", �Ĵ���_7��); break;
				case 8:	this->M_����RM = String::Format("[%s]", �Ĵ���_8��); break;
				case 9:	this->M_����RM = String::Format("[%s]", �Ĵ���_9��); break;
				case 10:this->M_����RM = String::Format("[%s]", �Ĵ���_10��); break;
				case 11:this->M_����RM = String::Format("[%s]", �Ĵ���_11��); break;
				case 12:this->M_����RM = String::Format("[%s]", �Ĵ���_12��); break;
				case 13:this->M_����RM = String::Format("[%s]", �Ĵ���_13��); break;
				case 14:this->M_����RM = String::Format("[%s]", �Ĵ���_14��); break;
				case 15:this->M_����RM = String::Format("[%s]", �Ĵ���_15��); break;
			}
		}
		else if (this->Mod == 0x1)
		{
			//�õ�8λƫ��(�޷���)
			BYTE disp8 = *(BYTE*)(pModRM + 0x1);
			//��ƫ��תΪ16������ʾ���з�����
			char symbol = disp8 & 0x80 ? '-' : '+';		//�õ��з������������Ų��֡�
			INT8 disp8_signed = (symbol == '-') ? ~(disp8 - 1) & 0x7F : disp8;	//�õ��з����������ֲ��֣�(����)�����һȡ����ȥ������λ

			switch (this->RM)
			{
				case 0:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_0��, symbol, disp8_signed);  insLen += 1; break;
				case 1:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_1��, symbol, disp8_signed);  insLen += 1; break;
				case 2:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_2��, symbol, disp8_signed);  insLen += 1; break;
				case 3:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_3��, symbol, disp8_signed);  insLen += 1; break;
				case 4:this->M_����RM = String::Format("[%s]", sibStr);   break;	//����SIB�ֶ�
				case 5:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_5��, symbol, disp8_signed);  insLen += 1; break;
				case 6:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_6��, symbol, disp8_signed);  insLen += 1; break;
				case 7:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_7��, symbol, disp8_signed);  insLen += 1; break;
				case 8:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_8��, symbol, disp8_signed);  insLen += 1; break;
				case 9:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_9��, symbol, disp8_signed);  insLen += 1; break;
				case 10:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_10��, symbol, disp8_signed);  insLen += 1; break;
				case 11:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_11��, symbol, disp8_signed);  insLen += 1; break;
				case 12:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_12��, symbol, disp8_signed);  insLen += 1; break;
				case 13:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_13��, symbol, disp8_signed);  insLen += 1; break;
				case 14:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_14��, symbol, disp8_signed);  insLen += 1; break;
				case 15:this->M_����RM = String::Format("[%s %c %02X]", �Ĵ���_15��, symbol, disp8_signed);  insLen += 1; break;

			}
		}
		else if (this->Mod == 0x2)
		{
			//�õ�32λƫ��(�޷���)
			UINT32 disp32 = *(UINT32*)(pModRM + 0x1);
			//��ƫ��תΪ16������ʾ���з�����
			char symbol = disp32 & 0x80000000 ? '-' : '+';		//�õ��з������������Ų��֡�
			INT32 disp32_signed = (symbol == '-') ? ~(disp32 - 1) & 0x7FFFFFFF : disp32;//�õ��з����������ֲ��֣�(����)�����һȡ����ȥ������λ

			switch (this->RM)
			{

				case 0:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_0��, symbol, disp32_signed); insLen += 4; break;
				case 1:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_1��, symbol, disp32_signed); insLen += 4; break;
				case 2:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_2��, symbol, disp32_signed); insLen += 4; break;
				case 3:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_3��, symbol, disp32_signed); insLen += 4; break;
				case 4:	this->M_����RM = String::Format("[%s]", sibStr); break;	//����SIB�ֶ�
				case 5:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_5��, symbol, disp32_signed); insLen += 4; break;
				case 6:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_6��, symbol, disp32_signed); insLen += 4; break;
				case 7:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_7��, symbol, disp32_signed); insLen += 4; break;
				case 8:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_8��, symbol, disp32_signed); insLen += 4; break;
				case 9:	this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_9��, symbol, disp32_signed); insLen += 4; break;
				case 10:this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_10��, symbol, disp32_signed); insLen += 4; break;
				case 11:this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_11��, symbol, disp32_signed); insLen += 4; break;
				case 12:this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_12��, symbol, disp32_signed); insLen += 4; break;
				case 13:this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_13��, symbol, disp32_signed); insLen += 4; break;
				case 14:this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_14��, symbol, disp32_signed); insLen += 4; break;
				case 15:this->M_����RM = String::Format("[%s %c %08X]", �Ĵ���_15��, symbol, disp32_signed); insLen += 4; break;
			}
		}
		else if (this->Mod == 0x3)
		{
			switch (this->RM)
			{
				//��Ѱַ�Ĵ���
				case 0:	this->R_����RM = �Ĵ���_0��; break;
				case 1:	this->R_����RM = �Ĵ���_1��; break;
				case 2:	this->R_����RM = �Ĵ���_2��; break;
				case 3:	this->R_����RM = �Ĵ���_3��; break;
				case 4:	this->R_����RM = �Ĵ���_4��; break;
				case 5:	this->R_����RM = �Ĵ���_5��; break;
				case 6:	this->R_����RM = �Ĵ���_6��; break;
				case 7:	this->R_����RM = �Ĵ���_7��; break;
				case 8:	this->R_����RM = �Ĵ���_8��; break;
				case 9:	this->R_����RM = �Ĵ���_9��; break;
				case 10:this->R_����RM = �Ĵ���_10��; break;
				case 11:this->R_����RM = �Ĵ���_11��; break;
				case 12:this->R_����RM = �Ĵ���_12��; break;
				case 13:this->R_����RM = �Ĵ���_13��; break;
				case 14:this->R_����RM = �Ĵ���_14��; break;
				case 15:this->R_����RM = �Ĵ���_15��; break;
			}
		}

		//2.2 ����RegOpcode������G��
		switch (this->RegOpcode)
		{
			//��Ѱַ�Ĵ���
			case 0:	this->G_����RegOpcode = �Ĵ���_0��; break;
			case 1:	this->G_����RegOpcode = �Ĵ���_1��; break;
			case 2:	this->G_����RegOpcode = �Ĵ���_2��; break;
			case 3:	this->G_����RegOpcode = �Ĵ���_3��; break;
			case 4:	this->G_����RegOpcode = �Ĵ���_4��; break;
			case 5:	this->G_����RegOpcode = �Ĵ���_5��; break;
			case 6:	this->G_����RegOpcode = �Ĵ���_6��; break;
			case 7:	this->G_����RegOpcode = �Ĵ���_7��; break;
			case 8:	this->G_����RegOpcode = �Ĵ���_8��; break;
			case 9:	this->G_����RegOpcode = �Ĵ���_9��; break;
			case 10:this->G_����RegOpcode = �Ĵ���_10��; break;
			case 11:this->G_����RegOpcode = �Ĵ���_11��; break;
			case 12:this->G_����RegOpcode = �Ĵ���_12��; break;
			case 13:this->G_����RegOpcode = �Ĵ���_13��; break;
			case 14:this->G_����RegOpcode = �Ĵ���_14��; break;
			case 15:this->G_����RegOpcode = �Ĵ���_15��; break;
			default:MessageBox(0, TEXT("MOD.RegOpcode���ֵ����"), 0, 0); this->G_����RegOpcode = "MOD.RegOpcode���ֵ����"; break;
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
		//��M�еļĴ�������滻Ϊ����ļĴ�����
		Replace_M(addressSize, this->M_����RM);
		return this->M_����RM;
	}
	String GetR(int addressSize, int operandSize, String Zz_z, String opcodeStr)
	{
		//��R�еļĴ�������滻Ϊ����ļĴ�����
		Replace_R_G(operandSize, Zz_z, this->R_����RM);
		return this->R_����RM;
	}
	String GetG(int addressSize, int operandSize, String Zz_z, String opcodeStr)
	{
		//��G�еļĴ�������滻Ϊ����ļĴ�����
		Replace_R_G(operandSize, Zz_z, this->G_����RegOpcode);
		return this->G_����RegOpcode;
	}

	//************************************
	// Method:     GetS 
	// Description:ͨ��MOD.reg�����μĴ���
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
			default:return "����!�μĴ���ֻ��6��";
		}
	}

	//************************************
	// Method:     Get�ڴ�������Ŀ��
	// Description:��ȡModRM�漰���ڴ�������Ŀ�ȣ�ֻ�д������ŵĲ���������Ҫ��ȣ�����M���Ŀ��
	// Parameter:  int operandSize - 
	// Parameter:  String Zz_z - 
	// Parameter:  String opcodeStr - 
	// Returns:    OperandWidth - 
	//************************************
	OperandWidth Get�ڴ�������Ŀ��(int operandSize, String Zz_z, String opcodeStr)
	{
		//����M���ڴ���������
		AnalyzeMemWidth(operandSize, opcodeStr, Zz_z);

		return this->�ڴ���������;
	}
	String GetE�Ĳ���������()
	{
		//��MORRM.Mod==00��01��10ʱ�����ǡ�M��memory������������������
		if (this->Mod != 0x3)
		{
			return ����������_�������ŵĲ�����;
		}
		//��MORRM.Mod==11ʱ���ǡ�R��register��������û��������
		else
		{
			return ����������_���������ŵĲ�����;
		}
	}
	String GetM�Ĳ���������()
	{
		return ����������_�������ŵĲ�����;
	}
	String GetR�Ĳ���������()
	{
		//��R�����Ǵ������ŵĲ�����
		return ����������_���������ŵĲ�����;
	}
	String GetG�Ĳ���������()
	{
		//��G������û�������ŵĲ�����
		return ����������_���������ŵĲ�����;
	}
	String GetS�Ĳ���������()
	{
		return ����������_���������ŵĲ�����;
	}
	//************************************
	// Method:     Map_ModRM_SIB
	// Description:����Mod�ֶκ�SIB�ֶεĸ���
	// Parameter:  PBYTE pModRM - 
	// Parameter:  PrefixREX ǰ׺REX - 
	// Returns:    void - 
	//************************************
	void Map_ModRM_SIB(PBYTE pModRM)
	{
		/*
		* ����������ModR/M���ֶ�
		* ��6~7λ��"Mod"
		* ��3~5λ��"Reg/Opcode"
		* ��0~2λ��"r/m"
		*/
		this->IsExist = true;
		BYTE ModRM = *pModRM;
		this->Mod = ModRM >> 0x6;					//����6λ	
		this->RegOpcode = (ModRM >> 0x3) & (0x7);	//����2λ,��λ��0000 0111
		this->RM = ModRM & 0x7;						//��λ��0000 0011

		/*
			* ����������SIB���ֶ�
			* ��6~7λ��"Scale"
			* ��3~5λ��"Index"
			* ��0~2λ��"Base"
			* ��Base+Index*2^Scale��
		*/
		if (this->RM == 0x4 && (this->Mod == 0x0 || this->Mod == 0x1 || this->Mod == 0x2))
		{//��Mod��=00 01 10 , r/m��=100ʱ������SIB�ֶ�

			this->SIB.IsExist = true;
			BYTE SIB_Byte = *(pModRM + 1);
			this->SIB.Scale = SIB_Byte >> 0x6;
			this->SIB.Index = (SIB_Byte >> 0x3) & (0x7);
			this->SIB.Base = SIB_Byte & 0x7;
		}
	}

};

