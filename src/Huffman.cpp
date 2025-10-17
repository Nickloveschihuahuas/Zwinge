#include "Huffman.h"
#include <iostream>
#include <algorithm>
#include <bitset>

// Constants for tree serialization
const unsigned char INTERNAL_NODE_MARKER = 0;
const unsigned char LEAF_NODE_MARKER = 1;

Huffman::Huffman() : root(nullptr) {}

Huffman::~Huffman() {}

void Huffman::buildFrequencyMap(std::ifstream& inputFile) {
    frequencies.clear();
    unsigned char byte;
    while (inputFile.read(reinterpret_cast<char*>(&byte), 1)) {
        frequencies[byte]++;
    }
    inputFile.clear();
    inputFile.seekg(0);
}

void Huffman::buildHuffmanTree() {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNode::CompareNodes> pq;

    for (auto const& [byte, freq] : frequencies) {
        pq.push(new HuffmanNode(byte, freq));
    }

    if (pq.empty()) {
        root = nullptr;
        return;
    }

    if (pq.size() == 1) {
        root.reset(pq.top());
        pq.pop();
        std::unique_ptr<HuffmanNode> single_leaf = std::move(root);
        root = std::make_unique<HuffmanNode>(single_leaf->frequency, std::move(single_leaf), nullptr);
        return;
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* newNode = new HuffmanNode(left->frequency + right->frequency,
                                               std::unique_ptr<HuffmanNode>(left),
                                               std::unique_ptr<HuffmanNode>(right));
        pq.push(newNode);
    }

    root.reset(pq.top());
    pq.pop();
}

void Huffman::generateHuffmanCodes(const HuffmanNode* node, const std::string& currentCode) {
    if (!node) {
        return;
    }

    if (!node->left && !node->right) {
        huffmanCodes[node->data] = currentCode.empty() ? "0" : currentCode;
    } else {
        generateHuffmanCodes(node->left.get(), currentCode + "0");
        generateHuffmanCodes(node->right.get(), currentCode + "1");
    }
}

void Huffman::writeTree(std::ofstream& outputFile, const HuffmanNode* node) {
    if (!node) {
        return;
    }

    if (!node->left && !node->right) {
        outputFile.put(LEAF_NODE_MARKER);
        outputFile.put(node->data);
    } else {
        outputFile.put(INTERNAL_NODE_MARKER);
        writeTree(outputFile, node->left.get());
        writeTree(outputFile, node->right.get());
    }
}

std::unique_ptr<HuffmanNode> Huffman::readTree(std::ifstream& inputFile) {
    if (!inputFile.good() || inputFile.eof()) {
        return nullptr;
    }

    unsigned char marker;
    inputFile.get(reinterpret_cast<char&>(marker));

    if (marker == LEAF_NODE_MARKER) {
        unsigned char data;
        inputFile.get(reinterpret_cast<char&>(data));
        return std::make_unique<HuffmanNode>(data, 0);
    } else if (marker == INTERNAL_NODE_MARKER) {
        std::unique_ptr<HuffmanNode> left = readTree(inputFile);
        std::unique_ptr<HuffmanNode> right = readTree(inputFile);
        return std::make_unique<HuffmanNode>(0, std::move(left), std::move(right));
    }
    return nullptr;
}

bool Huffman::compressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream ifs(inputFile, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Error: Could not open input file for compression: " << inputFile << std::endl;
        return false;
    }

    buildFrequencyMap(ifs);

    if (frequencies.empty()) {
        std::cerr << "Warning: Input file is empty. Creating an empty compressed file." << std::endl;
        std::ofstream ofs_empty(outputFile, std::ios::binary);
        if (!ofs_empty.is_open()) {
            std::cerr << "Error: Could not create output file: " << outputFile << std::endl;
            return false;
        }
        ofs_empty.close();
        return true;
    }

    buildHuffmanTree();
    huffmanCodes.clear();
    generateHuffmanCodes(root.get(), "");

    std::ofstream ofs(outputFile, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Error: Could not create output file: " << outputFile << std::endl;
        return false;
    }

    writeTree(ofs, root.get());

    std::streampos padding_pos = ofs.tellp();
    ofs.put(0);

    std::string currentBits;
    unsigned char byte;
    while (ifs.read(reinterpret_cast<char*>(&byte), 1)) {
        currentBits += huffmanCodes[byte];
        while (currentBits.length() >= 8) {
            std::bitset<8> bs(currentBits.substr(0, 8));
            ofs.put(static_cast<unsigned char>(bs.to_ulong()));
            currentBits = currentBits.substr(8);
        }
    }

    unsigned char paddingBits = 0;
    if (!currentBits.empty()) {
        paddingBits = 8 - currentBits.length();
        while (currentBits.length() < 8) {
            currentBits += '0';
        }
        std::bitset<8> bs(currentBits.substr(0, 8));
        ofs.put(static_cast<unsigned char>(bs.to_ulong()));
    }

    ofs.seekp(padding_pos);
    ofs.put(paddingBits);
    ofs.seekp(0, std::ios::end);

    ifs.close();
    ofs.close();

    std::cout << "File compressed successfully: " << inputFile << " -> " << outputFile << std::endl;
    return true;
}

bool Huffman::decompressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream ifs(inputFile, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Error: Could not open input file for decompression: " << inputFile << std::endl;
        return false;
    }

    std::ofstream ofs(outputFile, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Error: Could not create output file: " << outputFile << std::endl;
        return false;
    }

    root = readTree(ifs);

    if (!root) {
        std::cerr << "Warning: Could not read Huffman tree from compressed file. File might be empty or corrupted." << std::endl;
        ifs.close();
        ofs.close();
        return true;
    }

    unsigned char paddingBits;
    ifs.read(reinterpret_cast<char*>(&paddingBits), 1);

    HuffmanNode* currentNode = root.get();
    unsigned char currentByte;

    std::streampos start_data_pos = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    std::streampos end_file_pos = ifs.tellg();
    ifs.seekg(start_data_pos);

    long long compressedDataSize = end_file_pos - start_data_pos;

    long long bytesRead = 0;
    while (ifs.read(reinterpret_cast<char*>(&currentByte), 1)) {
        bytesRead++;
        std::bitset<8> bs(currentByte);
        std::string byteBits = bs.to_string();

        if (bytesRead == compressedDataSize) {
            byteBits = byteBits.substr(0, 8 - paddingBits);
        }

        for (char bit : byteBits) {
            if (bit == '0') {
                currentNode = currentNode->left.get();
            } else {
                currentNode = currentNode->right.get();
            }

            if (!currentNode) {
                std::cerr << "Error during decompression: Reached null node in Huffman tree." << std::endl;
                ifs.close();
                ofs.close();
                return false;
            }

            if (!currentNode->left && !currentNode->right) {
                ofs.put(currentNode->data);
                currentNode = root.get();
            }
        }
    }

    ifs.close();
    ofs.close();

    std::cout << "File decompressed successfully: " << inputFile << " -> " << outputFile << std::endl;
    return true;
}