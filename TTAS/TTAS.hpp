#pragma once

#include <atomic>
#include <cstdint>
namespace SpinLock {

	class TTASSpinlock {
	public:
		TTASSpinlock();
		~TTASSpinlock();
		void Lock();
		void Unlock();
	
	private:
		std::atomic<uint16_t> lock_; // my cache line size is 64 bits
	};
}
