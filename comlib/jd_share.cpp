
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
using namespace std;

int JD_Name_transfer(const unsigned char * input, char * outbuff, int InMaxSz)
{
	uint32_t tmp;
	unsigned char buff[3];

	memcpy(&tmp, input, 4);

	buff[0] = tmp & 0x7f;

	buff[1] = tmp >> 7;
	buff[1] &= 7;

	buff[2] = tmp >> 10;
	buff[2] &= 0x3f;

	return snprintf(outbuff, InMaxSz, "%03d%01d%02d%03d", (int)buff[0], (int)buff[1], (int)buff[2], (int)input[2]);
}


unsigned int crc_make(unsigned char *ptr, int len, unsigned int firstcrc)
{
	unsigned int crc = firstcrc;
	unsigned char i;
	while (len != 0) {
		if (len < 0)
			len = 0;
		crc ^= *ptr;
		for (i = 0; i < 8; i++) {
			if ((crc & 0x0001) == 0)
				crc = crc >> 1;
			else {
				crc = crc >> 1;
				crc ^= 0xa001;
			}
		}
		len -= 1;
		ptr++;
	}
	return crc;
}

/*
* 函数名称： crc_check()
* 功    能： CRC16校验码比较
* 入口参数： len：数据个数   Buff：数组地址    firstcrc：0xFFFF   *match_byte:NULL
* 出口参数： 1 :校验通过   0：校验失败
*/

unsigned char crc_check(unsigned int len, unsigned char *Buff, unsigned int firstcrc)
{
	unsigned int crc0, crc1;

	if (len <= 2) {
		return 0;
	}
	crc0 = crc_make(Buff, (len - 2), firstcrc);
	crc1 = (Buff[len - 1] << 8) + Buff[len - 2];

	if (crc0 == crc1)
		return 1;
	else
		return 0;
}

void disp_x_buff(FILE * fp, unsigned char * buff, int num)
{
	for (int i = 0; i < num; i++) {
		fprintf(fp, "%#02x ", buff[i]);
	}
	fprintf(fp, "\n");
}


int ChkifCMD(int argc, char *argv[], const char * cmd)
{
	for (int i = 0; i < argc; i++) {
		if (!strcmp(cmd, argv[i])) {
			return 1;
		}
	}
	return 0;
}

char * ChkCmdVal(int argc, char * argv[], const char *cmd)
{
	for (int i = 0; i < argc; i++) {
		if (!strcmp(cmd, argv[i])) {
			if (i + 1 < argc) {
				return argv[i + 1];
			}
		}
	}
	return nullptr;
}


void SetWatchFile(const char * fil
	, void(*profun)(void * priv, const char* fil), void * priv)
{
	int fd = inotify_init1(IN_NONBLOCK);

	if (fd < 0) {
		return;
	}

	char buff[64];
	sprintf(buff, "touch %s", fil);
	system(buff);

	int wd = inotify_add_watch(fd, fil, IN_MODIFY | IN_CREATE | IN_DELETE | IN_DELETE_SELF);

	if (wd < 0) {
		close(fd);
		return;
	}

	pollfd ipollfd;
	ipollfd.fd = fd;
	ipollfd.events = POLL_IN;
	ipollfd.revents = 0;
	int len;
	while (true) {
		if (0 >= poll(&ipollfd, 1, -1))
			continue;
		if (0 < (len = read(fd, buff, 128))) {
			inotify_event * ievent;
			for (char * ptr = buff; ptr < buff + len;
				ptr += sizeof(inotify_event) + ievent->len
				) {
				ievent = (inotify_event*)(ptr);
#if 1
				fprintf(stdout, "len = %d ,mask = %d,watch fd = %d,len = %d", len, ievent->mask, ievent->wd, ievent->len);
				if (ievent->len) {
					fprintf(stdout, ",name= %s", ievent->name);
				}
				fprintf(stdout, "\n");
#endif
				if(profun)
				profun(priv, fil);
			}
		}
	}
}

