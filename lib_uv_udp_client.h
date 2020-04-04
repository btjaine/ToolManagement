#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib,"Secur32.lib")


//申请的内存大小
#define ALLOCATION_MEM_SIZE   4096

class lib_uv_udp_client;


typedef  struct _udp_sender_
{
	uv_udp_send_t	m_uv_udp_send_;
	lib_uv_udp_client* m_p_client_instance = nullptr;
}UDP_SENDER,*LP_USP_SENDER;
//这个结构主要是为了能迅速找到类实例自己
typedef struct _udp_loop_instance_stu{
	uv_udp_t				m_uv_udp_;//一定要放在第一位
	lib_uv_udp_client* m_p_client_instance = nullptr;
	DWORD               m_dword_recv_size;//本次收到数据长度
	struct sockaddr_in m_sockaddr_in_addr_;//发送端的信息
	unsigned flags;
	//初始化缓冲区
	void _init_data_buffer()
	{
		ZeroMemory(m_uv_udp_.data, ALLOCATION_MEM_SIZE);
		m_dword_recv_size = 0;
	};
	_udp_loop_instance_stu()
	{
		ZeroMemory(this, sizeof(_udp_loop_instance_stu));
		m_p_client_instance = nullptr;
		m_uv_udp_.data = static_cast<void*>(new char[ALLOCATION_MEM_SIZE]);
		m_dword_recv_size = 0;
		_init_data_buffer();
	};
	~_udp_loop_instance_stu()
	{
		if (m_uv_udp_.data)
		{
			delete[] static_cast<char*>(m_uv_udp_.data);
			m_uv_udp_.data = nullptr;
		}
	}
}UDP_RECV_INSTANCE_STU,*LP_UDP_RECV_INSTANCE_STU;



typedef struct _udp_async_handle_stu_{
	uv_async_t				m_uv_async_;//这个结构一定要在第一个，很后面会使用到这个
	lib_uv_udp_client*  m_p_client_instance = nullptr;
	DWORD				    m_dword_send_size;//将要发送的数据大小
	
	_udp_async_handle_stu_()
	{
		ZeroMemory(this, sizeof(_udp_async_handle_stu_));
		m_p_client_instance = nullptr;
		m_uv_async_.data = static_cast<void*>(new char[ALLOCATION_MEM_SIZE]);
		m_dword_send_size = 0;
		_init_data_buffer();
	};
	//初始化缓冲区
	void _init_data_buffer()
	{
		ZeroMemory(m_uv_async_.data, ALLOCATION_MEM_SIZE);
		m_dword_send_size = 0;
	};
	~_udp_async_handle_stu_()
	{
		if (m_uv_async_.data)
		{
			delete[] static_cast<char*>(m_uv_async_.data);
			m_uv_async_.data = nullptr;
		}
	};
}UDP_ASYNC_HANDLE_STU,*LP_UDP_ASYNC_HANDLE_STU;

class lib_uv_udp_client
{
public:
	lib_uv_udp_client();
	~lib_uv_udp_client();

public://暴露的接口
	//set udp server ip and port
	bool set_server_ip_port(char* c_ip, int n_port);
	//开启
	bool run();
	//停止
	bool stop();
	//发送信息到UDP server
	int    send_to_udp_server(const char* c_send_text, int n_text_len);
	//接收的回调 c_recv_text：接收到的信息     n_recv_size：接收的字节长度--只要回调完了。才会继续接收下一条。所以这里不可以执行抬负载的操作
	virtual void on_udp_recv_cb(char* c_recv_text, int n_recv_size);
	//发送完成后的回调，回调完这个函数才可以发送系一条信息。
	virtual void on_udp_send_cb(uv_udp_send_t * req, int status);
public:
	//在线程中运行
	static DWORD __stdcall   thread_run_udp_client(void* pInstance)
	{
		//开启人脸服务
		const auto p = static_cast<lib_uv_udp_client*>(pInstance);
		p->true_thread_run_udp_client();
		return 0;
	};
	void  true_thread_run_udp_client()
	{
		m_uv_loop_ = uv_default_loop();
		
		//初始化udp_send_t
		m_udp_sender_.m_p_client_instance = this;
		//初始化uv_udp_t
		uv_udp_init(m_uv_loop_, &m_udp_loop_instance_stu_.m_uv_udp_);
		m_udp_loop_instance_stu_.m_p_client_instance = this;
		//初始化uv_async_handle
		m_uv_async_stu_.m_p_client_instance = this;
		uv_async_init(m_uv_loop_, reinterpret_cast<uv_async_t*>(&m_uv_async_stu_), async_cb);
		// 告诉事件循环，要帮我管理recv事件
		uv_udp_recv_start(&m_udp_loop_instance_stu_.m_uv_udp_, udp_alloc_cb, udp_recv_cb);
		uv_run(m_uv_loop_, UV_RUN_DEFAULT);
	}
public:
	//udp async-------------------------------------------------------------------
	UDP_ASYNC_HANDLE_STU			m_uv_async_stu_;
	//接收用，目的是为了能得到自身的instance----------------------------------
	UDP_RECV_INSTANCE_STU    m_udp_loop_instance_stu_;
	//udp loop
	uv_loop_t* m_uv_loop_ = nullptr;
	//uv_udp_send_t	m_uv_udp_send_;-----------------------------------------
	UDP_SENDER  m_udp_sender_;//发送用，为了能得到主实例
	//udp server sockaddr
	struct sockaddr_in m_sockaddr_in_addr_;
public:
	//类内回调函数
	static void udp_send_cb(uv_udp_send_t* req, int status);
	static void async_cb(uv_async_t* handle);
	static  void udp_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static  void  udp_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
private:
	//run threaad handle
	HANDLE m_handle_cient_ = nullptr;
};

