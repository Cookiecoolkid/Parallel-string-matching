#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <omp.h>
#include <algorithm>

namespace fs = std::filesystem;

typedef long long ll;

// Function to perform brute-force string matching
bool brute_force_search(const std::string& text, const std::string& pattern) {
    ll n = text.size();
    ll m = pattern.size();
    bool found = false;

    #pragma omp parallel for shared(found)
    for (ll i = 0; i <= n - m; ++i) {
        if (found) continue;
        ll j = 0;
        while (j < m && text[i + j] == pattern[j]) {
            ++j;
        }
        if (j == m) {
            #pragma omp critical
            found = true;
        }
    }

    return found;
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

// Function to get all files in a directory recursively
std::vector<std::string> get_all_files(const std::string& directory, const std::string& extension = "") {
    std::vector<std::string> files;
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && (extension.empty() || entry.path().extension() == extension)) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

int main() {
#ifdef VERBOSE
    double start_time = omp_get_wtime();
#endif
    std::string text_directory = "data/software_antivirus/opencv-4.10.0/";
    std::string patterns_directory = "data/software_antivirus/virus/";

    std::vector<std::string> text_files = get_all_files(text_directory);

    std::vector<std::string> pattern_files = get_all_files(patterns_directory);

    // Read patterns from the pattern files
    std::vector<std::string> patterns;
    for (const auto& pattern_file : pattern_files) {
        std::string pattern = read_file(pattern_file);
        if (!pattern.empty()) {
            patterns.push_back(pattern);
        }
    }

    // Parallelize the file reading and pattern matching
    #pragma omp parallel for
    for (size_t i = 0; i < text_files.size(); ++i) {
        std::string text = read_file(text_files[i]);
        if (text.empty()) {
            continue;
        }

        std::vector<std::string> matched_patterns;
        for (size_t j = 0; j < patterns.size(); ++j) {
            if (brute_force_search(text, patterns[j])) {
                matched_patterns.push_back(pattern_files[j]);
            }
        }

        if (!matched_patterns.empty()) {
            #pragma omp critical
            {
                std::cout <<  text_files[i];
                for (const auto& pattern_file : matched_patterns) {
                    std::cout << " " << fs::path(pattern_file).filename().string();
                }
                std::cout << std::endl;
            }
        }
    }
    
#ifdef VERBOSE
    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;
#endif
    return 0;
}