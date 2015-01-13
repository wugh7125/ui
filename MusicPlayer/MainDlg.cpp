/****************************************************************************
*  功    能：使用SOUI自主设计音乐播放器                                     *
*  作    者：小可                                                           *
*  添加时间：2014.01.09 17：00                                              *
*  版本类型：初始版本                                                       *
*  联系方式：QQ-1035144170                                                  *
****************************************************************************/

// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
    m_bCut=false;
	m_3DType=false;
	m_bLayoutInited=FALSE;

	m_pBassMusic=NULL;
	m_pMusicState=NULL;
} 

CMainDlg::~CMainDlg()
{
	shellNotifyIcon.Hide();
	if (hStream)
	{
		BASS_ChannelStop(hStream);
		hStream=NULL;

	}
	
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 		MARGINS mar = {5,5,30,5};
	// 		DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	shellNotifyIcon.Create(m_hWnd,GETRESPROVIDER->LoadIcon(_T("ICON_LOGO"),16));
	shellNotifyIcon.Show();
	//InitListCtrl();

	//拖拽功能
	SWindow *pListBox = FindChildByName(L"music_tree_box");
	if(pListBox)
	{
		HRESULT hr=::RegisterDragDrop(m_hWnd,GetDropTarget());
		RegisterDragDrop(pListBox->GetSwnd(),new CTestDropTarget1(pListBox));
	}

	//初始化声音组件
	m_pBassMusic = CBassMusicEngine::GetInstance();
	if ( m_pBassMusic == NULL )
	{
		if ( SMessageBox(NULL,TEXT("声音引擎初始化失败"),_T("警告"),MB_OK|MB_ICONEXCLAMATION) == IDOK )
		{
			PostQuitMessage(0);
			return TRUE;
		}
	}	
	m_pBassMusic->Init(m_hWnd,m_pMusicState);

    //测试：播放音乐
	OnButPlay();

	STreeBox *pTreeBox=FindChildByName2<STreeBox>(L"music_tree_box");

	if (pTreeBox)
	{
		//pTreeBox->SetCheck(_T("..."));
		HSTREEITEM item=pTreeBox->GetNextSiblingItem(pTreeBox->GetRootItem());
		pTreeBox->SetCheck(TRUE);
	}

	return 0;
}


//TODO:消息映射
void CMainDlg::OnClose()
{
	PostMessage(WM_QUIT);
	
}

void CMainDlg::OnMaximize()
{
	//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
	if (!m_bCut)
	{
		STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
		if(pTab)
		{
			pTab->SetCurSel(_T("在线音乐"));
			m_bCut=true;
		}
		
	}else
	{
		STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
		if(pTab)
		{
			pTab->SetCurSel(_T("我的音乐"));
			m_bCut=false;
		}
	}

}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

#include <helper/smenu.h>
LRESULT CMainDlg::OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL/* bHandled*/)
{
	switch (lParam)
	{
	case  WM_RBUTTONDOWN:
	{
	    SetForegroundWindow(m_hWnd);
            //显示右键菜单
            SMenu menu;
            menu.LoadMenu(_T("menu_tray"),_T("smenu"));
            POINT pt;
            GetCursorPos(&pt);
            menu.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
	}break;
	case WM_LBUTTONDOWN:
	{
		//显示/隐藏主界面
		if (IsIconic())
		   ShowWindow(SW_SHOWNORMAL);
		else
		   ShowWindow(SW_MINIMIZE);
	}break;
	default:
		break;
	}
	return S_OK;
}

//演示如何响应菜单事件
void CMainDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
	if (uNotifyCode == 0)
	{
		switch (nID)
		{
		case 6:
			PostMessage(WM_CLOSE);
			break;
		default:
			break;
		}
	}
}


BOOL CMainDlg::OnTurn3D( EventArgs *pEvt )
{
	EventTurn3d *pEvt3dTurn = (EventTurn3d*)pEvt;
	STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_3d");
	if(pEvt3dTurn->bTurn2Front_)
	{
		pTab->SetCurSel(_T("musiclist"));
		
	}else
	{
		pTab->SetCurSel(_T("lrc"));
	}
	return TRUE;
}

void CMainDlg::OnTurn3DBut()
{
	if (!m_3DType)
	{
		STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_3d");
		if(pTab)
		{
			STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
			if(pTurn3d)
			{
				pTurn3d->Turn(pTab->GetPage(_T("musiclist")),pTab->GetPage(_T("lrc")),FALSE);
				m_3DType=true;
			}
		}
	}else
	{
		STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_3d");
		if(pTab)
		{
			STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
			if(pTurn3d)
			{
				pTurn3d->Turn(pTab->GetPage(_T("lrc")),pTab->GetPage(_T("musiclist")),TRUE);
				m_3DType=false;
			}
		}

	}
	
}

void CMainDlg::OnBtnMyMusic()
{
	STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
	if(pTab)
	{
		pTab->SetCurSel(_T("我的音乐"));
	}

}

void CMainDlg::OnBtnOnlineMusic()
{
	STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
	if(pTab)
	{
		pTab->SetCurSel(_T("在线音乐"));
	}

}

void CMainDlg::OnBtnMyDevice()
{
	STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_main");
	if(pTab)
	{
		pTab->SetCurSel(_T("我的设备"));
	}

}

void CMainDlg::OnButPrev()
{
}

void CMainDlg::OnButPlay()
{
	if (hStream)
	{
		BASS_ChannelStop(hStream);
		hStream=NULL;
	}

	CString lpszFileName="testMP3/BEYOND - 不再犹豫.mp3";
	hStream=BASS_StreamCreateFile(FALSE, lpszFileName.GetBuffer(),0,0,BASS_SAMPLE_MONO);
	if (hStream)
	{
		//开始播放
		BOOL bResult = BASS_ChannelPlay(hStream, FALSE);
		if (bResult)
		{
			int i=0;
		}
	}

}

void CMainDlg::OnButPlayNext()
{
}

void CMainDlg::OnButLyric()
{

	STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_3d");
	if(pTab)
	{
		STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
		if(pTurn3d)
		{
			pTurn3d->Turn(pTab->GetPage(_T("musiclist")),pTab->GetPage(_T("lrc")),FALSE);
		}
	}
}

void CMainDlg::OnButMusicList()
{

	STabCtrl *pTab= FindChildByName2<STabCtrl>(L"tab_3d");
	if(pTab)
	{
		STurn3dView * pTurn3d = FindChildByName2<STurn3dView>(L"turn3d");
		if(pTurn3d)
		{
			pTurn3d->Turn(pTab->GetPage(_T("lrc")),pTab->GetPage(_T("musiclist")),TRUE);
		}
	}
}

void CMainDlg::OnFlywndState( EventArgs *pEvt )
{
    FlyStateEvent *pEvtFlywnd = (FlyStateEvent*)pEvt;
    if(pEvtFlywnd->nPercent == SAnimator::PER_END)
    {
        //测试：隐藏音乐频谱 (可以用定时器判断分层窗口的收缩状态来控制)

        SWindow *pSpectrum = FindChildByName2<SWindow>("spectrum");
        if(pSpectrum) pSpectrum->SetVisible(pEvtFlywnd->bEndPos,TRUE);        
//         if (!m_bCut)
//         {
// 
//             m_bCut=true;
// 
//         }else
//         {
//             SWindow *pSpectrum = FindChildByName2<SWindow>("spectrum");
//             if(pSpectrum) pSpectrum->SetVisible(FALSE,TRUE);
//             m_bCut=false;
//         }        
    }
}