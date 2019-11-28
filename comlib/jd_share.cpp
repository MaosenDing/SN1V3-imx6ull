
#include <stdint.h>
#include <string.h>
#include <stdio.h>


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
