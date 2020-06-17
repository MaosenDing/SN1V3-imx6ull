#ifndef __sn1v3cfg__H___
#define __sn1v3cfg__H___


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

	//定义和类型参考《LOC无线定日镜控制系统设计要求》

	enum dateType
	{
		STRING16,
		STRING32,
		STRING64,
		STRING128,

		FLOAT32,
		DOUBLE64,

		INT32,

		TIM16,
		MAC,
		MAC4,

		BOOLTYPE,
		IP,
	};

	enum DATASTATUS {
		dataInit = 0,//数据未初始化
		dataFromTable = 1,//数据从表初始化
		dataTransFaultDefault = 2,//数据转化失败 默认初始化
		dataScanfNullSetDefault = 3,//数据空  默认初始化
		dataChanged = 4,//数据已修改
	};

	//写出标志位
	enum WRITEOUTMETHOD {
		writeScanf = 1 << 0,//写出扫描的部分
		writeDefault = 1 << 1,//写出默认部分
		writeChanged = 1 << 2,//写出修改部分

		writeDataNote = 1 << 3,//写出注释

		writeUseful = writeScanf | writeChanged,
		writeAll = writeScanf | writeDefault | writeChanged | writeDataNote,
	};


	typedef struct {
		const char * name;
		size_t diff;
		dateType typ;
		void(*default_value)(void *);
		size_t SeqInTable;
		DATASTATUS dataStatus;
	}CFG_INFO;

#define Set_data(table,member,typ,def_method,seq)    {#member,offsetof(table,member),typ,def_method,seq,dataInit}

	typedef struct {
		char HeliostatID[16];
		char ReducerM[16];
		char PutterM[16];
		char DriveM[16];
		char FWSensorM[16];
		char FYSensorM[16];
		int InstallTime[5];//年月日时分
		char PSN_MAC[4];
		char MDC2_SV_1[16];
		char MDC2_SV_2[16];
		char MDC2_SV_3[16];
		char MDC2_SV_4[16];
		char MDC2_SV_5[16];
		char DRI1_SV[16];
		char DRI2_SV[16];
		char Wiresless_SV[16];
		char HeliostatType[16];
		char SN2ID[16];
		float latitude;
		float longtitude;
		float elevation;
		float Temperature;
		float Pressure;
		float DeltaT;
		float HeliostatPointX;
		float HeliostatPointY;
		float HeliostatPointZ;
	}T1_table;


	typedef struct {
		int32_t FWStartSpeed;
		int32_t FWMaxSpeed;
		int32_t FYStartSpeed;
		int32_t FYMaxSpeed;
		int32_t FWReductRt;
		int32_t PutterReductRt;
		int32_t PutterScrewPara;
		int32_t PutterMaxLen;
		int32_t PutterPara4;
		int32_t ServoControlCycle;
		int32_t CompesatPara;
		int32_t MaxMotorCurr;
	}T2_table;



	typedef struct {
		//sn1
		float SN1_e1;
		float SN1_e2;
		float SN1_e3;
		float SN1_e4;
		float SN1_e5;
		float SN1_e6;
		float SN1_e7;
		float SN1_e8;
		float SN1_e9;
		float SN1_e10;
		float SN1_e11;
		float SN1_e12;
		float SN1_e13;
		float SN1_e14;
		int SN1_e15;
		//sn2
		float SN2_e1;
		float SN2_e2;
		float SN2_e3;
		float SN2_e4;
		float SN2_e5;
		float SN2_e6;
		float SN2_e7;
		float SN2_e8;
		float SN2_e9;
		float SN2_e10;
		float SN2_e11;
		float SN2_e12;
		float SN2_e13;
		float SN2_e14;
		//sn3
		float SN3_e1;
		float SN3_e2;
		float SN3_e3;
		float SN3_e4;
		float SN3_e5;
		float SN3_e6;
		int SN3_e7;
		//零位
		float OLW_e1;
		float OLW_e2;
		float OLW_e3;
		int OLW_e4;
	}T3_table;


	typedef struct {
		char M_AP1[32];
		char M_PASS[32];

		char B_AP2[32];
		char B_PASS[32];

		char D_AP0[32];
		char D_PASS[32];
		int32_t CM_Group;
		int32_t CM_Times;
		int32_t CM_Cycle;
		int32_t AP_SS;
		int32_t Baudrate;
		int32_t TransmitPower;
		//ip在内存中使用小端格式 即
		//5 50 168 192表示192.168.50.5
		char ServerIP[4];
		int32_t ServerPort;
		char LocalIP[4];
		int32_t Channel;
		int32_t SinglePacketMax;
	}T4_table;


	typedef struct {
		int32_t SN1_P1;//行像素
		int32_t SN1_P2;//列像素
		float SN1_P3;//像元大小
		float SN1_P4;//焦距
		int32_t SN1_P5;//增益
		int32_t SN1_P6;//曝光
		int32_t SN1_P7;//水平翻转 true/false
		int32_t SN1_P8;//竖直翻转 true/false
		int32_t SN1_P9;//灰度阈值
		float SN1_P10;//二值化阈值
		float SN1_P11;//圆度阈值
		float SN1_P12;//上午有效数据百分比阈值
		float SN1_P13;//下午有效数据百分比阈值
		float SN1_P14;//图像采集用 太阳高度角阈值？
		int32_t SN1_P15;//主动修正标志位？ true/false
		int32_t SN1_P16;//校正标志位 true/false

		int32_t SN1_P17;//采集间隔时间
		float SN1_P18;//《=1mrad占比
		float SN1_P19;//《=2mrad占比
		float SN2_P1;//目标sn3相机坐标x
		float SN2_P2;//目标sn3相机坐标y
		float SN2_P3;//目标sn3相机坐标z

		int32_t SN2_P4;//SN2用T8表频率(行/秒)
		int32_t SN2_P5;//SN2用最大转角速度（度/秒）
		int32_t SN2_P6;//SN2用法线最大扫描距离（米）

		int SaveOrg; //是否保留原视图片
		int SaveBin;//是否保留二值化图片
		char SavePath[64];//图片保持目录
	}T6_table;

#define T1_Set_data(seq,member,typ,def_method)      Set_data(T1_table,member,typ,def_method,seq)
#define T2_Set_data(seq,member,typ,def_method)      Set_data(T2_table,member,typ,def_method,seq)
#define T3_Set_data(seq,member,typ,def_method)      Set_data(T3_table,member,typ,def_method,seq)
#define T4_Set_data(seq,member,typ,def_method)      Set_data(T4_table,member,typ,def_method,seq)
#define T6_Set_data(seq,member,typ,def_method)      Set_data(T6_table,member,typ,def_method,seq)

#define setDefault(type ,val) [](void *addr){*(type*)addr = val;}
#define setDefaultString(str,maxlen) [](void *addr){strncpy((char *)addr, str, maxlen);}

	typedef struct {
		int scanftrueFlg[5] = {0};
		T1_table T1;
		T2_table T2;
		T3_table T3;
		T4_table T4;
		T6_table T6;
	}Tg_table;
	
#define Set_Group(group,mask,seq,TabelIndex)    {#group,#group".txt",mask,offsetof(Tg_table,group),group,sizeof(group)/sizeof(CFG_INFO),seq,TabelIndex}
	//扫描标志位
	enum CFGMASK {
		Mask_T1 = 1 << 1,
		Mask_T2 = 1 << 2,
		Mask_T3 = 1 << 3,
		Mask_T4 = 1 << 4,
		Mask_T6 = 1 << 5,
		Mask_All = Mask_T1 | Mask_T2| Mask_T3| Mask_T4| Mask_T6,
	};

	typedef struct {
		const char * groupName;
		const char * cfgName;
		const int cfgMask;
		const size_t diff;
		CFG_INFO * const group;
		const size_t sz;
		const int seq;
		const int cfgindex;
	}CFG_GROUP;

	size_t max_group_cnt();
	const CFG_GROUP * find_group_index(size_t i);
	const CFG_GROUP * find_group_name(const char * groupName);

#ifdef __cplusplus
}
#endif

#endif
