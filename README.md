# Zwinge

A C++ implementation of the Huffman coding algorithm for efficient file compression and decompression.

## Features

*   Compresses any given file into a `.huff` format.
*   Decompresses `.huff` files back to their original content.
*   Command-line interface for easy use.

## How Huffman Coding Works

Huffman coding is a variable-length prefix coding technique used for lossless data compression. It works by analyzing the frequency of each byte (or character) in the input data. More frequent bytes are assigned shorter binary codes, while less frequent bytes are assigned longer codes. This results in an overall reduction in the total number of bits required to represent the data.

The compression process involves:
1.  **Frequency Analysis:** Counting the occurrences of each byte.
2.  **Huffman Tree Construction:** Building a binary tree where leaves represent bytes and internal nodes represent combined frequencies. The path from the root to a leaf gives the Huffman code for that byte.
3.  **Encoding:** Replacing each byte in the input with its corresponding Huffman code.
4.  **Header Generation:** Storing the Huffman tree structure (or the codes themselves) in the compressed file so the decompressor can reconstruct it.

Decompression reverses this process by reading the header, reconstructing the tree, and then using the codes to traverse the tree and reconstruct the original bytes.

## Building the Project

To build this project, you will need a C++ compiler (like g++).

```bash
cd huffman_compressor
mkdir build
cd build
cmake ..
make