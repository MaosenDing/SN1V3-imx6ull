
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

