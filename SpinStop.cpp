#include "SpinStop.h"

#include <thread>
#include <chrono>

namespace SpinLock {

	SpinStop::SpinStop() : wait_time_(WAIT_TIME), iter_(0) {}

	void SpinStop::operator()() {
		ExponentialBack();
	}

	void SpinStop::ExponentialBack() {
		if(iter_ == 0)
		{
			++iter_;
			std::this_thread::yield();
		}
		else if(iter_ > 0)
		{
			if (iter_ < MAX_ITER_COUNT) {
				++iter_;
				std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_));
				wait_time_ = wait_time_ + wait_time_ + ADD_TO_WAIT_TIME;
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_));
			}
		}
		
		SpinLockPause();
	}
}
