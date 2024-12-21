#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>

typedef unsigned long long ull;

// Function to build the next array for KMP algorithm
std::vector<int> build_next_array(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> next(m, 0);
    int j = 0;
    int k = 1;

    while (k < m) {
        if (pattern[k] == pattern[j]) {
            next[k] = ++j;
            k++;
        } else {
            if (j != 0) {
                j = next[j - 1];
            } else {
                next[k] = 0;
                k++;
            }
        }
    }

    return next;
}

// Function to perform KMP string matching
std::vector<ull> kmp_search(const std::string& text, const std::string& pattern) {
    std::vector<ull> positions;
    ull n = text.size();
    ull m = pattern.size();
    std::vector<int> next = build_next_array(pattern);
    ull i = 0; // index for text
    ull j = 0; // index for pattern
    ull newline_count = 0;

    while (i < n) {
        if (text[i] == '\n') ++newline_count;
        
        if (text[i] == pattern[j]) {
            ++i;
            ++j;

            if (j == m) {
                positions.push_back(i - j - newline_count);
                j = next[j - 1];
            }
        } else {
            if (j != 0) {
                j = next[j - 1];
            } else {
                ++i;
            }
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
    std::string pattern;
    while (std::getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
    }

    // pattern matching
    for (size_t i = 0; i < patterns.size(); ++i) {
        std::vector<ull> positions = kmp_search(text, patterns[i]);
        
        std::cout << "Pattern \"" << patterns[i] << "\" found at positions: " << positions.size();
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