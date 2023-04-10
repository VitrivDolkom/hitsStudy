#include <iostream>
#include <queue>
#include <thread>
#include <signal.h>
#include <chrono>
#include <string>
#include <mutex>
#include <algorithm>
#include <random>
#include <condition_variable>

using namespace std;
using namespace std::chrono;

bool endStatus = false;
void signalHandler(int signal) {
	if (signal == SIGINT) {
		cout << "Конец рабочего дня, все ушли отдыхать" << '\n';
		endStatus = true;
	}
}

struct Request {
	int priority;
	int group;

	Request(int group, int priority) {
		this->group = group;
		this->priority = priority;
	}
};

struct Device {
	int id;
	string status = " не работает";
	int group;
	thread t;
};

struct Group {
	queue<Request> q;
	vector<Device> devices;
};

struct CustomCompare {
	bool operator()(const Request& a, const Request& b) const {
		return a.priority > b.priority;
	}
};

void printDevices(vector<vector<Device>>& devices, int currentSize, int capacity) {
	for (size_t i = 0; i < devices.size(); ++i) {
		for (size_t j = 0; j < devices[i].size(); ++j) {
			cout << "Прибор из группы " << i << " под номером " << j << devices[i][j].status << '\n';
		}
	}

	cout << '\n';
}

void handleRequest(priority_queue<Request, vector<Request>, CustomCompare>& requests,
		vector<vector<Device>>& devices, int group, int device, int capacity, condition_variable& queueCv,
		mutex& mutexQueue) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> sleepRange(1000, 5000);

	while (!endStatus) {
		unique_lock<mutex> lock(mutexQueue);

		queueCv.wait(lock);

		if (requests.empty()) {
			devices[group][device].status = " ожидает";
		} else {
			requests.pop();
			devices[group][device].status = " работает";
			printDevices(devices, requests.size(), capacity);
			lock.unlock();
			queueCv.notify_one();

			this_thread::sleep_for(milliseconds(sleepRange(gen)));
			devices[group][device].status = " не работает";
		}
	}

}

void requestGenerator(priority_queue<Request, vector<Request>, CustomCompare>& requests, int n, int m,
		int capacity, condition_variable& queueCv, mutex& mutexQueue) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> groupRange(0, n - 1);
	std::uniform_int_distribution<> priorityRange(1, 3);
	std::uniform_int_distribution<> sleepRange(200, 1000);

	while (!endStatus) {
		unique_lock<mutex> lock(mutexQueue);

		if (requests.size() < capacity) {
			cout << "Размер очереди - " << requests.size() << " / " << capacity << "\n";
			Request request(priorityRange(gen), groupRange(gen));
			requests.push(request);
			mutexQueue.unlock();
			queueCv.notify_one();
		}

		this_thread::sleep_for(milliseconds(sleepRange(gen)));
	}
}

int main() {
	setlocale(LC_ALL, "Russian");

	int n, m, capacity;
	cout << "Введите количество групп заявок\n";
	cin >> n;
	cout << "Введите количество приборов на каждую группу\n";
	cin >> m;
	cout << "Введите максимальную ёмкость\n";
	cin >> capacity;

	vector<vector<Device>> devices;

	for (int i = 0; i < n; ++i) {
		devices.push_back(vector<Device>(m));
	}

	priority_queue<Request, vector<Request>, CustomCompare> requests;

	condition_variable queueCv;
	mutex mutexQueue;

	thread generatorThread(requestGenerator, ref(requests), n, m, capacity, ref(queueCv), ref(mutexQueue));
	generatorThread.detach();

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; j++) {
			devices[i][j].t = thread(handleRequest, ref(requests), ref(devices), i, j, capacity, ref(queueCv),
					ref(mutexQueue));
			devices[i][j].t.detach();
		}
	}

	signal(SIGINT, signalHandler);
	while (!endStatus) {
	}

	return 0;
}
