#pragma once
#include "pch.h"
//人脸识别用头文件
#include <seeta/FaceEngine.h>
#include <seeta/Struct_cv.h>
#include <seeta/Struct.h>
#include <array>
#include <map>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <string>
#include <seeta/QualityAssessor.h>
#include <sqlite3.h>
using namespace std;
using namespace cv;

//最多只能5000人
#define FACE_DATABASE_SIZE 5000

typedef  struct _em_id_name
{
	char   m_c_em_id[128];//员工ID
	char   m_c_em_name[256];//员工名字
	void _init()
	{
		ZeroMemory(this, sizeof(_em_id_name));
	};
	_em_id_name()
	{
		_init();
	};
}EM_ID_NAME, * lpEM_ID_NAME;

//人脸用的全局变量
int   g_nFace_id = 0;
seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
seeta::ModelSetting FD_model("e:/model/fd_2_00.dat", device, g_nFace_id);
seeta::ModelSetting PD_model("e:/model/pd_2_00_pts5.dat", device, g_nFace_id);
//seeta::ModelSetting PD_model81("d:/model/pd_2_00_pts81.dat", device, g_nFace_id);
seeta::ModelSetting FR_model("e:/model/fr_2_10.dat", device, g_nFace_id);

class CFaceDatabase
{
public:
	CFaceDatabase();
private:
	char g_cProgramPath[1024];//程序运行路径
	char  m_str_log[2048];
	seeta::QualityAssessor QA;
	seeta::FaceEngine* engine = nullptr;//用来检测人靓的
	//数据库
	sqlite3* conn = nullptr;
	//程序运行路径
	CString     m_strFilePath;
	//保存Em_Id_name的vecor
	std::vector<EM_ID_NAME> GalleryImageFilename;
	//从设备抓取的原始图片的路径
	char m_c_original_photo_path[1024];
	//裁剪后，正常图像的路径
	char m_c_normal_photo_path[1024];
	//人脸对比相似度
	float m_f_threshold = 0.7f;
	//人脸特征ID和Em_id_name的对应关系 
	std::map<int64_t, EM_ID_NAME> GalleryIndexMap;
	//人脸注册特征库Id
	std::vector<int64_t> GalleryIndex;
	char m_c_photo_file_name[1024] = {0};
private:
	//获取程序运行的全路径
	void GetProgramFullPath();
	//查询数据
	int executeNoQuery(sqlite3* db, const char* sql);
	int executeWithQuery(sqlite3* db, char*** result, int* col, const char* sql);
	bool  get_em_id_name_from_sqlite_database(sqlite3* conn);
	int   find_em_id_from_GalleryImageFilename(char* c_em_id);//从GalleryImageFilename中获取该em_id在vector中的位置
	bool wide_char_2_multi_char(wchar_t* buf, char* str);
public://暴露的api
	//从数据库中载入人脸信息
	bool LoadFaceFromDatabase();
	//添加人脸信息到Database--图片放在固定的目录下，文件名mycut.jpg
	bool AddFaceInfoToDatabase(char* c_em_id, char* c_em_name);
	//裁剪人脸图片并且保存到指定位置
	bool ManFaceToJpg(Mat mat_src_frame);
	//这个函数是排序，选择面积最大的
	static bool GreaterSort(SeetaFaceInfo a, SeetaFaceInfo b);
};

