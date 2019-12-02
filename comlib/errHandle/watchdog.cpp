
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "watchdog.h"

#define DEV_FILE "/dev/watchdog"

//#include <glog/logging.h>
#include <thread>
#include <chrono>

using namespace  std;
using namespace chrono;



int wdg_test(int argc, char* argv[])
{
	int fd = open(DEV_FILE, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "fail to open file:%s\n", DEV_FILE);
		return -1;
	}

	while (true)
	{
		this_thread::sleep_for(seconds(30));
		int ret = ioctl(fd, WDIOC_KEEPALIVE);
		if (0 != ret) {
			close(fd);
			return -1;
		}
	}
	close(fd);
	return 0;
}


