#pragma once
//�����ȶ���DLL_IMPORT_EXPORT���ٰ���Bonjour.hͷ�ļ�
#define DLL_IMPORT_EXPORT     _declspec(dllexport)	//cpp�ļ�����Ϊ������(dllexport)
#include "Bonjour.h"

//Ϊ�˲���¶DLL�����õ���������class����Щͷ�ļ�������Bonjour.h�а�����������д��Bonjour.cpp��
#include "Println.h"
#include "RealFunc.h"	
#include <tlhelp32.h>	//����CreateToolhelp32Snapshotö�ٽ���

//����ȫ�ֱ���
PVOID g_pHookFunc_Dr0 = 0;
PVOID g_pHookFunc_Dr1 = 0;
PVOID g_pHookFunc_Dr2 = 0;
PVOID g_pHookFunc_Dr3 = 0;
BOOL g_IsUsed_Dr0 = false;
BOOL g_IsUsed_Dr1 = false;
BOOL g_IsUsed_Dr2 = false;
BOOL g_IsUsed_Dr3 = false;


//************************************
// Method:     InlineHook 
// Description:
// Parameter:  PBYTE pOriginFunc - 
// Parameter:  OUT PVOID * ppRealFunc - 
// Parameter:  PBYTE pHookFunc - 
// Returns:    void - 
//************************************
void Bonjour::InlineHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc)
{
	//
	setlocale(LC_ALL, "");

#ifdef _WIN64
	Println::INFO(String::Format("Inline Hook for 64-bit Processes, Ŀ�꺯����ַ=%p", pOriginFunc));
#else
	Println::INFO(String::Format("Inline Hook for 32-bit Processes, Ŀ�꺯����ַ=%p", pOriginFunc));
#endif

	//����ԭ����ǰ14�ֽڵĴ���ճ����2GB��Χ�ڵ������ڴ棬ͬʱ������Ե�ַ��ͬʱ��ĩβ�����תָ������ת��ԭʼ����
	RealFunc realFunc;
	realFunc.CopyTopOfOriginFunc(pOriginFunc, ��������С�ֽ���_InlineHook);
	*ppRealFunc = realFunc.RealFuncճ������λ��;

	//�޸ı��ҹ����������Ǵ��봦���ڴ汣�����ԣ��ɶ�д��ִ��
	DWORD oldProtect = 0;
	DWORD newProtect = 0;
	VirtualProtect((LPVOID)pOriginFunc, realFunc.OriginFunc�����ǵĴ��볤��, PAGE_EXECUTE_READWRITE, &oldProtect);

	//��ԭ�����ı����Ǵ����λ��д����תָ��,��ת�����Ӻ���
	memset(pOriginFunc, 0x90, realFunc.OriginFunc�����ǵĴ��볤��);	//��ʼ��ΪNOP
	if (sizeof(void*) == 8)
	{//For 64-bit Processes, 64λ���ʵ�ֳ���2GB����ת��

		/*
				PUSH ��32λ��ַ
				MOV dword ptr ss:[rsp+0x4],��32λ��ַ
				RET
		*/
		BYTE PUSH[1] = { 0x68 };
		BYTE MOV_DWORD_PTR_SS[4] = { 0xC7,0x44,0x24,0x04 };
		BYTE RET[1] = { 0xC3 };
		UINT32 hookFunc_low32 = (UINT32)pHookFunc;
		UINT32 hookFunc_high32 = (UINT64)pHookFunc >> 32;
		memcpy(pOriginFunc, &PUSH, 1);
		memcpy(pOriginFunc + 1, &hookFunc_low32, 4);
		memcpy(pOriginFunc + 1 + 4, &MOV_DWORD_PTR_SS, 4);
		memcpy(pOriginFunc + 1 + 4 + 4, &hookFunc_high32, 4);
		memcpy(pOriginFunc + 1 + 4 + 4 + 4, &RET, 1);

	}
	else
	{//For 32-bit Processes, ֻ��ʹ����ͨ��ת

		BYTE JMP = { 0xE9 };
		UINT32 opcode = pHookFunc - (pOriginFunc + 5);	//Ŀ���ַ = ��ǰָ�����һ��ָ���ַ + opcode
		memcpy(pOriginFunc, &JMP, 1);
		memcpy(pOriginFunc + 1, &opcode, 4);

	}
	//�ָ����ҹ����������Ǵ��봦���ڴ汣������
	VirtualProtect((LPVOID)pOriginFunc, realFunc.OriginFunc�����ǵĴ��볤��, oldProtect, NULL);


	Println::SUCCESS("Inline Hook��� \n");
}

//************************************
// Method:     VEHHook 
// Description:
// Parameter:  PBYTE pOriginFunc - 
// Parameter:  OUT PVOID * ppRealFunc - 
// Parameter:  PBYTE pHookFunc - 
// Parameter:  enum DRX Drx - ѡ����ԼĴ�����Dr0~Dr3�������ظ�����
// Parameter:  DWORD64 Dr7Type - Ӳ���ϵ������
// Returns:    void - 
//************************************
void Bonjour::VEHHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc, enum DRX Drx, DWORD64 Dr7Type)
{
	//
	switch (Drx)
	{
		case DR0:if (g_IsUsed_Dr0) { Println::Error(String::Format(" Dr0�Ĵ����Ѿ������ù�Ӳ�ϣ�Hook��ֹ")); return; } g_IsUsed_Dr0 = true; g_pHookFunc_Dr0 = pHookFunc; break;
		case DR1:if (g_IsUsed_Dr1) { Println::Error(String::Format(" Dr1�Ĵ����Ѿ������ù�Ӳ�ϣ�Hook��ֹ")); return; } g_IsUsed_Dr1 = true; g_pHookFunc_Dr1 = pHookFunc; break;
		case DR2:if (g_IsUsed_Dr2) { Println::Error(String::Format(" Dr2�Ĵ����Ѿ������ù�Ӳ�ϣ�Hook��ֹ")); return; } g_IsUsed_Dr2 = true; g_pHookFunc_Dr2 = pHookFunc; break;
		case DR3:if (g_IsUsed_Dr3) { Println::Error(String::Format(" Dr3�Ĵ����Ѿ������ù�Ӳ�ϣ�Hook��ֹ")); return; } g_IsUsed_Dr3 = true; g_pHookFunc_Dr3 = pHookFunc; break;
	}

	//
	setlocale(LC_ALL, "");


#ifdef _WIN64
	Println::INFO(String::Format("VEH Hook for 64-bit Processes, Ŀ�꺯����ַ=%p�����ԼĴ���=[Dr%d]", pOriginFunc, Drx));
#else
	Println::INFO(String::Format("VEH Hook for 32-bit Processes, Ŀ�꺯����ַ=%p�����ԼĴ���=[Dr%d]", pOriginFunc, Drx));
#endif

	//����ԭ����[�׾����]ճ����2GB��Χ�ڵ������ڴ棬ͬʱ������Ե�ַ	*��ͬʱ��ĩβ�����תָ������ת��ԭʼ����
	RealFunc realFunc;
	realFunc.CopyTopOfOriginFunc(pOriginFunc, ��������С�ֽ���_VEHHook);
	*ppRealFunc = realFunc.RealFuncճ������λ��;

	//Ϊÿ���߳�����Ӳ���ϵ㣨����ǰ�̣߳�
	SetHardwareBP(pOriginFunc, Drx, Dr7Type);

	//���VEH�����쳣��������������ת�������Ӻ��� ������=1��ʾ����Veh����ͷ����=0��ʾ���뵽VEH����β��
	AddVectoredExceptionHandler(1, MyVectoredExceptionHandle);

	Println::SUCCESS("VEH Hook��� \n");
}

LONG NTAPI MyVectoredExceptionHandle(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	//Ӳ���ϵ�ֻ�ᴥ��STATUS_SINGLE_STEP�����쳣
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
	{
		//���Dr6�ĵ�4λ�Ƿ���1��λ,�Ϳ����жϸõ����Ƿ�����ΪӲ���ϵ㱻���µ�
		//���Dr6����һλΪ1���ж�����Dr0~Dr3�е��ĸ��ϵ���µ�
		//Dr0->0001��Dr1->0010��Dr2->0100��Dr3->1000��
		if ((ExceptionInfo->ContextRecord->Dr6 & 0x1) && g_pHookFunc_Dr0 != 0x0) //��ȫ��飺��DR��Ӳ���Ƿ��������ֶ�������(��ΪDR7������Ϊ��������DR)
		{
			//��ת�����Ӻ���
#ifdef _WIN64
			ExceptionInfo->ContextRecord->Rip = (DWORD64)g_pHookFunc_Dr0;
#else
			ExceptionInfo->ContextRecord->Eip = (DWORD)g_pHookFunc_Dr0;
#endif
			//����'�쳣�Ѵ���'
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		//DR1
		else if ((ExceptionInfo->ContextRecord->Dr6 & 0x2) && g_pHookFunc_Dr1 != 0x0)
		{
#ifdef _WIN64
			ExceptionInfo->ContextRecord->Rip = (DWORD64)g_pHookFunc_Dr1;
#else
			ExceptionInfo->ContextRecord->Eip = (DWORD)g_pHookFunc_Dr1;
#endif
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		//DR2
		else if ((ExceptionInfo->ContextRecord->Dr6 & 0x4) && g_pHookFunc_Dr2 != 0x0)
		{
#ifdef _WIN64
			ExceptionInfo->ContextRecord->Rip = (DWORD64)g_pHookFunc_Dr2;
#else
			ExceptionInfo->ContextRecord->Eip = (DWORD)g_pHookFunc_Dr2;
#endif
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		//DR3
		else if ((ExceptionInfo->ContextRecord->Dr6 & 0x8) && g_pHookFunc_Dr3 != 0x0)
		{
#ifdef _WIN64
			ExceptionInfo->ContextRecord->Rip = (DWORD64)g_pHookFunc_Dr3;
#else
			ExceptionInfo->ContextRecord->Eip = (DWORD)g_pHookFunc_Dr3;
#endif
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	//�����������¸������쳣������
	return EXCEPTION_CONTINUE_SEARCH;
}

//************************************
// Method:     SetHardwareBP 
// Description:�������߳���Ӳ���ϵ�
// Parameter:  PVOID pOriginFunc - Ŀ��ϵ��ַ
// Parameter:  enum DRX Drx - ��������Ӳ�ϵĵ��ԼĴ���
// Parameter:  DWORD64 Dr7Type - Ӳ�ϵ����ͣ���ִ�С���д�롢����д��
// Returns:    BOOL - 
//************************************
BOOL Bonjour::SetHardwareBP(PVOID pOriginFunc, enum DRX Drx, DWORD64 Dr7Type)
{
	CONTEXT context;
	context.ContextFlags = CONTEXT_ALL;
	DWORD64 CurrentThreadID = GetCurrentThreadId();
	DWORD64 CurrentProcessID = GetCurrentProcessId();

	// �����߳̿���
	THREADENTRY32 threadEntry32 = { sizeof(THREADENTRY32) };
	HANDLE  hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE) { printf("��error�������߳̿���ʧ��... \n"); return FALSE; }
	// ��ȡ��һ���߳�
	Thread32First(hThreadSnap, &threadEntry32);
	do
	{
		//ɸѡ��ǰ�����µ������̣߳��������Ӳ���ϵ�
		//����ǵ�ǰ�߳�����������Ϊ����ǰ�̺߳���ûЧ��

		if (threadEntry32.th32OwnerProcessID == CurrentProcessID
			&& threadEntry32.th32ThreadID != CurrentThreadID)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry32.th32ThreadID);
			if (hThread == 0) { Println::Warn(String::Format("��ID=0x%llX���߳�ʧ��...LastErrorCode=%d", GetLastError())); return FALSE; };
			SuspendThread(hThread);
			GetThreadContext(hThread, &context);
			switch (Drx)
			{//��ע�⣬��ΪDR7����Ϊ�������мĴ�����Ӳ�ϣ������������ֻ����ĳһ��DR����ô��Ҫ�ڴ����ж�����DR������Ӳ������ȫ���

				case DR0:
				{
					if (context.Dr0 != 0x0 && (LPVOID)context.Dr0 != pOriginFunc) { Println::Warn(String::Format("���Ŀ����̵�Dr0���ԼĴ����ѱ�ռ�ӣ��߳�ID=%08d,Dr0=%p ", threadEntry32.th32ThreadID, context.Dr0)); }
					context.Dr0 = (DWORD64)pOriginFunc; break;
				}
				case DR1:
				{
					if (context.Dr1 != 0x0 && (LPVOID)context.Dr1 != pOriginFunc) { Println::Warn(String::Format("���Ŀ����̵�Dr1���ԼĴ����ѱ�ռ�ӣ��߳�ID=%08d,Dr1=%p ", threadEntry32.th32ThreadID, context.Dr1)); }
					context.Dr1 = (DWORD64)pOriginFunc; break;
				}
				case DR2:
				{
					if (context.Dr2 != 0x0 && (LPVOID)context.Dr2 != pOriginFunc) { Println::Warn(String::Format("���Ŀ����̵�Dr2���ԼĴ����ѱ�ռ�ӣ��߳�ID=%08d,Dr2=%p ", threadEntry32.th32ThreadID, context.Dr2)); }
					context.Dr2 = (DWORD64)pOriginFunc; break;
				}
				case DR3:
				{
					if (context.Dr3 != 0x0 && (LPVOID)context.Dr3 != pOriginFunc) { Println::Warn(String::Format("���Ŀ����̵�Dr3���ԼĴ����ѱ�ռ�ӣ��߳�ID=%08d,Dr3=%p ", threadEntry32.th32ThreadID, context.Dr3)); }
					context.Dr3 = (DWORD64)pOriginFunc; break;
				}
			}

			context.Dr7 = Dr7Type;
			//context.Dr7 = 0x55;		//�ֲ��ϵ�+ִ�жϵ� (��Ч)
			//context.Dr7 = 0xFFFF0055;	//�ֲ��ϵ�+����д���ݶϵ㣨��Ч��
			//context.Dr7 = 0x55550055;	//�ֲ��ϵ�+д��ϵ㣨��Ч��
			//context.Dr7 = 0xAA;		//ȫ�ֶϵ�+ִ�жϵ㣨��Ч...��

			SetThreadContext(hThread, &context);
			ResumeThread(hThread);
		}


	} while (Thread32Next(hThreadSnap, &threadEntry32));

	Println::INFO(String::Format("�Ѷ������̵߳�[Dr%d]�Ĵ�������Ӳ���ϵ�,�¶ϵ�ַ=%p,�ϵ�����=0x%llx", Drx, pOriginFunc, Dr7Type));
	return TRUE;
}
