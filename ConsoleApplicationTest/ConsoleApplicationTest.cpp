// ConsoleApplicationTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "../Module/tool.h"
#pragma comment(lib,"../x64/Debug/Module.lib")

int main()
{
	MyNtQuerySystemInformation([](SYSTEM_PROCESS_INFORMATION* info) {
		printf("线程数:%d", info->NumberOfThreads);
	});
	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
	//auto ret = CreateFileW(L"E://1.txt",
	//	GENERIC_READ,
	//	FILE_SHARE_READ,
	//	NULL,
	//	OPEN_EXISTING,        //打开已存在的文件 
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);
	//if (ret == INVALID_HANDLE_VALUE) {
	//	printf("open file error!\n");
	//	CloseHandle(ret);
	//}

	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
	//MessageBoxA(NULL, "123", "Title", MB_OK);
	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
}



