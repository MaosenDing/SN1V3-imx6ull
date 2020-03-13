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

#define Set_Group(group)    {#group,group,sizeof(group)/sizeof(CFG_INFO)}


	typedef struct {
		const char * groupName;
		CFG_INFO * group;
		size_t sz;
	}CFG_GROUP;

	size_t max_group_cnt();
	CFG_GROUP * find_group_index(size_t i);
	CFG_GROUP * find_group_name(const char * groupName);

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
#define T3_Set_data(member,typ,def_method)      Set_data(T3_table,member,typ,def_method)


	


#ifdef __cplusplus
}
#endif

#endif
