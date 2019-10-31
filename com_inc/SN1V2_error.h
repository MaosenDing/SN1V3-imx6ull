#ifndef __SN1V2_error____h__
#define __SN1V2_error____h__

#include "SN1V2_rtConfig.h"
/*error code*/
	enum ERR_STA {
		err_ok = 0,//��ȷִ��
		err_out_of_memory = -1,//�ڴ治��
		err_cannot_read_file = -2,//�޷���ȡ�ļ�
		err_inval_path = -3,//��Ч·��
		err_cannot_open_file = -4,//�޷����ļ�
		err_cannot_write_file = -5,//�޷�д��
		err_inval_para = -6,//��������
		err_invaild_memory_access = -7,//�ڴ���ʴ���
		//�ı�ģ��
		err_txt_part_error = -100,//�ı������쳣
		err_conf_tran_error = -101,//�����ļ�����ת���쳣
		err_conf_get_null= -102,//û�и����ò���
		//ͼ��ģ��
		err_sensor_open = -200,//�����쳣
		err_sensor_close = -201,//�ر��쳣
		err_sensor_set = -202,//�����쳣
		err_sensor_catch = -203,//�����쳣
		err_JPG_encode = -204,//jpgת���쳣
		err_Inval_image = -205,//��Ч��ͼ������
		err_binaryzation_aim_null = -206,//��ֵ��Ŀ�궪ʧ
		err_binaryzation_process = -207,//��ֵ�������쳣
		err_Image_filter = -208,//ͼ��ɸѡ�쳣
		err_diff_cal = -209,//ƫ������쳣
		err_Image_type_undefined = -210,//ͼ������δ����
		err_Image_type_unsupported = -211,//��֧�ֵ�ͼ��
		err_Image_bound_beyond_region_err = -212,//ͼ��߽����� ����߽��������ڹ涨��Χ
		err_Image_clone_bound_err = -213,//ͼ���Ʊ߽����

		err_sensor_set_get_conf=-214,//�޷���ȡ������Ϣ
		err_sensor_set_set_conf=-215,//�޷�����ͼ����Ϣ
		err_sensor_set_err_par=-216,//���ò�������
		//���ݴ���
		err_port_send = -300,//���������쳣
		err_port_rec = -301,//���������쳣
		//ʱ��
		err_tim_sync = -400 ,//ʱ��ͬ���쳣
		err_tim_analysis_error = -401,//ʱ������쳣
		err_tim_obt = -402,//ʱ���ȡ�쳣
		err_dly_long = -403,//��ʱ����
		err_dly_short = -404,//��ʱ����
		err_tim_analysis_not_support = -405,//�ý�����ʽ��֧��
		err_tim_set_outOfRange = -406,//���������÷�Χ
		err_tim_set_error = -407,//ʱ�����ó���
		err_tim_tm_transfer_error=-408,//�ַ���תstruct tm����
		err_tim_data_error=-409,//����ȷ��ʱ������
		err_cre_today_saved=-410,//���յ�CRE�����Ѿ��ɼ����
		err_cre_get_NAN=-411,//�����cre�����к���nanֵ
		//��Ӫ��
		err_tab_cal = -500,//��Ӫ������쳣
		//У��
		err_data_filter_day_break = -600,
		err_data_filter_NULL_RES = -601,
		//���վ�
		err_Heli_not_ready = -700,//���վ�δ׼�����
		//����
		err_UNKNOWN = -10000,//δ֪����
	};




#endif

