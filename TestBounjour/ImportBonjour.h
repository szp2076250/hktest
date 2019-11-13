#pragma once
#include <windows.h>

//����ͷ�ļ�������ֱ������Bonjour��Ŀ�µ�ͷ�ļ�
#include "../Bonjour/Bonjour.h"

#ifdef _WIN64 
//����DLL(x64),��DLL������õ�ʱ��һ��Ҫͳһ�汾����DEBUG�汾��exe�������DEBUG�汾��dll
#ifdef _DEBUG 
#pragma comment (lib,"..\\x64\\Debug\\Bonjour.lib")
#else 
#pragma comment (lib,"..\\x64\\Release\\Bonjour.lib")
#endif

#else
//����DLL(x86)
#ifdef _DEBUG 
#pragma comment (lib,"..\\Debug\\Bonjour.lib")  
#else
#pragma comment (lib,"..\\Release\\Bonjour.lib")  
#endif
#endif


//extern "C" __declspec(dllexport) void InlineHook(PBYTE pOriginFunc, OUT PVOID* ppRealFunc, PBYTE pHookFunc);