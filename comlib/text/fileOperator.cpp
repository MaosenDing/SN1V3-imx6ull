#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "SN1V2_com.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <string.h>
#include <memory>
#include "errHandle/errHandle.h"
using namespace std;




//************************************
// Method:    loadFile
// FullName:  loadFile
// Access:    public static 
// Returns:   ERR_STA
// Qualifier:
// Parameter: char * fname
// Parameter: string & refVect
//************************************
ERR_STA loadFile(const char *fname ,string & refVect)
{
	ifstream ifs(fname);
	
	if (ifs)
	{
		ifs.seekg(0, ifs.end);
		int filelen = ifs.tellg();
		ifs.seekg(0, ifs.beg);		

		try
		{
			refVect.resize(filelen);
		}
		catch (std::bad_alloc &bd)
		{
			SN1V2_ERROR_CODE_RET( err_out_of_memory);
		}

		ifs.read(&refVect[0], filelen);
		//read error
		if (!ifs)
		{
			refVect.clear();
			SN1V2_ERROR_CODE_RET( err_cannot_read_file);
		}
		//read size error 
		if (refVect.size() != (size_t)ifs.gcount())
		{
			refVect.clear();
			SN1V2_ERROR_CODE_RET( err_cannot_read_file);
		}

		return err_ok;
	}
	else
	{
		SN1V2_ERR_LOG("file open error,file = %s", fname);
		SN1V2_ERROR_CODE_RET( err_cannot_open_file);
	}
	SN1V2_ERROR_CODE_RET( err_UNKNOWN);
}


ERR_STA loadFile(const char *fname, vector<uint8_t> & refVect) 
{
	ifstream ifs(fname, ios::binary);

	if (ifs)
	{
		ifs.seekg(0, ifs.end);
		int filelen = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		try
		{
			refVect.resize(filelen);
		}
		catch (std::bad_alloc &bd)
		{
			SN1V2_ERROR_CODE_RET( err_out_of_memory);
		}

		ifs.read((char *)&refVect[0], filelen);
		//read error
		if (!ifs)
		{
			refVect.clear();
			SN1V2_ERROR_CODE_RET(err_cannot_read_file);
		}
		//read size error 
		if (refVect.size() != (size_t)ifs.gcount())
		{
			refVect.clear();
			SN1V2_ERROR_CODE_RET( err_cannot_read_file);
		}

		return err_ok;
	}
	else
	{
		SN1V2_ERR_LOG("file = %s", fname);
		SN1V2_ERROR_CODE_RET( err_cannot_open_file);
	}
	SN1V2_ERROR_CODE_RET( err_UNKNOWN);
}


ERR_STA loadFileC(const char *fname, char ** refVect, int * outsize)
{
	if (fname == nullptr || refVect == nullptr || outsize == nullptr)
	{
		SN1V2_ERROR_CODE_RET(err_inval_para);
	}

	ifstream ifs(fname, ios::binary);

	if (ifs)
	{
		ifs.seekg(0, ifs.end);
		int filelen = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		char * tmp;
		try
		{
			tmp = new char[filelen];
		}
		catch (std::bad_alloc &)
		{
			SN1V2_ERROR_CODE_RET( err_out_of_memory);
		}
		if (tmp == nullptr)
		{
			SN1V2_ERROR_CODE_RET( err_out_of_memory);
		}

		ifs.read( &tmp[0], filelen);
		//read error
		if (!ifs)
		{
			delete[] tmp;
			SN1V2_ERROR_CODE_RET( err_cannot_read_file);
		}
		*refVect = tmp;
		*outsize = filelen;
		return err_ok;
	}
	else
	{
		SN1V2_ERROR_CODE_RET( err_cannot_open_file);
	}
	SN1V2_ERROR_CODE_RET( err_UNKNOWN);
}






//************************************
// Method:    split
// FullName:  split
// Access:    public static 
// Returns:   void
// Qualifier:
// Parameter: const string & input
// Parameter: vector<string> & outVector
// Parameter: char fdelimiter
//************************************
static void split(const string & input, vector<string> &outVector, char fdelimiter)
{
	outVector.clear();

	size_t pos2 = input.find(fdelimiter);
	size_t pos1 = 0;
	while (string::npos != pos2)
	{
		outVector.push_back(input.substr(pos1, pos2 - pos1));

		pos1 = pos2 + 1;
		pos2 = input.find(fdelimiter, pos1);
	}
	if (pos1 != input.length())
	{
		outVector.push_back(input.substr(pos1));
	}
}



//************************************
// Method:    RTF
// FullName:  RTF
// Access:    public 
// Returns:   ERR_STA
// Qualifier:
// Parameter: char * filePath 文件路径
// Parameter: char fdelimiter 分格符
// Parameter: vector<int> & frange 要取的数据段 如0 1 3 5
// Parameter: vector <string> & outVector 取出的数据
//************************************
ERR_STA RTF(char * filePath, char fdelimiter, vector<int>& frange, vector <string> & outVector)
{
	string bin;
	//载入文件
	ERR_STA sta = loadFile(filePath, bin);
	if (sta) return sta;

	//分割字符
	vector<string> tmpVector;
	try
	{
		split(bin, tmpVector, fdelimiter);
	}
	catch (std::bad_alloc & e)
	{
		SN1V2_ERROR_CODE_RET( err_out_of_memory);
	}
	

	//填充数据
	int error_time = 0;
	outVector.clear();
	for (unsigned int getcode : frange)
	{
		if (getcode < tmpVector.size())
		{
			outVector.push_back(tmpVector[getcode]);
		}
		else
		{
			error_time++;
			outVector.emplace_back("NULL");
		}
	}
	if (error_time)
	{
		SN1V2_ERROR_CODE_RET( err_txt_part_error);
	}
	return err_ok;
}



//************************************
// Method:    WTF
// FullName:  WTF
// Access:    public 
// Returns:   ERR_STA
// Qualifier:
// Parameter: char * filePath 文件路径
// Parameter: char fdelimiter 分格符 如'\' ','
// Parameter: char wtype	写入类型 ‘a’ 'w' 两种
// Parameter: string & fdata 要写入的数据
//************************************
ERR_STA WTF(char * filePath, char fdelimiter, char wtype, string & fdata)
{
	if (filePath || fdelimiter || wtype)
	{
		ofstream f;
		switch (wtype)
		{
		case 'w':
			f.open(filePath, ios::trunc);
			break;
		case 'a':
			f.open(filePath, ios::app);
			break;
		default:
			SN1V2_ERROR_CODE_RET( err_inval_para);
		}
		
		if (f.good())
		{
			if ('a' == wtype)
			{
				f << fdelimiter;
				if (!f.good())
				{
					SN1V2_ERROR_CODE_RET( err_cannot_write_file);
				}
			}
			f << fdata;
			if (f)
			{
				return err_ok;
			}
			else
			{
				SN1V2_ERROR_CODE_RET( err_cannot_write_file);
			}
		}
		else
		{
			SN1V2_ERROR_CODE_RET( err_cannot_open_file);
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET( err_inval_para);
	}
	SN1V2_ERROR_CODE_RET( err_UNKNOWN);
}



//************************************
// Method:    saveBin
// FullName:  saveBin
// Access:    public 
// Returns:   ERR_STA
// Qualifier:
// Parameter: char * filePath
// Parameter: char * data
// Parameter: unsigned long dataSize
//************************************
ERR_STA saveBin(char * filePath, char * data, unsigned long dataSize)
{
	if (filePath || dataSize || dataSize)
	{
		ofstream f(filePath, ios::trunc);
		
		if (f)
		{
			f.write(data, dataSize);
			if (f)
			{
				return err_ok;
			}
			else
			{
				SN1V2_ERROR_CODE_RET( err_cannot_write_file);
			}
		}
		else
		{
			SN1V2_ERROR_CODE_RET( err_cannot_open_file);
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET( err_inval_para);
	}
	SN1V2_ERROR_CODE_RET( err_UNKNOWN);
}


ERR_STA saveBin(char * filePath, vector<uint8_t> & vdata)
{
	if (filePath)
	{
		ofstream f(filePath, ios::trunc);

		if (f)
		{
			f.write((char *)&vdata[0], vdata.size());
			if (f)
			{
				return err_ok;
			}
			else
			{
				SN1V2_ERROR_CODE_RET( err_cannot_write_file);
			}
		}
		else
		{
			SN1V2_ERROR_CODE_RET(err_cannot_open_file);
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_inval_para);
	}
	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}



#include <dirent.h>

shared_ptr<vector<string> > getAllFileName(char * path,const char * postfix)
{
	DIR *dfd;
	char tmpname[256];
	shared_ptr<vector<string> > ret = make_shared<vector<string>>();
	
	if ((dfd = opendir(path)) == NULL)
	{
		cout << "can't open dir " << path << endl;
		return ret;
	}

	struct dirent *dp;
	while ((dp = readdir(dfd)) != NULL)
	{
		if (strncmp(dp->d_name, ".", 1) == 0)
			continue; /* 跳过当前文件夹和上一层文件夹以及隐藏文件*/
		if (strlen(path) + strlen(dp->d_name) + 2 > sizeof(tmpname))
		{
			cout << "file" << path << "/" << dp->d_name << " is too long" << endl;
		}
		else
		{
			memset(tmpname, 0, sizeof(tmpname));
			sprintf(tmpname, "%s/%s", path, dp->d_name);

			struct stat stbuff;
			if (stat(tmpname, &stbuff) == -1)
			{
				cout << "can't access " << tmpname << endl;
				continue;
			}
			if (stbuff.st_mode & S_IFREG)
			{
				if (strstr(tmpname, postfix))
				{
					ret->emplace_back(dp->d_name);
				}
			}
		}
	}
	closedir(dfd);
	return ret;
}




