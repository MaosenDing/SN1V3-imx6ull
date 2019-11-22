#include <errHandle/errHandle.h>
#include <string>
#include <SN1V2_com.h>
#include <fstream>
#include <regex>
#include <algorithm>
#include <time_interval.h>
#include <vector>
#include <math.h>
using namespace std;


int ipchange(const char * srName)
{
	auto loadbin = make_shared<string>();
	int fil = loadFile((char  *)srName, *loadbin);
	//cout << *loadbin << endl;
	ofstream outtest("ch.txt");
	if (fil == err_ok) {
		TimeInterval p("ipchange got:");
		regex reg("\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}/\\d{1,2}");
		smatch match;
		string::const_iterator star = loadbin->begin();
		string::const_iterator end = loadbin->end();
		while (regex_search(star, end, match, reg)) {
			star = match[0].second;

			int ip[4];
			int mask;
			int snum;
			if (5 == (snum = sscanf((char *)&(*match[0].first)
				, "%d.%d.%d.%d/%d"
				, &ip[0]
				, &ip[1]
				, &ip[2]
				, &ip[3]
				, &mask
			))) {
				uint32_t newip =
					ip[0] << (3 * 8)
					| ip[1] << (2 * 8)
					| ip[2] << (1 * 8)
					| ip[3] << (0 * 8);

				uint32_t num_mask = 0;

				for (int i = 0; i < 32 - mask; i++) {
					num_mask |= 1 << i;
				}
				num_mask = ~num_mask;

				uint32_t lowIp = newip & num_mask;

				uint32_t highIp = lowIp | (~num_mask);

				char tmpbuff[64];

				int outnum = snprintf(tmpbuff, 64, "%d.%d.%d.%d %d.%d.%d.%d\r\n"
					, (lowIp >> 24) & 0xff
					, (lowIp >> 16) & 0xff
					, (lowIp >> 8) & 0xff
					, (lowIp >> 0) & 0xff

					, (highIp >> 24) & 0xff
					, (highIp >> 16) & 0xff
					, (highIp >> 8) & 0xff
					, (highIp >> 0) & 0xff
				);				
				outtest.write(tmpbuff, outnum);
			}
		}
	}
	return 0;
}

