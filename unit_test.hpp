#pragma once

#include <vector>
#include <thread>
#include <functional>

namespace AcronisLabs {

	template<class LockType>
	class TesterSpinlock {
	public:
		class Timer {
		public:
			Timer(const size_t samples, double &sum, std::mutex &mutex, double &max)
				: samples_(samples), sum_all_threads_(sum), guard_(mutex), max_(max) {}
			~Timer();
			void measure(bool type); //if type == true => we measure begin, otherwise end
		private:
			//think about overflow of the time
			struct timespec begin_{0, 0}, end_{0, 0};
			struct timespec sum_{0, 0};
			const size_t samples_;
			double &sum_all_threads_;
			std::mutex &guard_;
			double &max_;
		};
	public:
		void JustWork();
		void TwoLocks();
		size_t ThreadsIncreaseCounters(size_t kIter, size_t kThreads);
		void MaxTimeLockUnlock(size_t kThreads, size_t kIter, double &sum, std::mutex &mutex, double &max);
	private:
		void CreateThreads(size_t kThreads, const std::function<void()> &callBack);
	private:
		LockType lock_; // it should have Lock() and Unlock()
	};

	template<class LockType>
	void TesterSpinlock<LockType>::Timer::measure(bool type) {
		if (clock_gettime(CLOCK_MONOTONIC, type ? &begin_ : &end_) != 0) {
			std::cerr << "error in getting time\n";
			std::exit(EXIT_SUCCESS); // think over how to deal with error per thread
		}
		if (!type) { // if we call end, we need to add the current time to the overall statistics
			sum_.tv_sec += end_.tv_sec - begin_.tv_sec;
			sum_.tv_nsec += end_.tv_nsec - begin_.tv_nsec;
		}
	}

	template<class LockType>
	void TesterSpinlock<LockType>::MaxTimeLockUnlock(const size_t kThreads,
	                                                 const size_t kIter,
	                                                 double &sum,
	                                                 std::mutex &mutex,
	                                                 double &max) {
		if (kThreads == 0 || kIter % kThreads != 0)
			throw std::runtime_error("bad input arguments for evaluating perfomance!");

		size_t per_thread = kIter / kThreads;

		auto simple = [per_thread, this, &sum, &mutex, &max]() {
			TesterSpinlock::Timer clock(per_thread, sum, mutex, max);
			for (size_t i = 0; i < per_thread; ++i) {
				clock.measure(true);
				lock_.Lock();
				clock.measure(false);
				lock_.Unlock();
			}
			//here in the destructor of Timer time per thread will be printed in stdout
		};
		CreateThreads(kThreads, simple);
	}

	template<class LockType>
	TesterSpinlock<LockType>::Timer::~Timer() {
		//Calculate the average time per thread
		//average time spent by each thread for acquiring a lock
		double AverageTimeThread = 1.00 * sum_.tv_sec + 1e-9 * sum_.tv_nsec;
		AverageTimeThread /= samples_;
		guard_.lock();
		max_ = std::max(max_, AverageTimeThread);
		sum_all_threads_ += AverageTimeThread;
		guard_.unlock();
	}

	template<class LockType>
	void TesterSpinlock<LockType>::JustWork() {
		//add time limit for the execution of the test
		lock_.Lock();
		lock_.Unlock();
	}

	template<class LockType>
	void TesterSpinlock<LockType>::TwoLocks() {
		//add time limit, to detect deadlocks
		lock_.Lock();
		lock_.Unlock();

		lock_.Lock();
		lock_.Unlock();
	}

	template<class LockType>
	size_t TesterSpinlock<LockType>
	::ThreadsIncreaseCounters(const size_t kIter, const size_t kThreads) {
		size_t shared_counter = 0;
		auto test = [&]() {
			for (size_t i = 0; i < kIter; ++i) {
				lock_.Lock();
				size_t current = shared_counter;
				std::this_thread::sleep_for(
					std::chrono::milliseconds(10));
				shared_counter = current + 1;
				lock_.Unlock();
			}
		};

		CreateThreads(kThreads, test);
		return shared_counter;
	}

	template<class LockType>
	void TesterSpinlock<LockType>::CreateThreads(const size_t kThreads,
	                                             const std::function<void()> &callBack) {
		std::vector<std::thread> pool;
		for (int i = 0; i < kThreads; ++i) {
			pool.emplace_back(callBack);
		}

		for (auto &el: pool)
			el.join();
	}
}
