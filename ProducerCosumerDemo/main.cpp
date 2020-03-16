#include<iostream>
#include"BlockQueue.h"

#ifdef WIN32

#include <windows.h>
#define sleep(x) (Sleep((x) * 1000))

#else
#include <unistd.h>
#endif

mutex mt_prod;

void consumer(CBlockQueue* bq) {
	//CBlockQueue* bq = static_cast<CBlockQueue*>(arg);

	while (bq->available()) {
		int data = -1;
		bq->pop(data);
		cout << "data comsumed done: " << data << ", trhead id: " << this_thread::get_id() << "\n";
		//sleep(0.5);
	}

	cout << "consumer " << this_thread::get_id() << " is done.\n";
}

void producer(CBlockQueue* bq, int start, int maxNum) {
	//CBlockQueue* bq = static_cast<CBlockQueue*>(arg);
	//unique_lock<mutex> lck(mt_prod);

	while (start++ < maxNum) {
		//int data = rand() % 1024;
		bq->push(start);
		cout << "data produced done: " << start << "\n";
		//sleep(0.2);
	}

	bq->stop();
	cout << "producer " << this_thread::get_id() << " is done.\n";
}

int main() {
	CBlockQueue bqueue;

	thread th_prod(producer, &bqueue, 0, 100);

	vector<thread> th_cons;
	const int nums = 5;
	for (int i = 0; i < nums; i++) {
		th_cons.emplace_back(consumer, &bqueue);
	}

	th_prod.join();
	for (auto& t : th_cons) {
		t.join();
	}

	return 0;
}