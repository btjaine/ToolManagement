#pragma once

#pragma warning(disable: 4819)

#include "Utils/WinImplBase.h"
#include "UIListEx.h"

#include "ComboExUI.h"
#include "CWndUI.h"
#include "OpencvReader.h"
#include "CFaceDatabase.h"
#include "lib_uv_udp_client_face.h"

extern  char g_cProgramPath[1024];

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
	

private:
	CLabelUI* m_pLabel_Check_Result;//认证结果和业务处理过程
public:
	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

private:
	CWndUI* m_p_main_window_video_capture_ = nullptr;//主窗口视频显示
	//读取视频的类
	OpencvReader* m_p_OpencvReader_Instance = nullptr;
	//人脸识别类
	CFaceDatabase* m_pFaceDatabase = nullptr;
	//UDP客户端
	lib_uv_udp_client_face* m_pUdpClient = nullptr;
	//启动人脸检测的服务
	HANDLE   m_handle_start_face_server = nullptr;
	//退出人脸检测现成的循环
	bool m_b_exit_face_server_flag = false;
public:
	static DWORD __stdcall Run_face_Server(void* pInstance)
	{
		//开启人脸服务
		CMainWindow* p = (CMainWindow*)pInstance;
		p->TrueRun_face_Server();
		return 0;
	};
	DWORD TrueRun_face_Server();
	HANDLE start_face_server();
	void stop_face_server();
};

