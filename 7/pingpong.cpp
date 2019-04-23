#include <condition_variable>
#include <thread>
#include <mutex>
#include <iostream>
std::mutex m;
std::condition_variable dataReady;
bool ready = false;

void ping() {
	std::unique_lock<std::mutex> lock(m);
	for (int i = 0; i < 500000; ++i) {

		while (ready)
			dataReady.wait(lock);

		std::cout<<"ping\n";
		ready = true;
		dataReady.notify_one();
	}
}

void pong() {
	std::unique_lock<std::mutex> lock(m);
	for (int i = 0; i < 500000; ++i) {
		while (!ready)
			dataReady.wait(lock);

		std::cout << "pong\n";
		ready = false;
		dataReady.notify_one();
	}
}

int main()
{
	std::thread t1(ping);
	std::thread t2(pong);
	t1.join();
	t2.join();
	return 0;
}
