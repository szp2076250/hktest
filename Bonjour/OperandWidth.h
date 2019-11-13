#pragma once
#include "String.h"
#include <windows.h>

/*
*	�ڴ�������Ŀ�ȣ� �������ŵĲ��������д����ԣ����� MOV ecx,ptr byte [eax] ��
*/
class OperandWidth {
private:
	int widthValue = 0;				//���ֵ:8��16��32��64.
	String widthStr="����ֵ...";		//����ַ���: word ptr = 16 , dword ptr = 32 ,  qword ptr = 64 .

public:

	//************************************
	// Method:     SetStr 
	// Description:ǿ�����ÿ���ַ���������Boundָ����Ҫ��������Ϊ������dword * 2 �� word * 2��
	// Parameter:  String widthStr - 
	// Returns:    void - 
	//************************************
	void SetStr(String widthStr)
	{
		this->widthStr = widthStr;
	}


	void SetValue(int widthValue)
	{
		this->widthValue = widthValue;
	}

	String ToString()
	{
		//�������ַ�����δǿ�����ã������value����string
		if (this->widthStr.Equals("����ֵ...")) 
		{
			switch (this->widthValue)
			{
				case 0:widthStr = "";				break; //���ڴ������
				case 8:widthStr = "byte ptr ";		break;
				case 16:widthStr = "word ptr ";		break;
				case 32:widthStr = "dword ptr ";	break;
				case 64:widthStr = "qword ptr ";	break;
				default:MessageBox(0, TEXT("��error���ڴ�������Ŀ������"), 0, 0); break;
			}
		}
		//�������ַ����ѱ�ǿ�����ã���ֱ�ӷ����ַ��� (��Boundָ��)
		return this->widthStr;
	}
};