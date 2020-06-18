#ifndef _____main_H___
#define _____main_H___

typedef  void(jd_service)();

#define ADD_JD_SERVICE(svc) static const jd_service * const \
	jd_service_group##svc __attribute__((section(".jdsvc"))) = (svc);

extern jd_service * _jdsvc_start[];
extern jd_service * _jdsvc_end[];

#if 0
void test1()
{
	cout << "test 1 ppp " << endl;
}
ADD_JD_SERVICE(test1);


for (jd_service ** pjdsvc = _jdsvc_start; pjdsvc < _jdsvc_end; pjdsvc++) {
	(*pjdsvc)();
}
#endif


#endif