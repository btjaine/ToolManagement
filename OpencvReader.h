#pragma once
#include "pch.h"
#include "easylogging++.h"
#include <opencv2/opencv.hpp>            // C++
#include <opencv2/highgui/highgui_c.h>   // C
#include <opencv2/imgproc/imgproc_c.h>   // C

using namespace  cv;
using namespace   std;

// ReSharper disable once IdentifierTypo
class OpencvReader
{
public:
	OpencvReader();
	explicit OpencvReader(int i_usb_camera_index);
	OpencvReader(char* url);
	~OpencvReader();

public://��¶�Ľӿ�
	//�����豸�����豸����
	void set_device_type(int n_type);;
	//��ʼ��ȡrtsp��
	bool start();
	//ֹͣ��ȡrtsp��
	void stop();
	//����rtsp����ַ
	void set_rtsp_url(const char* c_rtsp_url);
	//����usb����ͷ������0��ʼ
	void set_usb_camera_index(int i_index);
	//��ȡһ֡����
	bool  get_frame(Mat& d_mat);
	//set show windows hwnd
	void set_show_window(HWND hwnd_show_window);
public:
	//��ץȡrtsp���ŵ�һ���߳�������
	static DWORD __stdcall start_thread(void* pthis);;
	//���ץȡrtsp�����߳�
	void true_start_thread();
private:
	//rtsp��ַ
	char m_c_rtsp_address_[1024] = {0};
	//��������С
	int m_n_queue_size_=3;
	//��ͼcapture
	VideoCapture  m_video_capture_;
	//usb����ͷ��index
	int m_i_usb_camera_index_ = 0;
	//����start�̵߳ľ��
	HANDLE   m_handle_start=nullptr;
	//�˳�captureѭ��
	bool m_b_exit_capture_loop_ = false;
	//��Ƶ��
	int m_i_width = 0;
	//��Ƶ��
	int m_i_height = 0;
	//֡��
	int m_i_fps = 25;
	//����ͷͬ����
	SRWLOCK m_rw_Capture_data_{};
	//�豸���� 0:usb camare   1:IPC rtsp��Э��
	int m_n_device_type_ = 0;
	//������ʾ���ڵĴ��ھ��--�ⲿ�������ġ���ΪҪ������������ʾ�������
	HWND  m_show_window_ = nullptr;
	//�ڲ�������������ʾcapture�� namedWindow-Ϊ�˷���ʹ�ã���������õķ���
	HWND  m_opencv_window_ = nullptr;
private:
	void _init_parameter();
};

