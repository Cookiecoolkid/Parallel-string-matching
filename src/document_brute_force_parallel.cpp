#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <omp.h>

typedef unsigned long long ull;

// Function to perform brute-force string matching
std::vector<ull> brute_force_search(const std::string& text, const std::string& pattern) {
    std::vector<ull> positions;
    ull n = text.size();
    ull m = pattern.size();
    ull newline_count = 0;

    #pragma omp parallel for
    for (ull i = 0; i <= n - m; ++i) {
        if (text[i] == '\n') {
            ++newline_count;
        }
        ull j = 0;
        while (j < m && text[i + j] == pattern[j]) {
            ++j;
        }
        if (j == m) {
            positions.push_back(i - newline_count);
        }
    }

    return positions;
}

// Function to read a file into a string
std::string read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, ' ');
    if (!file.read(&buffer[0], size)) {
        std::cerr << "Error reading file: " << filename << std::endl;
        return "";
    }

    return buffer;
}

int main() {
    double start_time = omp_get_wtime();
    
    std::string textfile = "./data/document_retrieval/document.txt";
    std::string patternsfile = "./data/document_retrieval/target.txt";

    // Read the entire text file into a string
    std::string text = read_file(textfile);

    if (text.empty()) {
        return 1;
    }

    std::ifstream patterns_file(patternsfile);
    if (!patterns_file.is_open()) {
        std::cerr << "Error opening file: " << patternsfile << std::endl;
        return 1;
    }

    // Read patterns from the patterns file
    std::vector<std::string> patterns;
    std::map<std::string, std::vector<ull>> pattern_positions;
    std::string pattern;
    while (std::getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
    }

    // Parallelize the pattern matching
    #pragma omp parallel for
    for (size_t i = 0; i < patterns.size(); ++i) {
        std::vector<ull> positions = brute_force_search(text, patterns[i]);

        pattern_positions[patterns[i]] = positions;
    }

    // Output the results in the order of patterns in target.txt
    for (const auto& p : patterns) {
        const auto& positions = pattern_positions[p];
        std::cout << "Pattern \"" << p << "\" found at positions: " << positions.size();
        for (auto pos : positions) {
            std::cout << " " << pos;
        }
        std::cout << std::endl;
    }

    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;

    patterns_file.close();

    return 0;
}