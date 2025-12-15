#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

const size_t MEMORY_LIMIT = 10000; // Макс. записей в памяти
const size_t NUM_BUCKETS = 10;     // Число временных файлов

// Простая хеш-функция для строки (можно заменить на std::hash)
size_t hash_string(const std::string& s) {
    size_t h = 0;
    for (char c : s) {
        h = h * 31 + c;
    }
    return h;
}

// Фаза 1: Чтение входного файла и запись частичных агрегатов во временные файлы
void phase1(const std::string& input_file, const std::vector<std::string>& temp_files) {
    std::ifstream in(input_file);
    if (!in) {
        throw std::runtime_error("Cannot open input file");
    }

    // Вектор мапов: по одному на bucket
    std::vector<std::unordered_map<std::string, long long>> buckets(NUM_BUCKETS);

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string key;
        long long value;
        if (!(iss >> key >> value)) continue;

        size_t bucket_id = hash_string(key) % NUM_BUCKETS;
        buckets[bucket_id][key] += value;

        // Проверка лимита: если хоть один bucket переполнен — сбросим все
        bool flush = false;
        for (const auto& bucket : buckets) {
            if (bucket.size() > MEMORY_LIMIT / NUM_BUCKETS) {
                flush = true;
                break;
            }
        }

        if (flush) {
            for (size_t i = 0; i < NUM_BUCKETS; ++i) {
                std::ofstream out(temp_files[i], std::ios::app);
                for (const auto& [k, v] : buckets[i]) {
                    out << k << " " << v << "\n";
                }
                buckets[i].clear();
            }
        }
    }

    // Сброс остатков
    for (size_t i = 0; i < NUM_BUCKETS; ++i) {
        std::ofstream out(temp_files[i], std::ios::app);
        for (const auto& [k, v] : buckets[i]) {
            out << k << " " << v << "\n";
        }
    }
}

// Фаза 2: Обработка каждого bucket'а отдельно и запись финального результата
void phase2(const std::vector<std::string>& temp_files, const std::string& output_file) {
    std::ofstream out(output_file);
    if (!out) {
        throw std::runtime_error("Cannot open output file");
    }

    for (const auto& temp_file : temp_files) {
        std::unordered_map<std::string, long long> final_agg;
        std::ifstream in(temp_file);
        if (!in) continue;

        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string key;
            long long value;
            if (!(iss >> key >> value)) continue;
            final_agg[key] += value;
        }

        // Запись результата для этого bucket'а
        for (const auto& [k, v] : final_agg) {
            out << k << " " << v << "\n";
        }
    }
}

// Удаление временных файлов
void cleanup(const std::vector<std::string>& temp_files) {
    for (const auto& f : temp_files) {
        fs::remove(f);
    }
}

int main() {
    std::string input_file = "input.txt";
    std::string output_file = "output.txt";

    std::vector<std::string> temp_files;
    for (size_t i = 0; i < NUM_BUCKETS; ++i) {
        temp_files.push_back("temp_" + std::to_string(i) + ".txt");
        fs::remove(temp_files.back()); // очистка старых файлов
    }

    try {
        phase1(input_file, temp_files);
        phase2(temp_files, output_file);
        cleanup(temp_files);
        std::cout << "External aggregation completed. Output in " << output_file << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        cleanup(temp_files);
        return 1;
    }

    return 0;
}
