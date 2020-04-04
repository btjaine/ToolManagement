#pragma once
#include "pch.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include "easylogging++.h"
#include "CFaceDatabase.h"

//人脸用的全局变量
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
	//建立数据库
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
	//转为char
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
	//这个地方保存信息
	EM_ID_NAME  m_stu_em_id_name;
	GalleryImageFilename.clear();//clear后size为零但是capbility:空间依然在
   //建立数据库表
	const char* createSQL = "create table if not exists T_em_id_name (em_id varchar(256) PRIMARY KEY NOT NULL, em_name varchar(512));";
	//create table
	int ret = executeNoQuery(conn, createSQL);
	if (ret == -1)
	{
		LOG(INFO) << "Create _em_id_name table failed!";
		return false;
	}
	//开始从数据库中获取数据
	sqlite3_stmt* pstmt = nullptr;
	const char* pzTail = nullptr;
	const unsigned char* pTmp = nullptr;
	const char* selectSQL = "select *  from T_em_id_name;";
	//执行查询
	ret = sqlite3_prepare_v2(conn, selectSQL, strlen(selectSQL), &pstmt, &pzTail);
	if (SQLITE_OK != ret)
	{
		if (pstmt)
		{
			sqlite3_finalize(pstmt);
		}
		LOG(INFO) << "sqlite3_prepare_v2-》select *  from T_em_id_name failed!";
		return false;
	}
	//开始循环读取数据
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
		//看看文件是否存在
		ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
		sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, m_stu_em_id_name.m_c_em_id);
		if ((_access(m_c_photo_file_name, 0)) == -1)
		{
			//文件不存在
			LOG(INFO) << "file " << m_stu_em_id_name.m_c_em_id << "not exist!not in queue!";
			continue;
		}
		//压如vector
		GalleryImageFilename.emplace_back(m_stu_em_id_name);
	}
	sqlite3_finalize(pstmt);
	return true;
}

auto CFaceDatabase::GreaterSort(SeetaFaceInfo a, SeetaFaceInfo b) -> bool
{
	return (a.pos.width * a.pos.height > b.pos.width * b.pos.height);
}

//LoadFaceFromDatabase:根据em_id来装载图片的，如果
bool CFaceDatabase::LoadFaceFromDatabase()
{
	char m_c_sql[1024];//查询语句
	char m_c_photo_file_name[1024] = { 0 };
	EM_ID_NAME  m_stu_em_id_name;
	GalleryImageFilename.resize(FACE_DATABASE_SIZE);
	GalleryImageFilename.clear();
    //开始从原来的数据库中载入数据
	if(get_em_id_name_from_sqlite_database(conn)==false)
	{
		if (conn)
		{
			sqlite3_close(conn);
		}
		LOG(INFO) << "get_em_id_name_from_sqlite_database-failed! system not run";
		return false;
	}
	//每个员工对一个ID--GalleryIndex没有初始化-1，会出问题吗
	GalleryIndex.resize(FACE_DATABASE_SIZE);//保存注册在识别数据中的ID，可以通过这个ID找到数据库中对一个的人脸信息
	GalleryIndex.clear();
	
	for (size_t i = 0; i < GalleryImageFilename.size(); ++i)
	{
		//从Vector中获取em信息
		EM_ID_NAME& filename = GalleryImageFilename[i];//从get_em_id_name_from_sqlite_database中获取，并且，是照片一定存在的
		//增加一条记录，这个一定是和GalleryImageFilename一一对应的，即使注册人脸失败，也是对应的
		GalleryIndex.emplace_back(-1);//压如，一定要压入，否则没有数据
		int64_t& index = GalleryIndex[i];
		//图像路径---读取图像
		sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, filename.m_c_em_id);
		seeta::cv::ImageData image = cv::imread(m_c_photo_file_name);
		if (image.empty() == false)
		{
			//读到照片成功的情况下，才去申请空间--先去注册到人脸库，注册成功了，才去申请
			LOG(INFO)<< "Registering... " << filename.m_c_em_name << filename.m_c_em_id;
			//注册到人脸库，返回ID
			auto id = engine->Register(image);
			if (id < 0)
			{
				LOG(INFO) << "Registered failed"<< filename.m_c_em_name<< filename.m_c_em_id;
				continue;
			}
			//将ID放回给GalleryIndex,保存起来
			index = id;
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered success!", filename.m_c_em_name);
			LOG(INFO) << m_str_log;
		}
		else
		{
			//没有这张图片，就删掉这条记录
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  read image  failed!", filename.m_c_em_name);
			LOG(INFO) << m_str_log;
		}
	}
	//将注册的id和员工信息的结构对应起来--------------所以要更新一条新数据的话，要先把数据放到 GalleryImageFilename，然后，在GalleryIndex中增加一条记录，然后，Register-》然后GalleryIndexMap.insert-》在txt中增加一条信息
	GalleryIndexMap.clear();
	for (size_t i = 0; i < GalleryIndex.size(); ++i)
	{
		// save index and name pair
		if (GalleryIndex[i] < 0) continue; //如果上面注册失败的话，就可能会出现错误，因为GalleryIndex[i]返回值有可能相同,所以，这里，如果GalleryIndex[i] 小于0就不玩了
		GalleryIndexMap.insert(std::make_pair(GalleryIndex[i], GalleryImageFilename[i]));
	}
	return true;
}

bool CFaceDatabase::AddFaceInfoToDatabase(const char* c_em_id, const char* c_em_name, const char* c_file_name)
{
	Mat m_mat_regist_src;
	//应该先看看有没有这个图片
	ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
	sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s", m_c_normal_photo_path, c_file_name);
	m_mat_regist_src = cv::imread(m_c_photo_file_name);
	if (m_mat_regist_src.empty())  return false;
	//先查查这em_id是否在数据库中，如果在，就需要做更改，如果不在就要添加
	bool m_b_is_new_em_id = false;
	EM_ID_NAME  m_stu_em_id_name;
	
	char m_c_sql[1024];//查询语句
	int i = 0;
	int ncol = 0;
	char** array;
	ZeroMemory(m_c_sql, sizeof(m_c_sql));
	sprintf_s(m_c_sql, sizeof(m_c_sql), "select *  from  T_em_id_name where  em_id='%s';", c_em_id);
	int ret = executeWithQuery(conn, &array, &ncol, m_c_sql);
	if (ret == -1)
	{
		ZeroMemory(m_str_log, sizeof(m_str_log));
		sprintf_s(m_str_log,sizeof(m_str_log),"run->%s失败", m_c_sql);
		LOG(INFO) << m_str_log;
		return false;
	}
	else
	{
		if (array[ncol] == nullptr)
		{
			//说明没有这条记录-就写到数据库里
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
			//将信息放到队列------------------------------------------------------------------------------------------------------
			m_stu_em_id_name._init();
			//员工id
			strcpy_s(m_stu_em_id_name.m_c_em_id, sizeof(m_stu_em_id_name.m_c_em_id), c_em_id);
			//员工name
			strcpy_s(m_stu_em_id_name.m_c_em_name, sizeof(m_stu_em_id_name.m_c_em_name), c_em_name);
			//压入vector
			GalleryImageFilename.emplace_back(m_stu_em_id_name);
			m_b_is_new_em_id = true;//新增的
		}
	}
	//获取这个em_id在GalleryImageFilename中的位置
	int m_n_current_em_id_pos = find_em_id_from_GalleryImageFilename((char*)c_em_id);
	if (m_n_current_em_id_pos < 0)
	{
		//这个地方一定应该能找到，如果找不到，就不对了
		ZeroMemory(m_str_log, sizeof(m_str_log));
		sprintf_s(m_str_log, sizeof(m_str_log), "not find->%s in  find_em_id_from_GalleryImageFilename", c_em_id);
		LOG(INFO) << m_str_log;
		return false;
	}
	if (m_b_is_new_em_id == true)
	{
		//这个是新增分支
		 //因为是顺序增加的。所以，这里要从
		GalleryIndex.emplace_back(-1);
		int64_t& index = GalleryIndex[m_n_current_em_id_pos];
		LOG(INFO) << "Registering... " << c_em_name;
		//图像路径---读取图像
		seeta::cv::ImageData image = m_mat_regist_src;
		if (image.empty() == false)
		{
			//注册到人脸库，返回ID
			auto id = engine->Register(image);
			if (id < 0)
			{
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered failed!", c_em_id);
				LOG(INFO) << m_str_log;
				return false;
			}
			//将ID放回给GalleryIndex,保存起来
			index = id;
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered sucess!", c_em_name);
			LOG(INFO) << m_str_log;
			//注册到对应map
			GalleryIndexMap.insert(std::make_pair(GalleryIndex[m_n_current_em_id_pos], GalleryImageFilename[m_n_current_em_id_pos]));
			//保存照片用ID保存
			ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
			sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, c_em_id);
			imwrite(m_c_photo_file_name,m_mat_regist_src);
		}
	}
	else
	{
		//原来就有的。需要更新
	   //先删enginne中的信息
		int64_t& index = GalleryIndex[m_n_current_em_id_pos];
		//删掉这个index
		engine->Delete(index);
		//删掉GalleryIndexMap
		map<int64_t, EM_ID_NAME>::iterator iter;
		iter = GalleryIndexMap.find(index);
		if (iter != GalleryIndexMap.end())
		{
			GalleryIndexMap.erase(iter);
		}
		//然后重新注册
		seeta::cv::ImageData image = m_mat_regist_src;
		if (image.empty() == false)
		{
			//注册到人脸库，返回ID
			auto id = engine->Register(image);
			if (id < 0)
			{
				//m_str_log.Format("%s  Registered failed!", c_em_id);
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered failed!", c_em_id);
				LOG(INFO) << m_str_log;
				return false;
			}
			//将ID放回给GalleryIndex,保存起来
			index = id;
			//m_str_log.Format("%s  Registered success!", c_em_name);
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, sizeof(m_str_log), "%s  Registered success!", c_em_name);
			LOG(INFO) << m_str_log;
			//注册到对应map
			GalleryIndexMap.insert(std::make_pair(GalleryIndex[m_n_current_em_id_pos], GalleryImageFilename[m_n_current_em_id_pos]));
			//保存照片用ID保存
			ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
			sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, c_em_id);
			imwrite(m_c_photo_file_name, m_mat_regist_src);
			//日志
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
	//检测这个画面中的人脸
	faces = engine->DetectFaces(image);
	//按照人脸的面积降序排列，最大的是第一个元素
	if (faces.size() >= 2)
	{
		std::sort(faces.begin(), faces.end(), CFaceDatabase::GreaterSort);//降序排列
	}
	if (faces.empty())
	{
		//日志
		//m_str_log.Format("not  find  %s face", m_TFOriginalText.c_em_id);
		LOG(INFO) <<"can not find face ,can not cut face";
		return false;
	}
	//每张人脸框进行检测识别-----这个地方只检测第一元素，因为上面已经对verct降序排列啦
	for (SeetaFaceInfo& face : faces)
	{
		auto points = engine->DetectPoints(image, face);
		auto score = QA.evaluate(image, face.pos, points.data());
		if (score == 0)
		{
			return false;
		}
		//截取图像，然后以em_id保存文件
		cv::Rect rect(face.pos.x, face.pos.y, face.pos.width, face.pos.height);
		cv::Mat  ROI = mat_src_frame(rect);
		//保存裁剪后图片，格式:mycut.jpg
		ZeroMemory(m_c_photo_file_name, sizeof(m_c_photo_file_name));
		sprintf_s(m_c_photo_file_name, sizeof(m_c_photo_file_name), "%s%s.jpg", m_c_normal_photo_path, "mycut");
		if (cv::imwrite(m_c_photo_file_name, ROI) == false)
		{
			ZeroMemory(m_str_log, sizeof(m_str_log));
			sprintf_s(m_str_log, "save cut picture %s failed!", m_c_photo_file_name);
			LOG(INFO) << m_str_log;
			return false;
		}
		//写日志
		ZeroMemory(m_str_log, sizeof(m_str_log));
		sprintf_s(m_str_log, "save cut picture %s success", m_c_photo_file_name);
		LOG(INFO) << m_str_log;
		return true;
	}
	return true;
}

//返回值：0：找到了匹配的照片  -1：接收缓冲区为空  -2：原始Mat数据为空   -3:人脸质量不合格  -4：没有匹配的人脸 
int  CFaceDatabase::RecognizeMan(Mat srcMat, EM_ID_NAME* pOut)
{
	if (GalleryImageFilename.size() <= 0)  return -4;
	if (pOut==nullptr)  return -1;
	ZeroMemory(pOut, sizeof(EM_ID_NAME));
	seeta::cv::ImageData image = srcMat;// cv::imread(m_c_photo_file_name);
	if (image.empty() == true) return -2;
	//检测这个画面中的人脸
	auto faces = engine->DetectFaces(image);
	//按照人脸的面积降序排列，最大的是第一个元素
	if (faces.size() >= 2)  std::sort(faces.begin(), faces.end(), GreaterSort);//降序排列
	//每张人脸框进行检测识别-----这个地方只检测第一元素，因为上面已经对verct降序排列啦
	for (SeetaFaceInfo& face : faces)
	{
		// Query top 1
		int64_t index = -1;
		float similarity = 0;
		//获取监测点
		auto points = engine->DetectPoints(image, face);
		//检测下这个人脸的质量，如果人脸质量==0就忽略掉
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
			//从工程数据库获取这种人脸最大相似度的照片
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
				//得到了当前的人脸比对结果------em_id,执行结果(-1:失败    0：成功)
				*pOut = GalleryIndexMap[index];
				ZeroMemory(m_str_log, sizeof(m_str_log));
				sprintf_s(m_str_log, sizeof(m_str_log), "match face:Id:%s  name:%s", pOut->m_c_em_id, pOut->m_c_em_name);
				LOG(INFO) << m_str_log;
				//找到的后就不玩了
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
	//获取字节长度   
	i_length = WideCharToMultiByte(CP_ACP, 0, buf, -1, nullptr, 0, nullptr, nullptr);
	//将wchar_t值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, buf, -1, str, i_length, nullptr, nullptr);
	return true;
}