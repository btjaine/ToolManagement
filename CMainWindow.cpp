#include "pch.h"
#include "CMainWindow.h"

#include "MsgWnd.h"


DUI_BEGIN_MESSAGE_MAP(CMainWindow, CNotifyPump)
		DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CMainWindow::CMainWindow()
{
}

CMainWindow::~CMainWindow()
{
	if (m_p_OpencvReader_Instance)
	{
		m_p_OpencvReader_Instance->stop();
		delete m_p_OpencvReader_Instance;
		m_p_OpencvReader_Instance = nullptr;
	}
}

LRESULT CMainWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 退出程序
	PostQuitMessage(0);
	return 0;
}

CDuiString CMainWindow::GetSkinFile()
{
	return _T("XML_MAIN");
}

LPCTSTR CMainWindow::GetWindowClassName() const
{
	return _T("MainWnd");
}

CControlUI* CMainWindow::CreateControl(LPCTSTR pstrClass)
{
	//这里注册是XML中可以认识到
	if (_tcscmp(pstrClass, _T("ComboEx")) == 0) return new CComboExUI;
	if (_tcscmp(pstrClass, _T("ListLabelElementEx")) == 0) return  new CListLabelElementExUI;
	if (_tcsicmp(pstrClass, _T("Wnd")) == 0)
	{
		auto pUI = new CWndUI;//40, 100, 620, 450
		const HWND    h_wnd = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, m_pm.GetPaintWindow(), NULL, NULL, NULL);
		pUI->Attach(h_wnd);
		::ShowWindow(h_wnd, SW_SHOW);
		return pUI;
	}
	if (_tcscmp(pstrClass, _T("ListEx_YIN")) == 0)  return new CListUIEx;
	return nullptr;
}

void CMainWindow::InitWindow()
{
	//主窗口视频
	m_p_main_window_video_capture_ = static_cast<CWndUI*>(m_pm.FindControl(_T("Main_Video_Capture")));
	//建立摄像头
	m_p_OpencvReader_Instance = new OpencvReader(0);
	m_p_OpencvReader_Instance->set_device_type(0);
	m_p_OpencvReader_Instance->set_show_window(m_p_main_window_video_capture_->m_hWnd);
	m_p_OpencvReader_Instance->start();
}

void CMainWindow::OnClick(TNotifyUI& msg)
{
	const CDuiString m_strCtrlname = msg.pSender->GetName();
	if (m_strCtrlname.CompareNoCase(_T("ExitSystem")) == 0)
	{
		if (MSGID_OK == CMsgWnd::MessageBox(m_hWnd, _T("Tool Borrowing"), _T("Are you sure  to exit？")))
		{
			::DestroyWindow(m_hWnd);
		}
		return;
	}
}
