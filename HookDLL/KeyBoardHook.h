#pragma once
#include "stdafx.h"

extern"C" _declspec(dllexport) bool OnHook();
extern"C" _declspec(dllexport) bool UnHook();
extern"C" _declspec(dllexport) void Inject(DWORD pid);
extern"C" _declspec(dllexport) void Init();