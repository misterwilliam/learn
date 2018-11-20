#include "gtest/gtest.h"
#include "stats.h"

TEST(Avg, Empty) {
  uint64_t samples[10];
  EXPECT_EQ(Avg(samples, 0), 0.0);
}

TEST(Avg, TwoSamples) {
  uint64_t samples[10] = {1, 3};
  EXPECT_EQ(Avg(samples, 2), 2.0);
}
