#ifndef __SN1V2_error____h__
#define __SN1V2_error____h__

#include "SN1V2_rtConfig.h"
/*error code*/
	enum ERR_STA {
		err_ok = 0,//正确执行
		err_out_of_memory = -1,//内存不足
		err_cannot_read_file = -2,//无法读取文件
		err_inval_path = -3,//无效路劲
		err_cannot_open_file = -4,//无法打开文件
		err_cannot_write_file = -5,//无法写入
		err_inval_para = -6,//参数错误
		err_invaild_memory_access = -7,//内存访问错误
		//文本模块
		err_txt_part_error = -100,//文本部分异常
		err_conf_tran_error = -101,//配置文件部分转化异常
		err_conf_get_null= -102,//没有该配置参数
		//图像模块
		err_sensor_open = -200,//启动异常
		err_sensor_close = -201,//关闭异常
		err_sensor_set = -202,//设置异常
		err_sensor_catch = -203,//拍摄异常
		err_JPG_encode = -204,//jpg转换异常
		err_Inval_image = -205,//无效的图像数据
		err_binaryzation_aim_null = -206,//二值化目标丢失
		err_binaryzation_process = -207,//二值化处理异常
		err_Image_filter = -208,//图像筛选异常
		err_diff_cal = -209,//偏差计算异常
		err_Image_type_undefined = -210,//图像类型未定义
		err_Image_type_unsupported = -211,//不支持的图像
		err_Image_bound_beyond_region_err = -212,//图像边界问题 亮点边界大于最大于规定范围
		err_Image_clone_bound_err = -213,//图像复制边界错误

		err_sensor_set_get_conf=-214,//无法获取设置信息
		err_sensor_set_set_conf=-215,//无法设置图像信息
		err_sensor_set_err_par=-216,//设置参数错误
		//数据传输
		err_port_send = -300,//发送数据异常
		err_port_rec = -301,//接收数据异常
		//时间
		err_tim_sync = -400 ,//时间同步异常
		err_tim_analysis_error = -401,//时间解析异常
		err_tim_obt = -402,//时间获取异常
		err_dly_long = -403,//延时过长
		err_dly_short = -404,//延时不足
		err_tim_analysis_not_support = -405,//该解析方式不支持
		err_tim_set_outOfRange = -406,//超出可设置范围
		err_tim_set_error = -407,//时间设置出错
		err_tim_tm_transfer_error=-408,//字符串转struct tm错误
		err_tim_data_error=-409,//不正确的时间数据
		err_cre_today_saved=-410,//今日的CRE数据已经采集完毕
		err_cre_get_NAN=-411,//计算的cre数据中含有nan值
		//运营表
		err_tab_cal = -500,//运营表计算异常
		//校正
		err_data_filter_day_break = -600,
		err_data_filter_NULL_RES = -601,
		//定日镜
		err_Heli_not_ready = -700,//定日镜未准备完成
		//其他
		err_UNKNOWN = -10000,//未知错误
	};




#endif

