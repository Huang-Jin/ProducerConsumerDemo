#pragma once
#include<iostream>
#include<queue>
#include<thread>
#include<mutex>
#include<atomic>
#include<condition_variable>

using namespace std;

#define TASK_NUM 8

class CBlockQueue
{
private:
	mutex _mt;
	condition_variable _cv_con, _cv_prod;
	queue<int> _tasks;
	atomic<bool> _stopped;

	const int _capacity;

	bool stopped() {
		return _stopped.load();
	}

	bool empty() {
		return (_tasks.size() == 0 ? true : false);
	}

	bool full() {
		return (_tasks.size() == _capacity ? true : false);
	}
	
public:
	CBlockQueue();
	~CBlockQueue();

	void stop() {
		_stopped.store(true);
		_cv_con.notify_all();
	}

	bool available() {
		return !stopped() || !empty();
	}

	void push(const int & data);
	void pop(int & data);

};

CBlockQueue::CBlockQueue() : _capacity(TASK_NUM), _stopped(false) {

}

CBlockQueue::~CBlockQueue() {
	stop();
	_cv_con.notify_all();
	_cv_prod.notify_all();
}

void CBlockQueue::push(const int & data) {
	unique_lock<mutex> _lck(_mt);
	while (full()) {
		_cv_con.notify_one();
		//cout << "Task Queue is full, notify one consumer...\n";
		_cv_prod.wait(_lck);
	}

	_tasks.push(data);
	_cv_con.notify_one();
}

void CBlockQueue::pop(int & data) {
	unique_lock<mutex> _lck(_mt);
	while (empty()) {
		if (this->stopped()) return;

		_cv_prod.notify_one();
		//cout << "Task Queue is empty, notify one producer...\n";
		_cv_con.wait(_lck, [this]() { return this->stopped() || !this->empty(); });
	}

	data = _tasks.front();
	_tasks.pop();
	_cv_prod.notify_one();
}

