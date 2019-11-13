#pragma once
#include <windows.h>

//引用头文件，这里直接引用Bonjour项目下的头文件
#include "../Bonjour/Bonjour.h"

#ifdef _WIN64 
//导入DLL(x64),在DLL互相调用的时候，一定要统一版本，即DEBUG版本的exe必须调用DEBUG版本的dll
#ifdef _DEBUG 
#pragma comment (lib,"..\\x64\\Debug\\Bonjour.lib")
#else 
#pragma comment (lib,"..\\x64\\Release\\Bonjour.lib")
#endif

#else
//导入DLL(x86)
#ifdef _DEBUG 
#pragma comment (lib,"..\\Debug\\Bonjour.lib")  
#else
#pragma comment (lib,"..\\Release\\Bonjour.lib")  
#endif
#endif


//extern "C" __declspec(dllexport) void InlineHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc);