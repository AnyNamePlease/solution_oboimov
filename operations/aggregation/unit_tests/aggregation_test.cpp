#include <gtest/gtest.h>
#include <utility>
#include <vector>
#include "../AvgWithPartitioning.h"

TEST(AggregationTest, EmptyInput) {
    std::vector<std::pair<int, int>> input;
    auto result = AvgWithPartitioning(input, 8);
    EXPECT_TRUE(result.empty());
}

TEST(AggregationTest, SingleKey) {
    std::vector<std::pair<int, int>> input = {{1, 10}, {1, 20}, {1, 30}};
    auto result = AvgWithPartitioning(input, 10);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].first, 1);
    EXPECT_EQ(result[0].second, 20); // (10+20+30)/3 = 20
}

TEST(AggregationTest, MultipleKeysWithinBlock) {
    std::vector<std::pair<std::string, int>> input = {
        {"a", 4}, {"b", 6}, {"a", 2}, {"b", 4}
    };
    auto result = AvgWithPartitioning(input, 10); // block_size > 2
    ASSERT_EQ(result.size(), 2);
    // Порядок не гарантируется, проверим через map
    std::unordered_map<std::string, int> res_map(result.begin(), result.end());
    EXPECT_EQ(res_map["a"], 3); // (4+2)/2 = 3
    EXPECT_EQ(res_map["b"], 5); // (6+4)/2 = 5
}

TEST(AggregationTest, BlockOverflowTriggersFlush) {
    // block_size = 1 → каждый новый ключ вызывает flush
    std::vector<std::pair<int, int>> input = {{1, 10}, {2, 20}, {1, 30}};
    auto result = AvgWithPartitioning(input, 1);
    ASSERT_EQ(result.size(), 2);
    std::unordered_map<int, int> res_map(result.begin(), result.end());
    EXPECT_EQ(res_map[1], 20); // (10+30)/2 = 20
    EXPECT_EQ(res_map[2], 20);
}

TEST(AggregationTest, DoubleValues) {
    std::vector<std::pair<char, double>> input = {{'x', 1.5}, {'x', 2.5}};
    auto result = AvgWithPartitioning(input, 5);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].first, 'x');
    EXPECT_DOUBLE_EQ(result[0].second, 2.0);
}

/*TODO*/
