#include <iostream>
#include <fstream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>
constexpr auto data = "data.bin";
constexpr size_t mem = 6 * 1024 * 1024;
constexpr int tNum = 2;
constexpr size_t tMem = mem / tNum;
constexpr size_t tMemUint = tMem / sizeof(uint64_t);

std::atomic<bool> finished = false;
std::atomic<int> finishedStep = 0;
std::atomic<int> finishedSort = 0;
std::condition_variable cond;
std::mutex sortFinishM;
std::mutex outQueM;
std::mutex streamReadM;
std::mutex iterFinishM;

void split(uint64_t * numbers, std::ifstream & Sdata, int id, std::queue<std::string>&outF) {
	int f = 0;
	
	while (!Sdata.eof()) {
		std::unique_lock<std::mutex> lock(streamReadM);
		Sdata.read(reinterpret_cast<char *>(numbers), tMemUint * sizeof(uint64_t));
		std::streamsize curSize = Sdata.gcount() / sizeof(uint64_t);
		lock.unlock();
		if (curSize != 0) {
			std::sort(numbers, numbers + curSize);
			std::string name = std::to_string(0) + '.' + std::to_string(id) + "." + std::to_string(f) + ".bin";
			std::ofstream out(name, std::ios::binary);
			out.write(reinterpret_cast<char*>(numbers), curSize * sizeof(uint64_t));
			outF.push(name);
			++f;
		}
	}

}

void merge(std::string & str1, std::string & str2, uint64_t * num, int id, int i, int f, std::queue<std::string>&outF) {
	std::ifstream f1(str1, std::ios::binary);
	std::ifstream f2(str2, std::ios::binary);
	std::string name = std::to_string(i) + '.' + std::to_string(id) + '.' + std::to_string(f) + ".bin";
	std::ofstream out(name, std::ios::binary);

	size_t lim = tMemUint / 4;
	uint64_t* left = num;
	uint64_t* right = num + lim;
	uint64_t* med = right + lim;

	f1.read(reinterpret_cast<char*>(left), lim * sizeof(uint64_t));
	size_t redL = f1.gcount() / sizeof(uint64_t);
	f2.read(reinterpret_cast<char*>(right), lim * sizeof(uint64_t));
	size_t redR = f2.gcount() / sizeof(uint64_t);

	size_t limM = 2 * lim;
	size_t l = 0;
	size_t m = 0;
	size_t r = 0;

	while (!f1.eof() || !f2.eof() || l < redL || r < redR) {
		if (l == redL && !f1.eof()) {
			f1.read(reinterpret_cast<char*>(left), lim * sizeof(uint64_t));
			redL = f1.gcount() / sizeof(uint64_t);
			l = 0;
		}
		if (r == redR && !f2.eof()) {
			f2.read(reinterpret_cast<char*>(right), lim * sizeof(uint64_t));
			redR = f2.gcount() / sizeof(uint64_t);
			r = 0;
		}
		if (m == limM) {
			out.write(reinterpret_cast<char*>(med), m * sizeof(uint64_t));
			m = 0;
		}

		if (l < redL && r < redR)
			if (right[r] < left[l]) {
				med[m] = right[r];
				++m;
				++r;
			}
			else {
				med[m] = left[l];
				++l;
				++m;
			}
		else
			if (l == redL && r < redR) {
				med[m] = right[r];
				++m;
				++r;
			}
			else
				if (r == redR && l < redL) {
					med[m] = left[l];
					++l;
					++m;
				}
	}

	out.write(reinterpret_cast<char*>(med), m * sizeof(uint64_t));
	std::unique_lock<std::mutex> qlock(outQueM);
	outF.push(name);
}

void Sort(uint64_t * nums, std::ifstream & Sdata, int id, std::queue<std::string>&outF) {
	uint64_t * numbers = nums + id * tMemUint;
	int i = 0;
	int f = 0;

	split(numbers, Sdata, id, outF);

	++i;
	std::unique_lock<std::mutex> lock(iterFinishM);
	++finishedStep;
	cond.notify_all();
	while (finishedStep < tNum)
		cond.wait(lock);
	lock.unlock();

	while (outF.size() >= 2) {
		std::unique_lock<std::mutex> qLock(outQueM);
			std::string tmp1 = outF.front();
			outF.pop();
			std::string tmp2 = outF.front();
			outF.pop();
			qLock.unlock();
			merge(tmp1, tmp2, numbers, id, i, f, outF);
			++f;
	}
	std::unique_lock<std::mutex> fLock(sortFinishM);
	++finishedSort;
	if (finishedSort == tNum)
		if (outF.empty())
			std::cerr << "error, no output files" << std::endl; 
		else std::cout << outF.front() << std::endl;
	
}

int main() {
	uint64_t * nums = new uint64_t[mem / sizeof(uint64_t)];
	std::ifstream Sdata(data, std::ios::binary);
	std::queue<std::string> outF;
	std::vector<std::thread> threads;
	if (!Sdata)
		throw std::invalid_argument("can't open file");
	for (int i = 0; i < tNum; ++i)
		threads.emplace_back(Sort, std::ref(nums),std::ref(Sdata), i, std::ref(outF));
	for (int i = 0; i < tNum; ++i)
		threads[i].join();
	delete[] nums;
	system("pause");
	return 0;
}
