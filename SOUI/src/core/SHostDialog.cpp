#include "souistd.h"
#include "core/SHostDialog.h"
#include "core/SMsgLoop.h"

namespace SOUI
{
    SHostDialog::SHostDialog(LPCTSTR pszXmlName):SHostWnd(pszXmlName),m_nRetCode(IDOK)
    {
    }

    SHostDialog::~SHostDialog(void)
    {
    }

    INT_PTR SHostDialog::DoModal(HWND hParent/*=NULL*/)
    {
        if(!hParent)
        {
            hParent = ::GetActiveWindow();
        }
               
        BOOL bEnableParent = FALSE;
        if (hParent && hParent != ::GetDesktopWindow() && ::IsWindowEnabled(hParent))
        {
            ::EnableWindow(hParent, FALSE);
            bEnableParent = TRUE;
        }
        
        if(!Create(hParent,WS_POPUP | WS_CLIPCHILDREN,0, 0,0,0,0))
            return 0;

        CSimpleWnd::SendMessage(WM_INITDIALOG, (WPARAM)m_hWnd);

        HWND hWndLastActive = ::SetActiveWindow(m_hWnd);
        
        if(GetExStyle()&WS_EX_TOOLWINDOW)
            ::ShowWindow(m_hWnd,SW_SHOWNOACTIVATE);
        else
            ::ShowWindow(m_hWnd,SW_SHOWNORMAL);
        

        Run();

        // From MFC
        // hide the window before enabling the parent, etc.

        if ( IsWindow() )
        {
            CSimpleWnd::SetWindowPos(
                NULL, 0, 0, 0, 0,
                SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
        }

        if (bEnableParent)
        {
            ::EnableWindow(hParent, TRUE);
        }

        ::SetActiveWindow(hWndLastActive);

        if ( IsWindow() )
            CSimpleWnd::DestroyWindow();
       
        return m_nRetCode;
    }

    void SHostDialog::EndDialog( INT_PTR nResult )
    {
        m_nRetCode = nResult;
        PostMessage(WM_QUIT);
    }

    void SHostDialog::OnOK()
    {
        EndDialog(IDOK);
    }

    void SHostDialog::OnCancel()
    {
        EndDialog(IDCANCEL);
    }

    void SHostDialog::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
    {
        SHostWnd::OnKeyEvent(WM_KEYDOWN,(WPARAM)nChar,MAKELPARAM(nRepCnt,nFlags));
        if(SHostWnd::IsMsgHandled()) return;
        if(nChar == VK_ESCAPE || nChar == VK_RETURN)
        {
            SWindow *pBtnExit = FindChildByID(nChar==VK_ESCAPE?IDCANCEL:IDOK);
            if(pBtnExit)
            {
                pBtnExit->FireCommand();
            }
        }
    }
}
