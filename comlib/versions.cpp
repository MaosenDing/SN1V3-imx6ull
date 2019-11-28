#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <fstream>

#include <vector>
#include <string>
#include <regex>
using namespace std;

#define FIX_VERSION_PATH "/etc/version"
#define SOFT_VERSION "2.42"
#define CFG_VERSION_PATH "./SCG.txt"

int getFixVersion(char * buff, int maxsz)
{
	ifstream ifs(FIX_VERSION_PATH);
	string tmpfil;
	if (ifs) {
		ifs.seekg(0, ifs.end);
		int filelen = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		tmpfil.resize(filelen);
		ifs.read(&tmpfil[0], filelen);
		if (filelen > 0) {
			return snprintf(buff, maxsz, "%s", tmpfil.c_str());
		}
	}
	return snprintf(buff, maxsz, "1.0");
}

int getSoftVersion(char * buff, int maxsz)
{
	return snprintf(buff, maxsz, SOFT_VERSION);
}

int getCfgVersion(char * buff, int maxsz)
{
	ifstream ifs(CFG_VERSION_PATH);
	string tmpfil;
	if (ifs) {
		ifs.seekg(0, ifs.end);
		int filelen = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		tmpfil.resize(filelen);
		ifs.read(&tmpfil[0], filelen);
		if (filelen > 0) {

			static regex reg("%([A-Z]{3}),([\\d\\w.+-]*)");
			smatch match;

			string::const_iterator star = tmpfil.begin();
			string::const_iterator end = tmpfil.end();
			while (regex_search(star, end, match, reg)) {

				if (!strcmp(match[1].str().c_str(), "VER")) {
					return snprintf(buff, maxsz, "%s", match[2].str().c_str());
				}
				star = match[0].second;
			}
		}
	}
	return snprintf(buff, maxsz, "3.0");
}

char * getVersion()
{
#define max_sz (256)
	static char versionbuff[max_sz];
	int diffsz = 0;

	diffsz += getFixVersion(versionbuff + diffsz, max_sz - diffsz);
	diffsz += snprintf(versionbuff + diffsz, max_sz - diffsz, ",");

	diffsz += getSoftVersion(versionbuff + diffsz, max_sz - diffsz);
	diffsz += snprintf(versionbuff + diffsz, max_sz - diffsz, ",");

	diffsz += getCfgVersion(versionbuff + diffsz, max_sz - diffsz);
	diffsz += snprintf(versionbuff + diffsz, max_sz - diffsz, " ");

	return versionbuff;
}
