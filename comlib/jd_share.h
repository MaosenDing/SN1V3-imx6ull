#ifndef __JD_share_H___
#define __JD_share_H___



int JD_Name_transfer(const unsigned char * input, char * outbuff, int InMaxSz);

unsigned int crc_make(unsigned char *ptr, int len, unsigned int firstcrc);

unsigned char crc_check(unsigned int len, unsigned char *Buff, unsigned int firstcrc);


void disp_x_buff(FILE * fp, unsigned char * buff, int num);
void disp_x_buff(unsigned char * buff, int num);


int ChkifCMD(int argc, char *argv[], const char * cmd);
char * ChkCmdVal(int argc, char * argv[], const char *cmd);

void SetWatchFile(const char * fil
	, void(*profun)(void * priv, const char* fil), void * priv);



float Angle_Convert(unsigned char *dat);
unsigned int Angle_Convert_UShort(float angle);

#endif

