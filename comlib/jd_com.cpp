
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
