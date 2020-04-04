#pragma once
#include "pch.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include "easylogging++.h"
#include "CFaceDatabase.h"

//�����õ�ȫ�ֱ���
int   g_nFace_id = 0;
seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
seeta::ModelSetting FD_model("e:/model/fd_2_00.dat", device, g_nFace_id);
seeta::ModelSetting PD_model("e:/model/pd_2_00_pts5.dat", device, g_nFace_id);
//seeta::ModelSetting PD_model81("d:/model/pd_2_00_pts81.dat", device, g_nFace_id);
seeta::ModelSetting FR_model("e:/model/fr_2_10.dat", device, g_nFace_id);

CFaceDatabase::CFaceDatabase()
{
	GetProgramFullPath();
	engine = new seeta::FaceEngine(FD_model, PD_model, FR_model, 2, 16);
	engine->FD.set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 40);
	engine->FD.set(seeta::FaceDetector::PROPERTY_VIDEO_STABLE, 1);
	//�������ݿ�
	ZeroMemory(m_str_log, sizeof(m_str_log));
	sprintf_s(m_str_log, sizeof(m_str_log),"%s%s" ,g_cProgramPath, "em_id_name.db");
	int ret = sqlite3_open(m_str_log, &conn);
	if (ret != SQLITE_OK)
	{
		LOG(INFO) << "Create em_id_name.db failed!";
	}
	else
	{
		LOG(INFO) << "Create em_id_name.db success!";
	}
	ZeroMemory(m_c_normal_photo_path, sizeof(m_c_normal_photo_path));
	string m_strtemp = g_cProgramPath;
	m_strtemp += "setting.ini";
	GetPrivateProfileStringA("PHOTO_INFO", "NORMAL_PHOTO_PATH", "c:/NormalPhoto/", m_c_normal_photo_path, 1024, m_strtemp.c_str());
}

void CFaceDatabase::GetProgramFullPath()
{
	int nPos;
	CString m_strFilePath;
	GetModuleFileName(nullptr, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	m_strFilePath.ReleaseBuffer();
	nPos = m_strFilePath.ReverseFind('\\');
	m_strFilePath = m_strFilePath.Left(nPos + 1);
	//תΪchar
	ZeroMemory(g_cProgramPath, sizeof(g_cProgramPath));
	wide_char_2_multi_char(m_strFilePath.GetBuffer(0), g_cProgramPath);
}

int CFaceDatabase::executeNoQuery(sqlite3* db, const char* sql)
{
	sqlite3_stmt* pstmt = nullptr;

	if (sqlite3_prepare_v2(db, sql, strlen(sql), &pstmt, nullptr) != SQLITE_OK)
	{
		if (pstmt != nullptr)
			sqlite3_finalize(pstmt);
		return -1;
	}
	if (sqlite3_step(pstmt) != SQLITE_DONE)
	{
		sqlite3_finalize(pstmt);
		return -1;
	}
	sqlite3_finalize(pstmt);
	return 0;
}

int CFaceDatabase::executeWithQuery(sqlite3* db, char*** result, int* col, const char* sql)
{
	int ret, row;

	ret = sqlite3_get_table(db, sql, result, &row, col, nullptr);
	if (ret != SQLITE_OK)
	{
		return -1;
	}
	(*result)[(row + 1) * (*col)] = nullptr;

	return 0;
}

bool  CFaceDatabase::get_em_id_name_from_sqlite_database(sqlite3* conn)
{
	//����ط�������Ϣ
	EM_ID_NAME  m_stu_em_id_name;
	GalleryImageFilename.clear();//clear��sizeΪ�㵫��capbility:�ռ���Ȼ��
   //�������ݿ��
	const char* createSQL = "create table if not exists T_em_id_name (em_id varchar(256) PRIMARY KEY NOT NULL, em_name varchar(512));";
	//create table
	int ret = executeNoQuery(conn, createSQL);
	if (ret == -1)
	{
		LOG(INFO) << "Create _em_id_name table failed!";
		return false;
	}
	//��ʼ�����ݿ��л�ȡ����
	sqlite3_stmt* pstmt = nullptr;
	const char* pzTail = nullptr;
	const unsigned char* pTmp = nullptr;
	const char* selectSQL = "select *  from T_em_id_name;";
	//ִ�в�ѯ
	ret = sqlite3_prepare_v2(conn, selectSQL, strlen(selectSQL), &pstmt, &pzTail);
	if (SQLITE_OK != ret)
	{
		if (pstmt)
		{
			sqlite3_finalize(pstmt);
		}
		LOG(INFO) << "sqlite3_prepare_v2-��select *  from T_em_id_name failed!";
		return false;
	}
	//��ʼѭ����ȡ����
	int nCol = 0;
	while (sqlite3_step(pstmt) == SQLITE_ROW)
	{
		m_stu_em_id_name._init();
		nCol = 0;
		//ID
		pTmp = sqlite3_column_text(pstmt, nCol++);
		strcpy_s(m_stu_em_id_name.m_c_em_id, sizeof(m_stu_em_id_name.m_c_em_id), (char*)pTmp);
		//name
		pTmp = sqlite3_column_text(pstmt, nCol++);
		strcpy_s(m_stu_em_id_name.m_c_em_name, sizeof(m_stu_em_id_name.m_c_em_name), (char*)pTmp);
		//�����ļ��Ƿ����
		ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
		sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, m_stu_em_id_name.m_c_em_id);
		if ((_access(m_c_photo_file_name, 0)) == -1)
		{
			//�ļ�������
			LOG(INFO) << "file " << m_stu_em_id_name.m_c_em_id << "not exist!not in queue!";
			continue;
		}
		//ѹ��vector
		GalleryImageFilename.emplace_back(m_stu_em_id_name);
	}
	sqlite3_finalize(pstmt);
	return true;
}

auto CFaceDatabase::GreaterSort(SeetaFaceInfo a, SeetaFaceInfo b) -> bool
{
	return (a.pos.width * a.pos.height > b.pos.width * b.pos.height);
}

//LoadFaceFromDatabase:����em_id��װ��ͼƬ�ģ����
bool CFaceDatabase::LoadFaceFromDatabase()
{
	char m_c_sql[1024];//��ѯ���
	char m_c_photo_file_name[1024] = { 0 };
	EM_ID_NAME  m_stu_em_id_name;
	GalleryImageFilename.resize(FACE_DATABASE_SIZE);
	GalleryImageFilename.clear();
    //��ʼ��ԭ�������ݿ�����������
	if(get_em_id_name_from_sqlite_database(conn)==false)
	{
		if (conn)
		{
			sqlite3_close(conn);
		}
		LOG(INFO) << "get_em_id_name_from_sqlite_database-failed! system not run";
		return false;
	}
	//ÿ��Ա����һ��ID--GalleryIndexû�г�ʼ��-1�����������
	GalleryIndex.resize(FACE_DATABASE_SIZE);//����ע����ʶ�������е�ID������ͨ�����ID�ҵ����ݿ��ж�һ����������Ϣ
	GalleryIndex.clear();
	
	for (size_t i = 0; i < GalleryImageFilename.size(); ++i)
	{
		//��Vector�л�ȡem��Ϣ
		EM_ID_NAME& filename = GalleryImageFilename[i];//��get_em_id_name_from_sqlite_database�л�ȡ�����ң�����Ƭһ�����ڵ�
		//����һ����¼�����һ���Ǻ�GalleryImageFilenameһһ��Ӧ�ģ���ʹע������ʧ�ܣ�Ҳ�Ƕ�Ӧ��
		GalleryIndex.emplace_back(-1);//ѹ�磬һ��Ҫѹ�룬����û������
		int64_t& index = GalleryIndex[i];
		//ͼ��·��---��ȡͼ��
		sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, filename.m_c_em_id);
		seeta::cv::ImageData image = cv::imread(m_c_photo_file_name);
		if (image.empty() == false)
		{
			//������Ƭ�ɹ�������£���ȥ����ռ�--��ȥע�ᵽ�����⣬ע��ɹ��ˣ���ȥ����
			LOG(INFO)<< "Registering... " << filename.m_c_em_name << filename.m_c_em_id;
			//ע�ᵽ�����⣬����ID
			auto id = engine->Register(image);
			if (id < 0)
			{
				LOG(INFO) << "Registered failed"<< filename.m_c_em_name<< filename.m_c_em_id;
				continue;
			}
			//��ID�Żظ�GalleryIndex,��������
			index = id;
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered success!", filename.m_c_em_name);
			LOG(INFO) << m_str_log;
		}
		else
		{
			//û������ͼƬ����ɾ��������¼
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  read image  failed!", filename.m_c_em_name);
			LOG(INFO) << m_str_log;
		}
	}
	//��ע���id��Ա����Ϣ�Ľṹ��Ӧ����--------------����Ҫ����һ�������ݵĻ���Ҫ�Ȱ����ݷŵ� GalleryImageFilename��Ȼ����GalleryIndex������һ����¼��Ȼ��Register-��Ȼ��GalleryIndexMap.insert-����txt������һ����Ϣ
	GalleryIndexMap.clear();
	for (size_t i = 0; i < GalleryIndex.size(); ++i)
	{
		// save index and name pair
		if (GalleryIndex[i] < 0) continue; //�������ע��ʧ�ܵĻ����Ϳ��ܻ���ִ�����ΪGalleryIndex[i]����ֵ�п�����ͬ,���ԣ�������GalleryIndex[i] С��0�Ͳ�����
		GalleryIndexMap.insert(std::make_pair(GalleryIndex[i], GalleryImageFilename[i]));
	}
	return true;
}

bool CFaceDatabase::AddFaceInfoToDatabase(const char* c_em_id, const char* c_em_name, const char* c_file_name)
{
	Mat m_mat_regist_src;
	//Ӧ���ȿ�����û�����ͼƬ
	ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
	sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s", m_c_normal_photo_path, c_file_name);
	m_mat_regist_src = cv::imread(m_c_photo_file_name);
	if (m_mat_regist_src.empty())  return false;
	//�Ȳ����em_id�Ƿ������ݿ��У�����ڣ�����Ҫ�����ģ�������ھ�Ҫ���
	bool m_b_is_new_em_id = false;
	EM_ID_NAME  m_stu_em_id_name;
	
	char m_c_sql[1024];//��ѯ���
	int i = 0;
	int ncol = 0;
	char** array;
	ZeroMemory(m_c_sql, sizeof(m_c_sql));
	sprintf_s(m_c_sql, sizeof(m_c_sql), "select *  from  T_em_id_name where  em_id='%s';", c_em_id);
	int ret = executeWithQuery(conn, &array, &ncol, m_c_sql);
	if (ret == -1)
	{
		ZeroMemory(m_str_log, sizeof(m_str_log));
		sprintf_s(m_str_log,sizeof(m_str_log),"run->%sʧ��", m_c_sql);
		LOG(INFO) << m_str_log;
		return false;
	}
	else
	{
		if (array[ncol] == nullptr)
		{
			//˵��û��������¼-��д�����ݿ���
			ZeroMemory(m_c_sql, sizeof(m_c_sql));
			sprintf_s(m_c_sql, sizeof(m_c_sql), "insert  into  T_em_id_name  values ('%s','%s');", c_em_id, c_em_name);
			ret = executeNoQuery(conn, m_c_sql);
			if (ret == -1)
			{
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log,sizeof(m_str_log), "run->%s failed", m_c_sql);
				LOG(INFO) << m_str_log;
				return false;
			}
			//����Ϣ�ŵ�����------------------------------------------------------------------------------------------------------
			m_stu_em_id_name._init();
			//Ա��id
			strcpy_s(m_stu_em_id_name.m_c_em_id, sizeof(m_stu_em_id_name.m_c_em_id), c_em_id);
			//Ա��name
			strcpy_s(m_stu_em_id_name.m_c_em_name, sizeof(m_stu_em_id_name.m_c_em_name), c_em_name);
			//ѹ��vector
			GalleryImageFilename.emplace_back(m_stu_em_id_name);
			m_b_is_new_em_id = true;//������
		}
	}
	//��ȡ���em_id��GalleryImageFilename�е�λ��
	int m_n_current_em_id_pos = find_em_id_from_GalleryImageFilename((char*)c_em_id);
	if (m_n_current_em_id_pos < 0)
	{
		//����ط�һ��Ӧ�����ҵ�������Ҳ������Ͳ�����
		ZeroMemory(m_str_log, sizeof(m_str_log));
		sprintf_s(m_str_log, sizeof(m_str_log), "not find->%s in  find_em_id_from_GalleryImageFilename", c_em_id);
		LOG(INFO) << m_str_log;
		return false;
	}
	if (m_b_is_new_em_id == true)
	{
		//�����������֧
		 //��Ϊ��˳�����ӵġ����ԣ�����Ҫ��
		GalleryIndex.emplace_back(-1);
		int64_t& index = GalleryIndex[m_n_current_em_id_pos];
		LOG(INFO) << "Registering... " << c_em_name;
		//ͼ��·��---��ȡͼ��
		seeta::cv::ImageData image = m_mat_regist_src;
		if (image.empty() == false)
		{
			//ע�ᵽ�����⣬����ID
			auto id = engine->Register(image);
			if (id < 0)
			{
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered failed!", c_em_id);
				LOG(INFO) << m_str_log;
				return false;
			}
			//��ID�Żظ�GalleryIndex,��������
			index = id;
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered sucess!", c_em_name);
			LOG(INFO) << m_str_log;
			//ע�ᵽ��Ӧmap
			GalleryIndexMap.insert(std::make_pair(GalleryIndex[m_n_current_em_id_pos], GalleryImageFilename[m_n_current_em_id_pos]));
			//������Ƭ��ID����
			ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
			sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, c_em_id);
			imwrite(m_c_photo_file_name,m_mat_regist_src);
		}
	}
	else
	{
		//ԭ�����еġ���Ҫ����
	   //��ɾenginne�е���Ϣ
		int64_t& index = GalleryIndex[m_n_current_em_id_pos];
		//ɾ�����index
		engine->Delete(index);
		//ɾ��GalleryIndexMap
		map<int64_t, EM_ID_NAME>::iterator iter;
		iter = GalleryIndexMap.find(index);
		if (iter != GalleryIndexMap.end())
		{
			GalleryIndexMap.erase(iter);
		}
		//Ȼ������ע��
		seeta::cv::ImageData image = m_mat_regist_src;
		if (image.empty() == false)
		{
			//ע�ᵽ�����⣬����ID
			auto id = engine->Register(image);
			if (id < 0)
			{
				//m_str_log.Format("%s  Registered failed!", c_em_id);
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered failed!", c_em_id);
				LOG(INFO) << m_str_log;
				return false;
			}
			//��ID�Żظ�GalleryIndex,��������
			index = id;
			//m_str_log.Format("%s  Registered success!", c_em_name);
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered success!", c_em_name);
			LOG(INFO) << m_str_log;
			//ע�ᵽ��Ӧmap
			GalleryIndexMap.insert(std::make_pair(GalleryIndex[m_n_current_em_id_pos], GalleryImageFilename[m_n_current_em_id_pos]));
			//������Ƭ��ID����
			ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
			sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, c_em_id);
			imwrite(m_c_photo_file_name, m_mat_regist_src);
			//��־
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered success!", c_em_id);
			LOG(INFO) << m_str_log;
		}
	}
	return true;
}

bool CFaceDatabase::ManFaceToJpg(Mat mat_src_frame)
{
	char m_c_photo_file_name[1024] = {0};
	
	std::vector<SeetaFaceInfo> faces;
	seeta::cv::ImageData image = mat_src_frame;
	if (image.empty() == true)
	{
		LOG(INFO) << "mat_src_frame is empty ,can not cut face";
		return false;
	}
	//�����������е�����
	faces = engine->DetectFaces(image);
	//��������������������У������ǵ�һ��Ԫ��
	if (faces.size() >= 2)
	{
		std::sort(faces.begin(), faces.end(), CFaceDatabase::GreaterSort);//��������
	}
	if (faces.empty())
	{
		//��־
		//m_str_log.Format("not  find  %s face", m_TFOriginalText.c_em_id);
		LOG(INFO) <<"can not find face ,can not cut face";
		return false;
	}
	//ÿ����������м��ʶ��-----����ط�ֻ����һԪ�أ���Ϊ�����Ѿ���verct����������
	for (SeetaFaceInfo& face : faces)
	{
		auto points = engine->DetectPoints(image, face);
		auto score = QA.evaluate(image, face.pos, points.data());
		if (score == 0)
		{
			return false;
		}
		//��ȡͼ��Ȼ����em_id�����ļ�
		cv::Rect rect(face.pos.x, face.pos.y, face.pos.width, face.pos.height);
		cv::Mat  ROI = mat_src_frame(rect);
		//����ü���ͼƬ����ʽ:mycut.jpg
		ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
		sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, "mycut");
		if (cv::imwrite(m_c_photo_file_name, ROI) == false)
		{
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, "save cut picture %s failed!", m_c_photo_file_name);
			LOG(INFO) << m_str_log;
			return false;
		}
		//д��־
		ZeroMemory(m_str_log, sizeof(m_str_log));
		sprintf_s(m_str_log, "save cut picture %s success", m_c_photo_file_name);
		LOG(INFO) << m_str_log;
		return true;
	}
	return true;
}

//����ֵ��0���ҵ���ƥ�����Ƭ  -1�����ջ�����Ϊ��  -2��ԭʼMat����Ϊ��   -3:�����������ϸ�  -4��û��ƥ������� 
int  CFaceDatabase::RecognizeMan(Mat srcMat, EM_ID_NAME* pOut)
{
	if (GalleryImageFilename.size() <= 0)  return -4;
	if (pOut==nullptr)  return -1;
	ZeroMemory(pOut, sizeof(EM_ID_NAME));
	seeta::cv::ImageData image = srcMat;// cv::imread(m_c_photo_file_name);
	if (image.empty() == true) return -2;
	//�����������е�����
	auto faces = engine->DetectFaces(image);
	//��������������������У������ǵ�һ��Ԫ��
	if (faces.size() >= 2)  std::sort(faces.begin(), faces.end(), GreaterSort);//��������
	//ÿ����������м��ʶ��-----����ط�ֻ����һԪ�أ���Ϊ�����Ѿ���verct����������
	for (SeetaFaceInfo& face : faces)
	{
		// Query top 1
		int64_t index = -1;
		float similarity = 0;
		//��ȡ����
		auto points = engine->DetectPoints(image, face);
		//�������������������������������==0�ͺ��Ե�
	   auto score = QA.evaluate(image, face.pos, points.data());
		if (score == 0)
		{
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log,sizeof(m_str_log),"Face(x=%d  y=%d  w=%d  h=%d)ignored!", face.pos.x, face.pos.y, face.pos.width, face.pos.height);
			LOG(INFO) << m_str_log;
			return -3;
		}
		else
		{
			//�ӹ������ݿ��ȡ��������������ƶȵ���Ƭ
			similarity = 0.0f;
			auto queried = engine->QueryTop(image, points.data(), 1, &index, &similarity);
			// no face queried from database
			if (queried < 1)
			{
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "Face(x=%d  y=%d  w=%d  h=%d)not match face!", face.pos.x, face.pos.y, face.pos.width, face.pos.height);
				LOG(INFO) << m_str_log;
				return  -4;
			}
			// similarity greater than threshold, means recognized
			if (similarity > m_f_threshold)
			{
				//�õ��˵�ǰ�������ȶԽ��------em_id,ִ�н��(-1:ʧ��    0���ɹ�)
				*pOut = GalleryIndexMap[index];
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "match face:Id:%s  name:%s", pOut->m_c_em_id, pOut->m_c_em_name);
				LOG(INFO) << m_str_log;
				//�ҵ��ĺ�Ͳ�����
				return 0;
			}
			else
			{
				return -4;
			}
		}
	}
	return -4;
}

int   CFaceDatabase::find_em_id_from_GalleryImageFilename(char* c_em_id)
{
	if (c_em_id == nullptr || strlen(c_em_id) <= 0)  return -1;

	int m_n_size = GalleryImageFilename.size();
	for (int i = 0; i < m_n_size; i++)
	{
		if (strcmp(GalleryImageFilename[i].m_c_em_id, c_em_id) == 0)
		{
			return i;
		}
	}
	return -1;
}

bool CFaceDatabase::wide_char_2_multi_char(wchar_t* buf, char* str)
{
	auto i_length = 0;
	//��ȡ�ֽڳ���   
	i_length = WideCharToMultiByte(CP_ACP, 0, buf, -1, nullptr, 0, nullptr, nullptr);
	//��wchar_tֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, buf, -1, str, i_length, nullptr, nullptr);
	return true;
}