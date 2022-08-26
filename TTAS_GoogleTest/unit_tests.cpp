#include <gtest/gtest.h>

#include "../unit_test.hpp"
#include "TTAS.hpp"

#include <mutex>



using namespace AcronisLabs;

TEST(TTAS, JustWork) {
  TesterSpinlock<TTAS> spinlock;
  spinlock.JustWork();
}

TEST(TTAS, TwoLocks) {
  TesterSpinlock<TTAS> spinlock;
  spinlock.TwoLocks();
}

TEST(TTAS, BigNumberOfThreads) {
  TesterSpinlock<TTAS> spinlock;
  auto res = spinlock.ThreadsIncreaseCounters(1000, 10);
  ASSERT_EQ(res, 1000 * 10);
}

TEST(TTAS, BenchAverageTime_Threads) {
  TesterSpinlock<TTAS> spinlock;
  double sum = 0.0;
  std::cout << "###Evaluation started:###" << std::endl;
  std::mutex guard;
  double max = std::numeric_limits<double>::min();
  for (int i = 10; i < 100; i += 10) {
	spinlock.MaxTimeLockUnlock(i, i * 100, sum, guard, max);
	std::cout << "#" << i << " threads - " << sum / i * 10000 << " seconds " << " max " << max * 10000 << std::endl;
	max = std::numeric_limits<double>::min();
	sum = 0;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}