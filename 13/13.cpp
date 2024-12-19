#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <regex>
#include <numeric>
#include <optional>

// Position Structure
struct Pos {
    int64_t x, y;

    Pos add(const Pos& other) const {
        return {x + other.x, y + other.y};
    }

    bool operator==(const Pos& other) const {
        return x == other.x && y == other.y;
    }
};

// Custom Hash for Pos
namespace std {
    template <>
    struct hash<Pos> {
        std::size_t operator()(const Pos& pos) const {
            return std::hash<int64_t>()(pos.x) ^ (std::hash<int64_t>()(pos.y) << 1);
        }
    };
}

// Custom Hash for pair
struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
    }
};

// Block Structure
struct Blk {
    Pos da, db, prize;
};

// Parse a block from input using regex
Blk parse_block(const std::string& block) {
    std::regex re(R"(Button A: X\+(\d+), Y\+(\d+)\nButton B: X\+(\d+), Y\+(\d+)\nPrize: X\=(\d+), Y=(\d+))");
    std::smatch match;

    if (!std::regex_search(block, match, re)) {
        throw std::runtime_error("Invalid block format");
    }

    return {
        {std::stoll(match[1]), std::stoll(match[2])},
        {std::stoll(match[3]), std::stoll(match[4])},
        {std::stoll(match[5]), std::stoll(match[6])}
    };
}

// Minimum cost using BFS
std::optional<int32_t> min_cost(const Blk& blk) {
    std::queue<std::pair<Pos, int32_t>> queue;
    std::unordered_set<std::pair<Pos, int32_t>, PairHash> seen;

    queue.push({{0, 0}, 0});
    std::optional<int32_t> min = std::nullopt;

    while (!queue.empty()) {
        auto [pos, cost] = queue.front();
        queue.pop();

        if (seen.count({pos, cost}) || (min && cost >= *min)) continue;
        seen.insert({pos, cost});

        if (pos.y > blk.prize.y || pos.x > blk.prize.x) continue;

        if (pos == blk.prize) {
            min = min ? std::min(*min, cost) : cost;
        }

        queue.push({pos.add(blk.da), cost + 3});
        queue.push({pos.add(blk.db), cost + 1});
    }

    return min;
}

// Solve using algebraic approach
std::optional<int64_t> solve(const Blk& blk) {
    constexpr int64_t ADD = 10000000000000;

    int64_t ax = blk.da.x, ay = blk.da.y;
    int64_t bx = blk.db.x, by = blk.db.y;
    int64_t px = ADD + blk.prize.x, py = ADD + blk.prize.y;

    int64_t a_nom = bx * py - by * px;
    int64_t a_den = bx * ay - ax * by;

    if (a_den == 0 || a_nom % a_den != 0) return std::nullopt;
    int64_t a = a_nom / a_den;

    int64_t b_den = bx;
    int64_t b_nom = px - ax * a;

    if (b_den == 0 || b_nom % b_den != 0) return std::nullopt;
    int64_t b = b_nom / b_den;

    return a * 3 + b;
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

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    // Parse blocks
    std::regex block_split("\n\n");
    std::sregex_token_iterator it(input.begin(), input.end(), block_split, -1), end;
    std::vector<Blk> blocks;

    for (; it != end; ++it) {
        blocks.push_back(parse_block(it->str()));
    }

    // Compute costs
    int32_t cost1 = std::accumulate(blocks.begin(), blocks.end(), 0, [](int32_t acc, const Blk& blk) {
        return acc + min_cost(blk).value_or(0);
    });

    int64_t cost2 = std::accumulate(blocks.begin(), blocks.end(), int64_t(0), [](int64_t acc, const Blk& blk) {
        return acc + solve(blk).value_or(0);
    });

    std::cout << cost1 << std::endl;
    std::cout << cost2 << std::endl;

    return 0;
}