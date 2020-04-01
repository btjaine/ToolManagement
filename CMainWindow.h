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
	//退出窗体的Duilib函数重载
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//Duilib获取XML文件
	CDuiString GetSkinFile();
	//Duilib获取窗体类名字
	LPCTSTR GetWindowClassName() const;
	//Duilib创建自定义控件
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

public:
	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

private:
	CWndUI* m_p_main_window_video_capture_ = nullptr;//主窗口视频显示
	/**
	 * \brief 
	 */
	OpencvReader* m_p_OpencvReader_Instance = nullptr;
};

