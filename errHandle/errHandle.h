#ifndef _err_handle_h____
#define _err_handle_h____

#ifdef __cplusplus

#include <glog/logging.h>
#include <glog/log_severity.h>

#include <stdio.h>
//#define SN1V2_ERROR_CODE_RET(__p) 	do{fprintf(stderr,"%s,%s,%d:%s\n",__FILE__,__FUNCTION__,__LINE__,#__p);return __p;} while(0)

#define SN1V2_ERROR_CODE_RET(__p) 	do{LOG(ERROR)<< #__p ;return __p;} while(0)
#define SN1V2_WARN_CODE_RET(__p) 	do{LOG(WARNING)<< #__p ;return __p;} while(0)
#define SN1V2_INFO_CODE_RET(__p) 	do{LOG(INFO)<< #__p ;return __p;} while(0)

//#define SN1V2_ERROR_WITH(__p) 	do{fprintf(stderr,"%s,%s,%d,ret=%d\n",__FILE__,__FUNCTION__,__LINE__,__p);} while(0)

#define SN1V2_ERROR_WITH(__p) 	do{LOG(ERROR)<< "ret ="<< (int)__p << endl;} while(0)


//#define SN1V2_ERROR_MWSSAGE_WITH(__message,__p) 	do{fprintf(stderr,"%s,%s,%d,%s , ret=%d\n",__FILE__,__FUNCTION__,__LINE__,__message,__p);} while(0)

#define SN1V2_ERROR_MWSSAGE_WITH(__message,__p) 	do{LOG(ERROR)<< "ret ="<< (int)__p << "message=" << #__p<< endl;} while(0)
#define SN1V2_WARN_MWSSAGE_WITH(__message,__p) 	do{LOG(WARNING)<< "ret ="<< (int)__p << "message=" << #__p<< endl;} while(0)
#define SN1V2_INFO_MWSSAGE_WITH(__message,__p) 	do{LOG(INFO)<< "ret ="<< (int)__p << "message=" << #__p<< endl;} while(0)

#define SN1V2_ERR_LOG(__format,...) 	do{char ____buff[128];sprintf(____buff,__format,##__VA_ARGS__);LOG(ERROR)<<____buff;} while(0)
#define SN1V2_WARN_LOG(__format,...) 	do{char ____buff[128];sprintf(____buff,__format,##__VA_ARGS__);LOG(WARNING)<<____buff;} while(0)
#define SN1V2_INF_LOG(__format,...) 	do{char ____buff[128];sprintf(____buff,__format,##__VA_ARGS__);LOG(INFO)<<____buff;} while(0)


//#define SN1V2_OUT(__format,...) 	do{fprintf(stdout,__format,##__VA_ARGS__);} while(0)


int logInit(const char * LogName, const char * SavePath, int setLevel);
#endif

#endif
