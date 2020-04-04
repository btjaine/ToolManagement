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


//������ڴ��С
#define ALLOCATION_MEM_SIZE   4096

class lib_uv_udp_client;


typedef  struct _udp_sender_
{
	uv_udp_send_t	m_uv_udp_send_;
	lib_uv_udp_client* m_p_client_instance = nullptr;
}UDP_SENDER,*LP_USP_SENDER;
//����ṹ��Ҫ��Ϊ����Ѹ���ҵ���ʵ���Լ�
typedef struct _udp_loop_instance_stu{
	uv_udp_t				m_uv_udp_;//һ��Ҫ���ڵ�һλ
	lib_uv_udp_client* m_p_client_instance = nullptr;
	DWORD               m_dword_recv_size;//�����յ����ݳ���
	struct sockaddr_in m_sockaddr_in_addr_;//���Ͷ˵���Ϣ
	unsigned flags;
	//��ʼ��������
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
	uv_async_t				m_uv_async_;//����ṹһ��Ҫ�ڵ�һ�����ܺ����ʹ�õ����
	lib_uv_udp_client*  m_p_client_instance = nullptr;
	DWORD				    m_dword_send_size;//��Ҫ���͵����ݴ�С
	
	_udp_async_handle_stu_()
	{
		ZeroMemory(this, sizeof(_udp_async_handle_stu_));
		m_p_client_instance = nullptr;
		m_uv_async_.data = static_cast<void*>(new char[ALLOCATION_MEM_SIZE]);
		m_dword_send_size = 0;
		_init_data_buffer();
	};
	//��ʼ��������
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

public://��¶�Ľӿ�
	//set udp server ip and port
	bool set_server_ip_port(char* c_ip, int n_port);
	//����
	bool run();
	//ֹͣ
	bool stop();
	//������Ϣ��UDP server
	int    send_to_udp_server(const char* c_send_text, int n_text_len);
	//���յĻص� c_recv_text�����յ�����Ϣ     n_recv_size�����յ��ֽڳ���--ֻҪ�ص����ˡ��Ż����������һ�����������ﲻ����ִ��̧���صĲ���
	virtual void on_udp_recv_cb(char* c_recv_text, int n_recv_size);
	//������ɺ�Ļص����ص�����������ſ��Է���ϵһ����Ϣ��
	virtual void on_udp_send_cb(uv_udp_send_t * req, int status);
public:
	//���߳�������
	static DWORD __stdcall   thread_run_udp_client(void* pInstance)
	{
		//������������
		const auto p = static_cast<lib_uv_udp_client*>(pInstance);
		p->true_thread_run_udp_client();
		return 0;
	};
	void  true_thread_run_udp_client()
	{
		m_uv_loop_ = uv_default_loop();
		
		//��ʼ��udp_send_t
		m_udp_sender_.m_p_client_instance = this;
		//��ʼ��uv_udp_t
		uv_udp_init(m_uv_loop_, &m_udp_loop_instance_stu_.m_uv_udp_);
		m_udp_loop_instance_stu_.m_p_client_instance = this;
		//��ʼ��uv_async_handle
		m_uv_async_stu_.m_p_client_instance = this;
		uv_async_init(m_uv_loop_, reinterpret_cast<uv_async_t*>(&m_uv_async_stu_), async_cb);
		// �����¼�ѭ����Ҫ���ҹ���recv�¼�
		uv_udp_recv_start(&m_udp_loop_instance_stu_.m_uv_udp_, udp_alloc_cb, udp_recv_cb);
		uv_run(m_uv_loop_, UV_RUN_DEFAULT);
	}
public:
	//udp async-------------------------------------------------------------------
	UDP_ASYNC_HANDLE_STU			m_uv_async_stu_;
	//�����ã�Ŀ����Ϊ���ܵõ������instance----------------------------------
	UDP_RECV_INSTANCE_STU    m_udp_loop_instance_stu_;
	//udp loop
	uv_loop_t* m_uv_loop_ = nullptr;
	//uv_udp_send_t	m_uv_udp_send_;-----------------------------------------
	UDP_SENDER  m_udp_sender_;//�����ã�Ϊ���ܵõ���ʵ��
	//udp server sockaddr
	struct sockaddr_in m_sockaddr_in_addr_;
public:
	//���ڻص�����
	static void udp_send_cb(uv_udp_send_t* req, int status);
	static void async_cb(uv_async_t* handle);
	static  void udp_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static  void  udp_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
private:
	//run threaad handle
	HANDLE m_handle_cient_ = nullptr;
};

