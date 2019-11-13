#pragma once
#include <windows.h>
#include <iostream>

//��Ҫ����Ŀ-�Ҽ�-����-��������-������-����-����������-���'d3d11.lib'���������ʾ���޷��������ⲿ����IDXGISwapChain��
#include "d3d11.h"		//D3D11  
#include "d3dcompiler.h"//D3D11

#include "ImportBonjour.h"//



//ָ�򱻹ҹ����������Ǵ���ĺ���ָ��
HRESULT(STDMETHODCALLTYPE* RealPresent)(IDXGISwapChain** self, UINT SyncInterval, UINT Flags);
void (STDMETHODCALLTYPE* RealDrawIndexed)(ID3D11DeviceContext** self, _In_  UINT IndexCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation);
HRESULT(STDMETHODCALLTYPE* RealCreateQuery)(ID3D11Device** self, _In_  const D3D11_QUERY_DESC* pQueryDesc, _COM_Outptr_opt_  ID3D11Query** ppQuery);
//���Ӻ���
HRESULT STDMETHODCALLTYPE MyPresent(IDXGISwapChain** self, UINT SyncInterval, UINT Flags)
{
	wprintf(L"����MyPresent, ��ȡ������this=%p,����1=%x,����2=%x \n", self, SyncInterval, Flags);
	return RealPresent(self, SyncInterval, Flags);
}
void STDMETHODCALLTYPE MyDrawIndexed(ID3D11DeviceContext** self, _In_  UINT IndexCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation)
{
	wprintf(L"����MyDrawIndexed, ��ȡ������this=%p,����1=%x,����2=%x,����3=%x \n", self, IndexCount, StartIndexLocation, BaseVertexLocation);
	return RealDrawIndexed(self, IndexCount, StartIndexLocation, BaseVertexLocation);
}
HRESULT STDMETHODCALLTYPE MyCreateQuery(ID3D11Device** self, _In_  const D3D11_QUERY_DESC* pQueryDesc, _COM_Outptr_opt_  ID3D11Query** ppQuery)
{
	wprintf(L"����MyCreateQuery, ��ȡ������this=%p,����1=%x,����2=%x\n", self, pQueryDesc, ppQuery);
	return RealCreateQuery(self, pQueryDesc, ppQuery);
}



LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;
DWORD WINAPI TestHook_D3D11(LPVOID lpThreadParameter)
{
	//ID3D11Device* pDevice;
	//ID3D11DeviceContext* pContext;
	//IDXGISwapChain* pSwapChain;

	//�Խ����������ڴ���������
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	//���������������ڻ�ȡ������и������ĵ�ַ����Hook
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));   //���0
	scd.BufferCount = 1;                              //����ֻ����һ���󻺳壨˫���壩���Ϊ1
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //������ɫ��ʽ,����ʹ��RGBA
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;  //���ű�
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //ɨ����
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //��ȾĿ�����
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //����ģʽ�л�
	scd.OutputWindow = hWnd;  //����Ϸ�����ڲ���������һ������
	scd.SampleDesc.Count = 1;                      //1�ز���
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //���ò���
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;  //�Ƿ�ȫ��
	scd.BufferDesc.Width = 1920;
	scd.BufferDesc.Height = 1080;
	scd.BufferDesc.RefreshRate.Numerator = 144;     //ˢ����
	scd.BufferDesc.RefreshRate.Denominator = 1;     //��ĸ
	scd.SampleDesc.Quality = 0;                     //�����ȼ�
	D3D_FEATURE_LEVEL featrueLevel = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featrueLevel, 1, D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext);

	//Present()
	//ͨ������������麯��Presentʱ��
	//00007FF673951D5C 48 8B 45 48       mov         rax, qword ptr[pSwapChain] ;rax��š�����������ĵ�ַ��
	//00007FF673951D60 48 8B 00          mov         rax, qword ptr[rax]		;rax��š�����������ĵ�1����Ա��ֵ���������ĵ�ַ
	//00007FF673951D63 45 33 C0          xor         r8d, r8d					;r8d ����3
	//00007FF673951D66 33 D2             xor         edx, edx					;edx ����2
	//00007FF673951D68 48 8B 4D 48       mov         rcx, qword ptr[pSwapChain]	;rcx ����1 ���ݶ���ָ�롾����������ĵ�ַ��
	//00007FF673951D6C FF 50 40          call        qword ptr[rax + 40h]		;CALL���ǽ��������� 40h/8h-1=7h ����Ա����
	PBYTE pSwapChainVT = *(PBYTE*)pSwapChain;							//�õ����ĵ�ַ
	PBYTE pPresent = *(PBYTE*)(pSwapChainVT + (7 + 1) * sizeof(void*));	//�õ�����7h����Ա�����ĵ�ַ

	//DrawIndexed()
	//ͨ������������麯��DrawIndexedʱ��
	//00007FF7F79C1E0F 48 8B 05 3A 45 01 00 mov         rax, qword ptr[pContext];rax��š������Ķ���ĵ�ַ��
	//00007FF7F79C1E16 48 8B 00             mov         rax, qword ptr[rax]		;rax��š������Ķ����һ����Ա��ֵ���������ĵ�ַ
	//00007FF7F79C1E19 45 33 C9             xor         r9d, r9d				;r9d ����4
	//00007FF7F79C1E1C 45 33 C0             xor         r8d, r8d				;r8d ����3
	//00007FF7F79C1E1F BA 24 00 00 00       mov         edx, 24h				;edx ����2 
	//00007FF7F79C1E24 48 8B 0D 25 45 01 00 mov         rcx, qword ptr[pContext];rcx ����1 ��š������Ķ���ĵ�ַ��
	//00007FF7F79C1E2B FF 50 60             call        qword ptr[rax + 60h]	;CALL���������Ķ�������60h/8h-1=Bh����Ա����
	PBYTE pContextVT = *(PBYTE*)pContext;									//�õ����ĵ�ַ
	PBYTE pDrawIndexed = *(PBYTE*)(pContextVT + (0xB + 1) * sizeof(void*));	//�õ�����Bh����Ա�����ĵ�ַ

	//CreateQuery()
	//	00007FF6228167C4 48 8B 45 08		  mov         rax, qword ptr[pDevice]
	//	00007FF6228167C8 48 8B 00             mov         rax, qword ptr[rax]
	//	00007FF6228167CB 45 33 C0             xor         r8d, r8d					//����3 r8
	//	00007FF6228167CE 33 D2                xor         edx, edx					//����2 rdx
	//	00007FF6228167D0 48 8B 4D 08          mov         rcx, qword ptr[pDevice]   //����1 rcx ����ָ��
	//	00007FF6228167D4 FF 90 C0 00 00 00    call        qword ptr[rax + 0C0h]
	PBYTE pDeviceVT = *(PBYTE*)pDevice;										//�õ����ĵ�ַ
	PBYTE pCreateQuery = *(PBYTE*)(pDeviceVT + (0x17 + 1) * sizeof(void*));	//�õ�����Ա�����ĵ�ַ


	//PSSetShaderResources()
	//	00007FF785312A71 48 8B 45 28          mov         rax, qword ptr[pContext]
	//	00007FF785312A75 48 8B 00             mov         rax, qword ptr[rax]
	//	00007FF785312A78 45 33 C9             xor         r9d, r9d					//����4 r9
	//	00007FF785312A7B 45 33 C0             xor         r8d, r8d					//����3 r8
	//	00007FF785312A7E 33 D2                xor         edx, edx					//����2 rdx
	//	00007FF785312A80 48 8B 4D 28          mov         rcx, qword ptr[pContext]	//����1 rcx ����ָ��
	//	00007FF785312A84 FF 50 40             call        qword ptr[rax + 40h]
	PBYTE pPSSetShaderResources = *(PBYTE*)((PBYTE)pContextVT + 0x8 * 0x8);


	Bonjour bonjour;
	//Inline Hook 
	//bonjour.InlineHook((PBYTE)pPresent, (PVOID*)& RealPresent, (PBYTE)MyPresent);
	//bonjour.InlineHook((PBYTE)pDrawIndexed, (PVOID*)& RealDrawIndexed, (PBYTE)MyDrawIndexed);
	//bonjour.InlineHook((PBYTE)pCreateQuery, (PVOID*)& RealCreateQuery, (PBYTE)MyCreateQuery);

	//VEH Hook
	bonjour.VEHHook((PBYTE)pPresent, (PVOID*)& RealPresent, (PBYTE)MyPresent, DRX::DR0, Ӳ������_��ִ��);
	bonjour.VEHHook((PBYTE)pDrawIndexed, (PVOID*)& RealDrawIndexed, (PBYTE)MyDrawIndexed, DRX::DR1, Ӳ������_��ִ��);
	bonjour.VEHHook((PBYTE)pCreateQuery, (PVOID*)& RealCreateQuery, (PBYTE)MyCreateQuery, DRX::DR2, Ӳ������_��ִ��);


	//����
	//pSwapChain->Present(0x666, 0x555);
	//pContext->DrawIndexed(2, 4, 6);
	//pDevice->CreateQuery(0, 0);

	return 0;
}