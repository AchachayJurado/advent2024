#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using Pos = std::pair<int, int>;
using Path = std::vector<Pos>;
using Map = std::unordered_map<Pos, int, std::hash<std::string>>;

struct PosHash {
    size_t operator()(const Pos& pos) const {
        return std::hash<int>()(pos.first) ^ std::hash<int>()(pos.second);
    }
};

// Helper to find neighbors
std::vector<Pos> neighbors(const Pos& pos, const Map& map) {
    static const std::vector<std::pair<int, int>> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    std::vector<Pos> result;

    int curr_height = map.at(pos);
    for (const auto& [dy, dx] : directions) {
        Pos new_pos = {pos.first + dy, pos.second + dx};
        if (map.find(new_pos) != map.end() && map.at(new_pos) - curr_height == 1) {
            result.push_back(new_pos);
        }
    }
    return result;
}

// Count paths to positions with height 9
int count_paths(const Pos& from, const Map& map) {
    std::queue<Pos> queue;
    std::unordered_set<Pos, PosHash> visited;
    queue.push(from);

    int sum = 0;
    while (!queue.empty()) {
        Pos current = queue.front();
        queue.pop();

        if (visited.count(current)) continue;
        visited.insert(current);

        if (map.at(current) == 9) {
            ++sum;
        }

        for (const auto& neighbor : neighbors(current, map)) {
            queue.push(neighbor);
        }
    }
    return sum;
}

// Count paths and track full paths
int count_paths2(const Pos& from, const Map& map) {
    std::queue<std::pair<Path, Pos>> queue;
    std::unordered_set<std::pair<Path, Pos>, PosHash> visited;

    queue.push({{from}, from});
    int sum = 0;

    while (!queue.empty()) {
        auto [path, current] = queue.front();
        queue.pop();

        if (visited.count({path, current})) continue;
        visited.insert({path, current});

        if (map.at(current) == 9) {
            ++sum;
        }

        for (const auto& neighbor : neighbors(current, map)) {
            Path new_path = path;
            new_path.push_back(current);
            queue.push({new_path, neighbor});
        }
    }
    return sum;
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

    Map map;
    std::string line;
    int y = 0;

    while (std::getline(file, line)) {
        for (int x = 0; x < line.size(); ++x) {
            char c = line[x];
            int height = (c >= '0' && c <= '9') ? c - '0' : 1000;
            map[{y, x}] = height;
        }
        ++y;
    }

    // Find all positions with height 0
    std::vector<Pos> zeros;
    for (const auto& [pos, height] : map) {
        if (height == 0) {
            zeros.push_back(pos);
        }
    }

    // Calculate path sums
    int sum1 = 0, sum2 = 0;
    for (const auto& zero : zeros) {
        sum1 += count_paths(zero, map);
        sum2 += count_paths2(zero, map);
    }

    std::cout << sum1 << std::endl;
    std::cout << sum2 << std::endl;

    return 0;
}