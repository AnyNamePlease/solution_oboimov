#include "external_aggregation.hpp"
#include <sstream>
#include <vector>
#include <unordered_map>

size_t hash_string(const std::string& s) {
    size_t h = 0;
    for (char c : s) h = h * 31 + static_cast<unsigned char>(c);
    return h;
}

std::pair<std::string, long long> parse_line(const std::string& line) {
    std::istringstream iss(line);
    std::string key;
    long long value = 0;
    if (!(iss >> key >> value)) {
        throw std::runtime_error("Invalid line: " + line);
    }
    return {key, value};
}

void external_aggregation_phase1(
    std::istream& input,
    std::vector<std::ostream*>& temp_outputs,
    size_t memory_limit
) {
    const size_t num_buckets = temp_outputs.size();
    if (num_buckets == 0) return;

    std::vector<std::unordered_map<std::string, long long>> buckets(num_buckets);

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        auto [key, value] = parse_line(line);
        size_t bucket_id = hash_string(key) % num_buckets;
        buckets[bucket_id][key] += value;

        // Проверка на переполнение
        bool flush = false;
        for (const auto& b : buckets) {
            if (b.size() > memory_limit / num_buckets) {
                flush = true;
                break;
            }
        }

        if (flush) {
            for (size_t i = 0; i < num_buckets; ++i) {
                for (const auto& [k, v] : buckets[i]) {
                    (*temp_outputs[i]) << k << " " << v << "\n";
                }
                buckets[i].clear();
            }
        }
    }

    // Сброс остатков
    for (size_t i = 0; i < num_buckets; ++i) {
        for (const auto& [k, v] : buckets[i]) {
            (*temp_outputs[i]) << k << " " << v << "\n";
        }
    }
}

void external_aggregation_phase2(
    const std::vector<std::istream*>& temp_inputs,
    std::ostream& output
) {
    for (auto* in : temp_inputs) {
        std::unordered_map<std::string, long long> agg;
        std::string line;
        while (std::getline(*in, line)) {
            if (line.empty()) continue;
            auto [key, value] = parse_line(line);
            agg[key] += value;
        }
        for (const auto& [k, v] : agg) {
            output << k << " " << v << "\n";
        }
    }
}
