#pragma once
#include <vector>
#include <unordered_map>
#include <utility>
#include <numeric>

template<typename Key_t, typename Value_t>
std::vector<std::pair<Key_t, Value_t>> AvgWithPartitioning(
    const std::vector<std::pair<Key_t, Value_t>>& kv_array,
    std::size_t block_size
) {
    if (kv_array.empty() || block_size == 0) {
        return {};
    }

    // partial_results: вектор временных блоков, каждый — map<key, {sum, count}>
    std::vector<std::unordered_map<Key_t, std::pair<Value_t, size_t>>> partial_results;

    std::unordered_map<Key_t, std::pair<Value_t, size_t>> current_block;

    for (const auto& [key, value] : kv_array) {
        // Добавляем в текущий блок
        auto& [sum, cnt] = current_block[key];
        sum += value;
        ++cnt;

        // Если превысили лимит — сливаем блок
        if (current_block.size() > block_size) {
            partial_results.push_back(std::move(current_block));
            current_block.clear();
        }
    }

    // Сливаем остаток
    if (!current_block.empty()) {
        partial_results.push_back(std::move(current_block));
    }

    // Финальная агрегация: объединяем все блоки
    std::unordered_map<Key_t, std::pair<Value_t, size_t>> final_map;
    for (const auto& block : partial_results) {
        for (const auto& [key, sum_cnt] : block) {
            final_map[key].first += sum_cnt.first;
            final_map[key].second += sum_cnt.second;
        }
    }

    // Формируем результат: (ключ, среднее)
    std::vector<std::pair<Key_t, Value_t>> result;
    result.reserve(final_map.size());
    for (const auto& [key, sum_cnt] : final_map) {
        Value_t average = sum_cnt.first / static_cast<Value_t>(sum_cnt.second);
        result.emplace_back(key, average);
    }

    return result;
}
