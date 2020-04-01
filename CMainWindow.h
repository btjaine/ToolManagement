#pragma once

#pragma warning(disable: 4819)

#include "Utils/WinImplBase.h"
#include "UIListEx.h"

#include "ComboExUI.h"
#include "CWndUI.h"
#include "OpencvReader.h"

class CMainWindow :public WindowImplBase
{
public:
	CMainWindow();
	~CMainWindow();

public:
	//�˳������Duilib��������
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//Duilib��ȡXML�ļ�
	CDuiString GetSkinFile();
	//Duilib��ȡ����������
	LPCTSTR GetWindowClassName() const;
	//Duilib�����Զ���ؼ�
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

public:
	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

private:
	CWndUI* m_p_main_window_video_capture_ = nullptr;//��������Ƶ��ʾ
	/**
	 * \brief 
	 */
	OpencvReader* m_p_OpencvReader_Instance = nullptr;
};

