
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "SN1V2_com.h"
#include "timSet.h"
#include <algorithm>
#include "jpgInc/jpeglib.h"
#include "time_interval.h"
#include <sys/stat.h>
#include "errHandle/errHandle.h"
#include "tableWork.h"
#include <thread>
#include <unistd.h>
#include <chrono>
#include <regex>
#include <sys/prctl.h>
//#include "camera.h"
//#include "communicate.h"
#include "errHandle/errHandle.h"

#include <iostream>
using namespace std;


struct MAIN_CMD {
	const char * cmd_name;
	int(*cmd_fun)(int, char *argv[]);
};

int rtf_test(int argc, char * argv[])
{
	clog << "RTF test" << endl;
	cout << "RTF test" << endl;
	return 0;
}

int cppReg(int argc, char * argv[])
{
	cout << "c++ reg test" << endl;
	if (argc >= 3) {
		string aim(argv[2]);
		cout << "input =" << aim << endl;
		regex reg("(\\d{4})-(\\d{1,2})-(\\d{1,2})_(\\d{1,2}):(\\d{1,2}):(\\d{1,2})");
		smatch match;

		string::const_iterator star = aim.begin();
		string::const_iterator end = aim.end();
		while (regex_search(star, end, match, reg)) {
			cout << "reg count = " << match.size() << endl;
			for (auto & p : match) {
				cout << p << endl;
			}
			star = match[0].second;
		}

		return 0;
	} else {
		cout << "par error" << endl;
	}
	return -1;
}




MAIN_CMD cmd_group[] = {
	{"RTF",rtf_test},
	{"CPPREG",cppReg},
};





int main(int argc, char *argv[])
{
	prctl(PR_SET_NAME, "main service");

	if (argc < 2) {
		cout << "test par error" << endl;
		return -1;
	}

	for (auto & p : cmd_group) {
		if (!strcmp(argv[1], p.cmd_name)) {
			return p.cmd_fun(argc, argv);
		}
	}

	cout << "no command" << endl;

	return 0;

}