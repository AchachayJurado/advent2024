#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

struct Test {
    uint64_t test_val;
    std::vector<uint64_t> operands;
};

std::vector<uint64_t> eval_operators(const std::vector<uint64_t>& operands, bool use_concat) {
    if (operands.empty()) {
        throw std::runtime_error("No operands");
    }

    auto head = operands[0];
    std::vector<uint64_t> tail(operands.begin() + 1, operands.end());

    if (tail.empty()) {
        return {head};
    }

    std::vector<uint64_t> results;
    for (const auto& r : eval_operators(tail, use_concat)) {
        results.push_back(head + r);
        results.push_back(head * r);

        if (!use_concat) continue;

        // Concatenation
        uint32_t digits = static_cast<uint32_t>(std::floor(std::log10(head)) + 1);
        uint64_t concat = r * static_cast<uint64_t>(std::pow(10, digits)) + head;
        results.push_back(concat);
    }

    return results;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error reading file: " << argv[1] << std::endl;
        return 1;
    }

    std::vector<Test> tests;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string test_val_str, operands_str;

        if (std::getline(iss, test_val_str, ':') && std::getline(iss, operands_str)) {
            Test t;
            t.test_val = std::stoull(test_val_str);

            std::istringstream operands_stream(operands_str);
            std::string operand;
            while (std::getline(operands_stream, operand, ' ')) {
                if (!operand.empty()) {
                    t.operands.push_back(std::stoull(operand));
                }
            }
            std::reverse(t.operands.begin(), t.operands.end());
            tests.push_back(t);
        } else {
            throw std::runtime_error("Invalid input");
        }
    }

    uint64_t sum1 = 0;
    for (const auto& t : tests) {
        auto results = eval_operators(t.operands, false);
        if (std::find(results.begin(), results.end(), t.test_val) != results.end()) {
            sum1 += t.test_val;
        }
    }
    std::cout << sum1 << std::endl;

    uint64_t sum2 = 0;
    for (const auto& t : tests) {
        auto results = eval_operators(t.operands, true);
        if (std::find(results.begin(), results.end(), t.test_val) != results.end()) {
            sum2 += t.test_val;
        }
    }
    std::cout << sum2 << std::endl;

    return 0;
}