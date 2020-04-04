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
	//���յĻص� c_recv_text�����յ�����Ϣ     n_recv_size�����յ��ֽڳ���--ֻҪ�ص����ˡ��Ż����������һ�����������ﲻ����ִ��̧���صĲ���
	virtual void on_udp_recv_cb(char* c_recv_text, int n_recv_size);
	//������ɺ�Ļص����ص�����������ſ��Է���ϵһ����Ϣ��
	virtual void on_udp_send_cb(uv_udp_send_t* req, int status);
private:
	//ϵͳ�õ��¼�
	SRWLOCK  m_srwlock_face;         //��дͬ������
	CONDITION_VARIABLE m_full_condition;  //��������
	char m_c_recv_buffer[4096];//���յ��ֽ�
	int m_n_recv_size=0;//�յ����ֽ���
public:
	//��ȡ���ܵ���Ϣ���ڵȴ��¼�������з��أ������̷���
	int GetRecvText(char* c_recv_buffer,int* n_recv_size);
	//���ѵȴ�
	void WakeRecvEvent()
	{
		m_n_recv_size = 0;
		WakeConditionVariable(&m_full_condition);
	};
};

