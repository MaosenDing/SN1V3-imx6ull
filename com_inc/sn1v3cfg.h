#ifndef __sn1v3cfg__H___
#define __sn1v3cfg__H___


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stddef.h>

	enum dateType
	{
		STRING16,
		STRING32,
		STRING64,
		STRING128,

		FLOAT32,
		DOUBLE64,
		INT32,
		LONG64,

		TIM16,
		MAC,

		BOOLTYPE,
	};

	typedef struct {
		const char * name;
		size_t diff;
		dateType typ;
		void(*default_value)(void *);
	}CFG_INFO;

#define Set_Group(group)    {group,sizeof(group)/sizeof(CFG_INFO)}


	typedef struct {
		CFG_INFO * group;
		size_t sz;
	}CFG_GROUP;

	size_t max_group_cnt();
	CFG_GROUP * find_group(size_t i);

#define Set_data(table,member,typ,def_method)    {#member,offsetof(table,member),typ,def_method}
	
	typedef struct {
		char HeliostatID[16];
		char ReducerM[16];
		char PutterM[16];
		char DriveM[16];
		char SensorM[16];
		char InstallTime[16];
		char SoftwareVersion[64];
		char mac[6];
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


#define T1_Set_data(member,typ,def_method)      Set_data(T1_table,member,typ,def_method)
	


#ifdef __cplusplus
}
#endif

#endif
