#include "sn1v3cfg.h"
#include <string.h>
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


void boolDefault(void * addr)
{
	int * pos = (int *)addr;
	*pos = 0;
}



CFG_INFO T1[] = {
	T1_Set_data(HeliostatID,STRING16,stingDefault),
	T1_Set_data(ReducerM,STRING16,stingDefault),
	T1_Set_data(PutterM,STRING16,stingDefault),
	T1_Set_data(DriveM,STRING16,stingDefault),
	T1_Set_data(SensorM,STRING16,stingDefault),
	T1_Set_data(InstallTime,TIM16,nullptr),
	T1_Set_data(SoftwareVersion,STRING64,stingDefault),

	T1_Set_data(mac,MAC,nullptr),
	T1_Set_data(HeliostatType,STRING16,stingDefault),
	T1_Set_data(SN2ID,STRING16,stingDefault),
	T1_Set_data(latitude,FLOAT32,floatDefault),
	T1_Set_data(longtitude,FLOAT32,floatDefault),
	T1_Set_data(elevation,FLOAT32,floatDefault),
	T1_Set_data(Temperature,FLOAT32,floatDefault),
	T1_Set_data(Pressure,FLOAT32,floatDefault),
	T1_Set_data(DeltaT,FLOAT32,floatDefault),
	T1_Set_data(HeliostatPointX,FLOAT32,floatDefault),
	T1_Set_data(HeliostatPointY,FLOAT32,floatDefault),
	T1_Set_data(HeliostatPointZ,FLOAT32,floatDefault),
};




CFG_INFO T3[] = {
	//sn1
	T3_Set_data(SN1_e1,FLOAT32,floatDefault),
	T3_Set_data(SN1_e2,FLOAT32,floatDefault),
	T3_Set_data(SN1_e3,FLOAT32,floatDefault),
	T3_Set_data(SN1_e4,FLOAT32,floatDefault),
	T3_Set_data(SN1_e5,FLOAT32,floatDefault),
	T3_Set_data(SN1_e6,FLOAT32,floatDefault),
	T3_Set_data(SN1_e7,FLOAT32,floatDefault),
	T3_Set_data(SN1_e8,FLOAT32,floatDefault),
	T3_Set_data(SN1_e9,FLOAT32,floatDefault),
	T3_Set_data(SN1_e10,FLOAT32,floatDefault),
	T3_Set_data(SN1_e11,FLOAT32,floatDefault),
	T3_Set_data(SN1_e12,FLOAT32,floatDefault),
	T3_Set_data(SN1_e13,FLOAT32,floatDefault),
	T3_Set_data(SN1_e14,FLOAT32,floatDefault),
	T3_Set_data(SN1_e15,BOOLTYPE,boolDefault),
	//sn2
	T3_Set_data(SN2_e1,FLOAT32,floatDefault),
	T3_Set_data(SN2_e2,FLOAT32,floatDefault),
	T3_Set_data(SN2_e3,FLOAT32,floatDefault),
	T3_Set_data(SN2_e4,FLOAT32,floatDefault),
	T3_Set_data(SN2_e5,FLOAT32,floatDefault),
	T3_Set_data(SN2_e6,FLOAT32,floatDefault),
	T3_Set_data(SN2_e7,FLOAT32,floatDefault),
	T3_Set_data(SN2_e8,FLOAT32,floatDefault),
	T3_Set_data(SN2_e9,FLOAT32,floatDefault),
	T3_Set_data(SN2_e10,FLOAT32,floatDefault),
	T3_Set_data(SN2_e11,FLOAT32,floatDefault),
	T3_Set_data(SN2_e12,FLOAT32,floatDefault),
	T3_Set_data(SN2_e13,FLOAT32,floatDefault),
	T3_Set_data(SN2_e14,FLOAT32,floatDefault),
	//sn3
	T3_Set_data(SN3_e1,FLOAT32,floatDefault),
	T3_Set_data(SN3_e2,FLOAT32,floatDefault),
	T3_Set_data(SN3_e3,FLOAT32,floatDefault),
	T3_Set_data(SN3_e4,FLOAT32,floatDefault),
	T3_Set_data(SN3_e5,FLOAT32,floatDefault),
	T3_Set_data(SN3_e6,FLOAT32,floatDefault),
	T3_Set_data(SN3_e7,BOOLTYPE,boolDefault),
	//零位
	T3_Set_data(OLW_e1,FLOAT32,floatDefault),
	T3_Set_data(OLW_e2,FLOAT32,floatDefault),
	T3_Set_data(OLW_e3,FLOAT32,floatDefault),
	T3_Set_data(OLW_e4,BOOLTYPE,boolDefault),
};




CFG_GROUP cfg_group[] = {
	Set_Group(T1),
	Set_Group(T3),
};

size_t max_group_cnt()
{
	return sizeof(cfg_group) / sizeof(CFG_GROUP);
}


CFG_GROUP * find_group_index(size_t i)
{
	if (i < max_group_cnt()) {
		return &cfg_group[i];
	}
	return nullptr;
}

CFG_GROUP * find_group_name(const char * groupName)
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



