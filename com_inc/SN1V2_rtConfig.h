#ifndef __SN1V2__runtimecfg__H___
#define __SN1V2__runtimecfg__H___


#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		int   thres;//灰度阈值
		float   thresPer;//灰度阈值百分比
		float   Lon;//经度
		float   lati;//纬度
		float   qualityThres;//圆度阈值
		float   SSA;//初始角度
		float   SHT;//定日镜调节周期
		float   SPT;//定日镜调节前置时间
		float   TDT;//角度
		float   SCH;//
		float   TEE;//
		float   TEP;
		float   TET;
		float   SFL;//焦距
		float   SPS;//像元大小
		int SID;//设备ID
		int FLAG_SAVE_ORG;//原始图像是否保存
		int FLAG_SAVE_BIN;//二进制图像是否保存
		int IMG_WIDTH;//图像宽度
		int IMG_HEIGTH;//图像高度
		unsigned int gain;//增益
		unsigned int expo;//曝光

		int isHorisFlip; //水平翻转
		int isVeriFlip;//竖直翻转

		unsigned int MinCntGrp;//最小像素团
		unsigned int SaveTimeTable;//保存运营表
		unsigned int CleanLastDate;//清楚上次数据

		float bfnoon_thres;//上午数量阈值百分比
		float afnoon_thres;//下午数量阈值百分比

		int bf_size;//上午预采集总数
		int af_size;//下午预采集总数

		int max_reserve_time;//最长保留时间

		int IsSaveCre;//生成CRE 正式为1 某些测试为0

		//时间表的启示结束小时限制 0~24 -1表示不受限制
		int limitHourBeforeNoon;
		int limitHourAfterNoon;

		char ForceSavePath[30];//强制保存路径

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
