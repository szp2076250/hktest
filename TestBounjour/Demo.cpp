// TestBounjour.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

#include "TestInlineHook_D3D11.h"


//step 1: define the OriginFunc pointer , the originFunction pointer must have exactly the same call signature including number of arguments and calling convention.
//定义函数指针
int(WINAPI* RealMessagebox)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType);

//step 2: define the Hook Function.
//定义钩子函数
int WINAPI MyMessageBox(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType)
{
	wprintf(L"进入MyMessageBox, 获取参数：param1=%p,param2=%s,param3=%s,param4=%d  \n", hWnd, lpText, lpCaption, uType);
	return RealMessagebox(hWnd, L"改过的内容", L"改过的标题", uType);
}

void  TestInlineHook_MessageBox()
{
	//step 3: instance Bonjour object
	Bonjour bonjour;

	//step 4: InlineHook MessageBox 
	//挂钩
	bonjour.InlineHook((PBYTE)MessageBox, (PVOID*)& RealMessagebox, (PBYTE)MyMessageBox);

	//step 5: Invoking MessageBox for test. 
	//测试
	//MessageBox(0, L"原始内容", L"原始标题", 0);
}


DWORD WINAPI TestVEHHook(LPVOID lpThreadParameter)
{
	Bonjour bonjour;
	bonjour.VEHHook((PBYTE)MessageBox, (PVOID*)& RealMessagebox, (PBYTE)MyMessageBox, DRX::DR0, 硬断类型_仅执行);
	return 0;
}


int main()
{
	//测试Inline Hook，挂钩MessageBox
	TestInlineHook_MessageBox();
	MessageBox(0, L"测试InlineHook", L"原始标题", 0);

	//测试VEH HOOK，挂钩MessageBox (需要由子线程向主线程下断)
	//WaitForSingleObject(CreateThread(0, 0, TestVEHHook, 0, 0, 0), INFINITE);
	//MessageBox(0, L"测试VEHHook", L"原始标题", 0);

	//测试挂钩D3D11 (Inline或VEH Hook)
	//WaitForSingleObject(CreateThread(0, 0, TestHook_D3D11, 0, 0, 0), INFINITE);
	//pSwapChain->Present(0x666, 0x555);
	//pContext->DrawIndexed(2, 4, 6);
	//pDevice->CreateQuery(0, 0);


	system("pause");
}

