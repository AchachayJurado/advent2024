#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
#include <numeric>
#include <utility>

using Pos = std::pair<int, int>; // (y, x)
using Grid = std::unordered_map<Pos, char>;
using CellSet = std::unordered_set<Pos, std::hash<std::string>>;

struct Entry {
    int area = 0;
    int peri = 0;
    CellSet cells;
};

// Flood-fill to find connected regions
std::optional<Entry> flood_fill(const Pos& pos, const Grid& grid, CellSet& visited) {
    if (visited.count(pos)) return std::nullopt;

    char c = grid.at(pos);
    std::queue<Pos> queue;
    queue.push(pos);

    Entry entry;
    while (!queue.empty()) {
        Pos current = queue.front();
        queue.pop();

        if (visited.count(current)) continue;

        visited.insert(current);
        entry.area++;
        entry.cells.insert(current);

        int y = current.first, x = current.second;
        for (const auto& [dy, dx] : std::vector<Pos>{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
            Pos neighbor = {y + dy, x + dx};
            if (grid.find(neighbor) != grid.end()) {
                if (grid.at(neighbor) != c) {
                    entry.peri++;
                } else if (!visited.count(neighbor)) {
                    queue.push(neighbor);
                }
            } else {
                entry.peri++;
            }
        }
    }
    return entry;
}

// Find all areas using flood-fill
std::vector<Entry> find_areas(const Grid& grid) {
    CellSet visited;
    std::vector<Entry> entries;

    for (const auto& [pos, _] : grid) {
        if (visited.count(pos)) continue;

        if (auto entry = flood_fill(pos, grid, visited)) {
            entries.push_back(*entry);
        }
    }
    return entries;
}

// Compute fence cost based on area and perimeter
int compute_fence_cost1(const Grid& grid) {
    auto areas = find_areas(grid);
    return std::accumulate(areas.begin(), areas.end(), 0, [](int sum, const Entry& entry) {
        return sum + entry.area * entry.peri;
    });
}

// Compute a more complex fence cost considering corners and edge cases
int compute_fence_cost2(const Grid& grid) {
    auto areas = find_areas(grid);
    int total_cost = 0;

    for (const auto& area : areas) {
        int sides = 0;
        for (const auto& pos : area.cells) {
            int y = pos.first, x = pos.second;
            char c = grid.at(pos);
            char default_char = '*';

            char top = grid.count({y - 1, x}) ? grid.at({y - 1, x}) : default_char;
            char right = grid.count({y, x + 1}) ? grid.at({y, x + 1}) : default_char;
            char bottom = grid.count({y + 1, x}) ? grid.at({y + 1, x}) : default_char;
            char left = grid.count({y, x - 1}) ? grid.at({y, x - 1}) : default_char;

            if (top != c && right != c) sides++;
            if (top != c && left != c) sides++;
            if (bottom != c && right != c) sides++;
            if (bottom != c && left != c) sides++;

            // Corners
            if (top == c && right == c && (!grid.count({y - 1, x + 1}) || grid.at({y - 1, x + 1}) != c)) sides++;
            if (bottom == c && right == c && (!grid.count({y + 1, x + 1}) || grid.at({y + 1, x + 1}) != c)) sides++;
            if (bottom == c && left == c && (!grid.count({y + 1, x - 1}) || grid.at({y + 1, x - 1}) != c)) sides++;
            if (top == c && left == c && (!grid.count({y - 1, x - 1}) || grid.at({y - 1, x - 1}) != c)) sides++;
        }
        total_cost += area.area * sides;
    }

    return total_cost;
}

// Main function
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

    Grid grid;
    std::string line;
    int y = 0;

    while (std::getline(file, line)) {
        for (int x = 0; x < line.size(); ++x) {
            grid[{y, x}] = line[x];
        }
        ++y;
    }

    std::cout << compute_fence_cost1(grid) << std::endl;
    std::cout << compute_fence_cost2(grid) << std::endl;

    return 0;
}