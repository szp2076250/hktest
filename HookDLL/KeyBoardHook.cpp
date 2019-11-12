#pragma once
#include "stdafx.h"
#include "wchar.h"
#include "stdlib.h"
#include "stdio.h"
#include "KeyBoardHook.h"
#include "dbghelp.h"
#include "tlhelp32.h"
#pragma comment(lib,"Dbghelp.lib")

// 全局共享变量（多进程之间共享数据）
#pragma data_seg(".Share")
HWND g_hWnd = NULL;				// 主窗口句柄
HHOOK hhk = NULL;				// 鼠标钩子句柄
HINSTANCE hInst = NULL;			// 本dll实例句柄
#pragma data_seg()
#pragma comment(linker, "/section:.Share,rws")


HHOOK gHook = 0;
extern HANDLE hProcess;				// 进程句柄

//钩子回调函数
LRESULT CALLBACK KeyboardProc(
	_In_  int code,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
)
{
	
	return CallNextHookEx(gHook, code, wParam, lParam);
}



//BOOL bIsInjected = FALSE;				// 是否注入完成
typedef int (WINAPI *MsgBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);			// 声明一个别名 MsgBoxA
typedef int (WINAPI *MsgBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);		// 声明一个别名 MsgBoxW
MsgBoxA oldMsgBoxA = NULL;			// 保存原函数地址
MsgBoxW oldMsgBoxW = NULL;			// 保存原函数地址
FARPROC pfMsgBoxA = NULL;				// 指向原函数地址的远指针
FARPROC pfMsgBoxW = NULL;				// 指向原函数地址的远指针
BYTE OldCodeA[5];					// 老的系统API入口代码
BYTE NewCodeA[5];					// 要跳转的API代码 (jmp xxxx)
BYTE OldCodeW[5];					// 老的系统API入口代码
BYTE NewCodeW[5];					// 要跳转的API代码 (jmp xxxx)
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);				// 我们自己的 MessageBoxA 函数
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
void HookOn(HANDLE pProcess);
void HookOff(HANDLE pProcess);

// 假 MessageBoxA
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int nRet = 0;
	
	//printf("HOOK !");
	// 先恢复 Hook，不然会造成死循环
	HookOff(hProcess);

	// 检验 MessageBoxA 是否失败（失败返回 0）
	nRet = ::MessageBoxA(hWnd, "Hook MessageBoxA", lpCaption, uType);
	nRet=::MessageBoxA(hWnd,lpText,lpCaption,uType);	// 调用原函数（如果你想暗箱操作的话）

	//再次 HookOn，否则只生效一次
	HookOn(hProcess);

	return nRet;
}

// 假 MessageBoxW
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	int nRet = 0;
	printf("HOOKW !");
	// 先恢复 Hook，不然会造成死循环
	HookOff(hProcess);

	// 检验 MessageBoxW 是否失败（失败返回 0）
	nRet = ::MessageBoxW(hWnd, L"Hook MessageBoxW", lpCaption, uType);
	nRet=::MessageBoxW(hWnd,lpText,lpCaption,uType);	// 调用原函数（如果你想暗箱操作的话）

	// 再次 HookOn，否则只生效一次
	HookOn(hProcess);

	return nRet;
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
		return;
	}

	// 恢复内存保护状态
	VirtualProtectEx(hProcess, pfMsgBoxA, 5, dwOldProtect, &dwTemp);

	// 同上，操作剩下的 MessageBoxW
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, PAGE_READWRITE, &dwOldProtect);
	dwRet = WriteProcessMemory(hProcess, pfMsgBoxW, NewCodeW, 5, &dwWrite);
	if (0 == dwRet || 0 == dwWrite) { /*TRACE("NewCodeW 写入失败");*/ }
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, dwOldProtect, &dwTemp);

	MessageBoxA(NULL, "TEST", "TEST", MB_OK);
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

	// 同上，操作剩下的 MessageBoxW
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfMsgBoxW, OldCodeW, 5, &dwWrite);
	if (0 == dwRet || 0 == dwWrite) { /*TRACE("OldCodeW 写入失败");*/ }
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, dwOldProtect, &dwTemp);

}

BOOL InjectAllProcess()
{
	DWORD                   dwPID = 0;
	HANDLE                  hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32          pe;

	// Get the snapshot of the system
	pe.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	// find process
	Process32First(hSnapShot, &pe);
	do
	{
		dwPID = pe.th32ProcessID;

		//鉴于系统安全性考虑，对于PID小于100的系统进程，不执行DLL诸如操作
		if (dwPID < 100)
			continue;
		
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, dwPID);
		//Inject(hProcess);
		auto pfnAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"),"LoadLibraryA");
		const char * path = "../Debug/InjectDLLResume.dll";
		auto baseAddress = VirtualAllocEx(hProcess, 0,strlen(path)+1, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(hProcess, baseAddress, path, strlen(path) + 1, NULL);
		auto hRemote = CreateRemoteThread(hProcess,NULL,0, pfnAddress, baseAddress,0,NULL);
		VirtualFreeEx(hProcess, baseAddress, strlen(path) + 1, MEM_RELEASE);

		CloseHandle(hRemote);
		CloseHandle(hProcess);
	} while (Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);
	
	return TRUE;
}

// 代码注入
void Inject(HANDLE hProcess)
{
	//hProcess = 
	//Inject(D);
	//HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	::hProcess = hProcess;
	// 如果还没有注入
	//if (!bIsInjected) {

		//保证只调用1次
		//bIsInjected = TRUE;

		// 获取函数地址
		HMODULE hmod = ::LoadLibrary(L"User32.dll");
		oldMsgBoxA = (MsgBoxA)::GetProcAddress(hmod, "MessageBoxA");	// 原 MessageBoxA 地址
		pfMsgBoxA = (FARPROC)oldMsgBoxA;								// 指向原 MessageBoxA 地址的指针
		oldMsgBoxW = (MsgBoxW)::GetProcAddress(hmod, "MessageBoxW");	// 原 MessageBoxW 地址
		pfMsgBoxW = (FARPROC)oldMsgBoxW;								// 指向原 MessageBoxW 地址的指针

		// 指针为空则结束运行
		if (pfMsgBoxA == NULL) { MessageBox(NULL, L"cannot get MessageBoxA()", L"error", 0); return; }
		if (pfMsgBoxW == NULL) { MessageBox(NULL, L"cannot get MessageBoxW()", L"error", 0); return; }

		// 将原API中的入口代码保存入 OldCodeA[]，OldCodeW[]
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

		// 开始 Hook
		HookOn(hProcess);
	//}
}

void Init() {
	//// 获取调用 dll 的进程句柄
	InjectAllProcess();
	//DWORD dwPid = ::GetCurrentProcessId();
	//// 获取调用 dll 的进程句柄
	//hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
	//Inject(hProcess);
}

void ChangeTable(HMODULE module) {
	HMODULE hModule = GetModuleHandle(NULL);
	
	ULONG size;
	PROC pfnOld = GetProcAddress(GetModuleHandleA("User32.dll"), "MessageBoxA");

	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)
		ImageDirectoryEntryToData(module,true,IMAGE_DIRECTORY_ENTRY_IMPORT,&size);
		while (pImport->FirstThunk) {
			int i = 0;
			char * ModuleName = (char*)((BYTE*)module + pImport->Name);
			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
				((BYTE*)module + pImport->FirstThunk);
			while (pThunk->u1.Function)
			{
				char * Func = (char*)((BYTE*)module + pThunk->u1.AddressOfData + 2);
				PROC *ppfn = (PROC*)&pThunk->u1.Function;
				printf("%s\r\n",Func);
				if(pfnOld == *ppfn)
				{
					MessageBoxA(NULL,"找到函数","",MB_OK);
				}
				pThunk++;
			}
			pImport++;
		}
}


bool OnHook()
{
	//if (gHook == 0)
	//{
	//	gHook = SetWindowsHookEx(WH_MOUSE, KeyboardProc, hProcess, 0);
	//	return true;
	//}
	return false;
}

bool UnHook()
{
	if (gHook != 0)
	{
		return UnhookWindowsHookEx(gHook);
	}
	return false;
}
