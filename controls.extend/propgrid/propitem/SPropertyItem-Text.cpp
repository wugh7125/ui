#include "StdAfx.h"
#include "SPropertyItem-Text.h"
#include "../SPropertyEmbedWndHelper.hpp"
#include "../SPropertyGrid.h"

namespace SOUI
{
    class SPropEdit: public SEdit
                   , public IPropInplaceWnd
    {
    public:
        SPropEdit(IPropertyItem *pOwner):m_pOwner(pOwner)
        {
            SASSERT(m_pOwner);
        }
        
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            if(nChar==VK_RETURN)
            {
                GetParent()->SetFocus();
            }else
            {
                SEdit::OnKeyDown(nChar,nRepCnt,nFlags);            
            }
        }
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_KEYDOWN(OnKeyDown)
        SOUI_MSG_MAP_END()
        
        virtual IPropertyItem* GetOwner(){return m_pOwner;}
        
        virtual void UpdateData()
        {
            SStringT strValue=GetWindowText();
            m_pOwner->SetValue(&strValue);
        }

    protected:
        CAutoRefPtr<IPropertyItem> m_pOwner;

    };
    
    void SPropertyItemText::DrawItem( IRenderTarget *pRT,CRect rc )
    {
        pRT->DrawText(m_strValue,m_strValue.GetLength(),rc,DT_SINGLELINE|DT_VCENTER);
    }
    
    void SPropertyItemText::OnInplaceActive(bool bActive)
    {
        if(bActive)
        {
            SASSERT(!m_pEdit);
            m_pEdit = new TplPropEmbedWnd<SPropEdit>(this);
            pugi::xml_document xmlDoc;
            pugi::xml_node xmlNode=xmlDoc.append_child(L"root");
            xmlNode.append_attribute(L"colorBkgnd").set_value(L"#ffffff");
            m_pOwner->OnInplaceActiveWndCreate(this,m_pEdit,xmlNode);
            m_pEdit->SetWindowText(m_strValue);
        }else
        {
            if(m_pEdit)
            {
                m_pOwner->OnInplaceActiveWndDestroy(this,m_pEdit);
                m_pEdit->Release();
                m_pEdit = NULL;
            }
        }
    }

    void SPropertyItemText::SetValue( void *pValue,UINT uType/*=0*/ )
    {
        m_strValue = *(SStringT*)pValue;
    }
}
