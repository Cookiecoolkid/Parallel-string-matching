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
std::unordered_map<std::string, std::vector<ull>> search(const std::string& text, TrieNode* root, const std::vector<std::string>& patterns) {
    std::unordered_map<std::string, std::vector<ull>> foundPositions;
    ull newline_count = 0; // Count of newlines encountered
    for (ull i = 0; i < text.size(); ++i) {
        if (text[i] == '\n') {
            newline_count++;
            continue;
        }
        TrieNode* node = root;
        ull adjusted_i = i - newline_count; // Adjusted index that ignores newlines
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
                foundPositions[patterns[node->patternIndex]].push_back(adjusted_i);
            }
        }
    }
    return foundPositions;
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

    // Perform the search using the Trie tree
    std::unordered_map<std::string, std::vector<ull>> foundPositions = search(text, root, patterns);

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