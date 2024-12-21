#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
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
    int i = 0;
    int j = 0;
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
#ifdef VERBOSE
    double start_time = omp_get_wtime();
#endif
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

    std::vector<std::string> patterns;
    std::map<std::string, std::vector<ull>> pattern_positions;
    std::string pattern;
    while (std::getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
    }

    // Parallelize the pattern matching
    #pragma omp parallel for
    for (size_t i = 0; i < patterns.size(); ++i) {
        std::vector<ull> positions = kmp_search(text, patterns[i]);
        
        pattern_positions[patterns[i]] = positions;
    }

    // Output the results in the order of patterns in target.txt
    for (const auto& p : patterns) {
        const auto& positions = pattern_positions[p];
        std::cout << positions.size();
        for (auto pos : positions) {
            std::cout << " " << pos;
        }
        std::cout << std::endl;
    }
#ifdef VERBOSE
    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;
#endif
    patterns_file.close();

    return 0;
}