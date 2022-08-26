#pragma once
#include <atomic>
#include <thread>

#include "../SpinStop.h"

namespace SpinLock {

	class TicketLock {
	public:
		void Lock() {
			const auto ticket = next_ticket_.fetch_add(1, std::memory_order_relaxed);
			SpinStop spinWait;
			while (now_serving_.load(std::memory_order_acquire) != ticket) {
				spinWait();
			}
		}

		void Unlock() {
			now_serving_.fetch_add(1, std::memory_order_release);
		}

	private:
		std::atomic<std::size_t> now_serving_{0};
		std::atomic<std::size_t> next_ticket_{0};
	};

}
