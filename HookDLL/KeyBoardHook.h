#pragma once
#include <Windows.h>

#ifdef SONGAPI
#define SONGAPI _declspec(dllimport)
#else 
#define SONGAPI _declspec(dllexport)
#endif 

//extern"C" _declspec(dllexport) bool OnHook();
//extern"C" _declspec(dllexport) bool UnHook();
//extern"C" _declspec(dllexport) void Inject(HANDLE hd);
extern"C" SONGAPI void Init();
