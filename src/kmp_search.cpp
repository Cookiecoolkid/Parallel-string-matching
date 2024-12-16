#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>

typedef unsigned long long ull;

// Function to compute the LPS (Longest Prefix Suffix) array for KMP algorithm
std::vector<int> compute_lps(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int len = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

// Function to perform KMP string matching
std::vector<ull> kmp_search(const std::string& text, const std::string& pattern) {
    std::vector<ull> positions;
    int n = text.size();
    int m = pattern.size();
    std::vector<int> lps = compute_lps(pattern);
    int i = 0; // index for text
    int j = 0; // index for pattern
    ull newline_count = 0;

    while (i < n) {
        if (text[i] == '\n') {
            ++newline_count;
        }
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }

        if (j == m) {
            positions.push_back(i - j - newline_count);
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
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
    std::string pattern;

    // record the execution time

    while (std::getline(patterns_file, pattern)) {
        std::vector<ull> positions = kmp_search(text, pattern);

        std::cout << "Pattern \"" << pattern << "\" found at positions: " << positions.size();
        for (auto pos : positions) {
            std::cout << " " << pos;
        }
        std::cout << std::endl;
    }

    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;

    text_file.close();
    patterns_file.close();

    return 0;
}