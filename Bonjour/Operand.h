#pragma once
#include "String.h"
#include "OperandWidth.h"
#include "Segment.h"
#include "PrefixSEG.h"

#define ����������_�������ŵĲ����� "����������_�������ŵĲ�����"		// �ڴ�Ѱַ�Ĳ����������Ǵ������ŵĲ����������磺MOV ecx , ptr dword [ecx]
#define ����������_���������ŵĲ����� "����������_���������ŵĲ�����"		//

/*
* ������
*/
class Operand {
public:
	String ��������ȵĲ������ַ���;
	String ����������;					//����������: �������ŵĲ����� = 0 ; ���������ŵĲ����� = 1 .
	OperandWidth �������ŵĲ������Ŀ��;	//�������ŵĲ������Ŀ�ȣ�word ptr =16 , dword ptr = 32 , qword ptr = 64.
	Segment ��;

	String ToString(PrefixSEG SEG)
	{
		if (����������.Equals(����������_�������ŵĲ�����))
		{
			return �������ŵĲ������Ŀ��.ToString() + ��.GetString(SEG) + ��������ȵĲ������ַ���;
		}
		else if (����������.Equals(����������_���������ŵĲ�����))
		{
			return ��������ȵĲ������ַ���;
		}

		//return �������ŵĲ������Ŀ��.ToString() + ��.GetString(SEG) + ��������ȵĲ������ַ���;
	}
};
