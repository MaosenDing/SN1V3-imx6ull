#ifndef __JD_share_H___
#define __JD_share_H___



int JD_Name_transfer(const unsigned char * input, char * outbuff, int InMaxSz);

unsigned int crc_make(unsigned char *ptr, int len, unsigned int firstcrc);



#endif

