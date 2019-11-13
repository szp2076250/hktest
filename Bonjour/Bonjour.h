#pragma once
#define _CRT_SECURE_NO_WARNINGS
//为了能让客户端程序和DLL程序共用该类的一份头文件，通常在类的头文件中使用宏和预编译指令来处理。
#ifdef DLL_IMPORT_EXPORT
#else 
#define DLL_IMPORT_EXPORT __declspec(dllimport)	
#endif

#include <iostream>
#include <windows.h>

//枚举4个调试寄存器
enum DRX { DR0, DR1, DR2, DR3 };
//声明全局变量（不能在这里初始化）
extern PVOID g_pHookFunc_Dr0;
extern PVOID g_pHookFunc_Dr1;
extern PVOID g_pHookFunc_Dr2;
extern PVOID g_pHookFunc_Dr3;
extern BOOL g_IsUsed_Dr0;
extern BOOL g_IsUsed_Dr1;
extern BOOL g_IsUsed_Dr2;
extern BOOL g_IsUsed_Dr3;
//定义硬件断点的类型（Dr7的值）
#define 硬断类型_仅执行 0x55		    //局部断点+执行断点 (有效)
#define 硬断类型_仅写入 0x55550055	//局部断点+写入断点（有效）
#define 硬断类型_读或写 0xFFFF0055   //局部断点+读或写断点（有效）

LONG NTAPI MyVectoredExceptionHandle(struct _EXCEPTION_POINTERS* ExceptionInfo);	//我的向量处理函数，用于跳转至各个钩子函数
//class DLL_IMPORT_EXPORT Bonjour
class Bonjour
{
public:
	void InlineHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc);
	void VEHHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc, enum DRX Drx, DWORD64 Dr7Type);

private:
	BOOL SetHardwareBP(PVOID pOriginFunc, enum DRX Drx, DWORD64 Dr7Type);
};
