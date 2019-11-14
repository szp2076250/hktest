#pragma once
#include "Header.h"

BOOL EnableDebugPrivilege(BOOL fEnable);

typedef void(*QueryCallBack)(SYSTEM_PROCESS_INFORMATION * pInfo);
BOOL MyNtQuerySystemInformation(QueryCallBack);