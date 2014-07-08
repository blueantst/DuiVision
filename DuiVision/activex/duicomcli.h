// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#ifndef __DUICOMCLI_H__
#define __DUICOMCLI_H__

#pragma once

#include <unknwn.h>
//#include "atldef.h"

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)

#ifndef ATLTRACENOTIMPL
#define ATLENSURE_THROW(a,b)
#endif

#ifndef ATLTRACENOTIMPL
#define ATLTRACENOTIMPL(funcname)   return E_NOTIMPL
#endif

#ifndef ATLASSERT
#define ATLASSERT DUIASSERT
#endif // ATLASSERT

#ifndef ATLASSUME
#define ATLASSUME(expr) do { ATLASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif // ATLASSUME

#ifndef ATL_NO_VTABLE
#define ATL_NO_VTABLE	__declspec(novtable)
#endif


#pragma warning (push)
#pragma warning (disable: 4127)  // conditional expression constant
#pragma warning (disable: 4571)  //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions


#pragma pack(push,8)

/////////////////////////////////////////////////////////////////////////////
// Smart Pointer helpers


inline IUnknown* DuiComPtrAssign(IUnknown** pp, IUnknown* lp)
{
	if (pp == NULL)
		return NULL;
		
	if (lp != NULL)
		lp->AddRef();
	if (*pp)
		(*pp)->Release();
	*pp = lp;
	return lp;
}

inline IUnknown* DuiComQIPtrAssign(IUnknown** pp, IUnknown* lp, REFIID riid)
{
	if (pp == NULL)
		return NULL;

	IUnknown* pTemp = *pp;
	*pp = NULL;
	if (lp != NULL)
		lp->QueryInterface(riid, (void**)pp);
	if (pTemp)
		pTemp->Release();
	return *pp;
}


/////////////////////////////////////////////////////////////////////////////
// COM Smart pointers

template <class T>
class _DuiNoAddRefReleaseOnCComPtr : public T
{
	private:
		STDMETHOD_(ULONG, AddRef)()=0;
		STDMETHOD_(ULONG, Release)()=0;
};

//CComPtrBase provides the basis for all other smart pointers
//The other smartpointers add their own constructors and operators
template <class T>
class CDuiComPtrBase
{
protected:
	CDuiComPtrBase() throw()
	{
		p = NULL;
	}
	CDuiComPtrBase( int nNull) throw()
	{
		DUIASSERT(nNull == 0);
		(void)nNull;
		p = NULL;
	}
	CDuiComPtrBase( T* lp) throw()
	{
		p = lp;
		if (p != NULL)
			p->AddRef();
	}
public:
	typedef T _PtrClass;
	~CDuiComPtrBase() throw()
	{
		if (p)
			p->Release();
	}
	operator T*() const throw()
	{
		return p;
	}
	T& operator*() const
	{
		DUIASSERT(p!=NULL);
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
		DUIASSERT(p==NULL);
		return &p;
	}
	_DuiNoAddRefReleaseOnCComPtr<T>* operator->() const throw()
	{
		ASSERT(p!=NULL);
		return (_DuiNoAddRefReleaseOnCComPtr<T>*)p;
	}
	bool operator!() const throw()
	{
		return (p == NULL);
	}
	bool operator<( T* pT) const throw()
	{
		return p < pT;
	}
	bool operator!=( T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==( T* pT) const throw()
	{
		return p == pT;
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}
	// Compare two objects for equivalence
	bool IsEqualObject( IUnknown* pOther) throw()
	{
		if (p == NULL && pOther == NULL)
			return true;	// They are both NULL objects

		if (p == NULL || pOther == NULL)
			return false;	// One is NULL the other is not

		CDuiComPtr<IUnknown> punk1;
		CDuiComPtr<IUnknown> punk2;
		p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
		pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
		return punk1 == punk2;
	}
	// Attach to an existing interface (does not AddRef)
	void Attach( T* p2) throw()
	{
		if (p)
			p->Release();
		p = p2;
	}
	// Detach the interface (does not Release)
	T* Detach() throw()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}
	 HRESULT CopyTo( T** ppT) throw()
	{
		DUIASSERT(ppT != NULL);
		if (ppT == NULL)
			return E_POINTER;
		*ppT = p;
		if (p)
			p->AddRef();
		return S_OK;
	}
	 HRESULT CoCreateInstance( REFCLSID rclsid,  LPUNKNOWN pUnkOuter = NULL,  DWORD dwClsContext = CLSCTX_ALL) throw()
	{
		DUIASSERT(p == NULL);
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
	}
	 HRESULT CoCreateInstance( LPCOLESTR szProgID,  LPUNKNOWN pUnkOuter = NULL,  DWORD dwClsContext = CLSCTX_ALL) throw()
	{
		CLSID clsid;
		HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
		DUIASSERT(p == NULL);
		if (SUCCEEDED(hr))
			hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
		return hr;
	}
	template <class Q>
	 HRESULT QueryInterface( Q** pp) const throw()
	{
		DUIASSERT(pp != NULL);
		return p->QueryInterface(__uuidof(Q), (void**)pp);
	}
	T* p;
};

template <class T>
class CDuiComPtr : public CDuiComPtrBase<T>
{
public:
	CDuiComPtr() throw()
	{
	}
	CDuiComPtr(int nNull) throw() :
		CDuiComPtrBase<T>(nNull)
	{
	}
	CDuiComPtr(T* lp) throw() :
		CDuiComPtrBase<T>(lp)

	{
	}
	CDuiComPtr( const CDuiComPtr<T>& lp) throw() :
		CDuiComPtrBase<T>(lp.p)
	{
	}
	T* operator=( T* lp) throw()
	{
        if(*this!=lp)
        {
    		return static_cast<T*>(DuiComPtrAssign((IUnknown**)&p, lp));
        }
        return *this;
	}
	template <typename Q>
	T* operator=( const CDuiComPtr<Q>& lp) throw()
	{
        if( !IsEqualObject(lp) )
        {
    		return static_cast<T*>(DuiComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T)));
        }
        return *this;
	}
	T* operator=( const CDuiComPtr<T>& lp) throw()
	{
        if(*this!=lp)
        {
    		return static_cast<T*>(DuiComPtrAssign((IUnknown**)&p, lp));
        }
        return *this;
	}
};

//specialization for IDispatch
template <>
class CDuiComPtr<IDispatch> : public CDuiComPtrBase<IDispatch>
{
public:
	CDuiComPtr() throw()
	{
	}
	CDuiComPtr(IDispatch* lp) throw() :
		CDuiComPtrBase<IDispatch>(lp)
	{
	}
	CDuiComPtr(const CDuiComPtr<IDispatch>& lp) throw() :
		CDuiComPtrBase<IDispatch>(lp.p)
	{
	}
	IDispatch* operator=(IDispatch* lp) throw()
	{
        if(*this!=lp)
        {
            return static_cast<IDispatch*>(DuiComPtrAssign((IUnknown**)&p, lp));
        }
        return *this;
	}
	IDispatch* operator=(const CDuiComPtr<IDispatch>& lp) throw()
	{
        if(*this!=lp)
        {
    		return static_cast<IDispatch*>(DuiComPtrAssign((IUnknown**)&p, lp.p));
        }
        return *this;
	}

// IDispatch specific stuff
	 HRESULT GetPropertyByName( LPCOLESTR lpsz,  VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar);
		DISPID dwDispID;
		HRESULT hr = GetIDOfName(lpsz, &dwDispID);
		if (SUCCEEDED(hr))
			hr = GetProperty(dwDispID, pVar);
		return hr;
	}
	 HRESULT GetProperty( DISPID dwDispID,  VARIANT* pVar) throw()
	{
		return GetProperty(p, dwDispID, pVar);
	}
	 HRESULT PutPropertyByName( LPCOLESTR lpsz,  VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar);
		DISPID dwDispID;
		HRESULT hr = GetIDOfName(lpsz, &dwDispID);
		if (SUCCEEDED(hr))
			hr = PutProperty(dwDispID, pVar);
		return hr;
	}
	 HRESULT PutProperty( DISPID dwDispID,  VARIANT* pVar) throw()
	{
		return PutProperty(p, dwDispID, pVar);
	}
	 HRESULT GetIDOfName( LPCOLESTR lpsz,  DISPID* pdispid) throw()
	{
		return p->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&lpsz), 1, LOCALE_USER_DEFAULT, pdispid);
	}
	// Invoke a method by DISPID with no parameters
	 HRESULT Invoke0( DISPID dispid,  VARIANT* pvarRet = NULL) throw()
	{
		DISPPARAMS dispparams = { NULL, NULL, 0, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}
	// Invoke a method by name with no parameters
	 HRESULT Invoke0( LPCOLESTR lpszName,  VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = Invoke0(dispid, pvarRet);
		return hr;
	}
	// Invoke a method by DISPID with a single parameter
	 HRESULT Invoke1( DISPID dispid, VARIANT* pvarParam1,  VARIANT* pvarRet = NULL) throw()
	{
		DISPPARAMS dispparams = { pvarParam1, NULL, 1, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}
	// Invoke a method by name with a single parameter
	 HRESULT Invoke1( LPCOLESTR lpszName, VARIANT* pvarParam1,  VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = Invoke1(dispid, pvarParam1, pvarRet);
		return hr;
	}
	// Invoke a method by DISPID with two parameters
	 HRESULT Invoke2( DISPID dispid,  VARIANT* pvarParam1,  VARIANT* pvarParam2,  VARIANT* pvarRet = NULL) throw();
	// Invoke a method by name with two parameters
	 HRESULT Invoke2( LPCOLESTR lpszName,  VARIANT* pvarParam1,  VARIANT* pvarParam2,  VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = Invoke2(dispid, pvarParam1, pvarParam2, pvarRet);
		return hr;
	}
	// Invoke a method by DISPID with N parameters
	 HRESULT InvokeN(DISPID dispid, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL) throw()
	{
		DISPPARAMS dispparams = { pvarParams, NULL, nParams, 0};
		return p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}
	// Invoke a method by name with Nparameters
	 HRESULT InvokeN(LPCOLESTR lpszName, VARIANT* pvarParams, int nParams, VARIANT* pvarRet = NULL) throw()
	{
		HRESULT hr;
		DISPID dispid;
		hr = GetIDOfName(lpszName, &dispid);
		if (SUCCEEDED(hr))
			hr = InvokeN(dispid, pvarParams, nParams, pvarRet);
		return hr;
	}
	 static HRESULT PutProperty( IDispatch* p,  DISPID dwDispID,  VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar != NULL);
		if (pVar == NULL)
			return E_POINTER;
		
		if(p == NULL)
			return E_INVALIDARG;
		
		DISPPARAMS dispparams = {NULL, NULL, 1, 1};
		dispparams.rgvarg = pVar;
		DISPID dispidPut = DISPID_PROPERTYPUT;
		dispparams.rgdispidNamedArgs = &dispidPut;

		if (pVar->vt == VT_UNKNOWN || pVar->vt == VT_DISPATCH || 
			(pVar->vt & VT_ARRAY) || (pVar->vt & VT_BYREF))
		{
			HRESULT hr = p->Invoke(dwDispID, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
				&dispparams, NULL, NULL, NULL);
			if (SUCCEEDED(hr))
				return hr;
		}
		return p->Invoke(dwDispID, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
				&dispparams, NULL, NULL, NULL);
	}
	 static HRESULT GetProperty( IDispatch* p,  DISPID dwDispID,  VARIANT* pVar) throw()
	{
		ASSERT(p);
		ASSERT(pVar != NULL);
		if (pVar == NULL)
			return E_POINTER;
		
		if(p == NULL)
			return E_INVALIDARG;
			
		DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
		return p->Invoke(dwDispID, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
				&dispparamsNoArgs, pVar, NULL, NULL);
	}
};

template <class T, const IID* piid = &__uuidof(T)>
class CDuiComQIPtr : public CDuiComPtr<T>
{
public:
	CDuiComQIPtr() throw()
	{
	}
	CDuiComQIPtr( T* lp) throw() :
		CDuiComPtr<T>(lp)
	{
	}
	CDuiComQIPtr( const CDuiComQIPtr<T,piid>& lp) throw() :
		CDuiComPtr<T>(lp.p)
	{
	}
	CDuiComQIPtr( IUnknown* lp) throw()
	{
		if (lp != NULL)
			lp->QueryInterface(*piid, (void **)&p);
	}
	T* operator=( T* lp) throw()
	{
        if(*this!=lp)
        {
		    return static_cast<T*>(DuiComPtrAssign((IUnknown**)&p, lp));
        }
        return *this;
	}
	T* operator=( const CDuiComQIPtr<T,piid>& lp) throw()
	{
        if(*this!=lp)
        {
    		return static_cast<T*>(DuiComPtrAssign((IUnknown**)&p, lp.p));
        }
        return *this;
	}
	T* operator=( IUnknown* lp) throw()
	{
        if(*this!=lp)
        {
    		return static_cast<T*>(DuiComQIPtrAssign((IUnknown**)&p, lp, *piid));
        }
        return *this;
	}
};

//Specialization to make it work
template<>
class CDuiComQIPtr<IUnknown, &IID_IUnknown> : public CDuiComPtr<IUnknown>
{
public:
	CDuiComQIPtr() throw()
	{
	}
	CDuiComQIPtr( IUnknown* lp) throw()
	{
		//Actually do a QI to get identity
		if (lp != NULL)
			lp->QueryInterface(__uuidof(IUnknown), (void **)&p);
	}
	CDuiComQIPtr( const CDuiComQIPtr<IUnknown,&IID_IUnknown>& lp) throw() :
		CDuiComPtr<IUnknown>(lp.p)
	{
	}
	IUnknown* operator=( IUnknown* lp) throw()
	{
        if(*this!=lp)
        {
		    //Actually do a QI to get identity
		    return DuiComQIPtrAssign((IUnknown**)&p, lp, __uuidof(IUnknown));
        }
        return *this;
	}

	IUnknown* operator=( const CDuiComQIPtr<IUnknown,&IID_IUnknown>& lp) throw()
	{
        if(*this!=lp)
        {
    		return DuiComPtrAssign((IUnknown**)&p, lp.p);
        }
        return *this;
	}
};

typedef CDuiComQIPtr<IDispatch, &__uuidof(IDispatch)> CComDispatchDriver;


#pragma pack(pop)

#pragma warning (pop)	


#endif	// __DUICOMCLI_H__
