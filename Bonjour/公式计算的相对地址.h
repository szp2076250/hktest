#pragma once

#include <windows.h>

//�����㹫ʽ��Ŀ���ַ = ��һ��ָ��ĵ�ַ + �з�����ƫ��
class ��ʽ�������Ե�ַ
{
private:
	PBYTE Ŀ���ַ = 0;
	PBYTE ԭָ���ַ = 0;
	int ԭָ��� = 0;
	PBYTE ��һ��ָ���ַ = 0;

	/*
		���磺Ӳ���� 39 1D 0A 59 03 00
			��opcode = 39 1D
			��ƫ��ֵ = 0x0003590A
			��ƫ�Ƶ�Ӳ���볤�� = 4 �ֽ�
	*/
	INT64 ƫ��ֵ = 0;	//���з�����
	PBYTE ָ��ƫ�Ƶ�ָ�� = 0;
	int ƫ�Ƶ�Ӳ�����ֽ��� = 0;

public:

	BOOL IsExist = false;

	void GenerateĿ���ַ(PBYTE ԭָ���ַ, int ԭָ���, PBYTE ָ��ƫ�Ƶ�ָ��, int ƫ�Ƶ�Ӳ�����ֽ���)
	{
		/*
			����Ϊֻ�С�Jx����������'Ex��Mx'(x64�µ�Mod==0&&RM==101)������»��漰��Ե�ַ�ļ���
			����һ����������ֻ�����һ����Ե�ַ
			2019.09.14
		*/
		if (IsExist) { MessageBox(0, TEXT("������һ�����������Ѵ���һ�������ת��ַ"), 0, 0); return; }


		this->ԭָ���ַ = ԭָ���ַ;
		this->ԭָ��� = ԭָ���;
		this->ָ��ƫ�Ƶ�ָ�� = ָ��ƫ�Ƶ�ָ��;
		this->ƫ�Ƶ�Ӳ�����ֽ��� = ƫ�Ƶ�Ӳ�����ֽ���;
		switch (ƫ�Ƶ�Ӳ�����ֽ���)
		{
			case 1:this->ƫ��ֵ = *(INT8*)this->ָ��ƫ�Ƶ�ָ��;  break;
			case 2:this->ƫ��ֵ = *(INT16*)this->ָ��ƫ�Ƶ�ָ��; break;
			case 4:this->ƫ��ֵ = *(INT32*)this->ָ��ƫ�Ƶ�ָ��; break;
			case 8:this->ƫ��ֵ = *(INT64*)this->ָ��ƫ�Ƶ�ָ��; break;
			default:MessageBox(0, TEXT("�����Ӳ���볤�������봫���ֽ���"), 0, 0); break;
		}

		//�����㹫ʽ��Ŀ���ַ = ��һ��ָ��ĵ�ַ + �з�����ƫ��
		this->��һ��ָ���ַ = this->ԭָ���ַ + this->ԭָ���;
		this->Ŀ���ַ = this->��һ��ָ���ַ + this->ƫ��ֵ;

		IsExist = TRUE;
		return;
	}

	PBYTE GetĿ���ַ() { return this->Ŀ���ַ; }
	INT64 Getƫ��ֵ() { return this->ƫ��ֵ; }
	PBYTE Getָ��ƫ�Ƶ�ָ��() { return this->ָ��ƫ�Ƶ�ָ��; }
	int Getƫ�Ƶ�Ӳ�����ֽ���() { return this->ƫ�Ƶ�Ӳ�����ֽ���; }

};