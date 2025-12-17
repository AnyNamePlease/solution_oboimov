#include <gtest/gtest.h>
#include <stdexcept>
#include "../CacheImp.h"

class CacheTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CacheTest, PutAndGet) {
    ExampleCache<int, int> cache(2);
    EXPECT_EQ(cache.size(), 0);

    cache.set(1, 10);
    cache.set(2, 20);
    EXPECT_EQ(cache.size(), 2);
    EXPECT_EQ(cache.get(1), 10);
    EXPECT_EQ(cache.get(2), 20);
}

TEST_F(CacheTest, LRU_Eviction) {
    ExampleCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.get(1);          // делаем 1 "недавним"
    cache.set(3, 30);      // вытесняем 2 (самый старый)
    
    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
    EXPECT_EQ(cache.get(1), 10);
    EXPECT_EQ(cache.get(3), 30);
}

TEST_F(CacheTest, UpdateExistingKey) {
    ExampleCache<int, int> cache(2);
    cache.set(1, 10);
    cache.set(1, 100); // обновление
    EXPECT_EQ(cache.get(1), 100);
    EXPECT_EQ(cache.size(), 1);
}

TEST_F(CacheTest, ZeroCapacityThrows) {
    EXPECT_THROW((ExampleCache<int, int>(0)), std::invalid_argument);
}

TEST_F(CacheTest, Clear) {
    ExampleCache<int, int> cache(3);
    cache.set(1, 10);
    cache.set(2, 20);
    cache.clear();
    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.contains(1));
}
