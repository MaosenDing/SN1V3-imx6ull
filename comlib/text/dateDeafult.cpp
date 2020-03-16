#include "sn1v3cfg.h"
#include <string.h>
#include <stdint.h>
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



const CFG_INFO T1[] = {
	T1_Set_data(HeliostatID,dateType::STRING16,stingDefault),
	T1_Set_data(ReducerM,dateType::STRING16,stingDefault),
	T1_Set_data(PutterM,dateType::STRING16,stingDefault),
	T1_Set_data(DriveM,dateType::STRING16,stingDefault),
	T1_Set_data(SensorM,dateType::STRING16,stingDefault),
	T1_Set_data(InstallTime,dateType::TIM16,nullptr),
	T1_Set_data(SoftwareVersion,dateType::STRING64,stingDefault),

	T1_Set_data(mac,dateType::MAC,nullptr),
	T1_Set_data(HeliostatType,dateType::STRING16,stingDefault),
	T1_Set_data(SN2ID,dateType::STRING16,stingDefault),
	T1_Set_data(latitude,dateType::FLOAT32,floatDefault),
	T1_Set_data(longtitude,dateType::FLOAT32,floatDefault),
	T1_Set_data(elevation,dateType::FLOAT32,floatDefault),
	T1_Set_data(Temperature,dateType::FLOAT32,floatDefault),
	T1_Set_data(Pressure,dateType::FLOAT32,floatDefault),
	T1_Set_data(DeltaT,dateType::FLOAT32,floatDefault),
	T1_Set_data(HeliostatPointX,dateType::FLOAT32,floatDefault),
	T1_Set_data(HeliostatPointY,dateType::FLOAT32,floatDefault),
	T1_Set_data(HeliostatPointZ,dateType::FLOAT32,floatDefault),
};


const CFG_INFO T2[] = {
	T2_Set_data(initSpeedx,dateType::FLOAT32,floatDefault),
};



const CFG_INFO T3[] = {
	//sn1
	T3_Set_data(SN1_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e4,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e5,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e6,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e7,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e8,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e9,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e10,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e11,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e12,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e13,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e14,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN1_e15,dateType::BOOLTYPE,boolDefault),
	//sn2
	T3_Set_data(SN2_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e4,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e5,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e6,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e7,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e8,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e9,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e10,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e11,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e12,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e13,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN2_e14,dateType::FLOAT32,floatDefault),
	//sn3
	T3_Set_data(SN3_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN3_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN3_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN3_e4,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN3_e5,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN3_e6,dateType::FLOAT32,floatDefault),
	T3_Set_data(SN3_e7,dateType::BOOLTYPE,boolDefault),
	//零位
	T3_Set_data(OLW_e1,dateType::FLOAT32,floatDefault),
	T3_Set_data(OLW_e2,dateType::FLOAT32,floatDefault),
	T3_Set_data(OLW_e3,dateType::FLOAT32,floatDefault),
	T3_Set_data(OLW_e4,dateType::BOOLTYPE,boolDefault),
};


const CFG_INFO T4[] = {
	T4_Set_data(AP1,dateType::STRING32,stingDefault),
};

const CFG_INFO T6[] = {
	T6_Set_data(SN1_P1,dateType::INT32,stingDefault),
};

const CFG_GROUP cfg_group[] = {
	Set_Group(T1,0),
	Set_Group(T2,1),
	Set_Group(T3,2),
	Set_Group(T4,3),
	Set_Group(T6,4),
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



