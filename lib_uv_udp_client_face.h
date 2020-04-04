#pragma once
#include "lib_uv_udp_client.h"
class lib_uv_udp_client_face :
	public lib_uv_udp_client
{
public:
	lib_uv_udp_client_face()
	{
		InitializeSRWLock(&m_srwlock_face);
	};
public:
	//接收的回调 c_recv_text：接收到的信息     n_recv_size：接收的字节长度--只要回调完了。才会继续接收下一条。所以这里不可以执行抬负载的操作
	virtual void on_udp_recv_cb(char* c_recv_text, int n_recv_size);
	//发送完成后的回调，回调完这个函数才可以发送系一条信息。
	virtual void on_udp_send_cb(uv_udp_send_t* req, int status);
private:
	//系统用的事件
	SRWLOCK  m_srwlock_face;         //读写同步对象
	CONDITION_VARIABLE m_full_condition;  //条件变量
	char m_c_recv_buffer[4096];//接收的字节
	int m_n_recv_size=0;//收到的字节数
public:
	//获取接受的信息，在等待事件内如果有返回，就立刻返回
	int GetRecvText(char* c_recv_buffer,int* n_recv_size);
	//唤醒等待
	void WakeRecvEvent()
	{
		m_n_recv_size = 0;
		WakeConditionVariable(&m_full_condition);
	};
};

