#pragma once
#include "pch.h"
//����ʶ����ͷ�ļ�
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

//���ֻ��5000��
#define FACE_DATABASE_SIZE 5000

typedef  struct _em_id_name
{
	char       m_c_em_id[128];//Ա��ID
	char       m_c_em_name[256];//Ա������
	int64_t   m_n_face_id_in_face_enginee;//����������ע���ID
	void _init()
	{
		ZeroMemory(this, sizeof(_em_id_name));
		m_n_face_id_in_face_enginee = -1;
	};
	_em_id_name()
	{
		_init();
	};
}EM_ID_NAME, * lpEM_ID_NAME;

//�����õ�ȫ�ֱ���
/*int   g_nFace_id = 0;
seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
seeta::ModelSetting FD_model("e:/model/fd_2_00.dat", device, g_nFace_id);
seeta::ModelSetting PD_model("e:/model/pd_2_00_pts5.dat", device, g_nFace_id);
//seeta::ModelSetting PD_model81("d:/model/pd_2_00_pts81.dat", device, g_nFace_id);
seeta::ModelSetting FR_model("e:/model/fr_2_10.dat", device, g_nFace_id);*/

class CFaceDatabase
{
public:
	CFaceDatabase();
private:
	char g_cProgramPath[1024];//��������·��
	char  m_str_log[2048];
	seeta::QualityAssessor QA;
	seeta::FaceEngine* engine = nullptr;//�������������
	//���ݿ�
	sqlite3* conn = nullptr;
	//��������·��
	CString     m_strFilePath;
	//����Em_Id_name��vecor
	std::vector<EM_ID_NAME> GalleryImageFilename;
	//���豸ץȡ��ԭʼͼƬ��·��
	char m_c_original_photo_path[1024];
	//�ü�������ͼ���·��
	char m_c_normal_photo_path[1024];
	//�����Ա����ƶ�
	float m_f_threshold = 0.6f;
	//��������ID��Em_id_name�Ķ�Ӧ��ϵ 
	std::map<int64_t, EM_ID_NAME> GalleryIndexMap;
	//����ע��������Id
	std::vector<int64_t> GalleryIndex;
	char m_c_photo_file_name[1024] = {0};
private:
	//��ȡ�������е�ȫ·��
	void GetProgramFullPath();
	//��ѯ����
	int executeNoQuery(sqlite3* db, const char* sql);
	int executeWithQuery(sqlite3* db, char*** result, int* col, const char* sql);
	bool  get_em_id_name_from_sqlite_database(sqlite3* conn);
	int   find_em_id_from_GalleryImageFilename(char* c_em_id);//��GalleryImageFilename�л�ȡ��em_id��vector�е�λ��
	bool wide_char_2_multi_char(wchar_t* buf, char* str);
public:
	//�������������ѡ��������
	static bool GreaterSort(SeetaFaceInfo a, SeetaFaceInfo b);
public://��¶��api
	//�����ݿ�������������Ϣ
	bool LoadFaceFromDatabase();
	//���������Ϣ��Database--ͼƬ���ڹ̶���Ŀ¼�£��ļ���mycut.jpg
	bool AddFaceInfoToDatabase(const char* c_em_id, const char* c_em_name,const char* c_file_name);
	//�ü�����ͼƬ���ұ��浽ָ��λ��--ͼƬ���ڹ̶���Ŀ¼�£��ļ���mycut.jpg  true:�ü��ɹ�  false:�ü�ʧ��
	bool ManFaceToJpg(Mat mat_src_frame);
	//����Mat����������ʶ��������.����ֵ��nullptr:û�з���  ��nullptr���ҵ��ˡ����ݱ�����pOut��
	int  RecognizeMan(Mat srcMat, EM_ID_NAME* pOut);
};

