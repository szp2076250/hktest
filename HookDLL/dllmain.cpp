// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <windows.h>

HANDLE hProcess = 0;
//关闭HOOK函数

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved 
)
{ 
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		//准备基本工作
		//MessageBoxA(NULL, "ATTACH", "TITLE", MB_OK);
		hProcess = hModule;
		
	} 
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH: 
		//OffHook();
		break;
	}
	return TRUE;
}

