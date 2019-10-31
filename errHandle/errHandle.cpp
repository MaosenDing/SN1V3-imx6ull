#include "errHandle/errHandle.h"
#include <glog/logging.h>
#include <glog/log_severity.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sys/prctl.h>
static void SigHandle(const char * data, int insize)
{
	//std::fstream fs("glog_dump.log", std::ios::app);
	//fs.write(data, insize);
	//fs.close();
	//LOG(ERROR) << data;
	char pname[64];
	prctl(PR_GET_NAME, pname);

	LOG(ERROR)<< "-------------segment fault-------------";
	LOG(ERROR) <<"thread :" << pname;
	LOG(ERROR).write(data, insize);
}


int logInit(const char * LogName, const char * SavePath, int setLevel)
{
	//����log�ļ���
	mkdir(SavePath, 0777);
	//log��ʼ��
	google::InitGoogleLogging(LogName);

	FLAGS_colorlogtostderr = true;//�����������Ļ����־��ʾ��Ӧ��ɫ

								  //FLAGS_servitysinglelog = true;// �������յȼ�����log�ļ�
	FLAGS_logbufsecs = 0;//������־�����Ĭ��Ϊ30�룬�˴���Ϊ�������
	FLAGS_max_log_size = 1; //�����־��СΪ 100MB
	FLAGS_stop_logging_if_full_disk = true;//�����̱�д��ʱ��ֹͣ��־���
	google::SetStderrLogging(google::GLOG_INFO); //���ü��� ���� google::INFO ����־ͬʱ�������Ļ
	google::InstallFailureSignalHandler();
	google::InstallFailureWriter(&SigHandle);
	//��־���ƺ������ַ
	char Info[50] = { 0 };
	char Warn[50] = { 0 };
	char Error[50] = { 0 };
	char Fatal[50] = { 0 };

	strcpy(Info, SavePath);
	strcpy(Warn, SavePath);
	strcpy(Error, SavePath);
	strcpy(Fatal, SavePath);

	strcat(Info, "/Info_");
	strcat(Warn, "/Warning_");
	strcat(Error, "/Error_");
	strcat(Fatal, "/Fatal_");

	switch (setLevel) {
	case google::GLOG_INFO:
		google::SetLogDestination(google::GLOG_INFO, Info);
	case	google::GLOG_WARNING:
		google::SetLogDestination(google::GLOG_WARNING, Warn);
	case		google::GLOG_ERROR:
		google::SetLogDestination(google::GLOG_ERROR, Error);
	case		google::GLOG_FATAL:
		google::SetLogDestination(google::GLOG_FATAL, Fatal);
	default:
		break;
	}
	return 0;
}

