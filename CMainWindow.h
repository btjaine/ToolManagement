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
	//�˳������Duilib��������
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//Duilib��ȡXML�ļ�
	CDuiString GetSkinFile();
	//Duilib��ȡ����������
	LPCTSTR GetWindowClassName() const;
	//Duilib�����Զ���ؼ�
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	

private:
	CLabelUI* m_pLabel_Check_Result;//��֤�����ҵ�������
public:
	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

private:
	CWndUI* m_p_main_window_video_capture_ = nullptr;//��������Ƶ��ʾ
	//��ȡ��Ƶ����
	OpencvReader* m_p_OpencvReader_Instance = nullptr;
	//����ʶ����
	CFaceDatabase* m_pFaceDatabase = nullptr;
	//UDP�ͻ���
	lib_uv_udp_client_face* m_pUdpClient = nullptr;
	//�����������ķ���
	HANDLE   m_handle_start_face_server = nullptr;
	//�˳���������ֳɵ�ѭ��
	bool m_b_exit_face_server_flag = false;
public:
	static DWORD __stdcall Run_face_Server(void* pInstance)
	{
		//������������
		CMainWindow* p = (CMainWindow*)pInstance;
		p->TrueRun_face_Server();
		return 0;
	};
	DWORD TrueRun_face_Server();
	HANDLE start_face_server();
	void stop_face_server();
};

