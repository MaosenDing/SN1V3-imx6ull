#ifndef __time_interval_h___
#define __time_interval_h___

//#define __my__GCC

#include <iostream>
#include <memory>
#include <string>
#if 0
#ifdef __my__GCC
#include <sys/time.h>
#else
#include <ctime>
#endif // __my__GCC
#endif
#include <chrono>
class TimeInterval
{
public:
	explicit TimeInterval(const char * d) : detail(d), start(std::chrono::system_clock::now())
	{
	}

	~TimeInterval()
	{
		auto end = std::chrono::system_clock::now();
		std::cout << detail
			<< std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count()
			<< "S" 
			<< std::endl;
	}
private:
	std::string detail;

	std::chrono::time_point<std::chrono::system_clock> start;
};
#if 1
#define TIME_INTERVAL_SCOPE(d)   TimeInterval __LINE__##__FUNCTION__ (d)
#else
#define TIME_INTERVAL_SCOPE(d)
#endif
#endif