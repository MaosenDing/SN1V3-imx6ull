#include "JDcomhead.h"
#include "jd_share.h"


//set last com time
static void shm_set_time(JD_INFO & jif, timeval & tv, int status, unsigned char testbuff[])
{
	JD_INFO_TIM & info_tim = (JD_INFO_TIM &)jif;
	info_tim.psn1->mdc_flag = SN1_SHM::MDC_TIME_OK;
	//status is ok
	if (status == 0) {
		if (info_tim.dbg_shm_ret_printf) printf("rec:mdc work flag ok!!!!!!!!!!!!\n");
		info_tim.psn1->helo_status = SN1_SHM::Helo_ok;
		info_tim.psn1->last_tv_sec = tv.tv_sec;
		info_tim.psn1->last_tv_usec = tv.tv_usec;
	}//other is error
	else {
		if (info_tim.dbg_shm_ret_printf)printf("rec:mdc work flag failed!!!!!!!!!!!!\n");
		info_tim.psn1->helo_status = SN1_SHM::Helo_not_ready;
	}
	if (info_tim.dbg_shm_ret_printf && testbuff) {
		printf("19 = %d\n", testbuff[0]);
		printf("20 = %d\n", testbuff[1]);
		printf("21 = %d\n", testbuff[2]);
		printf("22 = %d\n", testbuff[3]);
		printf("23 = %d\n", testbuff[4]);
	}
}

static int modify_raw_time(char * buffer, unsigned char * jd_data_buff)
{
	return sprintf(buffer, "%02d-%02d-%02d %02d:%02d:%02d.%03d status = %d"
		, (int)jd_data_buff[0]
		, (int)jd_data_buff[1]
		, (int)jd_data_buff[2]

		, (int)jd_data_buff[3]
		, (int)jd_data_buff[4]
		, (int)jd_data_buff[5]

		, (int)(jd_data_buff[6] | jd_data_buff[7] << 8)
		, (int)jd_data_buff[8]
	);
}

static void tmktime(tm & NowTm, unsigned char *jd_data_buff)
{
	NowTm.tm_year = (int)jd_data_buff[0] + 100;
	NowTm.tm_mon = (int)jd_data_buff[1] - 1;
	NowTm.tm_mday = (int)jd_data_buff[2];

	NowTm.tm_hour = (int)jd_data_buff[3];
	NowTm.tm_min = (int)jd_data_buff[4];
	NowTm.tm_sec = (int)jd_data_buff[5];
}

static int diff_timeval_ms(timeval & a, timeval & b)
{
	timeval dif;
	dif.tv_sec = a.tv_sec - b.tv_sec;
	dif.tv_usec = a.tv_usec - b.tv_usec;

	return dif.tv_sec * 1000 + dif.tv_usec / 1000;
}

static void disp_raw_time(unsigned char * jd_data_buff)
{
	char buff[256];
	modify_raw_time(buff, jd_data_buff);
	printf("%s\n", buff);
}

static int JD_ret_cap_status(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	SN1_SHM *psn1 = jit.psn1;

	struct
	{
		uint16_t error_cnt;
		int16_t error_code;
	}sendp;

	if (!jit.error_debug_flag) {
		sendp.error_code = psn1->last_error_code;
		sendp.error_cnt = psn1->error_count;
	} else {
		psn1->error_count++;
		sendp.error_code = -(psn1->error_count % 1000);
		sendp.error_cnt = psn1->error_count;
	}

	jfr.jd_send_buff = &sendp;
	jfr.jd_data_len = sizeof(sendp);
	jfr.jd_command |= 0x80;

	JD_send(jif, jfr);
	return JD_OK;
}





int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;
	if (jfr.jd_data_len >= 9) {
		tm NowTm;
		tmktime(NowTm, jfr.jd_data_buff);

		time_t tt = mktime(&NowTm);

		timeval rectv;
		rectv.tv_sec = tt;
		rectv.tv_usec = (int)(jfr.jd_data_buff[6] | jfr.jd_data_buff[7] << 8) * 1000;

		timeval nowtv;
		gettimeofday(&nowtv, nullptr);

		//enable time diff && time set
		if (jit.time_diff_max) {
			int diff_ms = diff_timeval_ms(nowtv, rectv);
			int abs_ms = abs(diff_ms);
			//disp diff time
			if (jit.dbg_tim_rec_printf) {

				char  buffer[256];
				int sz = 0;
				sz += sprintf(buffer + sz, "mdc time = ");
				sz += modify_raw_time(buffer + sz, jfr.jd_data_buff);
				sz += sprintf(buffer + sz, ",");
				tm & now_time = NowTm;
				sz += sprintf(buffer + sz, "sn1 time = %4d-%02d-%02d %02d:%02d:%02d--%06ld"
					, now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday
					, now_time.tm_hour, now_time.tm_min, now_time.tm_sec
					, nowtv.tv_usec
				);

				fprintf(jit.dbg_fp, "%s , diff tick = %d ,status = %d \n", buffer, diff_ms, (int)jfr.jd_data_buff[8]);
			}

			if (abs_ms > jit.time_diff_max) {
				settimeofday(&rectv, nullptr);
				gettimeofday(&nowtv, nullptr);
				disp_raw_time(jfr.jd_data_buff);

				if (jit.dbg_tim_rec_printf)fprintf(jit.dbg_fp, "time set ok \n");
			}
		}
		jit.psn1->mdc_id_num = jfr.jd_aim.value;
#if 0
		unsigned char buff[3];
		memcpy(buff, &jfr.jd_aim, 3);

		char outbuff[20];

		JD_Name_transfer(buff, outbuff, sizeof(outbuff));
		printf("mdc id = %s\n", outbuff);
#endif
		shm_set_time(jif, nowtv, (int)jfr.jd_data_buff[8], jfr.jd_data_len > 9 ? jfr.jd_data_buff + 9 : nullptr);
		//JD_ret_cap_status(jif, jfr);
	}
	return JD_OK;
}