#pragma once
#pragma warning(disable: 4819)

//ColeDateTime用头文件
#include <ATLComTime.h>
//DuiLib用头文件---必须先在项目设置中设置包含目录
#include "UIlib.h"
using namespace DuiLib;
//DuiLib用Lib文件
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_d.lib")
#   else
#       pragma comment(lib, "DuiLibA_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib.lib")
#   else
#       pragma comment(lib, "DuiLibA.lib")
#   endif
#endif
//线程函数指针
typedef unsigned(__stdcall* PTHREAD_START) (void*);
#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadID) \
((HANDLE) _beginthreadex( \
(void *) (psa), \
(unsigned) (cbStack), \
(PTHREAD_START) (pfnStartAddr),\
(void *) (pvParam),\
(unsigned) (fdwCreate), \
(unsigned *) (pdwThreadID)))


// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}
//释放数组指针宏
#define RELEASE_ARRAY(x)                      {if(x != NULL ){delete[] x;x=NULL;}}
