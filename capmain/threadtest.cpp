#include <stdio.h>

#include <iostream>
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <list>
#include <boost/thread/concurrent_queues/sync_bounded_queue.hpp>



using namespace std;

struct datanode{
	datanode():index(-1)
	{

	}


	datanode(int dat) :index(dat)
	{
	}

	int index;

	~datanode()
	{
		printf("destruct %d\n", index);
	}
};


int threadtest(int argc, char * argv[])
{
	boost::sync_bounded_queue<datanode> testqueue(10);

	printf("thread test\n");
	testqueue.push((4));
	testqueue.push((5));
	testqueue.push((6));
	printf("1\n");
	{
		datanode nod;
		testqueue.pull(nod);
		nod.index += 10;
		printf("p1 val + 10 = %d\n", nod.index);
	}
	{
		datanode nod1;
		testqueue.pull(nod1);
	}
	{
		datanode nod2;
		testqueue.pull(nod2);
	}
	printf("2\n");

	testqueue.ptr_pull();

	printf("3\n");
	return 0;
}