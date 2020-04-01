#pragma once
#pragma warning(disable: 4819)

//ColeDateTime��ͷ�ļ�
#include <ATLComTime.h>
//DuiLib��ͷ�ļ�---����������Ŀ���������ð���Ŀ¼
#include "UIlib.h"
using namespace DuiLib;
//DuiLib��Lib�ļ�
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
//�̺߳���ָ��
typedef unsigned(__stdcall* PTHREAD_START) (void*);
#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadID) \
((HANDLE) _beginthreadex( \
(void *) (psa), \
(unsigned) (cbStack), \
(PTHREAD_START) (pfnStartAddr),\
(void *) (pvParam),\
(unsigned) (fdwCreate), \
(unsigned *) (pdwThreadID)))


// �ͷ�ָ���
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// �ͷž����
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// �ͷ�Socket��
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}
//�ͷ�����ָ���
#define RELEASE_ARRAY(x)                      {if(x != NULL ){delete[] x;x=NULL;}}
