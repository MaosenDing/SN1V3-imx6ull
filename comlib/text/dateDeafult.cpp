#include "sn1v3cfg.h"

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


CFG_INFO T1_data[] = {
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


CFG_GROUP cfg_group[] = {
	Set_Group(T1_data),
};

size_t max_group_cnt()
{
	return sizeof(cfg_group) / sizeof(CFG_GROUP);
}


CFG_GROUP * find_group(size_t i)
{
	if (i < max_group_cnt()) {
		return &cfg_group[i];
	}
	return nullptr;
}





