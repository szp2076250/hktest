// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <string>

// 全局变量
HANDLE hProcess = NULL;				// 进程句柄
BOOL bIsInjected = FALSE;				// 是否注入完成
typedef int (WINAPI *MsgBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);			// 声明一个别名 MsgBoxA
typedef int (WINAPI *MsgBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);	// 声明一个别名 MsgBoxW

typedef HANDLE(WINAPI *CFA)(
	LPCSTR			      lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE);

typedef HANDLE(WINAPI *CFW)(
	LPCWSTR				  lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE);

CFA oldCFA;
CFW oldCFW;

MsgBoxA oldMsgBoxA = NULL;			// 保存原函数地址
MsgBoxW oldMsgBoxW = NULL;			// 保存原函数地址
FARPROC pfMsgBoxA = NULL;			// 指向原函数地址的远指针
FARPROC pfMsgBoxW = NULL;			// 指向原函数地址的远指针
FARPROC pfFuncA = NULL;
FARPROC pfFuncW = NULL;
BYTE OldCodeA[5];					// 老的系统API入口代码
BYTE NewCodeA[5];					// 要跳转的API代码 (jmp xxxx)
BYTE OldCodeW[5];					// 老的系统API入口代码
BYTE NewCodeW[5];					// 要跳转的API代码 (jmp xxxx)
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);				// 我们自己的 MessageBoxA 函数
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
void HookOn(HANDLE pProcess);
void HookOff(HANDLE pProcess);

HANDLE WINAPI MyCreateFileA(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	HookOff(hProcess);
	::MessageBoxA(NULL, "尝试CreateFileA", "提示", MB_OK);
	HANDLE hd = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
		dwFlagsAndAttributes, hTemplateFile);
	HookOn(hProcess);
	return hd;
}

HANDLE WINAPI MyCreateFileW(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	HookOff(hProcess);
	::MessageBoxA(NULL, "尝试CreateFileW", "提示", MB_OK);
	HANDLE hd =  CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
		dwFlagsAndAttributes, hTemplateFile);
	HookOn(hProcess);
	return hd;
}

// 假 MessageBoxA
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int nRet = 0;

	//printf("HOOK !");
	// 先恢复 Hook，不然会造成死循环
	HookOff(hProcess);

	// 检验 MessageBoxA 是否失败（失败返回 0）
	nRet = ::MessageBoxA(hWnd, "Hook MessageBoxA", lpCaption, uType);
	nRet = ::MessageBoxA(hWnd, lpText, lpCaption, uType);	// 调用原函数（如果你想暗箱操作的话）

	//再次 HookOn，否则只生效一次
	HookOn(hProcess);

	return nRet;
}

// 假 MessageBoxW
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	int nRet = 0;
	//printf("HOOKW !");
	// 先恢复 Hook，不然会造成死循环
	HookOff(hProcess);

	// 检验 MessageBoxW 是否失败（失败返回 0）
	nRet = ::MessageBoxW(hWnd, L"Hook MessageBoxW", lpCaption, uType);
	//nRet=::MessageBoxW(hWnd,lpText,lpCaption,uType);	// 调用原函数（如果你想暗箱操作的话）

	// 再次 HookOn，否则只生效一次
	HookOn(hProcess);

	return nRet;
}

// 代码注入
void Inject()
{
	 //如果还没有注入
	if (!bIsInjected) {

		//保证只调用1次
		bIsInjected = TRUE;

		//获取函数地址
		HMODULE hmod = ::LoadLibrary(L"User32.dll");
		oldMsgBoxA = (MsgBoxA)::GetProcAddress(hmod, "MessageBoxA");	// 原 MessageBoxA 地址
		pfMsgBoxA = (FARPROC)oldMsgBoxA;								// 指向原 MessageBoxA 地址的指针
		oldMsgBoxW = (MsgBoxW)::GetProcAddress(hmod, "MessageBoxW");	// 原 MessageBoxW 地址
		pfMsgBoxW = (FARPROC)oldMsgBoxW;								// 指向原 MessageBoxW 地址的指针

		//HMODULE hmod = ::LoadLibrary(L"Kernel32.dll");
		//oldCFA = (CFA)::GetProcAddress(hmod, "CreateFileA");
		//oldCFW = (CFW)::GetProcAddress(hmod, "CreateFileW");
		//pfFuncA = (FARPROC)oldCFA;
		//pfFuncW = (FARPROC)oldCFW;

	// 指针为空则结束运行
	if (pfMsgBoxA == NULL) { MessageBox(NULL, L"cannot get MessageBoxA()", L"error", 0); return; }
	if (pfMsgBoxW == NULL) { MessageBox(NULL, L"cannot get MessageBoxW()", L"error", 0); return; }

	//if (oldCFA == NULL) { MessageBox(NULL, L"cannot get CreateFileA()", L"error", 0); return; }
	//if (oldCFW == NULL) { MessageBox(NULL, L"cannot get CreateFileW()", L"error", 0); return; }

	// 将原API中的入口代码保存入 OldCodeA[]，OldCodeW[]
	//_asm
	//{
	//	lea edi, OldCodeA	; 把 OldCodeA 的地址给 edi
	//	mov esi, pfFuncA	; 把 MessageBoxA 的地址给 esi
	//	cld					; 方向标志位复位
	//	movsd				; 复制双子
	//	movsb				; 复制字节
	//}
	//_asm
	//{
	//	lea edi, OldCodeW; 以相同的方式操作 MessageBoxW
	//	mov esi, pfFuncW
	//	cld
	//	movsd
	//	movsb
	//}

	_asm
	{
		lea edi, OldCodeA; 把 OldCodeA 的地址给 edi
		mov esi, pfMsgBoxA; 把 MessageBoxA 的地址给 esi
		cld; 方向标志位复位
		movsd; 复制双子
		movsb; 复制字节
	}
	_asm
	{
		lea edi, OldCodeW; 以相同的方式操作 MessageBoxW
		mov esi, pfMsgBoxW
		cld
		movsd
		movsb
	}

	// 将原 API 第一个字节改为 jmp
	NewCodeA[0] = 0xe9;
	NewCodeW[0] = 0xe9;

	// 计算 jmp 后面要跟的地址
	_asm
	{
		lea eax, MyMessageBoxA; 将 MyMessageBoxA 的地址给 eax
		mov ebx, pfMsgBoxA; 将 MessageBoxA 的地址给 ebx
		sub eax, ebx; 计算 jmp 后面要跟的地址
		sub eax, 5
		mov dword ptr[NewCodeA + 1], eax
	}
	_asm
	{
		lea eax, MyMessageBoxW; 以相同的方式操作 MessageBoxW
		mov ebx, pfMsgBoxW
		sub eax, ebx
		sub eax, 5
		mov dword ptr[NewCodeW + 1], eax
	}

	////
	//_asm
	//{
	//	lea eax, MyCreateFileA; 
	//	mov ebx, pfFuncA; 
	//	sub eax, ebx; 计算 jmp 后面要跟的地址
	//	sub eax, 5
	//	mov dword ptr[NewCodeA + 1], eax
	//}
	//_asm
	//{
	//	lea eax, MyCreateFileW;
	//	mov ebx, pfFuncW
	//	sub eax, ebx
	//	sub eax, 5
	//	mov dword ptr[NewCodeW + 1], eax
	//}

	// 开始 Hook
	HookOn(hProcess);
	}
}

// 开启钩子（修改 API 头 5 个字节）
void HookOn(HANDLE hProcess)
{
	// 检验进程句柄是否为空
	//assert(hProcess != NULL);

	DWORD dwTemp = 0,		// 修改后的内存保护属性
		dwOldProtect,		// 之前的内存保护属性
		dwRet = 0,			// 内存写入成功标志，0不成功、1成功
		dwWrite;			// 写入进程内存的字节数

	// 更改虚拟内存保护
	VirtualProtectEx(
		hProcess,			// 进程句柄
		pfMsgBoxA,			// 指向保护区域地址的指针
		5,					// 要更改的区域的字节大小
		PAGE_READWRITE,		// 内存保护类型，PAGE_READWRITE：可读可写
		&dwOldProtect		// 接收原来的内存保护属性
	);

	// 判断是否成功写入内存
	dwRet = WriteProcessMemory(
		hProcess,			// 进程句柄
		pfMsgBoxA,			// 指向写入地址的指针
		NewCodeA,			// 指向存放写入内容的缓冲区指针
		5,					// 写入字节数
		&dwWrite			// 接收传输到进程中的字节数
	);
	if (0 == dwRet || 0 == dwWrite) {
		//TRACE("NewCodeA 写入失败");	// 记录日志信息
	}
	// 恢复内存保护状态
	VirtualProtectEx(hProcess, pfMsgBoxA, 5, dwOldProtect, &dwTemp);

	// 同上，操作剩下的 MessageBoxW
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, PAGE_READWRITE, &dwOldProtect);
	dwRet = WriteProcessMemory(hProcess, pfMsgBoxW, NewCodeW, 5, &dwWrite);
	if (0 == dwRet || 0 == dwWrite) { /*TRACE("NewCodeW 写入失败");*/ }
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, dwOldProtect, &dwTemp);

	//MessageBoxA(NULL, "s", "TITILE", MB_OK);
}

// 关闭钩子（修改 API 头 5 个字节）
void HookOff(HANDLE hProcess)
{
	// 检验进程句柄是否为空
	//ASSERT(hProcess != NULL);

	DWORD dwTemp = 0,			// 修改后的内存保护属性
		dwOldProtect = 0,		// 之前的内存保护属性
		dwRet = 0,				// 内存写入成功标志，0不成功、1成功
		dwWrite = 0;			// 写入进程内存的字节数

	// 更改虚拟内存保护
	VirtualProtectEx(
		hProcess,				// 进程句柄
		pfMsgBoxA,				// 指向保护区域地址的指针
		5,						// 要更改的区域的字节大小
		PAGE_READWRITE,			// 内存保护类型，PAGE_READWRITE：可读可写
		&dwOldProtect			// 接收原来的内存保护属性
	);

	dwRet = WriteProcessMemory(
		hProcess,				// 进程句柄
		pfMsgBoxA,				// 指向写入地址的指针
		OldCodeA,				// 指向存放写入内容的缓冲区指针
		5,						// 写入字节数
		&dwWrite				// 接收传输到进程中的字节数
	);
	if (0 == dwRet || 0 == dwWrite) {
		//TRACE("OldCodeA 写入失败");	// 记录日志信息
	}

	// 恢复内存保护状态
	VirtualProtectEx(hProcess, pfMsgBoxA, 5, dwOldProtect, &dwTemp);

	// 同上，操作剩下的 
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfMsgBoxW, OldCodeW, 5, &dwWrite);
	if (0 == dwRet || 0 == dwWrite) { /*TRACE("OldCodeW 写入失败");*/ }
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, dwOldProtect, &dwTemp);

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	DWORD dwPid = ::GetCurrentProcessId();
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		// 获取调用 dll 的进程句柄
		//char buf[100];
		//sprintf_s(buf, "load%d", dwPid);
		//MessageBoxA(NULL, buf, "Load..", MB_OK);
		hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
		Inject();
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

