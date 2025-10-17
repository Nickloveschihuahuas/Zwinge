#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <memory> // For std::unique_ptr

// Represents a node in the Huffman tree
struct HuffmanNode {
    unsigned char data;
    unsigned int frequency;
    std::unique_ptr<HuffmanNode> left;
    std::unique_ptr<HuffmanNode> right;

    HuffmanNode(unsigned char d, unsigned int freq) : data(d), frequency(freq), left(nullptr), right(nullptr) {}
    HuffmanNode(unsigned int freq, std::unique_ptr<HuffmanNode> l, std::unique_ptr<HuffmanNode> r)
        : data('\0'), frequency(freq), left(std::move(l)), right(std::move(r)) {}

    // For priority queue comparison
    struct CompareNodes {
        bool operator()(const HuffmanNode* a, const HuffmanNode* b) {
            return a->frequency > b->frequency;
        }
    };
};

class Huffman {
public:
    Huffman();
    ~Huffman();

    // Compress a file
    bool compressFile(const std::string& inputFile, const std::string& outputFile);

    // Decompress a file
    bool decompressFile(const std::string& inputFile, const std::string& outputFile);

private:
    std::map<unsigned char, unsigned int> frequencies;
    std::map<unsigned char, std::string> huffmanCodes;
    std::unique_ptr<HuffmanNode> root;

    // Build frequency map from input file
    void buildFrequencyMap(std::ifstream& inputFile);

    // Build the Huffman tree
    void buildHuffmanTree();

    // Generate Huffman codes from the tree
    void generateHuffmanCodes(const HuffmanNode* node, const std::string& currentCode);

    // Write the Huffman tree structure to the output file (header)
    void writeTree(std::ofstream& outputFile, const HuffmanNode* node);

    // Read the Huffman tree structure from the input file (header)
    std::unique_ptr<HuffmanNode> readTree(std::ifstream& inputFile);

    // Helper for clearing the tree (if not using unique_ptr for auto-management)
    // void deleteTree(HuffmanNode* node);
};

#endif // HUFFMAN_H