#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <omp.h>

typedef unsigned long long ull;

struct TrieNode {
    std::vector<TrieNode*> children;
    bool isEndOfWord;
    ull patternIndex; // Index of the pattern in the patterns vector

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
void search(const std::string& text, TrieNode* root, const std::vector<std::string>& patterns, std::unordered_map<std::string, std::vector<ull>>& foundPositions, ull start, ull end) {
    for (ull i = start; i < end; ++i) {
        TrieNode* node = root;
        for (ull j = i; j < text.size(); ++j) {
            if (node->children[static_cast<unsigned char>(text[j])] == nullptr) {
                break;
            }
            node = node->children[static_cast<unsigned char>(text[j])];
            if (node->isEndOfWord) {
                foundPositions[patterns[node->patternIndex]].push_back(i);
            }
        }
    }
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

    // Initialize the Trie tree
    TrieNode* root = new TrieNode();

    // Read patterns from the patterns file and insert them into the Trie tree
    std::vector<std::string> patterns;
    ull patternIndex = 0;
    std::string pattern;
    while (std::getline(patterns_file, pattern)) {
        patterns.push_back(pattern);
        insert(root, pattern, patternIndex++);
    }

    // Perform the search using the Trie tree with OpenMP parallelization
    std::unordered_map<std::string, std::vector<ull>> foundPositions;
    ull text_size = text.size();
    #pragma omp parallel
    {
        std::unordered_map<std::string, std::vector<ull>> localFoundPositions;
        ull num_threads = omp_get_num_threads();
        ull thread_id = omp_get_thread_num();
        ull chunk_size = text_size / num_threads;
        ull start = thread_id * chunk_size;
        ull end = (thread_id == num_threads - 1) ? text_size : start + chunk_size;
        search(text, root, patterns, localFoundPositions, start, end);

        #pragma omp critical
        {
            for (const auto& entry : localFoundPositions) {
                foundPositions[entry.first].insert(foundPositions[entry.first].end(), entry.second.begin(), entry.second.end());
            }
        }
    }

    // Adjust positions based on newlines
    ull newline_count = 0;
    for (auto& entry : foundPositions) {
        for (auto& pos : entry.second) {
            while (pos + newline_count < text.size() && text[pos + newline_count] == '\n') {
                newline_count++;
            }
            pos -= newline_count;
        }
    }

    // Output the results
    for (const auto& pattern : patterns) {
        auto it = foundPositions.find(pattern);
        if (it != foundPositions.end()) {
            std::cout << "Pattern \"" << pattern << "\" found at positions: " << it->second.size();
            for (auto pos : it->second) {
                std::cout << " " << pos;
            }
            std::cout << std::endl;
        } else {
            std::cout << "Pattern \"" << pattern << "\" found at positions: 0" << std::endl;
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Execution time: " << end_time - start_time << " seconds." << std::endl;

    // Clean up
    delete root;
    patterns_file.close();

    return 0;
}