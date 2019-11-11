#pragma once
// TCopyHook.h
// TCopyHook类实现了ICopyHook接口，TClassFactory实现了IClassFactory接口
//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <shlobj.h>
//---------------------------------------------------------------------------
class TCopyHook : public ICopyHook
{
public:
	TCopyHook() :m_refcnt(0) {}
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP_(UINT) CopyCallback(HWND hwnd, UINT wFunc, UINT wFlags,
		LPCTSTR pszSrcFile, DWORD dwSrcAttribs,
		LPCTSTR pszDestFile, DWORD dwDestAttribs);
private:
	int m_refcnt;
};
//---------------------------------------------------------------------------
class TClassFactory : public IClassFactory
{
public:
	TClassFactory() :m_refcnt(0) {}
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);
private:
	int m_refcnt;
};