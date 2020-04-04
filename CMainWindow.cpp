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
	m_pUdpClient->WakeRecvEvent();
	//停止检测线程
	stop_face_server();
	if (m_p_OpencvReader_Instance)
	{
		m_p_OpencvReader_Instance->stop();
		delete m_p_OpencvReader_Instance;
		m_p_OpencvReader_Instance = nullptr;
	}
	//停止udp
	m_pUdpClient->stop();
	if (m_pUdpClient)
	{
		delete m_pUdpClient;
		m_pUdpClient = nullptr;
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
	//控件初始化
	m_pLabel_Check_Result = static_cast<CLabelUI*>(m_pm.FindControl(_T("Check_Result")));
	//UDP Client对象  g_cProgramPath
	string  m_str_setting_file_name = g_cProgramPath;
	m_str_setting_file_name += "setting.ini";
	char m_c_udp_ip[128] = { 0 };
	ZeroMemory(m_c_udp_ip, sizeof(m_c_udp_ip));
	GetPrivateProfileStringA("TOOL_UDP_SERVER", "ip", "127.0.0.1", m_c_udp_ip, sizeof(m_c_udp_ip), m_str_setting_file_name.c_str());
	//端口号
	int m_n_udp_port = GetPrivateProfileIntA("TOOL_UDP_SERVER", "port", 7890, m_str_setting_file_name.c_str());
	m_pUdpClient = new lib_uv_udp_client_face();
	m_pUdpClient->set_server_ip_port(m_c_udp_ip, m_n_udp_port);
	//开启udp
	m_pUdpClient->run();
	//主窗口视频
	m_p_main_window_video_capture_ = static_cast<CWndUI*>(m_pm.FindControl(_T("Main_Video_Capture")));
	//建立摄像头
	m_p_OpencvReader_Instance = new OpencvReader(0);
	m_p_OpencvReader_Instance->set_device_type(0);
	m_p_OpencvReader_Instance->set_show_window(m_p_main_window_video_capture_->m_hWnd);
	m_p_OpencvReader_Instance->start();
	
	//开启人脸检测线程
	m_handle_start_face_server = nullptr;
	start_face_server();
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

DWORD CMainWindow::TrueRun_face_Server()
{
	CDuiString   m_strText;
	char m_c_recv_text[1024];
	int m_n_recv_size = 0;
	//人脸识别类
	m_pLabel_Check_Result->SetText(L"Loading model...");
	Mat  m_srcMat;
	EM_ID_NAME   m_em_id_name;
	m_b_exit_face_server_flag = false;
	m_pFaceDatabase = nullptr;
	m_pFaceDatabase = new CFaceDatabase();
	//装入原来的人脸库
	m_pFaceDatabase->LoadFaceFromDatabase();
	string m_str_prot_text;//协议内容
	while (m_b_exit_face_server_flag==false)
	{
		m_pLabel_Check_Result->SetText(L"Recognizing...");
		//获取frame
		m_p_OpencvReader_Instance->get_frame(m_srcMat);
		//识别
		m_em_id_name._init();
		int m_n_result = m_pFaceDatabase->RecognizeMan(m_srcMat, &m_em_id_name);
		if (m_n_result == 0)
		{
			//识别成功->发结果给客户端
			m_strText.Format(L"Recognize:%S", m_em_id_name.m_c_em_id);
			m_pLabel_Check_Result->SetText(m_strText);
			LOG(INFO) << "Recognize:" << m_em_id_name.m_c_em_id;
		}
		else if (m_n_result==-4)
		{
			//没有匹配的人脸
			m_pLabel_Check_Result->SetText(L"can not find face");
			//保存人脸
			if (m_pFaceDatabase->ManFaceToJpg(m_srcMat) == true)
			{
				//发送信息到发枪端
				m_str_prot_text = "6,-1";
				m_pUdpClient->send_to_udp_server(m_str_prot_text.c_str(), m_str_prot_text.length());
				//检测一次看看是否收到发枪端发来的信息
				ZeroMemory(m_c_recv_text, sizeof(m_c_recv_text));
				m_n_recv_size = 0;//收到的字节数
				if (m_pUdpClient->GetRecvText(m_c_recv_text, &m_n_recv_size) > 0)
				{
					LOG(INFO) << "recv info:" << m_c_recv_text;
					//将信息加入到人脸数据库--解码
					if (m_pFaceDatabase->AddFaceInfoToDatabase("13952262397", "yinjian","mycut.jpg"))
					{
						LOG(INFO) << "add use" << "13952262397" << "-yinjian" << "success";
					}
					else
					{
						LOG(INFO) << "add use" << "13952262397" << "-yinjian" << "failed";
					}
				}
			}
		}
		waitKey(10);
	}
	//释放对象
	if (m_pFaceDatabase)
	{
		delete  m_pFaceDatabase;
		m_pFaceDatabase = nullptr;
	}
	return 0;
}

HANDLE  CMainWindow::start_face_server()
{
	m_handle_start_face_server = nullptr;
	m_handle_start_face_server = CreateThread(nullptr, 0, Run_face_Server, this, 0, nullptr);
	if (m_handle_start_face_server == nullptr || m_handle_start_face_server == INVALID_HANDLE_VALUE)
	{
		LOG(INFO) << "run start_face_server failed";
	}
	else
	{
		LOG(INFO) << "run start_face_server success";
	}
	return m_handle_start_face_server;
}

void  CMainWindow::stop_face_server()
{
	m_b_exit_face_server_flag = true;
	if (m_handle_start_face_server)
	{
		WaitForSingleObject(m_handle_start_face_server, INFINITE);
		CloseHandle(m_handle_start_face_server);
		m_handle_start_face_server = nullptr;
	}
}