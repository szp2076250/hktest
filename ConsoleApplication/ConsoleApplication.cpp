// ConsoleApplication.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include "../HookDLL/KeyBoardHook.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/WindowHookDLL.lib")
#else
#pragma comment(lib,"../Debug/WindowHookDLL.lib")
#endif

int main()
{
	MSG msg;
	Init();
	getchar();
	//while (1)
	//{
	//	if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessageW(&msg);
	//	}
	//	else
	//		Sleep(0);    //避免CPU全负载运行
	//}
}

