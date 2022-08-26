#include "TTAS.hpp"
#include "../SpinStop.h"

#include <algorithm>
#include <cassert>

namespace SpinLock {

	TTASSpinlock::TTASSpinlock() : lock_(0) {}

	TTASSpinlock::~TTASSpinlock() { assert(lock_.load(std::memory_order_seq_cst) == 0); }

	void TTASSpinlock::Lock() {
		SpinStop spin_wait;
		while (lock_.exchange(1, std::memory_order_acquire)) {
			do {
				spin_wait();
			} while (lock_.load(std::memory_order_relaxed));
		}

	}

	void TTASSpinlock::Unlock() {
		lock_.store(0, std::memory_order_release);
	}
}