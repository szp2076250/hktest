#pragma once
#include "stdafx.h"

#ifdef SONGAPI
#define SONGAPI _declspec(dllimport)
#else 
#define SONGAPI _declspec(dllexport)
#endif 

extern"C" SONGAPI bool OnHook();
extern"C" SONGAPI bool UnHook();
extern"C" SONGAPI void Inject(DWORD pid);
extern"C" SONGAPI void Init();
