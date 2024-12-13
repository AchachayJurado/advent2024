#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

// Memory Block Structure
struct MemBlk {
    size_t pos, len;

    // Descending order by position
    bool operator<(const MemBlk& other) const {
        return pos > other.pos;
    }
};

// File Structure
struct File {
    size_t pos, size;
    uint32_t id;

    // Ascending order by position
    bool operator<(const File& other) const {
        return pos < other.pos;
    }
};

// Parse input into file and memory block heaps
std::pair<std::priority_queue<File>, std::priority_queue<MemBlk>> parse(const std::string& input) {
    std::priority_queue<File> files;
    std::priority_queue<MemBlk> memblks;
    size_t pos = 0;
    uint32_t file_id = 0;

    for (size_t i = 0; i < input.size(); i += 2) {
        size_t file_size = input[i] - '0';
        files.push({pos, file_size, file_id++});
        pos += file_size;

        if (i + 1 < input.size()) {
            size_t freesize = input[i + 1] - '0';
            if (freesize > 0) {
                memblks.push({pos, freesize});
                pos += freesize;
            }
        }
    }

    return {files, memblks};
}

// Compute checksum
uint64_t checksum(const std::vector<File>& files) {
    uint64_t sum = 0;
    for (const auto& file : files) {
        for (size_t pos = file.pos; pos < file.pos + file.size; ++pos) {
            sum += pos * file.id;
        }
    }
    return sum;
}

// Part 1: Greedy allocation
uint64_t part1(const std::string& input) {
    auto [files, memblks] = parse(input);
    std::vector<File> final_files;

    while (!files.empty()) {
        auto file = files.top();
        files.pop();

        if (!memblks.empty()) {
            auto memblk = memblks.top();
            memblks.pop();

            if (memblk.pos >= file.pos) {
                final_files.push_back(file);
                memblks.push(memblk);
                continue;
            }

            if (file.size <= memblk.len) {
                final_files.push_back({memblk.pos, file.size, file.id});
                if (memblk.len > file.size) {
                    memblks.push({memblk.pos + file.size, memblk.len - file.size});
                }
            } else {
                final_files.push_back({memblk.pos, memblk.len, file.id});
                files.push({file.pos, file.size - memblk.len, file.id});
            }
        }
    }

    return checksum(final_files);
}

// Part 2: Alternative allocation
uint64_t part2(const std::string& input) {
    auto [files, memblks] = parse(input);
    std::vector<File> final_files;

    while (!files.empty()) {
        auto file = files.top();
        files.pop();

        std::priority_queue<MemBlk> new_blks;

        while (!memblks.empty()) {
            auto memblk = memblks.top();
            memblks.pop();

            if (memblk.pos >= file.pos) {
                final_files.push_back(file);
                new_blks.push(memblk);
                while (!new_blks.empty()) {
                    memblks.push(new_blks.top());
                    new_blks.pop();
                }
                goto next_file;
            }

            if (file.size <= memblk.len) {
                final_files.push_back({memblk.pos, file.size, file.id});
                if (memblk.len > file.size) {
                    new_blks.push({memblk.pos + file.size, memblk.len - file.size});
                }
                while (!new_blks.empty()) {
                    memblks.push(new_blks.top());
                    new_blks.pop();
                }
                goto next_file;
            }

            new_blks.push(memblk);
        }

        final_files.push_back(file);
        while (!new_blks.empty()) {
            memblks.push(new_blks.top());
            new_blks.pop();
        }

    next_file:;
    }

    return checksum(final_files);
}

// Main Function
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

    std::string input;
    file >> input;

    std::cout << part1(input) << std::endl;
    std::cout << part2(input) << std::endl;

    return 0;
}