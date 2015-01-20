// RichEditOle.cpp : implementation file
//
#include "stdafx.h"
#include "RichEditOle.h"


HRESULT GetSmileyHost(SRichEdit * pRichedit,ISmileyHost ** ppHost)
{
    SComPtr<IRichEditOle> ole;
    if(!pRichedit->SSendMessage(EM_GETOLEINTERFACE,0,(LPARAM)(void**)&ole))
    {
        return S_FALSE;
    }
    SComPtr<IRichEditOleCallback> pCallback;
    HRESULT hr = ole->QueryInterface(IID_IRichEditOleCallback,(LPVOID*)&pCallback);
    if(SUCCEEDED(hr))
    {
        hr = pCallback->QueryInterface(__uuidof(ISmileyHost),(LPVOID*)ppHost);
    }
    return hr;
}

//////////////////////////////////////////////////////////////////////////
//  ImageItem

using namespace Gdiplus;

float FitSize(CSize& InSize, CSize& ImageSize)
{
    float scaleX= (InSize.cx>0) ? (float)InSize.cx/ImageSize.cx : 0;
    float scaleY= (InSize.cy>0)? (float)InSize.cy/ImageSize.cy : 0;
    float scale=1;
    if (scaleX&&scaleY) 
        scale=min(scaleX, scaleY);
    else if ( scaleX||scaleY )
        scale = scaleX ? scaleX : scaleY;
    InSize.cx=(INT)(ImageSize.cx * scale);
    InSize.cy=(INT)(ImageSize.cy * scale);
    return scale;
}

BOOL GetBitmapFromFile(Gdiplus::Bitmap* &m_pBitmap, const SStringW& strFilename, 
                       int& m_nFrameCount, CSize& m_FrameSize, CSize& ImageSize,
                       UINT* &m_pFrameDelays )
{
    Gdiplus::Bitmap * tempBitmap= new Gdiplus::Bitmap((LPCWSTR)strFilename);    
    if (!tempBitmap) return FALSE;
    GUID   pageGuid = FrameDimensionTime;
    // Get the number of frames in the first dimension.
    m_nFrameCount = max(1, tempBitmap->GetFrameCount(&pageGuid));


    CSize imSize(tempBitmap->GetWidth(),tempBitmap->GetHeight());
    m_FrameSize=ImageSize;
    float scale=FitSize(m_FrameSize,imSize);

    m_pBitmap=new Gdiplus::Bitmap(m_FrameSize.cx*m_nFrameCount, m_FrameSize.cy, PixelFormat32bppARGB);

    Graphics *g=new Gdiplus::Graphics(m_pBitmap);
    ImageAttributes attr;
    if ( scale!=1 ) 
    {
        g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g->SetPixelOffsetMode(PixelOffsetModeHighQuality);            
        if ( scale<1 )
        {
            attr.SetGamma((REAL)1.2,ColorAdjustTypeBitmap); //some darker to made sharpen
        }
    }
    g->Clear(Color(0));
    if (m_nFrameCount>1)
    {
        m_pFrameDelays=new UINT[m_nFrameCount];
        int nSize = tempBitmap->GetPropertyItemSize(PropertyTagFrameDelay);
        // Allocate a buffer to receive the property item.
        PropertyItem* pDelays = (PropertyItem*) new char[nSize];
        tempBitmap->GetPropertyItem(PropertyTagFrameDelay, nSize, pDelays);
        for (int i=0; i<m_nFrameCount; i++)
        {
            GUID pageGuid = FrameDimensionTime;
            tempBitmap->SelectActiveFrame(&pageGuid, i);
            Rect rect(i*m_FrameSize.cx,0,m_FrameSize.cx, m_FrameSize.cy);
            if (scale>=1 )
                g->DrawImage(tempBitmap,rect,0,0,tempBitmap->GetWidth(),tempBitmap->GetHeight(), UnitPixel/*, &attr*/);
            else
            {
                Bitmap bm2(tempBitmap->GetWidth(),tempBitmap->GetHeight(), PixelFormat32bppARGB);
                Graphics g2(&bm2);
                g2.DrawImage(tempBitmap,Rect(0,0,bm2.GetWidth(),bm2.GetHeight()),0,0,tempBitmap->GetWidth(),tempBitmap->GetHeight(), UnitPixel);
                g->DrawImage(&bm2,rect,0,0,bm2.GetWidth(),bm2.GetHeight(), UnitPixel, &attr);
            }
            m_pFrameDelays[i]=10*max(((int*) pDelays->value)[i], 10);
        }   
        delete [] pDelays;
    }
    else
    {
        Rect rect(0,0,m_FrameSize.cx, m_FrameSize.cy);
        g->DrawImage(tempBitmap,rect,0,0,tempBitmap->GetWidth(),tempBitmap->GetHeight(), UnitPixel, &attr);
        m_pFrameDelays=NULL;
    }
    ImageSize=CSize(tempBitmap->GetWidth(),tempBitmap->GetHeight());
    delete g;
    delete tempBitmap;
    return TRUE;
}


ImageItem::ImageItem() : 
m_nRef( 0 ), 
m_pBitmap( NULL ),
m_nFrameCount( 0 ),
m_pFrameDelays( NULL )
{

}
ImageItem::~ImageItem()
{
    if ( m_pBitmap ) delete m_pBitmap;
    if ( m_pFrameDelays ) delete [] m_pFrameDelays;
}

BOOL ImageItem::LoadImageFromFile(const SStringW& strFilename, int nHeight)
{
    ATLASSERT(m_pBitmap == NULL);
    m_imgid.m_nHeight=nHeight;
    m_imgid.m_strFilename=strFilename;

    CSize ImageSize(0, nHeight);
    return GetBitmapFromFile(m_pBitmap, strFilename, m_nFrameCount, m_FrameSize, ImageSize, m_pFrameDelays );
}

int ImageItem::Release()
{
    int nRef = --m_nRef;
    if (nRef < 1)
    {
        delete this;
    }
    return nRef;
}

void ImageItem::Draw( HDC hdc,LPCRECT pRect,int iFrame )
{
    if(m_pBitmap)
    {
        Graphics g(hdc);
        RectF rcDst(pRect->left,pRect->top,pRect->right-pRect->left,pRect->bottom-pRect->top);
        g.DrawImage(m_pBitmap,rcDst,m_FrameSize.cx*iFrame,0,m_FrameSize.cx,m_FrameSize.cy,UnitPixel);
    }
}
/////////////////////////////////////////////////////////////////////////////
ULONG_PTR   CSmileySource::_gdiPlusToken    = 0;
CSmileySource::IMAGEPOOL CSmileySource::_imgPool;


bool CSmileySource::GdiplusStartup( void )
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;

    return Gdiplus::Ok == Gdiplus::GdiplusStartup(&_gdiPlusToken, &gdiplusStartupInput, NULL);
}

void CSmileySource::GdiplusShutdown( void )
{
    if (_gdiPlusToken != 0)
    {
        Gdiplus::GdiplusShutdown(_gdiPlusToken);
        _gdiPlusToken = 0;
    }
}

CSmileySource::CSmileySource():m_pImg(NULL),m_cRef(0)
{

}

CSmileySource::~CSmileySource()
{
    if(m_pImg)
    {
        ImageID id = m_pImg->GetImageID();
        if(m_pImg->Release()==0)
        {
            _imgPool.erase(id);
        }
    }
}


HRESULT CSmileySource::Stream_Load( /* [in] */ LPSTREAM pStm )
{
    int nFileLen=0;
    pStm->Read(&nFileLen,4,NULL);
    wchar_t *pszFileName=new wchar_t[nFileLen+1];
    pStm->Read(pszFileName,nFileLen*2,NULL);
    pszFileName[nFileLen]=0;
    int nHeight = 0;
    pStm->Read(&nHeight,4,NULL);

    HRESULT hr = Init((WPARAM)pszFileName,(LPARAM)nHeight);
    delete []pszFileName;
    return hr;
}

HRESULT CSmileySource::Stream_Save( /* [in] */ LPSTREAM pStm )
{
    if(!m_pImg) return E_FAIL;
    ImageID id = m_pImg->GetImageID();
    int nFileLen = id.m_strFilename.GetLength();
    pStm->Write(&nFileLen,4,NULL);
    pStm->Write((LPCWSTR)id.m_strFilename,nFileLen*2,NULL);
    pStm->Write(&id.m_nHeight,4,NULL);

    return S_OK;
}

HRESULT CSmileySource::Init( /* [in] */ WPARAM wParam, /* [in] */ LPARAM lParam )
{
    ImageID imgid;
    imgid.m_strFilename = (wchar_t*)wParam;
    imgid.m_nHeight = (int)lParam;
    
    if(m_pImg)
    {
        if(!m_pImg->IsEqual(imgid))
        {//������ͼ
            ImageID oldID = m_pImg->GetImageID();
            if(m_pImg->Release() ==0 )
                _imgPool.erase(oldID);
            m_pImg = NULL;
        }else
        {//��ͬ��ͼ��ֱ�ӷ���
            return S_OK;
        }
    }
    IMAGEPOOL::iterator it = _imgPool.find(imgid);
    if(it==_imgPool.end())
    {//��pool��û���ҵ�
        ImageItem *pImg = new ImageItem;
        if(!pImg->LoadImageFromFile(imgid.m_strFilename,imgid.m_nHeight))
        {
            delete pImg;
            return E_INVALIDARG;
        }
        _imgPool[imgid] = pImg;
        m_pImg = pImg;
    }else
    {
        m_pImg = it->second;
    }
    m_pImg->AddRef();
    return S_OK;
}

HRESULT CSmileySource::GetFrameCount( /* [out] */ int *pFrameCount )
{
    if(!m_pImg) return E_FAIL;
    *pFrameCount = m_pImg->GetFrameCount();
    return S_OK;
}

HRESULT CSmileySource::GetFrameDelay( /* [in] */ int iFrame, /* [out] */ int *pFrameDelay )
{
    if(!m_pImg) return E_FAIL;
    if(iFrame>=m_pImg->GetFrameCount()) return E_INVALIDARG;
    *pFrameDelay = m_pImg->GetFrameDelays()[iFrame];
    return S_OK;
}


HRESULT CSmileySource::GetSize( /* [out] */ LPSIZE pSize )
{
    if(!m_pImg) return E_FAIL;
    *pSize = m_pImg->GetFrameSize();
    return S_OK;
}

HRESULT CSmileySource::Draw( /* [in] */ HDC hdc, /* [in] */ LPCRECT pRect , int iFrameIndex)
{
    if(!m_pImg) return E_FAIL;
    m_pImg->Draw(hdc,pRect,iFrameIndex);
    return S_OK;
}

ISmileySource * CSmileySource::CreateInstance()
{
    ISmileySource *pRet = new CSmileySource;
    pRet->AddRef();
    return pRet;
}

//////////////////////////////////////////////////////////////////////////
//  CSmileyHost
CSmileyHost::CSmileyHost() :m_pHost(0),m_cRef(1)
{

}

CSmileyHost::~CSmileyHost()
{
    if(m_pHost) m_pHost->GetContainer()->UnregisterTimelineHandler(this);
}

void CSmileyHost::ClearTimer()
{
    TIMERHANDLER_LIST::iterator it = m_lstTimerInfo.begin();
    while(it != m_lstTimerInfo.end())
    {
        delete (*it);
        it++;
    }
    m_lstTimerInfo.clear();
}

HRESULT STDMETHODCALLTYPE CSmileyHost::SetTimer( /* [in] */ ITimerHandler * pTimerHander, /* [in] */ int nInterval )
{
    TIMERHANDLER_LIST::iterator it = m_lstTimerInfo.begin();
    while(it != m_lstTimerInfo.end())
    {
        if((*it)->pTimerHandler == pTimerHander) return S_FALSE;
        it++;
    }
    TIMERINFO * pti= new TIMERINFO;
    pti->pTimerHandler=pTimerHander;
    pti->nInterval=nInterval;
    pti->nPassTime=0;

    m_lstTimerInfo.push_back(pti);
    return S_OK;
}


HRESULT STDMETHODCALLTYPE  CSmileyHost::OnTimer( int nInterval )
{
    TIMERHANDLER_LIST lstDone;
    
    //�ҵ����е�ʱ��Ķ�ʱ��,��ֹ��ִ�ж�ʱ��ʱ�����¶�ʱ������Ҫ�Ȳ�����ִ�С�
    TIMERHANDLER_LIST::iterator it = m_lstTimerInfo.begin();
    while(it != m_lstTimerInfo.end())
    {
        TIMERHANDLER_LIST::iterator it2=it;
        it++;
        TIMERINFO *pTi = *it2;
        pTi->nPassTime += nInterval;
        if(pTi->nPassTime >= pTi->nInterval)
        {
            lstDone.push_back(pTi);
            m_lstTimerInfo.erase(it2);
        }
    }
    
    //ִ�ж�ʱ��
    it = lstDone.begin();
    while(it!=lstDone.end())
    {
        (*it)->pTimerHandler->OnTimer();
        delete (*it);
        it++;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::CreateSource( ISmileySource ** ppSource )
{
    *ppSource = CSmileySource::CreateInstance();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::InvalidateRect( /* [in] */ LPCRECT pRect )
{
    m_pHost->InvalidateRect(pRect);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::GetHostRect( /* [out] */ LPRECT prcHost )
{
    ::GetWindowRect(m_pHost->GetContainer()->GetHostHwnd(),prcHost);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::SendMessage( /* [in] */ UINT uMsg, /* [in] */ WPARAM wParam, /* [in] */ LPARAM lParam, /* [out] */ LRESULT *pRet )
{
    LRESULT lRet=m_pHost->SSendMessage(uMsg,wParam,lParam);
    if(pRet) *pRet = lRet;
    return S_OK;
}

ULONG STDMETHODCALLTYPE CSmileyHost::Release( void )
{
    LONG lRet = --m_cRef;
    if(lRet == 0)
    {
        delete this;
    }
    return lRet;
}

ULONG STDMETHODCALLTYPE CSmileyHost::AddRef( void )
{
    return ++m_cRef;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::QueryInterface( /* [in] */ REFIID riid, /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject )
{
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::SetRichedit(/* [in] */DWORD_PTR dwRichedit)
{
    SASSERT(!m_pHost);
    m_pHost = (SRichEdit *)dwRichedit;
    m_pHost->GetContainer()->RegisterTimelineHandler(this);
    return S_OK;
}



//////////////////////////////////////////////////////////////////////////
// CRichEditOleCallback

CRichEditOleCallback::CRichEditOleCallback():m_dwRef(1),m_iStorage(0)
{
    HRESULT hResult = ::StgCreateDocfile(NULL,
        STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
        0, &m_stg );

    if ( m_stg == NULL ||
        hResult != S_OK )
    {
//         AfxThrowOleException( hResult );
    }

    m_pSmileyHost = new CSmileyHost;
}

CRichEditOleCallback::~CRichEditOleCallback()
{
    m_pSmileyHost->ClearTimer();
    m_pSmileyHost->Release();
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetNewStorage(LPSTORAGE* ppStg)
{
    WCHAR tName[150];
    swprintf(tName, L"REStorage_%d", ++m_iStorage);


    HRESULT hr = m_stg->CreateStorage(tName, 
        STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
        0, 0, ppStg );    

    return hr;
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

    HRESULT hr = S_OK;
    *ppvObject = NULL;

    if ( iid == IID_IUnknown ||
        iid == IID_IRichEditOleCallback )
    {
        *ppvObject = this;
        AddRef();
    }else if( iid == __uuidof(ISmileyHost))
    {
        *ppvObject = m_pSmileyHost;
        m_pSmileyHost->AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}



ULONG STDMETHODCALLTYPE 
CRichEditOleCallback::AddRef()
{
    return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
CRichEditOleCallback::Release()
{
    if ( --m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
                                        LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
    return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
    return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
                                      DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
    return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
    return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
                                     HMENU FAR *lphmenu)
{
    return S_OK;
}

BOOL CRichEditOleCallback::SetRicheditOleCallback(SRichEdit *pRichedit)
{
    CRichEditOleCallback *pCallback = new CRichEditOleCallback;
    pCallback->m_pSmileyHost->SetRichedit((DWORD_PTR)pRichedit);
    BOOL bRet=pRichedit->SSendMessage(EM_SETOLECALLBACK,0,(LPARAM)pCallback);
    pCallback->Release();
    return bRet;
}