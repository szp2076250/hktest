// ConsoleApplication.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
//#include <Windows.h>
//
//#include "wchar.h"
//#include "stdlib.h"
//#include "stdio.h"
//#include "dbghelp.h"
//#include "tlhelp32.h"
//#pragma comment(lib,"Dbghelp.lib")
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
	while (1)
	{
		if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
			Sleep(0);    //避免CPU全负载运行
	}

	//DWORD                   dwPID = 0;
	//HANDLE                  hSnapShot = INVALID_HANDLE_VALUE;
	//PROCESSENTRY32          pe;


	//// Get the snapshot of the system
	//pe.dwSize = sizeof(PROCESSENTRY32);
	//hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	//// find process
	//Process32First(hSnapShot, &pe);
	//do
	//{
	//	dwPID = pe.th32ProcessID;

	//	//
	//	if (dwPID < 100)
	//		continue;

	//	auto exePath = pe.szExeFile;
	//	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, dwPID);
	//	auto pfnAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryA");
	//	const char * path = "E:/project/Debug/InjectDLLResume.dll";
	//	auto baseAddress = VirtualAllocEx(hProcess, 0, strlen(path) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//	WriteProcessMemory(hProcess, baseAddress, path, strlen(path) + 1, NULL);
	//	auto hRemote = CreateRemoteThread(hProcess, NULL, 0, pfnAddress, baseAddress, 0, NULL);
	//	VirtualFreeEx(hProcess, baseAddress, strlen(path) + 1, MEM_RELEASE);

	//	CloseHandle(hRemote);
	//	CloseHandle(hProcess);
	//} while (Process32Next(hSnapShot, &pe));


}

