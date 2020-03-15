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
		cout << "data comsumed done: " << data << ", trhead id: " << this_thread::get_id() << "\n";
		//sleep(0.5);
	}

	cout << "consumer " << this_thread::get_id() << " is done.\n";
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
	cout << "producer " << this_thread::get_id() << " is done.\n";
}

int main() {
	CBlockQueue bqueue;

	thread th_prod(producer, &bqueue);

	vector<thread> th_cons;
	const int nums = 2;
	for (int i = 0; i < nums; i++) {
		th_cons.emplace_back(consumer, &bqueue);
	}

	th_prod.join();
	for (auto& t : th_cons) {
		t.join();
	}

	return 0;
}