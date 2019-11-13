#pragma once

#include <windows.h>
#include "String.h"


class Println
{
public:

	//************************************
	// Method:     INFO 
	// Description:打印白色字体
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void INFO(String msg)
	{
		//字体白色 (红+绿+蓝=白)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		printf(("【 INFO  】" + msg + "\n").GetPChar());
	}	

	//************************************
	// Method:     Warn 
	// Description:打印蓝色字体
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void Warn(String msg)
	{
		//字体蓝色 
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
		printf(("【 Warn  】" + msg + "\n").GetPChar());
		//字体恢复白色 (红+绿+蓝=白)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}


	//************************************
	// Method:     SUCCESS 
	// Description:绿色字体
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void SUCCESS(String msg)
	{
		//字体绿色
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(("【SUCCESS】" + msg + "\n").GetPChar());

		//字体恢复白色 (红+绿+蓝=白)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	//************************************
	// Method:     Error 
	// Description:红色
	// Parameter:  String msg - 
	// Returns:    void - 
	//************************************
	static void Error(String msg)
	{
		//字体红色
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(("【 Error 】" + msg + "\n").GetPChar());

		//字体恢复白色 (红+绿+蓝=白)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

};