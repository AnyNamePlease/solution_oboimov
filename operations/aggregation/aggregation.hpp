#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <istream>
#include <ostream>

// Выполняет частичную агрегацию по "бакетам" и пишет во временные потоки
void external_aggregation_phase1(
    std::istream& input,
    std::vector<std::ostream*>& temp_outputs,
    size_t memory_limit = 10000
);

// Объединяет временные файлы и пишет финальный результат
void external_aggregation_phase2(
    const std::vector<std::istream*>& temp_inputs,
    std::ostream& output
);

// Вспомогательная: разбирает строку "key value"
std::pair<std::string, long long> parse_line(const std::string& line);
