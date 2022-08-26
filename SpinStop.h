#pragma once

namespace SpinLock {

	constexpr unsigned long long int WAIT_TIME = 200;
	constexpr unsigned long long int ADD_TO_WAIT_TIME = 10;
	constexpr unsigned long long int MAX_ITER_COUNT = 8;

	static void SpinLockPause() {
		__asm volatile("pause" : : : "memory");
	}

	class SpinStop {
	public:
		SpinStop();
		void operator()();
	private:
		void ExponentialBack();
		unsigned long long int wait_time_;
		unsigned long long int iter_;
	};	
}
