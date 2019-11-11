#pragma once
// TCopyHook.h
// TCopyHook��ʵ����ICopyHook�ӿڣ�TClassFactoryʵ����IClassFactory�ӿ�
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