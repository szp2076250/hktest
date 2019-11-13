#pragma once

#include <windows.h>
#include "String.h"


class Println
{
public:

	//************************************
	// Method:     INFO 
	// Description:��ӡ��ɫ����
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void INFO(String msg)
	{
		//�����ɫ (��+��+��=��)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		printf(("�� INFO  ��" + msg + "\n").GetPChar());
	}	

	//************************************
	// Method:     Warn 
	// Description:��ӡ��ɫ����
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void Warn(String msg)
	{
		//������ɫ 
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
		printf(("�� Warn  ��" + msg + "\n").GetPChar());
		//����ָ���ɫ (��+��+��=��)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}


	//************************************
	// Method:     SUCCESS 
	// Description:��ɫ����
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void SUCCESS(String msg)
	{
		//������ɫ
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(("��SUCCESS��" + msg + "\n").GetPChar());

		//����ָ���ɫ (��+��+��=��)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	//************************************
	// Method:     Error 
	// Description:��ɫ
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void Error(String msg)
	{
		//�����ɫ
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(("�� Error ��" + msg + "\n").GetPChar());

		//����ָ���ɫ (��+��+��=��)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

};