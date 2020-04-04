#include "pch.h"
#include "easylogging++.h"
#include "lib_uv_udp_client.h"

SRWLOCK m_rw_Capture_data_;

lib_uv_udp_client::lib_uv_udp_client()
{
	InitializeSRWLock(&m_rw_Capture_data_);
}

lib_uv_udp_client::~lib_uv_udp_client()
{
}

bool lib_uv_udp_client::set_server_ip_port(char* c_ip, int n_port)
{
	if (c_ip == nullptr || strlen(c_ip) <= 0)  return false;
	if (n_port <= 0)  return false;
	ZeroMemory(&m_sockaddr_in_addr_, sizeof(m_sockaddr_in_addr_));
	uv_ip4_addr(c_ip, n_port, &m_sockaddr_in_addr_);
	return true;
}

bool lib_uv_udp_client::run()
{
	bool m_b_result = true;
	//开始运行，在线程中进行event_base_dispatch()
	m_handle_cient_ = nullptr;
	m_handle_cient_ = CreateThread(nullptr, 0, thread_run_udp_client, this, 0, nullptr);
	if (m_handle_cient_ == nullptr || m_handle_cient_ == INVALID_HANDLE_VALUE)
	{
		LOG(INFO)<< "lib_uv_udp_client failed";
		m_b_result = false;
	}
	else
	{
		LOG(INFO) << "lib_uv_udp_client success";
	}
	return true;
}

bool lib_uv_udp_client::stop()
{
	if (m_handle_cient_)
	{
		send_to_udp_server((char*)("-1"), 0);
		WaitForSingleObject(m_handle_cient_,INFINITE);
		CloseHandle(m_handle_cient_);
		m_handle_cient_ = nullptr;
		uv_loop_close(m_uv_loop_);
	}
	return true;
}

//Returns:	0 on success, or an error code < 0 on failure.
int lib_uv_udp_client::send_to_udp_server(const char* c_send_text, int n_text_len)
{
	int m_n_result = 0;
	uv_async_t* p_async_handle = &m_uv_async_stu_.m_uv_async_;
	if (n_text_len>0)
	{
		AcquireSRWLockExclusive(&m_rw_Capture_data_);  //获得SRW锁---下一次再发从的话，需要获得，等待下次发送完成了。
		if (p_async_handle->data == nullptr)  return -1;
		if (c_send_text == nullptr || strlen(c_send_text) <= 0)  return -1;
		if (n_text_len <= 0)  return -1;
		m_uv_async_stu_._init_data_buffer();
		memcpy_s(p_async_handle->data, ALLOCATION_MEM_SIZE, c_send_text, n_text_len);
		m_uv_async_stu_.m_dword_send_size = n_text_len;
	}
	else
	{
		m_uv_async_stu_._init_data_buffer();
	}
	m_n_result = uv_async_send(p_async_handle);
	return m_n_result;
}
void lib_uv_udp_client::on_udp_send_cb(uv_udp_send_t* req, int status)
{
	
}

void lib_uv_udp_client::on_udp_recv_cb(char* c_recv_text, int n_recv_size)
{
	if (m_udp_loop_instance_stu_.m_dword_recv_size>0)
	{
		LOG(INFO) << c_recv_text << std::endl;
		send_to_udp_server(c_recv_text, n_recv_size);
	}
}

void lib_uv_udp_client::udp_send_cb(uv_udp_send_t* req, int status)
{
	const auto p_udp_sender = reinterpret_cast<UDP_SENDER*>(req);
	const auto m_p_client_instance = p_udp_sender->m_p_client_instance;
	
	m_p_client_instance->on_udp_send_cb(req, status);
	//释放
	ReleaseSRWLockExclusive(&m_rw_Capture_data_);
}

void lib_uv_udp_client::async_cb(uv_async_t* handle)
{
	   const auto m_p_async_stu = reinterpret_cast<LP_UDP_ASYNC_HANDLE_STU>(handle);
		auto m_p_client = m_p_async_stu->m_p_client_instance;
		//真正的发送
		if (m_p_client->m_uv_async_stu_.m_dword_send_size <= 0)
		{
			//退出操作
			uv_close(reinterpret_cast<uv_handle_t*>(&m_p_client->m_udp_loop_instance_stu_.m_uv_udp_), nullptr);
			uv_close(reinterpret_cast<uv_handle_t*>(&m_p_client->m_uv_async_stu_.m_uv_async_), nullptr);
			uv_stop(m_p_client->m_uv_loop_);
		}
		else
		{
			//正常发送
			uv_async_t* p_async_handle = &m_p_client->m_uv_async_stu_.m_uv_async_;
			uv_buf_t w_buf = uv_buf_init(reinterpret_cast<char*>(p_async_handle->data), m_p_client->m_uv_async_stu_.m_dword_send_size);
			ZeroMemory(&m_p_client->m_udp_sender_.m_uv_udp_send_, sizeof(uv_udp_send_t));
			uv_udp_send(&m_p_client->m_udp_sender_.m_uv_udp_send_, &m_p_client->m_udp_loop_instance_stu_.m_uv_udp_, &w_buf, 1, reinterpret_cast<const struct sockaddr*>(&m_p_client->m_sockaddr_in_addr_), udp_send_cb);
		}
}

void lib_uv_udp_client::udp_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	//每次接收信息都会先执行这个回调，然后在回调到udp_recv_cb
	auto m_p_udp_recv_instance = reinterpret_cast<UDP_RECV_INSTANCE_STU*>(handle);
	m_p_udp_recv_instance->_init_data_buffer();
	buf->base = reinterpret_cast<char*>(handle->data);
	buf->len = ALLOCATION_MEM_SIZE;
}

void lib_uv_udp_client::udp_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const sockaddr* addr, unsigned flags)
{
	auto m_p_udp_recv_instance = reinterpret_cast<UDP_RECV_INSTANCE_STU*>(handle);
	m_p_udp_recv_instance->m_dword_recv_size = nread;
	m_p_udp_recv_instance->flags = flags;
	//服务端的地址
	memcpy(&m_p_udp_recv_instance->m_sockaddr_in_addr_, addr, sizeof(sockaddr));
	//回调到类里
	m_p_udp_recv_instance->m_p_client_instance->on_udp_recv_cb(reinterpret_cast<char*>(m_p_udp_recv_instance->m_uv_udp_.data), nread);
}
