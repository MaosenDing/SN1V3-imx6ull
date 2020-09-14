#include "errHandle/errHandle.h"
#include <glog/logging.h>
#include <glog/log_severity.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <sys/prctl.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;

const char * FileFailOut = "Log dump.txt";
static int FileFailOutfd = -1;
static int errorFlag = 0;
static void SigHandle(const char * data, int insize)
{
	write(FileFailOutfd, data, insize);

	if (errorFlag == 0) {
		errorFlag = 1;
		const int MAX_CALLSTACK_DEPTH = 32;
		void *traceback[MAX_CALLSTACK_DEPTH];
		int depth = backtrace(traceback, MAX_CALLSTACK_DEPTH);
		backtrace_symbols_fd(traceback, depth, FileFailOutfd);
	}
}


static void myerrorInit(void)
{
	FileFailOutfd = open(FileFailOut, O_APPEND | O_WRONLY | O_CREAT, 0666);
	if (FileFailOutfd < 0) {
		cout << "open error " << endl;
		perror("111");
	}
}




int logInit(const char * LogName, const char * SavePath, int setLevel)
{
	//创建log文件夹
	mkdir(SavePath, 0777);
	//log初始化
	google::InitGoogleLogging(LogName);

	FLAGS_colorlogtostderr = true;//设置输出到屏幕的日志显示相应颜色
	FLAGS_timestamp_in_logfile_name = false;
								  //FLAGS_servitysinglelog = true;// 用来按照等级区分log文件
	FLAGS_logbufsecs = 0;//缓冲日志输出，默认为30秒，此处改为立即输出
	FLAGS_max_log_size = 1; //最大日志大小为 100MB
	FLAGS_stop_logging_if_full_disk = true;//当磁盘被写满时，停止日志输出
	google::SetStderrLogging(google::GLOG_INFO); //设置级别 高于 google::INFO 的日志同时输出到屏幕
	myerrorInit();
	google::InstallFailureSignalHandler();
	google::InstallFailureWriter(&SigHandle);
	//日志名称和输出地址
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

