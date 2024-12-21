#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <omp.h>

namespace fs = std::filesystem;

typedef unsigned long long ull;

struct TrieNode {
    std::vector<TrieNode*> children;
    bool isEndOfWord;
    ull patternIndex;

    TrieNode() : isEndOfWord(false), patternIndex(-1) {
        children.resize(256, nullptr); // ASCII 字符集
    }
};

void insert(TrieNode* root, const std::string& pattern, ull patternIndex) {
    TrieNode* node = root;
    for (char ch : pattern) {
        if (node->children[static_cast<unsigned char>(ch)] == nullptr) {
            node->children[static_cast<unsigned char>(ch)] = new TrieNode();
        }
        node = node->children[static_cast<unsigned char>(ch)];
    }
    node->isEndOfWord = true;
    node->patternIndex = patternIndex;
}

// Function to search for all patterns in the text using the Trie
std::unordered_map<ull, std::string> search(const std::string& text, TrieNode* root, const std::vector<std::string>& pattern_files) {
    std::unordered_map<ull, std::string> matchedPatterns;
    for (ull i = 0; i < text.size(); ++i) {
        TrieNode* node = root;
        for (ull j = i; j < text.size(); ++j) {
            if (node->children[static_cast<unsigned char>(text[j])] == nullptr) {
                break;
            }
            node = node->children[static_cast<unsigned char>(text[j])];
            if (node->isEndOfWord) {
                matchedPatterns[node->patternIndex] = pattern_files[node->patternIndex];
            }
        }
    }
    return matchedPatterns;
}

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
    double start_time = omp_get_wtime();
    
    std::string text_directory = "data/software_antivirus/opencv-4.10.0/";
    std::string patterns_directory = "data/software_antivirus/virus/";

    std::vector<std::string> text_files = get_all_files(text_directory);

    std::vector<std::string> pattern_files = get_all_files(patterns_directory);

    // Initialize the Trie tree
    TrieNode* root = new TrieNode();

    // Read patterns from the pattern files and insert them into the Trie tree
    ull patternIndex = 0;
    for (const auto& pattern_file : pattern_files) {
        std::string pattern = read_file(pattern_file);
        if (!pattern.empty()) {
            insert(root, pattern, patternIndex++);
        }
    }

    // matching process for each text file
    for (size_t i = 0; i < text_files.size(); ++i) {
        std::string text = read_file(text_files[i]);
        if (text.empty()) {
            continue;
        }

        std::unordered_map<ull, std::string> matchedPatterns = search(text, root, pattern_files);

        if (!matchedPatterns.empty()) {
            std::cout << text_files[i];
            for (const auto& [index, pattern_file] : matchedPatterns) {
                std::cout << " " << fs::path(pattern_file).filename().string();
            }
            std::cout << std::endl;
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;

    delete root;

    return 0;
}