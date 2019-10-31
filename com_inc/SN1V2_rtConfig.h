#ifndef __SN1V2__runtimecfg__H___
#define __SN1V2__runtimecfg__H___


#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		int   thres;//�Ҷ���ֵ
		float   thresPer;//�Ҷ���ֵ�ٷֱ�
		float   Lon;//����
		float   lati;//γ��
		float   qualityThres;//Բ����ֵ
		float   SSA;//��ʼ�Ƕ�
		float   SHT;//���վ���������
		float   SPT;//���վ�����ǰ��ʱ��
		float   TDT;//�Ƕ�
		float   SCH;//
		float   TEE;//
		float   TEP;
		float   TET;
		float   SFL;//����
		float   SPS;//��Ԫ��С
		int SID;//�豸ID
		int FLAG_SAVE_ORG;//ԭʼͼ���Ƿ񱣴�
		int FLAG_SAVE_BIN;//������ͼ���Ƿ񱣴�
		int IMG_WIDTH;//ͼ����
		int IMG_HEIGTH;//ͼ��߶�
		unsigned int gain;//����
		unsigned int expo;//�ع�

		int isHorisFlip; //ˮƽ��ת
		int isVeriFlip;//��ֱ��ת

		unsigned int MinCntGrp;//��С������
		unsigned int SaveTimeTable;//������Ӫ��
		unsigned int CleanLastDate;//����ϴ�����

		float bfnoon_thres;//����������ֵ�ٷֱ�
		float afnoon_thres;//����������ֵ�ٷֱ�

		int bf_size;//����Ԥ�ɼ�����
		int af_size;//����Ԥ�ɼ�����

		int max_reserve_time;//�����ʱ��

		int IsSaveCre;//����CRE ��ʽΪ1 ĳЩ����Ϊ0

		//ʱ������ʾ����Сʱ���� 0~24 -1��ʾ��������
		int limitHourBeforeNoon;
		int limitHourAfterNoon;

	}SN1_CFG;

#define SHARE_KEY_PATH "/"
#define SHARE_KEY_INT 0x2018
	//share memory type
#include <sys/time.h>

	typedef struct {
		enum MDC_STATUS {
			MDC_TIME_OK = (0x8888),
			MDC_TIME_FALSE = (0),
		}mdc_flag;

		enum HELO_STATUS {
			Helo_ok = (0x7777),
			Helo_not_ready = (0),
		}helo_status;

		int last_error_code;
		int error_count;

		int max_time_out_second;//define max time out for mdc

		long    first_tv_sec;         /* seconds */
		long    first_tv_usec;        /* and microseconds */

		long    last_tv_sec;         /* seconds */
		long    last_tv_usec;        /* and microseconds */

		int year, mon, day;
		
		int pid_mdc;
		int pid_aim;

		int mdc_id_num;

		char justformemory[1024];
	}SN1_SHM;

#define MDC_MAX_TIME_OUT_SECOND (30)

#define Transimit_img_Path ("/tmp/transmit_img")

#ifdef __cplusplus
}
#endif






#endif
