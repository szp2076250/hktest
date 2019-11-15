#pragma once
//必须先定义DLL_IMPORT_EXPORT，再包含Bonjour.h头文件
#define DLL_IMPORT_EXPORT     _declspec(dllexport)	//cpp文件中作为导出类(dllexport)
#include "Bonjour.h"

//为了不暴露DLL中所用到的其他类class，这些头文件不能在Bonjour.h中包含，而必须写在Bonjour.cpp中
#include "Println.h"
#include "RealFunc.h"	
#include <tlhelp32.h>	//用于CreateToolhelp32Snapshot枚举进程

//定义全局变量
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
	Println::INFO(String::Format("Inline Hook for 64-bit Processes, 目标函数地址=%p", pOriginFunc));
#else
	Println::INFO(String::Format("Inline Hook for 32-bit Processes, 目标函数地址=%p", pOriginFunc));
#endif

	//拷贝原函数前14字节的代码粘贴至2GB范围内的虚拟内存，同时修正相对地址，同时在末尾添加跳转指令以跳转回原始函数
	RealFunc realFunc;
	realFunc.CopyTopOfOriginFunc(pOriginFunc, 拷贝的最小字节数_InlineHook);
	*ppRealFunc = realFunc.RealFunc粘贴处的位置;

	//修改被挂钩函数被覆盖代码处的内存保护属性：可读写可执行
	DWORD oldProtect = 0;
	DWORD newProtect = 0;
	VirtualProtect((LPVOID)pOriginFunc, realFunc.OriginFunc被覆盖的代码长度, PAGE_EXECUTE_READWRITE, &oldProtect);

	//向原函数的被覆盖代码的位置写入跳转指令,跳转至钩子函数
	memset(pOriginFunc, 0x90, realFunc.OriginFunc被覆盖的代码长度);	//初始化为NOP
	if (sizeof(void*) == 8)
	{//For 64-bit Processes, 64位汇编实现超过2GB的跳转：

		/*
				PUSH 低32位地址
				MOV dword ptr ss:[rsp+0x4],高32位地址
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
	{//For 32-bit Processes, 只需使用普通跳转

		BYTE JMP = { 0xE9 };
		UINT32 opcode = pHookFunc - (pOriginFunc + 5);	//目标地址 = 当前指令的下一条指令地址 + opcode
		memcpy(pOriginFunc, &JMP, 1);
		memcpy(pOriginFunc + 1, &opcode, 4);

	}
	//恢复被挂钩函数被覆盖代码处的内存保护属性
	VirtualProtect((LPVOID)pOriginFunc, realFunc.OriginFunc被覆盖的代码长度, oldProtect, NULL);


	Println::SUCCESS("Inline Hook完成 \n");
}

//************************************
// Method:     VEHHook 
// Description:
// Parameter:  PBYTE pOriginFunc - 
// Parameter:  OUT PVOID * ppRealFunc - 
// Parameter:  PBYTE pHookFunc - 
// Parameter:  enum DRX Drx - 选择调试寄存器，Dr0~Dr3，不可重复设置
// Parameter:  DWORD64 Dr7Type - 硬件断点的类型
// Returns:    void - 
//************************************
void Bonjour::VEHHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc, enum DRX Drx, DWORD64 Dr7Type)
{
	//
	switch (Drx)
	{
		case DR0:if (g_IsUsed_Dr0) { Println::Error(String::Format(" Dr0寄存器已经被设置过硬断，Hook终止")); return; } g_IsUsed_Dr0 = true; g_pHookFunc_Dr0 = pHookFunc; break;
		case DR1:if (g_IsUsed_Dr1) { Println::Error(String::Format(" Dr1寄存器已经被设置过硬断，Hook终止")); return; } g_IsUsed_Dr1 = true; g_pHookFunc_Dr1 = pHookFunc; break;
		case DR2:if (g_IsUsed_Dr2) { Println::Error(String::Format(" Dr2寄存器已经被设置过硬断，Hook终止")); return; } g_IsUsed_Dr2 = true; g_pHookFunc_Dr2 = pHookFunc; break;
		case DR3:if (g_IsUsed_Dr3) { Println::Error(String::Format(" Dr3寄存器已经被设置过硬断，Hook终止")); return; } g_IsUsed_Dr3 = true; g_pHookFunc_Dr3 = pHookFunc; break;
	}

	//
	setlocale(LC_ALL, "");


#ifdef _WIN64
	Println::INFO(String::Format("VEH Hook for 64-bit Processes, 目标函数地址=%p，调试寄存器=[Dr%d]", pOriginFunc, Drx));
#else
	Println::INFO(String::Format("VEH Hook for 32-bit Processes, 目标函数地址=%p，调试寄存器=[Dr%d]", pOriginFunc, Drx));
#endif

	//拷贝原函数[首句代码]粘贴至2GB范围内的虚拟内存，同时修正相对地址	*，同时在末尾添加跳转指令以跳转回原始函数
	RealFunc realFunc;
	realFunc.CopyTopOfOriginFunc(pOriginFunc, 拷贝的最小字节数_VEHHook);
	*ppRealFunc = realFunc.RealFunc粘贴处的位置;

	//为每个线程设置硬件断点（除当前线程）
	SetHardwareBP(pOriginFunc, Drx, Dr7Type);

	//添加VEH向量异常处理函数，用于跳转至各钩子函数 。参数=1表示插入Veh链的头部，=0表示插入到VEH链的尾部
	AddVectoredExceptionHandler(1, MyVectoredExceptionHandle);

	Println::SUCCESS("VEH Hook完成 \n");
}

LONG NTAPI MyVectoredExceptionHandle(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	//硬件断点只会触发STATUS_SINGLE_STEP单步异常
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
	{
		//检测Dr6的低4位是否有1的位,就可以判断该单步是否是因为硬件断点被断下的
		//检测Dr6的哪一位为1来判断是由Dr0~Dr3中的哪个断点断下的
		//Dr0->0001；Dr1->0010；Dr2->0100；Dr3->1000；
		if ((ExceptionInfo->ContextRecord->Dr6 & 0x1) && g_pHookFunc_Dr0 != 0x0) //安全检查：该DR的硬断是否是我们手动开启的(因为DR7被设置为启动所有DR)
		{
			//跳转至钩子函数
#ifdef _WIN64
			ExceptionInfo->ContextRecord->Rip = (DWORD64)g_pHookFunc_Dr0;
#else
			ExceptionInfo->ContextRecord->Eip = (DWORD)g_pHookFunc_Dr0;
#endif
			//返回'异常已处理'
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

	//不处理，交由下个向量异常处理函数
	return EXCEPTION_CONTINUE_SEARCH;
}

//************************************
// Method:     SetHardwareBP 
// Description:对所有线程下硬件断点
// Parameter:  PVOID pOriginFunc - 目标断点地址
// Parameter:  enum DRX Drx - 用于设置硬断的调试寄存器
// Parameter:  DWORD64 Dr7Type - 硬断的类型（仅执行、仅写入、读或写）
// Returns:    BOOL - 
//************************************
BOOL Bonjour::SetHardwareBP(PVOID pOriginFunc, enum DRX Drx, DWORD64 Dr7Type)
{
	CONTEXT context;
	context.ContextFlags = CONTEXT_ALL;
	DWORD64 CurrentThreadID = GetCurrentThreadId();
	DWORD64 CurrentProcessID = GetCurrentProcessId();

	// 创建线程快照
	THREADENTRY32 threadEntry32 = { sizeof(THREADENTRY32) };
	HANDLE  hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE) { printf("【error】创建线程快照失败... \n"); return FALSE; }
	// 获取第一个线程
	Thread32First(hThreadSnap, &threadEntry32);
	do
	{
		//筛选当前进程下的所有线程，逐个设置硬件断点
		//如果是当前线程则跳过，因为挂起当前线程好像没效果

		if (threadEntry32.th32OwnerProcessID == CurrentProcessID
			&& threadEntry32.th32ThreadID != CurrentThreadID)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry32.th32ThreadID);
			if (hThread == 0) { Println::Warn(String::Format("打开ID=0x%llX的线程失败...LastErrorCode=%d", GetLastError())); return FALSE; };
			SuspendThread(hThread);
			GetThreadContext(hThread, &context);
			switch (Drx)
			{//需注意，因为DR7设置为启用所有寄存器的硬断，所以如果我们只启用某一个DR，那么需要在代码中对其他DR触发的硬断做安全检查

				case DR0:
				{
					if (context.Dr0 != 0x0 && (LPVOID)context.Dr0 != pOriginFunc) { Println::Warn(String::Format("检测目标进程的Dr0调试寄存器已被占坑，线程ID=%08d,Dr0=%p ", threadEntry32.th32ThreadID, context.Dr0)); }
					context.Dr0 = (DWORD64)pOriginFunc; break;
				}
				case DR1:
				{
					if (context.Dr1 != 0x0 && (LPVOID)context.Dr1 != pOriginFunc) { Println::Warn(String::Format("检测目标进程的Dr1调试寄存器已被占坑，线程ID=%08d,Dr1=%p ", threadEntry32.th32ThreadID, context.Dr1)); }
					context.Dr1 = (DWORD64)pOriginFunc; break;
				}
				case DR2:
				{
					if (context.Dr2 != 0x0 && (LPVOID)context.Dr2 != pOriginFunc) { Println::Warn(String::Format("检测目标进程的Dr2调试寄存器已被占坑，线程ID=%08d,Dr2=%p ", threadEntry32.th32ThreadID, context.Dr2)); }
					context.Dr2 = (DWORD64)pOriginFunc; break;
				}
				case DR3:
				{
					if (context.Dr3 != 0x0 && (LPVOID)context.Dr3 != pOriginFunc) { Println::Warn(String::Format("检测目标进程的Dr3调试寄存器已被占坑，线程ID=%08d,Dr3=%p ", threadEntry32.th32ThreadID, context.Dr3)); }
					context.Dr3 = (DWORD64)pOriginFunc; break;
				}
			}

			context.Dr7 = Dr7Type;
			//context.Dr7 = 0x55;		//局部断点+执行断点 (有效)
			//context.Dr7 = 0xFFFF0055;	//局部断点+读或写数据断点（有效）
			//context.Dr7 = 0x55550055;	//局部断点+写入断点（有效）
			//context.Dr7 = 0xAA;		//全局断点+执行断点（无效...）

			SetThreadContext(hThread, &context);
			ResumeThread(hThread);
		}


	} while (Thread32Next(hThreadSnap, &threadEntry32));

	Println::INFO(String::Format("已对所有线程的[Dr%d]寄存器设置硬件断点,下断地址=%p,断点类型=0x%llx", Drx, pOriginFunc, Dr7Type));
	return TRUE;
}
