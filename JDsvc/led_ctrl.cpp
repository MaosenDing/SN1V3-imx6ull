#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int led_status_green = 0;
static int led_status_red = 0;

char * green_name = "/sys/class/gpio/gpio198/value";
char * red_name = "/sys/class/gpio/gpio199/value";

static void led_ctrl(char * ledname, int status)
{
	int fd = open(ledname, O_RDWR);

	if (fd >= 0) {
		if (status) {
			write(fd, "1", 1);
		} else {
			write(fd, "0", 1);
		}
		close(fd);
	}
}


void led_init()
{
	system("/mnt/jaffs/user/io.sh led1 0");
	system("/mnt/jaffs/user/io.sh led2 0");
}





void led_trig_green(int insta)
{
	int setstatus = 0;
	if (insta > 1)
	{
		setstatus = led_status_green ? 0 : 1;
	} else if(insta == 0){
		setstatus = 0;
	} else {
		setstatus = 1;
	}

	if (setstatus)
	{
		led_status_green = 1;
		led_ctrl(green_name, 1);
	}
	else {
		led_status_green = 0;
		led_ctrl(green_name, 0);
	}
}

void led_trig_red(int insta)
{
	int setstatus = 0;
	if (insta > 1) {
		setstatus = led_status_red ? 0 : 1;
	} else if (insta == 0) {
		setstatus = 0;
	} else {
		setstatus = 1;
	}

	if (setstatus) {
		led_status_red = 1;
		led_ctrl(red_name, 1);
	} else {
		led_status_green = 0;
		led_ctrl(red_name, 0);
	}
}

int led_get_green()
{
	return led_status_green;
}
 
int led_get_red()
{
	return led_status_red;
}



