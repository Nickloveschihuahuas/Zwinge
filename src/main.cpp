#include <iostream>
#include <string>
#include "Huffman.h"

void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  To compress: ./huffman -c <input_file> <output_file.huff>" << std::endl;
    std::cout << "  To decompress: ./huffman -d <input_file.huff> <output_file>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    std::string option = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    Huffman huffman;

    if (option == "-c") {
        if (!huffman.compressFile(inputFile, outputFile)) {
            std::cerr << "Compression failed." << std::endl;
            return 1;
        }
    } else if (option == "-d") {
        if (!huffman.decompressFile(inputFile, outputFile)) {
            std::cerr << "Decompression failed." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Invalid option: " << option << std::endl;
        printUsage();
        return 1;
    }

    return 0;
}