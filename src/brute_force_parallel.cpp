#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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

int main() {
    double start_time = omp_get_wtime();
    
    std::string textfile = "./data/document_retrieval/document.txt";
    std::string patternsfile = "./data/document_retrieval/target.txt";
    std::ifstream text_file(textfile);
    std::ifstream patterns_file(patternsfile);

    if (!text_file.is_open() || !patterns_file.is_open()) {
        std::cerr << "Error opening file(s)." << std::endl;
        return 1;
    }

    // Read the entire text file into a string
    std::string text((std::istreambuf_iterator<char>(text_file)), std::istreambuf_iterator<char>());

    // Read patterns from the patterns file
    std::vector<std::string> patterns;
    std::string pattern;
    while (std::getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
    }

    // Parallelize the pattern matching
    #pragma omp parallel for
    for (size_t i = 0; i < patterns.size(); ++i) {
        std::vector<ull> positions = brute_force_search(text, patterns[i]);

        #pragma omp critical
        {
            std::cout << "Pattern \"" << patterns[i] << "\" found at positions: " << positions.size();
            for (auto pos : positions) {
                std::cout << " " << pos;
            }
            std::cout << std::endl;
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;

    text_file.close();
    patterns_file.close();

    return 0;
}