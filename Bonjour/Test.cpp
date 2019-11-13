//
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include "Bonjour.h"


//指向被挂钩函数被覆盖代码的函数指针
int(WINAPI* RealMessagebox)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType);

//钩子函数
int WINAPI MyMessageBox(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType)
{
	wprintf(L"进入我的MessageBox, 获取参数：参数1=%p,参数2=%s,参数3=%s,参数4=%d  \n", hWnd, lpText, lpCaption, uType);
	return RealMessagebox(hWnd, L"我爱她", L"改过的标题", uType);
}



int main()
{
	//
	Bonjour bonjour;

	//被挂钩函数
	PBYTE pOriginFunc = (PBYTE)MessageBox;

	//挂钩
	bonjour.InlineHook(pOriginFunc, (PVOID*)& RealMessagebox, (PBYTE)MyMessageBox);

	//测试
	MessageBox(0, L"内容", L"标题", 0);

	system("pause");
}

