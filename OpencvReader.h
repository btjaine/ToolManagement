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

public://暴露的接口
	//设置设备连接设备类型
	void set_device_type(int n_type);;
	//开始获取rtsp流
	bool start();
	//停止获取rtsp流
	void stop();
	//设置rtsp流地址
	void set_rtsp_url(const char* c_rtsp_url);
	//设置usb摄像头索引从0开始
	void set_usb_camera_index(int i_index);
	//获取一帧数据
	bool  get_frame(Mat& d_mat);
	//set show windows hwnd
	void set_show_window(HWND hwnd_show_window);
public:
	//将抓取rtsp流放到一个线程中运行
	static DWORD __stdcall start_thread(void* pthis);;
	//真的抓取rtsp流的线程
	void true_start_thread();
private:
	//rtsp网址
	char m_c_rtsp_address_[1024] = {0};
	//缓冲区大小
	int m_n_queue_size_=3;
	//主图capture
	VideoCapture  m_video_capture_;
	//usb摄像头的index
	int m_i_usb_camera_index_ = 0;
	//开启start线程的句柄
	HANDLE   m_handle_start=nullptr;
	//退出capture循环
	bool m_b_exit_capture_loop_ = false;
	//视频宽
	int m_i_width = 0;
	//视频高
	int m_i_height = 0;
	//帧率
	int m_i_fps = 25;
	//摄像头同步用
	SRWLOCK m_rw_Capture_data_{};
	//设备类型 0:usb camare   1:IPC rtsp流协议
	int m_n_device_type_ = 0;
	//设置显示窗口的窗口句柄--外部传进来的。因为要在宿主程序显示这个画面
	HWND  m_show_window_ = nullptr;
	//内部建立的用来显示capture的 namedWindow-为了方便使用，并不是最好的方案
	HWND  m_opencv_window_ = nullptr;
private:
	void _init_parameter();
};

