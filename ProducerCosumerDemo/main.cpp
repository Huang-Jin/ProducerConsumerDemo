#include<iostream>
#include"BlockQueue.h"

#ifdef WIN32

#include <windows.h>
#define sleep(x) (Sleep((x) * 1000))

#else
#include <unistd.h>
#endif

void consumer(CBlockQueue* bq) {
	//CBlockQueue* bq = static_cast<CBlockQueue*>(arg);

	while (!bq->stopped() || !bq->empty()) {
		int data;
		bq->pop(data);
		cout << "data comsumed done: " << data << ", trhead id: " << thread::id::id() << "\n";
		//sleep(0.5);
	}

	cout << "consumer " << thread::id::id() << " is done.\n";
}

void producer(CBlockQueue* bq) {
	//CBlockQueue* bq = static_cast<CBlockQueue*>(arg);

	int i = 0;
	while (i++ < 100) {
		//int data = rand() % 1024;
		int data = i;
		bq->push(data);
		cout << "data produced done: " << data << "\n";
		//sleep(0.2);
	}

	bq->stop();
	cout << "producer " << thread::id::id() << " is done.\n";
}

int main() {
	CBlockQueue bqueue;

	thread th_prod(producer, &bqueue);
	thread th_con0(consumer, &bqueue);
	thread th_con1(consumer, &bqueue);

	th_prod.join();
	th_con0.join();
	th_con1.join();

	return 0;
}