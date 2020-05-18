#ifndef __wifi_snd_h____
#define __wifi_snd_h____

#include "wifi_ctrl.h"

void mk_read_num_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session);
void mk_read_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session, int message_id, int pack_index = 0);


int transmit_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session);




#endif


