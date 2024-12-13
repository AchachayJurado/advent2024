#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <numeric>

// Type alias for cache
using Cache = std::unordered_map<std::pair<int64_t, int64_t>, size_t, 
    std::function<size_t(const std::pair<int64_t, int64_t>&)>>;

// Custom hash function for pair
struct PairHash {
    size_t operator()(const std::pair<int64_t, int64_t>& p) const {
        return std::hash<int64_t>()(p.first) ^ (std::hash<int64_t>()(p.second) << 1);
    }
};

// Recursive function to count lengths
size_t count_len(int64_t stone, int64_t times, Cache& cache) {
    if (times == 0) {
        return 1;
    }

    auto key = std::make_pair(stone, times);
    if (cache.find(key) != cache.end()) {
        return cache[key];
    }

    size_t result = 0;
    std::string stone_str = std::to_string(stone);
    if (stone == 0) {
        result = count_len(1, times - 1, cache);
    } else if (stone_str.length() % 2 == 0) {
        // Split the string into two halves and convert to integers
        std::string left = stone_str.substr(0, stone_str.size() / 2);
        std::string right = stone_str.substr(stone_str.size() / 2);
        result = count_len(std::stoll(left), times - 1, cache) +
                 count_len(std::stoll(right), times - 1, cache);
    } else {
        result = count_len(stone * 2024, times - 1, cache);
    }

    cache[key] = result;
    return result;
}

// Run function
size_t run(const std::vector<int64_t>& stones, int64_t times) {
    Cache cache(0, PairHash());
    size_t total = 0;

    for (const auto& stone : stones) {
        total += count_len(stone, times, cache);
    }

    return total;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    std::vector<int64_t> stones;
    int64_t value;
    while (iss >> value) {
        stones.push_back(value);
    }

    std::cout << run(stones, 25) << std::endl;
    std::cout << run(stones, 75) << std::endl;

    return 0;
}