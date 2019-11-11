
// TCopyHook.cpp
// TCopyHook对象和TClassFactory对象的实现文件
#include <stdio.h>
#include "TFactory.h"
//---------------------------------------------------------------------------
extern LONG nLocks;// 对象计数，用于DllCanUnloadNow
ULONG __stdcall TCopyHook::AddRef()
{
	if (m_refcnt == 0)
		nLocks++;
	m_refcnt++;
	return m_refcnt;
}
//---------------------------------------------------------------------------
ULONG __stdcall TCopyHook::Release()
{
	int nNewCnt = --m_refcnt;
	if (nNewCnt <= 0)
	{
		nLocks--;
		delete this;
	}
	return nNewCnt;
}
//---------------------------------------------------------------------------
HRESULT __stdcall TCopyHook::QueryInterface(REFIID dwIID, void **ppvObject)
{
	if (dwIID == IID_IUnknown)
		*ppvObject = static_cast<IUnknown*>(this);
	else
		if (dwIID == IID_IShellCopyHook)
			*ppvObject = static_cast<ICopyHook*>(this);
		else
			return E_NOINTERFACE;
	reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
	return S_OK;
}
//---------------------------------------------------------------------------
// 这就是CopyCallback方法，拷贝钩子的所有功能由它实现。参数的具体值参看MSDN
UINT __stdcall TCopyHook::CopyCallback(HWND hwnd, UINT wFunc, UINT wFlags,
	LPCTSTR pszSrcFile, DWORD dwSrcAttribs,
	LPCTSTR pszDestFile, DWORD dwDestAttribs)
{
	char szMessage[MAX_PATH + 14];
	sprintf(szMessage, "对%s进行的操作，是否继续？", pszSrcFile);
	return MessageBoxA(NULL, szMessage, "确认", MB_YESNO | MB_ICONEXCLAMATION);
}
//---------------------------------------------------------------------------
ULONG __stdcall TClassFactory::AddRef()
{
	if (m_refcnt == 0)
		nLocks++;
	m_refcnt++;
	return m_refcnt;
}
//---------------------------------------------------------------------------
ULONG __stdcall TClassFactory::Release()
{
	int nNewCnt = --m_refcnt;
	if (nNewCnt <= 0)
	{
		nLocks--;
		delete this;
	}
	return nNewCnt;
}
//---------------------------------------------------------------------------
HRESULT __stdcall TClassFactory::QueryInterface(REFIID dwIID, void **ppvObject)
{
	if (dwIID == IID_IUnknown)
		*ppvObject = static_cast<IUnknown*>(this);
	else
		if (dwIID == IID_IClassFactory)
			*ppvObject = static_cast<IClassFactory*>(this);
		else
			return E_NOINTERFACE;
	reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT __stdcall TClassFactory::CreateInstance(IUnknown* pUnkownOuter,
	REFIID riid, void** ppvObj)
{
	if (pUnkownOuter != NULL)
		return CLASS_E_NOAGGREGATION;
	TCopyHook *pObj = new TCopyHook;
	pObj->AddRef();
	HRESULT hr = pObj->QueryInterface(riid, ppvObj);
	pObj->Release();
	return hr;
}
//---------------------------------------------------------------------------
HRESULT __stdcall TClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		nLocks++;
	else
		nLocks--;
	return S_OK;
}