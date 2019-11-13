#pragma once
#include <windows.h>
#include <iostream>

//需要在项目-右键-属性-配置属性-链接器-输入-附加依赖项-添加'd3d11.lib'，否则会提示‘无法解析的外部命令IDXGISwapChain’
#include "d3d11.h"		//D3D11  
#include "d3dcompiler.h"//D3D11

#include "ImportBonjour.h"//



//指向被挂钩函数被覆盖代码的函数指针
HRESULT(STDMETHODCALLTYPE* RealPresent)(IDXGISwapChain** self, UINT SyncInterval, UINT Flags);
void (STDMETHODCALLTYPE* RealDrawIndexed)(ID3D11DeviceContext** self, _In_  UINT IndexCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation);
HRESULT(STDMETHODCALLTYPE* RealCreateQuery)(ID3D11Device** self, _In_  const D3D11_QUERY_DESC* pQueryDesc, _COM_Outptr_opt_  ID3D11Query** ppQuery);
//钩子函数
HRESULT STDMETHODCALLTYPE MyPresent(IDXGISwapChain** self, UINT SyncInterval, UINT Flags)
{
	wprintf(L"进入MyPresent, 获取参数：this=%p,参数1=%x,参数2=%x \n", self, SyncInterval, Flags);
	return RealPresent(self, SyncInterval, Flags);
}
void STDMETHODCALLTYPE MyDrawIndexed(ID3D11DeviceContext** self, _In_  UINT IndexCount, _In_  UINT StartIndexLocation, _In_  INT BaseVertexLocation)
{
	wprintf(L"进入MyDrawIndexed, 获取参数：this=%p,参数1=%x,参数2=%x,参数3=%x \n", self, IndexCount, StartIndexLocation, BaseVertexLocation);
	return RealDrawIndexed(self, IndexCount, StartIndexLocation, BaseVertexLocation);
}
HRESULT STDMETHODCALLTYPE MyCreateQuery(ID3D11Device** self, _In_  const D3D11_QUERY_DESC* pQueryDesc, _COM_Outptr_opt_  ID3D11Query** ppQuery)
{
	wprintf(L"进入MyCreateQuery, 获取参数：this=%p,参数1=%x,参数2=%x\n", self, pQueryDesc, ppQuery);
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

	//自建个窗口用于创建交换链
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	//创建交换链，用于获取各虚表中各函数的地址而从Hook
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));   //填充0
	scd.BufferCount = 1;                              //我们只创建一个后缓冲（双缓冲）因此为1
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //设置颜色格式,我们使用RGBA
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;  //缩放比
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //扫描线
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //渲染目标输出
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //允许模式切换
	scd.OutputWindow = hWnd;  //在游戏窗体内部绘制另外一个窗口
	scd.SampleDesc.Count = 1;                      //1重采样
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //常用参数
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;  //是否全屏
	scd.BufferDesc.Width = 1920;
	scd.BufferDesc.Height = 1080;
	scd.BufferDesc.RefreshRate.Numerator = 144;     //刷新率
	scd.BufferDesc.RefreshRate.Denominator = 1;     //分母
	scd.SampleDesc.Quality = 0;                     //采样等级
	D3D_FEATURE_LEVEL featrueLevel = D3D_FEATURE_LEVEL_11_0;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featrueLevel, 1, D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext);

	//Present()
	//通过对象调用其虚函数Present时：
	//00007FF673951D5C 48 8B 45 48       mov         rax, qword ptr[pSwapChain] ;rax存放【交换链对象的地址】
	//00007FF673951D60 48 8B 00          mov         rax, qword ptr[rax]		;rax存放【交换链对象的第1个成员的值】，即虚表的地址
	//00007FF673951D63 45 33 C0          xor         r8d, r8d					;r8d 参数3
	//00007FF673951D66 33 D2             xor         edx, edx					;edx 参数2
	//00007FF673951D68 48 8B 4D 48       mov         rcx, qword ptr[pSwapChain]	;rcx 参数1 传递对象指针【交换链对象的地址】
	//00007FF673951D6C FF 50 40          call        qword ptr[rax + 40h]		;CALL的是交换链虚表第 40h/8h-1=7h 个成员函数
	PBYTE pSwapChainVT = *(PBYTE*)pSwapChain;							//得到虚表的地址
	PBYTE pPresent = *(PBYTE*)(pSwapChainVT + (7 + 1) * sizeof(void*));	//得到虚表第7h个成员函数的地址

	//DrawIndexed()
	//通过对象调用其虚函数DrawIndexed时：
	//00007FF7F79C1E0F 48 8B 05 3A 45 01 00 mov         rax, qword ptr[pContext];rax存放【上下文对象的地址】
	//00007FF7F79C1E16 48 8B 00             mov         rax, qword ptr[rax]		;rax存放【上下文对象第一个成员的值】，即虚表的地址
	//00007FF7F79C1E19 45 33 C9             xor         r9d, r9d				;r9d 参数4
	//00007FF7F79C1E1C 45 33 C0             xor         r8d, r8d				;r8d 参数3
	//00007FF7F79C1E1F BA 24 00 00 00       mov         edx, 24h				;edx 参数2 
	//00007FF7F79C1E24 48 8B 0D 25 45 01 00 mov         rcx, qword ptr[pContext];rcx 参数1 存放【上下文对象的地址】
	//00007FF7F79C1E2B FF 50 60             call        qword ptr[rax + 60h]	;CALL的是上下文对象虚表第60h/8h-1=Bh个成员函数
	PBYTE pContextVT = *(PBYTE*)pContext;									//得到虚表的地址
	PBYTE pDrawIndexed = *(PBYTE*)(pContextVT + (0xB + 1) * sizeof(void*));	//得到虚表第Bh个成员函数的地址

	//CreateQuery()
	//	00007FF6228167C4 48 8B 45 08		  mov         rax, qword ptr[pDevice]
	//	00007FF6228167C8 48 8B 00             mov         rax, qword ptr[rax]
	//	00007FF6228167CB 45 33 C0             xor         r8d, r8d					//参数3 r8
	//	00007FF6228167CE 33 D2                xor         edx, edx					//参数2 rdx
	//	00007FF6228167D0 48 8B 4D 08          mov         rcx, qword ptr[pDevice]   //参数1 rcx 对象指针
	//	00007FF6228167D4 FF 90 C0 00 00 00    call        qword ptr[rax + 0C0h]
	PBYTE pDeviceVT = *(PBYTE*)pDevice;										//得到虚表的地址
	PBYTE pCreateQuery = *(PBYTE*)(pDeviceVT + (0x17 + 1) * sizeof(void*));	//得到虚表成员函数的地址


	//PSSetShaderResources()
	//	00007FF785312A71 48 8B 45 28          mov         rax, qword ptr[pContext]
	//	00007FF785312A75 48 8B 00             mov         rax, qword ptr[rax]
	//	00007FF785312A78 45 33 C9             xor         r9d, r9d					//参数4 r9
	//	00007FF785312A7B 45 33 C0             xor         r8d, r8d					//参数3 r8
	//	00007FF785312A7E 33 D2                xor         edx, edx					//参数2 rdx
	//	00007FF785312A80 48 8B 4D 28          mov         rcx, qword ptr[pContext]	//参数1 rcx 对象指针
	//	00007FF785312A84 FF 50 40             call        qword ptr[rax + 40h]
	PBYTE pPSSetShaderResources = *(PBYTE*)((PBYTE)pContextVT + 0x8 * 0x8);


	Bonjour bonjour;
	//Inline Hook 
	//bonjour.InlineHook((PBYTE)pPresent, (PVOID*)& RealPresent, (PBYTE)MyPresent);
	//bonjour.InlineHook((PBYTE)pDrawIndexed, (PVOID*)& RealDrawIndexed, (PBYTE)MyDrawIndexed);
	//bonjour.InlineHook((PBYTE)pCreateQuery, (PVOID*)& RealCreateQuery, (PBYTE)MyCreateQuery);

	//VEH Hook
	bonjour.VEHHook((PBYTE)pPresent, (PVOID*)& RealPresent, (PBYTE)MyPresent, DRX::DR0, 硬断类型_仅执行);
	bonjour.VEHHook((PBYTE)pDrawIndexed, (PVOID*)& RealDrawIndexed, (PBYTE)MyDrawIndexed, DRX::DR1, 硬断类型_仅执行);
	bonjour.VEHHook((PBYTE)pCreateQuery, (PVOID*)& RealCreateQuery, (PBYTE)MyCreateQuery, DRX::DR2, 硬断类型_仅执行);


	//测试
	//pSwapChain->Present(0x666, 0x555);
	//pContext->DrawIndexed(2, 4, 6);
	//pDevice->CreateQuery(0, 0);

	return 0;
}