#pragma once
#include "stdafx.h"
#include "wchar.h"
#include "stdlib.h"
#include "stdio.h"
#include "KeyBoardHook.h"
#include "dbghelp.h"
#include "tlhelp32.h"

#pragma comment(lib,"Dbghelp.lib")

extern HMODULE gModule;
HHOOK gHook = 0;

//���ӻص�����
LRESULT CALLBACK KeyboardProc(
	_In_  int code,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
)
{
	
	return CallNextHookEx(gHook, code, wParam, lParam);
}

bool OnHook()
{
	if (gHook == 0)
	{
		gHook = SetWindowsHookEx(WH_MOUSE, KeyboardProc, gModule, 0);
		return true;
	}
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

// ȫ�ֹ��������������֮�乲�����ݣ�
#pragma data_seg(".Share")
HWND g_hWnd = NULL;				// �����ھ��
HHOOK hhk = NULL;				// ��깳�Ӿ��
HINSTANCE hInst = NULL;			// ��dllʵ�����
#pragma data_seg()
#pragma comment(linker, "/section:.Share,rws")


// ȫ�ֱ���
HANDLE hProcess = NULL;				// ���̾��
//BOOL bIsInjected = FALSE;				// �Ƿ�ע�����
typedef int (WINAPI *MsgBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);			// ����һ������ MsgBoxA
typedef int (WINAPI *MsgBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);		// ����һ������ MsgBoxW
MsgBoxA oldMsgBoxA = NULL;			// ����ԭ������ַ
MsgBoxW oldMsgBoxW = NULL;			// ����ԭ������ַ
FARPROC pfMsgBoxA = NULL;				// ָ��ԭ������ַ��Զָ��
FARPROC pfMsgBoxW = NULL;				// ָ��ԭ������ַ��Զָ��
BYTE OldCodeA[5];					// �ϵ�ϵͳAPI��ڴ���
BYTE NewCodeA[5];					// Ҫ��ת��API���� (jmp xxxx)
BYTE OldCodeW[5];					// �ϵ�ϵͳAPI��ڴ���
BYTE NewCodeW[5];					// Ҫ��ת��API���� (jmp xxxx)
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);				// �����Լ��� MessageBoxA ����
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
void HookOn(HANDLE pProcess);
void HookOff(HANDLE pProcess);

// �� MessageBoxA
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int nRet = 0;
	
	//printf("HOOK !");
	// �Ȼָ� Hook����Ȼ�������ѭ��
	HookOff(hProcess);

	// ���� MessageBoxA �Ƿ�ʧ�ܣ�ʧ�ܷ��� 0��
	nRet = ::MessageBoxA(hWnd, "Hook MessageBoxA", lpCaption, uType);
	nRet=::MessageBoxA(hWnd,lpText,lpCaption,uType);	// ����ԭ������������밵������Ļ���

	//�ٴ� HookOn������ֻ��Чһ��
	HookOn(hProcess);

	return nRet;
}

// �� MessageBoxW
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	int nRet = 0;
	printf("HOOKW !");
	// �Ȼָ� Hook����Ȼ�������ѭ��
	HookOff(hProcess);

	// ���� MessageBoxW �Ƿ�ʧ�ܣ�ʧ�ܷ��� 0��
	nRet = ::MessageBoxW(hWnd, L"Hook MessageBoxW", lpCaption, uType);
	//nRet=::MessageBoxW(hWnd,lpText,lpCaption,uType);	// ����ԭ������������밵������Ļ���

	// �ٴ� HookOn������ֻ��Чһ��
	HookOn(hProcess);

	return nRet;
}


// �������ӣ��޸� API ͷ 5 ���ֽڣ�
void HookOn(HANDLE hProcess)
{
	// ������̾���Ƿ�Ϊ��
	//assert(hProcess != NULL);

	DWORD dwTemp = 0,		// �޸ĺ���ڴ汣������
		dwOldProtect,		// ֮ǰ���ڴ汣������
		dwRet = 0,			// �ڴ�д��ɹ���־��0���ɹ���1�ɹ�
		dwWrite;			// д������ڴ���ֽ���

	// ���������ڴ汣��
	VirtualProtectEx(
		hProcess,			// ���̾��
		pfMsgBoxA,			// ָ�򱣻������ַ��ָ��
		5,					// Ҫ���ĵ�������ֽڴ�С
		PAGE_READWRITE,		// �ڴ汣�����ͣ�PAGE_READWRITE���ɶ���д
		&dwOldProtect		// ����ԭ�����ڴ汣������
	);

	// �ж��Ƿ�ɹ�д���ڴ�
	dwRet = WriteProcessMemory(
		hProcess,			// ���̾��
		pfMsgBoxA,			// ָ��д���ַ��ָ��
		NewCodeA,			// ָ����д�����ݵĻ�����ָ��
		5,					// д���ֽ���
		&dwWrite			// ���մ��䵽�����е��ֽ���
	);
	if (0 == dwRet || 0 == dwWrite) {
		//TRACE("NewCodeA д��ʧ��");	// ��¼��־��Ϣ
	}

	// �ָ��ڴ汣��״̬
	VirtualProtectEx(hProcess, pfMsgBoxA, 5, dwOldProtect, &dwTemp);

	// ͬ�ϣ�����ʣ�µ� MessageBoxW
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, PAGE_READWRITE, &dwOldProtect);
	dwRet = WriteProcessMemory(hProcess, pfMsgBoxW, NewCodeW, 5, &dwWrite);
	if (0 == dwRet || 0 == dwWrite) { /*TRACE("NewCodeW д��ʧ��");*/ }
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, dwOldProtect, &dwTemp);

}

// �رչ��ӣ��޸� API ͷ 5 ���ֽڣ�
void HookOff(HANDLE hProcess)
{
	// ������̾���Ƿ�Ϊ��
	//ASSERT(hProcess != NULL);

	DWORD dwTemp = 0,			// �޸ĺ���ڴ汣������
		dwOldProtect = 0,		// ֮ǰ���ڴ汣������
		dwRet = 0,				// �ڴ�д��ɹ���־��0���ɹ���1�ɹ�
		dwWrite = 0;			// д������ڴ���ֽ���

	// ���������ڴ汣��
	VirtualProtectEx(
		hProcess,				// ���̾��
		pfMsgBoxA,				// ָ�򱣻������ַ��ָ��
		5,						// Ҫ���ĵ�������ֽڴ�С
		PAGE_READWRITE,			// �ڴ汣�����ͣ�PAGE_READWRITE���ɶ���д
		&dwOldProtect			// ����ԭ�����ڴ汣������
	);

	dwRet = WriteProcessMemory(
		hProcess,				// ���̾��
		pfMsgBoxA,				// ָ��д���ַ��ָ��
		OldCodeA,				// ָ����д�����ݵĻ�����ָ��
		5,						// д���ֽ���
		&dwWrite				// ���մ��䵽�����е��ֽ���
	);
	if (0 == dwRet || 0 == dwWrite) {
		//TRACE("OldCodeA д��ʧ��");	// ��¼��־��Ϣ
	}

	// �ָ��ڴ汣��״̬
	VirtualProtectEx(hProcess, pfMsgBoxA, 5, dwOldProtect, &dwTemp);

	// ͬ�ϣ�����ʣ�µ� MessageBoxW
	VirtualProtectEx(hProcess, pfMsgBoxW, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfMsgBoxW, OldCodeW, 5, &dwWrite);
	if (0 == dwRet || 0 == dwWrite) { /*TRACE("OldCodeW д��ʧ��");*/ }
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

		//����ϵͳ��ȫ�Կ��ǣ�����PIDС��100��ϵͳ���̣���ִ��DLL�������
		if (dwPID < 100)
			continue;

		//Inject(dwPID);
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, dwPID);
		auto pfnAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernerl32"),"LoadLibraryW");
		const char * path = "../Debug/InjectDLL.dll";
		auto baseAddress = VirtualAllocEx(hProcess, 0,strlen(path)+1, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		SIZE_T sizeofWrite;
		WriteProcessMemory(hProcess, baseAddress, path, sizeof(char), &sizeofWrite);
		CreateRemoteThread(hProcess,NULL,0, pfnAddress, baseAddress,0,NULL);
		//VirtualFreeEx(hProcess, baseAddress, strlen(path) + 1, MEM_RELEASE);
		//if (nMode == 1)
		//	Inject(dwPID, szDllPath);
		//else
		//	EjectDll(dwPID, szDllPath);
	} while (Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);

	return TRUE;
}

// ����ע��
void Inject(DWORD pid)
{
	//hProcess = 
	//Inject(D);
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	::hProcess = hProcess;
	// �����û��ע��
	//if (!bIsInjected) {

		//��ֻ֤����1��
		//bIsInjected = TRUE;

		// ��ȡ������ַ
		HMODULE hmod = ::LoadLibrary(L"User32.dll");
		oldMsgBoxA = (MsgBoxA)::GetProcAddress(hmod, "MessageBoxA");	// ԭ MessageBoxA ��ַ
		pfMsgBoxA = (FARPROC)oldMsgBoxA;								// ָ��ԭ MessageBoxA ��ַ��ָ��
		oldMsgBoxW = (MsgBoxW)::GetProcAddress(hmod, "MessageBoxW");	// ԭ MessageBoxW ��ַ
		pfMsgBoxW = (FARPROC)oldMsgBoxW;								// ָ��ԭ MessageBoxW ��ַ��ָ��

		// ָ��Ϊ�����������
		if (pfMsgBoxA == NULL) { MessageBox(NULL, L"cannot get MessageBoxA()", L"error", 0); return; }
		if (pfMsgBoxW == NULL) { MessageBox(NULL, L"cannot get MessageBoxW()", L"error", 0); return; }

		// ��ԭAPI�е���ڴ��뱣���� OldCodeA[]��OldCodeW[]
		_asm
		{
			lea edi, OldCodeA; �� OldCodeA �ĵ�ַ�� edi
			mov esi, pfMsgBoxA; �� MessageBoxA �ĵ�ַ�� esi
			cld; �����־λ��λ
			movsd; ����˫��
			movsb; �����ֽ�
		}
		_asm
		{
			lea edi, OldCodeW; ����ͬ�ķ�ʽ���� MessageBoxW
			mov esi, pfMsgBoxW
			cld
			movsd
			movsb
		}

		// ��ԭ API ��һ���ֽڸ�Ϊ jmp
		NewCodeA[0] = 0xe9;
		NewCodeW[0] = 0xe9;

		// ���� jmp ����Ҫ���ĵ�ַ
		_asm
		{
			lea eax, MyMessageBoxA; �� MyMessageBoxA �ĵ�ַ�� eax
			mov ebx, pfMsgBoxA; �� MessageBoxA �ĵ�ַ�� ebx
			sub eax, ebx; ���� jmp ����Ҫ���ĵ�ַ
			sub eax, 5
			mov dword ptr[NewCodeA + 1], eax
		}
		_asm
		{
			lea eax, MyMessageBoxW; ����ͬ�ķ�ʽ���� MessageBoxW
			mov ebx, pfMsgBoxW
			sub eax, ebx
			sub eax, 5
			mov dword ptr[NewCodeW + 1], eax
		}

		// ��ʼ Hook
		HookOn(hProcess);
	//}
}

void Init() {
	DWORD dwPid = ::GetCurrentProcessId();
	// ��ȡ���� dll �Ľ��̾��
	InjectAllProcess();
}

//void ChangeTable() {
//	HMODULE hModule = GetModuleHandle(NULL);
//	ULONG size;
//	PROC pfnOld = GetProcAddress(GetModuleHandleA("User32.dll"), "MessageBoxA");
//
//	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)
//		ImageDirectoryEntryToData(hModule,true,IMAGE_DIRECTORY_ENTRY_IMPORT,&size);
//		while (pImport->FirstThunk) {
//			int i = 0;
//			char * ModuleName = (char*)((BYTE*)hModule + pImport->Name);
//			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
//				((BYTE*)hModule + pImport->FirstThunk);
//			while (pThunk->u1.Function)
//			{
//				char * Func = (char*)((BYTE*)hModule + pThunk->u1.AddressOfData + 2);
//				PROC *ppfn = (PROC*)&pThunk->u1.Function;
//				printf("%s\r\n",Func);
//				if(pfnOld == *ppfn)
//				{
//					MessageBoxA(NULL,"�ҵ�����","",MB_OK);
//				}
//				pThunk++;
//			}
//			pImport++;
//		}
//}