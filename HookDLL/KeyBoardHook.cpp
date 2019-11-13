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
//#pragma data_seg(".Share")
//HWND g_hWnd = NULL;				// 主窗口句柄
//HHOOK hhk = NULL;				// 鼠标钩子句柄
//HINSTANCE hInst = NULL;			// 本dll实例句柄
//#pragma data_seg()
//#pragma comment(linker, "/section:.Share,rws")

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

		//
		if (dwPID < 100)
			continue;

		auto exePath = pe.szExeFile;
		if (exePath[0] != 'e') continue;
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, 0, dwPID);
		auto pfnAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"),"LoadLibraryA");
		const char * path = "E:/project/x64/Debug/InjectDLLResume.dll";
		auto baseAddress = VirtualAllocEx(hProcess, 0,strlen(path)+1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(hProcess, baseAddress, path, strlen(path) + 1, NULL);
		auto hRemote = CreateRemoteThread(hProcess,NULL,0, pfnAddress, baseAddress,0,NULL);
		if (hRemote != NULL) { 
			//WaitForSingleObject(hRemote, INFINITE);
			CloseHandle(hRemote);
			VirtualFreeEx(hProcess, baseAddress, strlen(path) + 1, MEM_RELEASE);
		}
		else {
			auto error = GetLastError();
			hProcess = NULL;
		}
		//CloseHandle(hProcess);
	} while (Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot); 
	
	return TRUE;
}


void Init() {
	InjectAllProcess();
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

