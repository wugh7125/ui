#ifndef _DUIRESPROVIDERBASE_
#define _DUIRESPROVIDERBASE_
#pragma once

#include "DuiImage-i.h"

#define DUIRES_XML_TYPE _T("XML")
#define DUIRES_IMGX_TYPE _T("IMGX")
#define DUIRES_BMP_TYPE _T("BMP")
#define DUIRES_ICON_TYPE _T("ICO")

#define INDEX_XML	_T("index.xml")		//�ļ�����Դ���ļ�ӳ����������ļ���

#define MAX_RES_TYPE		10			//��Դ����Ԥ���壬��󳤶Ȳ�����10
#define MAX_RES_NAME		100			//ע�⣺����Դ����ʱ����󳤶Ȳ�Ҫ����MAX_RES_NAME

class SOUI_EXP DuiResID
{
public:
	DuiResID(LPCTSTR pszType,LPCTSTR pszName)
	{
		memset(this,0,sizeof(DuiResID));
		if(pszType) _tcscpy_s(szType,MAX_RES_TYPE,pszType);
		if(pszName) _tcscpy_s(szName,MAX_RES_NAME,pszName);
		_tcslwr(szType);
		_tcslwr(szName);
	}

	TCHAR szType[MAX_RES_TYPE+1];
	TCHAR szName[MAX_RES_NAME+1];
};


template<>
class _COLL_NS::CElementTraits< DuiResID > :
	public _COLL_NS::CElementTraitsBase< DuiResID >
{
public:
	static ULONG Hash( INARGTYPE resid )
	{
		ULONG_PTR uRet=0;
		
		for(LPCTSTR p=resid.szType; *p; p++)
		{
			uRet=uRet*68+*p;
		}
		uRet*=10000;
		for(LPCTSTR p=resid.szName; *p; p++)
		{
			uRet=uRet*68+*p;
		}
		return (ULONG)uRet;
	}

	static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
	{
		return _tcscmp(element1.szType,element2.szType)==0
		    && _tcscmp(element1.szName,element2.szName)==0;
	}

	static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
	{
		int nRet=_tcscmp(element1.szType,element2.szType);
		if(nRet==0) nRet=_tcscmp(element1.szName,element2.szName);
		return nRet;
	}
};

namespace SOUI
{


class SOUI_EXP IDuiResProvider
{
public:
	virtual ~IDuiResProvider(){}
    virtual BOOL HasResource(LPCTSTR strType,LPCTSTR pszResName)=NULL;
    virtual HICON   LoadIcon(LPCTSTR strType,LPCTSTR pszResName,int cx=0,int cy=0)=NULL;
    virtual HBITMAP	LoadBitmap(LPCTSTR strType,LPCTSTR pszResName)=NULL;
    virtual IDuiImage * LoadImage(LPCTSTR strType,LPCTSTR pszResName)=NULL;
    virtual size_t GetRawBufferSize(LPCTSTR strType,LPCTSTR pszResName)=NULL;
    virtual BOOL GetRawBuffer(LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size)=NULL;
};
}//namespace SOUI
#endif//_DUIRESPROVIDERBASE_