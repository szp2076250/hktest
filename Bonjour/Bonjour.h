#pragma once
#define _CRT_SECURE_NO_WARNINGS
//Ϊ�����ÿͻ��˳����DLL�����ø����һ��ͷ�ļ���ͨ�������ͷ�ļ���ʹ�ú��Ԥ����ָ��������
#ifdef DLL_IMPORT_EXPORT
#else 
#define DLL_IMPORT_EXPORT __declspec(dllimport)	
#endif

#include <iostream>
#include <windows.h>

//ö��4�����ԼĴ���
enum DRX { DR0, DR1, DR2, DR3 };
//����ȫ�ֱ����������������ʼ����
extern PVOID g_pHookFunc_Dr0;
extern PVOID g_pHookFunc_Dr1;
extern PVOID g_pHookFunc_Dr2;
extern PVOID g_pHookFunc_Dr3;
extern BOOL g_IsUsed_Dr0;
extern BOOL g_IsUsed_Dr1;
extern BOOL g_IsUsed_Dr2;
extern BOOL g_IsUsed_Dr3;
//����Ӳ���ϵ�����ͣ�Dr7��ֵ��
#define Ӳ������_��ִ�� 0x55		    //�ֲ��ϵ�+ִ�жϵ� (��Ч)
#define Ӳ������_��д�� 0x55550055	//�ֲ��ϵ�+д��ϵ㣨��Ч��
#define Ӳ������_����д 0xFFFF0055   //�ֲ��ϵ�+����д�ϵ㣨��Ч��

LONG NTAPI MyVectoredExceptionHandle(struct _EXCEPTION_POINTERS* ExceptionInfo);	//�ҵ�������������������ת���������Ӻ���
//class DLL_IMPORT_EXPORT Bonjour
class Bonjour
{
public:
	void InlineHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc);
	void VEHHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc, enum DRX Drx, DWORD64 Dr7Type);

private:
	BOOL SetHardwareBP(PVOID pOriginFunc, enum DRX Drx, DWORD64 Dr7Type);
};
