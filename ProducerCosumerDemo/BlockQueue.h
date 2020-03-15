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
	mutex _mutex;
	unique_lock<mutex> _lock;
	condition_variable _cv_con, _cv_prod;
	queue<int> _tasks;
	atomic<bool> _stopped;

	const int _capacity;

	void lockQueue() {
		_lock.lock();
	}

	void unlockQueue() {
		_lock.unlock();
	}
	
public:
	CBlockQueue();
	~CBlockQueue();

	bool empty() {
		return (_tasks.size() == 0 ? true : false);
	}

	bool full() {
		return (_tasks.size() == _capacity ? true : false);
	}

	void stop() {
		_stopped.store(true);
		_cv_con.notify_all();
	}

	bool stopped() {
		return _stopped.load();
	}

	void push(const int & data);
	void pop(int & data);

};

CBlockQueue::CBlockQueue() : _capacity(TASK_NUM), _lock(_mutex), _stopped(false) {
	_lock.unlock();
}

CBlockQueue::~CBlockQueue() {
	//if(_lock.owns_lock()) _lock.unlock();
	stop();
	_cv_con.notify_all();
	_cv_prod.notify_all();
}

void CBlockQueue::push(const int & data) {
	lockQueue();
	while (full()) {
		_cv_con.notify_one();
		cout << "Task Queue is full, notify one consumer...\n";
		_cv_prod.wait(_lock);
	}

	_tasks.push(data);
	_cv_con.notify_one();
	unlockQueue();
}

void CBlockQueue::pop(int & data) {
	lockQueue();
	while (empty()) {
		if (this->stopped()) {
			unlockQueue();
			return;
		}
		_cv_prod.notify_one();
		cout << "Task Queue is empty, notify one producer...\n";
		_cv_con.wait(_lock, [this]() { return this->stopped() || !this->empty(); });
		//_cv_con.wait(_lock);
	}

	data = _tasks.front();
	_tasks.pop();
	_cv_prod.notify_one();
	unlockQueue();
}

