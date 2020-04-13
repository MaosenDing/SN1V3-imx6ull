#include "sn1v3cfg.h"
#include <string.h>
#include <stdint.h>
#include <algorithm>

//定义和类型参考《LOC无线定日镜控制系统设计要求》

using namespace std;

void stingDefault(void * addr)
{
	char * pos = (char *)addr;
	*pos = 0;
}

void floatDefault(void * addr)
{
	float * pos = (float *)addr;
	*pos = 0;
}

void doubleDefault(void * addr)
{
	double * pos = (double *)addr;
	*pos = 0;
}

void int32Default(void * addr)
{
	int32_t * pos = (int32_t *)addr;
	*pos = 0;
}

void boolDefault(void * addr)
{
	int * pos = (int *)addr;
	*pos = 0;
}

void IPServerDefault(void * addr)
{	
	//服务器默认ip 192.168.50.205
	const char serverip[4] = { 192,168,50,205};

	char * pos = (char *)addr;
	reverse_copy(serverip, serverip + 4, pos);
}

void PortServerDefault(void * addr)
{
	//服务器默认端口 808
	int32_t * port = (int32_t *)addr;
	*port = 808;
}


void IPLocalDefault(void * addr)
{
	//本地默认ip 192.168.50.111
	const char localip[4] = { 192,168,50,111 };

	char * pos = (char *)addr;
	reverse_copy(localip, localip + 4, pos);
}

void MACDefault(void * addr)
{
	//本地默认ip 192.168.50.111
	const char localip[6] = {0x11,0x22,0x33,0x44,0x55,0xff};

	char * pos = (char *)addr;
	copy(localip, localip + 6, pos);
}


const CFG_INFO T1[] = {
	T1_Set_data(1,HeliostatID,dateType::STRING16,stingDefault),
	T1_Set_data(2,ReducerM,dateType::STRING16,stingDefault),
	T1_Set_data(3,PutterM,dateType::STRING16,stingDefault),
	T1_Set_data(4,DriveM,dateType::STRING16,stingDefault),
	T1_Set_data(5,FWSensorM,dateType::STRING16,stingDefault),
	T1_Set_data(6,FYSensorM,dateType::STRING16,stingDefault),
	T1_Set_data(7,InstallTime,dateType::TIM16,nullptr),
	T1_Set_data(8,PSN,dateType::MAC,MACDefault),
	T1_Set_data(9,MDC2_SV_1,dateType::STRING16,stingDefault),
	T1_Set_data(10,MDC2_SV_2,dateType::STRING16,stingDefault),
	T1_Set_data(11,MDC2_SV_3,dateType::STRING16,stingDefault),
	T1_Set_data(12,MDC2_SV_4,dateType::STRING16,stingDefault),
	T1_Set_data(13,MDC2_SV_5,dateType::STRING16,stingDefault),
	T1_Set_data(14,DRI1_SV,dateType::STRING16,stingDefault),
	T1_Set_data(15,DRI2_SV,dateType::STRING16,stingDefault),
	T1_Set_data(16,Wiresless_SV,dateType::STRING16,stingDefault),
	T1_Set_data(17,HeliostatType,dateType::STRING16,stingDefault),
	T1_Set_data(18,SN2ID,dateType::STRING16,stingDefault),
	T1_Set_data(19,latitude,dateType::FLOAT32,floatDefault),
	T1_Set_data(20,longtitude,dateType::FLOAT32,floatDefault),
	T1_Set_data(21,elevation,dateType::FLOAT32,floatDefault),
	T1_Set_data(22,Temperature,dateType::FLOAT32,floatDefault),
	T1_Set_data(23,Pressure,dateType::FLOAT32,floatDefault),
	T1_Set_data(24,DeltaT,dateType::FLOAT32,floatDefault),
	T1_Set_data(25,HeliostatPointX,dateType::FLOAT32,floatDefault),
	T1_Set_data(26,HeliostatPointY,dateType::FLOAT32,floatDefault),
	T1_Set_data(27,HeliostatPointZ,dateType::FLOAT32,floatDefault),
};

const CFG_INFO T2[] = {
	T2_Set_data(1,FWStartSpeed,dateType::INT32,int32Default),
	T2_Set_data(2,FWMaxSpeed,dateType::INT32,int32Default),
	T2_Set_data(3,FYStartSpeed,dateType::INT32,int32Default),
	T2_Set_data(4,FYMaxSpeed,dateType::INT32,int32Default),
	T2_Set_data(5,FWReductRt,dateType::INT32,int32Default),
	T2_Set_data(6,PutterReductRt,dateType::INT32,int32Default),
	T2_Set_data(7,PutterScrewPara,dateType::INT32,int32Default),
	T2_Set_data(8,PutterMaxLen,dateType::INT32,int32Default),
	T2_Set_data(9,PutterPara4,dateType::INT32,int32Default),
	T2_Set_data(10,ServoControlCycle,dateType::INT32,int32Default),
	T2_Set_data(11,CompesatPara,dateType::INT32,int32Default),
	T2_Set_data(12,MaxMotorCurr,dateType::INT32,int32Default),
};



const CFG_INFO T3[] = {
	//sn1
	T3_Set_data(1,SN1_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(2,SN1_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(3,SN1_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(4,SN1_e4,dateType::FLOAT32,floatDefault),
	T3_Set_data(5,SN1_e5,dateType::FLOAT32,floatDefault),
	T3_Set_data(6,SN1_e6,dateType::FLOAT32,floatDefault),
	T3_Set_data(7,SN1_e7,dateType::FLOAT32,floatDefault),
	T3_Set_data(8,SN1_e8,dateType::FLOAT32,floatDefault),
	T3_Set_data(9,SN1_e9,dateType::FLOAT32,floatDefault),
	T3_Set_data(10,SN1_e10,dateType::FLOAT32,floatDefault),
	T3_Set_data(11,SN1_e11,dateType::FLOAT32,floatDefault),
	T3_Set_data(12,SN1_e12,dateType::FLOAT32,floatDefault),
	T3_Set_data(13,SN1_e13,dateType::FLOAT32,floatDefault),
	T3_Set_data(14,SN1_e14,dateType::FLOAT32,floatDefault),
	T3_Set_data(15,SN1_e15,dateType::BOOLTYPE,boolDefault),
	//sn2
	T3_Set_data(16,SN2_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(17,SN2_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(18,SN2_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(19,SN2_e4,dateType::FLOAT32,floatDefault),
	T3_Set_data(20,SN2_e5,dateType::FLOAT32,floatDefault),
	T3_Set_data(21,SN2_e6,dateType::FLOAT32,floatDefault),
	T3_Set_data(22,SN2_e7,dateType::FLOAT32,floatDefault),
	T3_Set_data(23,SN2_e8,dateType::FLOAT32,floatDefault),
	T3_Set_data(24,SN2_e9,dateType::FLOAT32,floatDefault),
	T3_Set_data(25,SN2_e10,dateType::FLOAT32,floatDefault),
	T3_Set_data(26,SN2_e11,dateType::FLOAT32,floatDefault),
	T3_Set_data(27,SN2_e12,dateType::FLOAT32,floatDefault),
	T3_Set_data(28,SN2_e13,dateType::FLOAT32,floatDefault),
	T3_Set_data(29,SN2_e14,dateType::FLOAT32,floatDefault),
	//sn3
	T3_Set_data(30,SN3_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(31,SN3_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(32,SN3_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(33,SN3_e4,dateType::FLOAT32,floatDefault),
	T3_Set_data(34,SN3_e5,dateType::FLOAT32,floatDefault),
	T3_Set_data(35,SN3_e6,dateType::FLOAT32,floatDefault),
	T3_Set_data(36,SN3_e7,dateType::BOOLTYPE,boolDefault),
	//零位
	T3_Set_data(37,OLW_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(38,OLW_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(39,OLW_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(40,OLW_e4,dateType::BOOLTYPE,boolDefault),
};




const CFG_INFO T4[] = {
	T4_Set_data(1,M_AP1,dateType::STRING32,stingDefault),
	T4_Set_data(2,B_AP2,dateType::STRING32,stingDefault),
	T4_Set_data(3,D_AP0,dateType::STRING32,stingDefault),

	T4_Set_data(4,CM_Group,dateType::INT32,int32Default),
	T4_Set_data(5,CM_Times,dateType::INT32,int32Default),
	T4_Set_data(6,CM_Cycle,dateType::INT32,int32Default),
	T4_Set_data(7,AP_SS,dateType::INT32,int32Default),
	T4_Set_data(8,Baudrate,dateType::INT32,int32Default),
	T4_Set_data(9,TransmitPower,dateType::INT32,int32Default),

	T4_Set_data(10,ServerIP,dateType::IP,IPServerDefault),
	T4_Set_data(11,ServerPort,dateType::INT32,setDefault(int,8888)),
	T4_Set_data(12,LocalIP,dateType::IP,IPLocalDefault),
	T4_Set_data(13,Channel,dateType::INT32,setDefault(int,0)),
	T4_Set_data(14,SinglePacketMax,dateType::INT32,int32Default),
};

const CFG_INFO T6[] = {
	T6_Set_data(1,SN1_P1,dateType::INT32,setDefault(int,1200)),
	T6_Set_data(2,SN1_P2,dateType::INT32,setDefault(int ,1600)),
	T6_Set_data(3,SN1_P3,dateType::FLOAT32,setDefault(float ,0)),
	T6_Set_data(4,SN1_P4,dateType::FLOAT32,setDefault(float ,0)),
	T6_Set_data(5,SN1_P5,dateType::INT32,setDefault(int ,20)),//增益
	T6_Set_data(6,SN1_P6,dateType::INT32,setDefault(int ,100)),//曝光
	T6_Set_data(7,SN1_P7,dateType::BOOLTYPE,boolDefault),//水平翻转
	T6_Set_data(8,SN1_P8,dateType::BOOLTYPE,boolDefault),//竖直翻转
	T6_Set_data(9,SN1_P9,dateType::INT32,setDefault(int ,200)),//灰度阈值
	T6_Set_data(10,SN1_P10,dateType::FLOAT32,setDefault(float ,0.8)),//二值化阈值
	T6_Set_data(11,SN1_P11,dateType::FLOAT32,setDefault(float ,0.7)),//圆度阈值
	T6_Set_data(12,SN1_P12,dateType::FLOAT32,setDefault(float ,0.8)),//上午有效百分比
	T6_Set_data(13,SN1_P13,dateType::FLOAT32,setDefault(float ,0.8)),//下午有效百分比
	T6_Set_data(14,SN1_P14,dateType::FLOAT32,setDefault(float ,20)),//太阳高度角阈值
	T6_Set_data(15,SN1_P15,dateType::INT32,boolDefault),//主动修正标志  bool
	T6_Set_data(16,SN1_P16,dateType::INT32,boolDefault),//校正标志位 bool
	T6_Set_data(17,SN1_P17,dateType::INT32,setDefault(int ,10)),//采集间隔时间
	T6_Set_data(18,SN1_P18,dateType::FLOAT32,setDefault(float ,0.95)),//1mrad
	T6_Set_data(19,SN1_P19,dateType::FLOAT32,setDefault(float ,0.95)),//2mrad


	T6_Set_data(19,SN2_P1,dateType::FLOAT32,floatDefault),//目标sn3相机坐标x
	T6_Set_data(20,SN2_P2,dateType::FLOAT32,floatDefault),//目标sn3相机坐标y
	T6_Set_data(21,SN2_P3,dateType::FLOAT32,floatDefault),//目标sn3相机坐标z
	T6_Set_data(22,SN2_P4,dateType::INT32,setDefault(int ,1)),//SN2用T8表频率(行/秒)
	T6_Set_data(23,SN2_P5,dateType::FLOAT32,setDefault(float ,20)),//SN2用最大转角速度（度/秒）
	T6_Set_data(24,SN2_P6,dateType::FLOAT32,setDefault(float ,1)),//SN2用法线最大扫描距离（米）

	T6_Set_data(25,SaveOrg,dateType::BOOLTYPE,boolDefault),
	T6_Set_data(26,SaveBin,dateType::BOOLTYPE,boolDefault),
	T6_Set_data(27,SavePath,dateType::STRING64,stingDefault),
};


const CFG_GROUP cfg_group[] = {
	Set_Group(T1,Mask_T1,0,1),
	Set_Group(T2,Mask_T2,1,2),
	Set_Group(T3,Mask_T3,2,3),
	Set_Group(T4,Mask_T4,3,4),
	Set_Group(T6,Mask_T6,4,6),
};

size_t max_group_cnt()
{
	return sizeof(cfg_group) / sizeof(CFG_GROUP);
}


const CFG_GROUP * find_group_index(size_t i)
{
	if (i < max_group_cnt()) {
		return &cfg_group[i];
	}
	return nullptr;
}

const CFG_GROUP * find_group_name(const char * groupName)
{
	if (!groupName) {
		return nullptr;
	}
	for (size_t i = 0; i < max_group_cnt(); i++) {
		if (!strcmp(cfg_group[i].groupName, groupName)) {
			return &cfg_group[i];
		}
	}
	return nullptr;
}



