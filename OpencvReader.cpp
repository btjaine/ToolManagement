#include "pch.h"
#include "OpencvReader.h"

OpencvReader::OpencvReader()
{
	_init_parameter();
}

OpencvReader::OpencvReader(int i_usb_camera_index)
{
	m_i_usb_camera_index_ = i_usb_camera_index;
	_init_parameter();
}

OpencvReader::OpencvReader(char* url)
{
	set_rtsp_url(url);
	_init_parameter();
}

OpencvReader::~OpencvReader()
{
}

void OpencvReader::set_device_type(int n_type)
{
	m_n_device_type_ = n_type;
}

DWORD OpencvReader::start_thread(void* pthis)
{
	const auto p = static_cast<OpencvReader*>(pthis);
	p->true_start_thread();
	return 0;
}

bool  OpencvReader::get_frame(Mat& d_mat)
{
	bool m_b_result = true;
	AcquireSRWLockExclusive(&m_rw_Capture_data_);  //获得SRW锁
	m_video_capture_.read(d_mat);
	ReleaseSRWLockExclusive(&m_rw_Capture_data_);
	if (d_mat.empty())
	{
		m_b_result = false;
	}
	return m_b_result;
}
bool OpencvReader::start()
{
	bool m_b_result = true;
	if (m_handle_start)
	{
		LOG(INFO) << "OpencvReader::start_thread   already run";
		return false;
	}
	m_b_exit_capture_loop_ = false;
	m_handle_start = nullptr;
	m_handle_start = CreateThread(nullptr, 0, OpencvReader::start_thread, this, 0, nullptr);
	if (m_handle_start == nullptr || m_handle_start == INVALID_HANDLE_VALUE)
	{
		LOG(INFO) << "do  OpencvReader::start_thread thread failed!";
		m_b_result = false;
	}
	else
	{
		LOG(INFO) << "do  OpencvReader::start_thread thread  success";
	}
	return m_b_result;
}

void OpencvReader::stop()
{
	char m_c_temp[1024] = { 0 };
	
	if (m_handle_start == nullptr)  return;
	m_b_exit_capture_loop_ = true;
	WaitForSingleObject(m_handle_start, -1);
	m_video_capture_.release();
	if (m_opencv_window_)
	{
		cvDestroyWindow("MainUSBCameraVideo");
		m_opencv_window_ = nullptr;
	}
	sprintf_s(m_c_temp, sizeof(m_c_temp), "%s   ---- exit!", m_c_rtsp_address_);
	LOG(INFO) << m_c_temp;
}

void OpencvReader::set_rtsp_url(const char* c_rtsp_url)
{
	ZeroMemory(m_c_rtsp_address_, sizeof(m_c_rtsp_address_));
	strcpy_s(m_c_rtsp_address_, sizeof(m_c_rtsp_address_), c_rtsp_url);
}

void OpencvReader::set_usb_camera_index(int i_index)
{
	m_i_usb_camera_index_ = i_index;
	
}

void OpencvReader::set_show_window(HWND hwnd_show_window)
{
	if (hwnd_show_window == nullptr)  return;
	m_show_window_ = hwnd_show_window;
	if (m_opencv_window_==nullptr)
	{
		//设置主窗口Opencv的显示窗口
		namedWindow("MainUSBCameraVideo", WINDOW_NORMAL);// WINDOW_NORMAL
		resizeWindow("MainUSBCameraVideo", 640, 480);// rect_video.right - rect_video.left, rect_video.bottom - rect_video.top);
		m_opencv_window_ = static_cast<HWND>(cvGetWindowHandle("MainUSBCameraVideo"));   // video的句柄
	}
	const HWND hParent = ::GetParent(m_opencv_window_);               //video句柄的父窗口
	::SetParent(m_opencv_window_, m_show_window_);   // 设置父窗口。
	::ShowWindow(hParent, SW_HIDE);//SW_HIDE
}

void OpencvReader::true_start_thread()
{
	try
	{
		if (m_n_device_type_==0)
		{
			m_video_capture_.open(m_i_usb_camera_index_);
		}
		else if (m_n_device_type_==1)
		{
			m_video_capture_.open(m_c_rtsp_address_);
		}
		if (!m_video_capture_.isOpened())
		{
			char m_c_temp[1024] = {0};
			sprintf_s(m_c_temp, sizeof(m_c_temp), "%s   ---- failed!", m_c_rtsp_address_);
			LOG(INFO) <<m_c_temp;
		}
		else
		{
			LOG(INFO) << "cam open success!";
		}
		
		//获取相关参数
		m_i_width = m_video_capture_.get(CAP_PROP_FRAME_WIDTH);
		m_i_height = m_video_capture_.get(CAP_PROP_FRAME_HEIGHT);
		m_i_fps = m_video_capture_.get(CAP_PROP_FPS);
		if (m_i_fps == 0) m_i_fps = 25;
		Mat frame;
		while(!m_b_exit_capture_loop_)
		{
			///读取rtsp视频帧，解码视频帧
		    AcquireSRWLockExclusive(&m_rw_Capture_data_);  //获得SRW锁
			/*if (!m_video_capture_.grab())
			{
				ReleaseSRWLockExclusive(&m_rw_Capture_data_);
				continue;
			}
			///yuv转换为rgb
			if (!m_video_capture_.retrieve(frame))
			{
				ReleaseSRWLockExclusive(&m_rw_Capture_data_);
				continue;
			}*/
			m_video_capture_.read(frame);
			if (m_opencv_window_)
			{
				cv::imshow("MainUSBCameraVideo", frame);
			}
			ReleaseSRWLockExclusive(&m_rw_Capture_data_);
			
			waitKey(1);
		}
	}
	catch (exception & ex)
	{
		LOG(INFO) << ex.what();
	}
}

void OpencvReader::_init_parameter()
{
	InitializeSRWLock(&m_rw_Capture_data_);//读取USB摄像头的独享锁
	m_show_window_ = nullptr;
	m_opencv_window_ = nullptr;
}
