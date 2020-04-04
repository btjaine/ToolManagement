#include "pch.h"
#include "lib_uv_udp_client_face.h"

void lib_uv_udp_client_face::on_udp_recv_cb(char* c_recv_text, int n_recv_size)
{
	m_n_recv_size = 0;
	ZeroMemory(m_c_recv_buffer, sizeof(m_c_recv_buffer));
	memcpy_s(m_c_recv_buffer, sizeof(m_c_recv_buffer), c_recv_text, n_recv_size);
	m_n_recv_size = n_recv_size;
	//唤醒阻塞的
	WakeConditionVariable(&m_full_condition);
}

void lib_uv_udp_client_face::on_udp_send_cb(uv_udp_send_t* req, int status)
{
}

int lib_uv_udp_client_face::GetRecvText(char* c_recv_buffer, int* n_recv_size)
{
	AcquireSRWLockExclusive(&m_srwlock_face);
	SleepConditionVariableSRW(&m_full_condition, &m_srwlock_face, 2*1000, 0);
	//大于0的时候才去复制系统
	if (m_n_recv_size>0)
	{
		*n_recv_size = m_n_recv_size;
		memcpy(c_recv_buffer, m_c_recv_buffer, m_n_recv_size);
	}
	m_n_recv_size = 0;
	ReleaseSRWLockExclusive(&m_srwlock_face);
	return *n_recv_size;
}
