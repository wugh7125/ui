//////////////////////////////////////////////////////////////////////////
//  Class Name: SItemPanel
// Description: A Framework wrapping frame to be used in a swindow.
//     Creator: Huang Jianxiong
//     Version: 2011.10.20 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma  once

#include "SwndContainerImpl.h"

namespace SOUI
{

class SItemPanel;

struct IItemContainer
{
    virtual void OnItemSetCapture(SItemPanel *pItem,BOOL bCapture)=0; //设置or释放鼠标捕获
    virtual BOOL OnItemGetRect(SItemPanel *pItem,CRect &rcItem)=0;    //获得表项的显示位置
    virtual BOOL IsItemRedrawDelay()=0;                               //指示表项的更新方式
    virtual void OnItemRequestRelayout(SItemPanel *pItem)=0;          //列表项请求重新布局
};

class SOUI_EXP SItemPanel : public SwndContainerImpl
{
public:
    static SItemPanel * Create(SWindow *pFrameHost,pugi::xml_node xmlNode,IItemContainer *pItemContainer);
    
protected:
    SItemPanel(SWindow *pFrameHost,pugi::xml_node xmlNode,IItemContainer *pItemContainer);
    virtual ~SItemPanel() {}

    virtual void OnFinalRelease();

public://SwndContainerImpl
    virtual LRESULT DoFrameEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

    virtual BOOL OnFireEvent(EventArgs &evt);

    virtual CRect GetContainerRect();

    virtual IRenderTarget * OnGetRenderTarget(const CRect & rc ,DWORD gdcFlags);

    virtual void OnReleaseRenderTarget(IRenderTarget *pRT,const CRect &rc,DWORD gdcFlags);

    virtual void OnRedraw(const CRect &rc);

    virtual BOOL OnReleaseSwndCapture();

    virtual SWND OnSetSwndCapture(SWND swnd);
    virtual HWND GetHostHwnd();
    virtual const SStringW & GetTranslatorContext();
    virtual void FrameToHost(RECT & rc);
    virtual BOOL IsTranslucent() const;
    virtual BOOL IsSendWheel2Hover() const;
    
    virtual BOOL OnCreateCaret(SWND swnd,HBITMAP hBmp,int nWidth,int nHeight);

    virtual BOOL OnShowCaret(BOOL bShow);

    virtual BOOL OnSetCaretPos(int x,int y);

    virtual BOOL UpdateWindow();

    virtual BOOL RegisterTimelineHandler(ITimelineHandler *pHandler);

    virtual BOOL UnregisterTimelineHandler(ITimelineHandler *pHandler);

    virtual SMessageLoop *GetMsgLoop();
    
    virtual IScriptModule * GetScriptModule();

public://SWindow
    virtual void ModifyItemState(DWORD dwStateAdd, DWORD dwStateRemove);

    virtual SWND SwndFromPoint(POINT ptHitTest, BOOL bOnlyText);

    virtual void Draw(IRenderTarget *pRT,const CRect & rc);

    virtual void SetSkin(ISkinObj *pSkin);
    virtual void SetColor(COLORREF crBk,COLORREF crSelBk);
    virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);

    virtual BOOL NeedRedrawWhenStateChange();
    virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);
    virtual BOOL IsLayeredWindow() const {return FALSE;}
    virtual void RequestRelayout(SWindow *pSource =NULL);
    
    CRect GetItemRect();
    void SetItemCapture(BOOL bCapture);
    void SetItemData(LPARAM dwData);
    LPARAM GetItemData();

    LPARAM GetItemIndex(){return m_lpItemIndex;}
    void SetItemIndex(LPARAM lp){m_lpItemIndex=lp;}

protected:
    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"colorNormal",m_crBk,FALSE)
        ATTR_COLOR(L"colorSelected",m_crSelBk,FALSE)
        ATTR_COLOR(L"colorHover",m_crHover,FALSE)
    SOUI_ATTRS_END()
protected:
    SWindow * m_pFrmHost;
    IItemContainer * m_pItemContainer;
    COLORREF m_crBk, m_crSelBk,m_crHover;
    LPARAM        m_dwData;
    LPARAM        m_lpItemIndex;
};


}//namespace SOUI