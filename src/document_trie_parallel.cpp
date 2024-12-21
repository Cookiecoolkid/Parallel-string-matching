#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <omp.h>
#include <algorithm>

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
void search(const std::string& text, TrieNode* root, const std::vector<std::string>& patterns, std::unordered_map<std::string, std::vector<ull>>& localFoundPositions, ull start, ull end) {
    ull newline_count = 0; // Count of newlines encountered in the current chunk
    for (ull i = start; i < end; ++i) {
        if (text[i] == '\n') {
            newline_count++;
            continue;
        }
        TrieNode* node = root;
        for (ull j = i; j < text.size(); ++j) {
            if (text[j] == '\n') {
                newline_count++;
                continue; 
            }
            if (node->children[static_cast<unsigned char>(text[j])] == nullptr) {
                break;
            }
            node = node->children[static_cast<unsigned char>(text[j])];
            if (node->isEndOfWord) {
                localFoundPositions[patterns[node->patternIndex]].push_back(i - newline_count);
            }
        }
    }
}

// Function to calculate the total number of newlines in each chunk
void calculateNewlineTotals(const std::string& text, ull num_threads, ull chunk_size, std::vector<ull>& newlineTotals) {
    #pragma omp parallel for
    for (ull i = 0; i < num_threads; ++i) {
        ull start = i * chunk_size;
        ull end = (i == num_threads - 1) ? text.size() : start + chunk_size;
        ull newline_count = 0;
        for (ull j = start; j < end; ++j) {
            if (text[j] == '\n') {
                newline_count++;
            }
        }
        newlineTotals[i] = newline_count;
    }

    for (ull i = 1; i < num_threads; ++i) {
        newlineTotals[i] += newlineTotals[i - 1];
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
    ull num_threads = omp_get_max_threads();
    ull chunk_size = text_size / num_threads;

    size_t max_pattern_length = 0;
    for (const auto& pattern : patterns) {
        max_pattern_length = std::max(max_pattern_length, pattern.size());
    }

    #pragma omp parallel
    {
        std::unordered_map<std::string, std::vector<ull>> localFoundPositions;
        ull thread_id = omp_get_thread_num();
        ull start = thread_id * chunk_size;
        ull end = (thread_id == num_threads - 1) ? text_size : start + chunk_size + max_pattern_length;
        search(text, root, patterns, localFoundPositions, start, end);

        #pragma omp critical
        {
            for (const auto& entry : localFoundPositions) {
                foundPositions[entry.first].insert(foundPositions[entry.first].end(), entry.second.begin(), entry.second.end());
            }
        }
    }


    // Calculate the total number of newlines in each chunk
    std::vector<ull> newlineTotals(num_threads);
    calculateNewlineTotals(text, num_threads, chunk_size, newlineTotals);

    // // Output the newline totals
    // for (ull i = 0; i < num_threads; ++i) {
    //     std::cout << "Chunk " << i << " newline total: " << newlineTotals[i] << std::endl;
    // }

    // std::cout << "======================================================" << std::endl;    


    // Update positions based on the total number of newlines in previous chunks
    for (auto& entry : foundPositions) {
        for (auto& pos : entry.second) {
            ull chunk_id = pos / chunk_size;
            if (chunk_id > 0) {
                pos -= newlineTotals[chunk_id - 1]; // Subtract the total number of newlines in previous chunks
            }
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

    delete root;
    patterns_file.close();

    return 0;
}