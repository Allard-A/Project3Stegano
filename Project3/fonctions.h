#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Useful sources used in this project :
// - https://en.wikipedia.org/wiki/BMP_file_format
// - https://www.fileformat.info/format/bmp/egff.htm (goes deeper)
// - I lost the geekforgeeks link
// - https://cppreference.com/cpp/header/fstream
// - https://cppreference.com/cpp/header/cstdint
// - https://cppreference.com/cpp/header/stdexcept

// Structure to hold BMP file information. (from header)
struct BMPInfo
{
    unsigned int pixelOffset;
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int componentCount;
};

// Opens the chosen file so you can read it.
std::ifstream openRead(const std::string& path);

// Opens the chosen file so you can edit it.
std::ofstream openWrite(const std::string& path);

// Closes the chosen file.
void close(std::ifstream& file);

// Closes the chosen file.
void close(std::ofstream& file);

// Displays the bytes of the chosen file.
void displayFileBytes(std::ifstream& file);

// Creates an identical copy of the chosen file.
void cloneFile(std::ifstream& file, const std::string& outputPath);

// Compares two files to see if they are identical.
bool compareFiles(const std::string& path1, const std::string& path2);

// Reads the first two bytes of the chosen file to know if the type is BMP.
bool isBMP(std::ifstream& file);

// Reads the header of the chosen file to know the size of the file.
unsigned int getFileSize(std::ifstream& file);

// Reads the header of the chosen file to know if Reserved1 and 2 are equal to 0.
bool isbfReserved(std::ifstream& file);

// Reads the header of the chosen file to get BMP information.
BMPInfo getBMPInfo(std::ifstream& file);

// Gets the offset of the specified pixel component.
unsigned int getPixelComponentOffset(const BMPInfo& info, unsigned int index);

// Reads the byte at the specified offset.
std::uint8_t readByteAt(std::ifstream& file, unsigned int offset);

// Writes a bit to the least significant bit of the byte at the specified offset.
void writeLSB(std::fstream& file, unsigned int offset, std::uint8_t bit);

// Reads the least significant bit of the byte at the specified offset.
std::uint8_t readLSB(std::ifstream& file, unsigned int offset);

// Hides one complete byte inside eight color components.
void hideByte(std::fstream& file, const BMPInfo& info, unsigned int startIndex, std::uint8_t value);

// Extracts one complete byte from eight color components.
std::uint8_t extractByte(std::ifstream& file, const BMPInfo& info, unsigned int startIndex);

// Transforms a char into its ASCII value.
std::uint8_t charToASCII(char letter);

// Transforms a string into an ASCII vector (that stores the ASCII values of each character).
std::vector<std::uint8_t> strToASCII(const std::string& text);

// Hides a complete string inside the image.
// First 2 bytes are used to store the length of the string, then the string is stored.
void hideString(std::fstream& file, const BMPInfo& info, const std::string& message);