#include<iostream>
#include"BlockQueue.h"

#ifdef WIN32

#include <windows.h>
#define sleep(x) (Sleep((x) * 1000))

#else
#include <unistd.h>
#endif

CRITICAL_SECTION cs;
//mutex mt_prod;

void consumer(CBlockQueue* bq) {
	//CBlockQueue* bq = static_cast<CBlockQueue*>(arg);

	while (bq->available()) {
		int data = -1;
		bq->pop(data);
		EnterCriticalSection(&cs);
		cout << "<" << this_thread::get_id() << ">: " << data << " comsumed.\n";
		LeaveCriticalSection(&cs);
		//sleep(0.5);
	}

	cout << "<" << this_thread::get_id() << ">: " << "consumer is done.\n";
}

void producer(CBlockQueue* bq, int start, int maxNum) {
	//CBlockQueue* bq = static_cast<CBlockQueue*>(arg);
	//unique_lock<mutex> lck(mt_prod);

	int i = 0;
	while (i++ < maxNum) {
		//int data = rand() % 1024;
		int data = i + start;
		bq->push(data);
		EnterCriticalSection(&cs);
		cout << "[" << this_thread::get_id() << "]: " << data << " produced.\n";
		LeaveCriticalSection(&cs);
		//sleep(0.2);
	}

	//if(start + i >= maxNum) bq->stop();
	cout << "[" << this_thread::get_id() << "]: " << "producer is done.\n";
}

int main() {
	CBlockQueue bqueue;
	InitializeCriticalSection(&cs);

	vector<thread> th_prods;
	const int num_prod = 3;
	for (int i = 0; i < num_prod; i++) {
		th_prods.emplace_back(producer, &bqueue, i * 100, num_prod * 100);
	}

	vector<thread> th_cons;
	const int num_con = 3;
	for (int i = 0; i < num_con; i++) {
		th_cons.emplace_back(consumer, &bqueue);
	}

	for (auto& t : th_prods) {
		t.join();
	}
	bqueue.stop();
	for (auto& t : th_cons) {
		t.join();
	}

	DeleteCriticalSection(&cs);
	return 0;
}