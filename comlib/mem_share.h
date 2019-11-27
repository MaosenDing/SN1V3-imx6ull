#ifndef __mem_share_h____
#define __mem_share_h____

#include <sys/types.h>
#include <sys/ipc.h>

key_t getKey(const char *path, int num);
void * getSHM(key_t key, int sz);
void shm_set_dayFlag(bool flag);







#endif
