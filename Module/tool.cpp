#include "tool.h"

BOOL EnableDebugPrivilege(BOOL fEnable) {
	// Enabling the debug privilege allows the application to see
	// information about service applications
	BOOL fOk = FALSE;    // Assume function fails
	HANDLE hToken;
	// Try to open this process's access token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
		&hToken)) {

		// Attempt to modify the "Debug" privilege
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return(fOk);
}

BOOL MyNtQuerySystemInformation(QueryCallBack onSuccess)
{
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	ULONG retlen = 2;
	NTSTATUS status = 0;
	HLOCAL hMem = NULL;

	NTQUERYSYSTEMINFORMATION NtQuerySystemInformation = NULL;
	SYSTEM_PROCESS_INFORMATION *pInfo = (SYSTEM_PROCESS_INFORMATION*)malloc(sizeof(SYSTEM_PROCESS_INFORMATION));
	NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(hNtdll,
		"NtQuerySystemInformation");
	auto lRetVal = NtQuerySystemInformation(SystemProcessInformation, pInfo, sizeof(SYSTEM_PROCESS_INFORMATION), NULL);
	if (pInfo != NULL) free(pInfo);

	switch (lRetVal)
	{
	case STATUS_INVALID_INFO_CLASS:
		printf("STATUS_INVALID_INFO_CLASS");
		break;
	case STATUS_ACCESS_VIOLATION:
		printf("STATUS_ACCESS_VIOLATION");
		break;
	case STATUS_INSUFFICIENT_RESOURCES:
		printf("STATUS_INSUFFICIENT_RESOURCES");
		break;
	case STATUS_WORKING_SET_QUOTA:
		printf("STATUS_WORKING_SET_QUOTA");
		break;
	case STATUS_NOT_IMPLEMENTED:
		printf("STATUS_NOT_IMPLEMENTED");
		break;
	case STATUS_INFO_LENGTH_MISMATCH:
		printf("STATUS_INFO_LENGTH_MISMATCH");
		do
		{
			pInfo = (SYSTEM_PROCESS_INFORMATION*)malloc(sizeof(ULONG)*retlen);
			if (pInfo)
			{
				memset(pInfo, 0, retlen);
				status = NtQuerySystemInformation(SystemProcessInformation, pInfo, retlen, &retlen);
				if (NT_SUCCESS(status))
				{
					//CallBack
					if (onSuccess != NULL) onSuccess(pInfo);
					if (pInfo != NULL) free(pInfo);
					return true;
				}
			}
			if (pInfo != NULL) free(pInfo);
		} while (status == STATUS_INFO_LENGTH_MISMATCH);
		break;
	}
	return false;
}
