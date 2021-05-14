#include <clockd_def.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <JDcomhead.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include "JDcomhead.h"
#include <fstream>
#include "SN1V2_error.h"

#include "mdc_ctrl.h"     /*******dms*20210121******/

#include <vector>
#include <string>
using namespace std;


struct JDTransMitImg {
	int Prepare_data()
	{
		ifstream readfile(Transimit_img_Path, ios::in | ios::binary);
		if (readfile.good()) {
			readfile.seekg(0, readfile.end);
			rdlen = readfile.tellg();
			readfile.seekg(0, readfile.beg);
			printf("get file len = %d\n", rdlen);
			memset(rdFileBuff, 0, MAX_PACK_NUM * PER_PACK_SIZE);
			if (rdlen > MAX_PACK_NUM * PER_PACK_SIZE) {
				errorCode = ERROR_PIX_TO_MUCH;
				rdlen = MAX_PACK_NUM * PER_PACK_SIZE;
				return errorCode;
			}
			readfile.read(rdFileBuff, rdlen);
			errorCode = ERROR_NULL;
		} else {
			errorCode = ERROR_OPEN;
			printf("open error \n");
			return errorCode;
		}
		return 0;
	}

	int getData(unsigned char * buff, int index)
	{
		if (errorCode) {
			printf("return error code = %d\n", errorCode);
			buff[0] = errorCode;
			return 1;
		}

		printf("index = %d , max = %d \n", index, MAX_PACK_NUM);
		if (index < MAX_PACK_NUM) {
			memcpy(buff, rdFileBuff + PER_PACK_SIZE * index, PER_PACK_SIZE);
			return 0;
		}
		return 1;
	}

	enum
	{
		MAX_PACK_NUM = 20,
		PER_PACK_SIZE = 200,

		ERROR_NULL = 0,
		ERROR_OPEN = 3,
		ERROR_PIX_TO_MUCH = 4,
	};

	char rdFileBuff[MAX_PACK_NUM * PER_PACK_SIZE];
	int rdlen;
	int errorCode;

};

JDTransMitImg JTM;



int JD_TransMit_img(JD_INFO & jif, JD_FRAME & jfr)
{
	//MDC_INFO & jit = (MDC_INFO &)jif;
	printf("JTM 111111 \n");
	if (jfr.jd_data_len >= 1) {

		int index = jfr.jd_data_buff[0];
		printf("JTM get data index = %d \n", index);

		if (index == 0) {
			//int errorcode = JTM.Prepare_data();
		}

		unsigned char tmpsend[201];
		if (0 == JTM.getData(&tmpsend[1], index)) {

			tmpsend[0] = jfr.jd_data_buff[0];
			jfr.jd_send_buff = tmpsend;

			jfr.jd_data_len = 1 + JTM.PER_PACK_SIZE;
			jfr.jd_command |= 0x80;
		} else {
			printf("JTM get error \n");

			tmpsend[0] = 0xff;

			jfr.jd_data_buff[0] |= 0x80;
			jfr.jd_send_buff = tmpsend;

			jfr.jd_data_len = 1 + 1;
		}

		jfr.jd_command |= 0x80;
		JD_send(jif, jfr);
	}
	return JD_OK;
}

