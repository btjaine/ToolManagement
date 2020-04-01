// ToolManagement.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP
#include "framework.h"
#include "ToolManagement.h"
#include  "CMainWindow.h"


//can not repeat program
HANDLE hMutex=nullptr;


void InitResource()
{
	// 资源类型
	#ifdef _DEBUG
	CPaintManagerUI::SetResourceType(UILIB_FILE);
	#else
	CPaintManagerUI::SetResourceType(UILIB_FILE);
	//CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
	#endif
		// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch (CPaintManagerUI::GetResourceType())
	{
		case UILIB_FILE:
		{
			strResourcePath += _T("skin/");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
		case UILIB_RESOURCE:
		{
			strResourcePath += _T("skin/");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
		case UILIB_ZIP:
		{
			strResourcePath += _T("ski/");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加密
			CPaintManagerUI::SetResourceZip(_T("duidemo_pwd.zip"), true, _T("duilib_ultimate"));
			//CPaintManagerUI::SetResourceZip(_T("duidemo.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
		case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("skin/");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if (hResource != NULL)
			{
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if (hGlobal != NULL)
				{
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if (dwSize > 0)
					{
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	hMutex = nullptr;
	hMutex = CreateMutex(nullptr, false, _T("QQ:1427208421 BetterJasonVistor"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (hMutex)
		{
			CloseHandle(hMutex);
			MessageBox(nullptr, L"Program running!", L"提示", MB_OK);
			return 0;
		}
	}
	//初始化olecom
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;
	// OLE
	HRESULT hRes = ::OleInitialize(NULL);
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);
	// 初始化资源
	InitResource();
	//建立窗口
	auto pMainWnd = new CMainWindow();
	if (pMainWnd == nullptr) return 0;
	pMainWnd->Create(NULL, _T("Better vistor"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 0, 0);
	pMainWnd->CenterWindow();
	// 消息循环
	CPaintManagerUI::MessageLoop();
	// 销毁窗口
	delete pMainWnd;
	pMainWnd = nullptr;
	// 清理资源
	CPaintManagerUI::Term();
	// OLE
	OleUninitialize();
	// COM
	::CoUninitialize();

	return 0;
}