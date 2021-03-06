// ConsoleApplicationTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "../Module/tool.h"
#include "../Bonjour/Bonjour.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/Bonjour.lib")
#pragma comment(lib,"../x64/Debug/Module.lib")
#else
#pragma comment(lib,"../Debug/Bonjour.lib")
#pragma comment(lib,"../Debug/Module.lib")
#endif


NTSTATUS (WINAPI *RealNtCreateFile)(
	OUT PHANDLE           FileHandle,
	IN ACCESS_MASK        DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK  IoStatusBlock,
	IN PLARGE_INTEGER     AllocationSize,
	IN ULONG              FileAttributes,
	IN ULONG              ShareAccess,
	IN ULONG              CreateDisposition,
	IN ULONG              CreateOptions,
	IN PVOID              EaBuffer,
	IN ULONG              EaLength);

NTSTATUS WINAPI MyNtCreateFile(
	OUT PHANDLE           FileHandle,
	IN ACCESS_MASK        DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK  IoStatusBlock,
	IN PLARGE_INTEGER     AllocationSize,
	IN ULONG              FileAttributes,
	IN ULONG              ShareAccess,
	IN ULONG              CreateDisposition,
	IN ULONG              CreateOptions,
	IN PVOID              EaBuffer,
	IN ULONG              EaLength
) {
	//FILE* fh;
	//fopen_s(&fh, "D:/file.txt", "a+");
	//wchar_t buf[1024];
	//wsprintf(buf, L"Create: %s\r\n", ObjectAttributes->ObjectName->Buffer);
	//fwrite(buf, sizeof(buf), wcslen(buf), fh);
	//fclose(fh);
	//return STATUS_OBJECT_NAME_NOT_FOUND;

	MessageBox(NULL, ObjectAttributes->ObjectName->Buffer, L"调用了NtCreateFile", MB_OK);
	return RealNtCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength
	);
}


int main()
{
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	auto originNtCreateFile = (NTCREATEFILE)GetProcAddress(hNtdll, "NtCreateFile");

	Bonjour jour;
	jour.InlineHook((PBYTE)originNtCreateFile, (PVOID*)&RealNtCreateFile, (PBYTE)MyNtCreateFile);

	MyNtQuerySystemInformation([](SYSTEM_PROCESS_INFORMATION* info) {
		printf("线程数:%d", info->NumberOfThreads);
	});
	////MessageBoxW(NULL, L"123", L"Title", MB_OK);
	auto ret = CreateFileW(L"E://1.txt",
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,        //打开已存在的文件 
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (ret == INVALID_HANDLE_VALUE) {
		printf("open file error!\n");
		CloseHandle(ret);
	}

	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
	//MessageBoxA(NULL, "123", "Title", MB_OK);
	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
	//MessageBoxW(NULL, L"123", L"Title", MB_OK);
}



