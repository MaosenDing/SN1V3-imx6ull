
#include "SN1V2_com.h"

key_t getKey(const char *path, int num)
{
	key_t key;
	if (-1 == (key = ftok(path, num))) {
		return -1;
	}
	return key;
}

void * getSHM(key_t key, int sz)
{
	int sid;

	if (-1 == (sid = shmget(key, sz, IPC_CREAT | IPC_EXCL | 0666))) {
		if (errno == EEXIST) {
			if (0 == (sid = shmget(key, sz, 0666))) {
				goto succ;
			}
		}
		return nullptr;
	}
succ:
	void *p = shmat(sid, nullptr, 0);

	if ((long int)p != -1) {
		return p;
	}
	return nullptr;
}




void shm_set_dayFlag(bool flag)
{
	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));

	if (flag) {
		time_t now = time(0);
		tm t2;

		localtime_r(&now, &t2);

		const int year = t2.tm_year + 1900;
		const int mon = t2.tm_mon + 1;
		const int day = t2.tm_mday;

		psn1->year = year;
		psn1->mon = mon;
		psn1->day = day;
	} else {
		psn1->year = -1;
	}
}



