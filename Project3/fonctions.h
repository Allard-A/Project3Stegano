#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

// Opens the chosen file so you can read it.
std::ifstream openFileRead(const std::string& path);

// Opens the chosen file so you can edit it.
std::ofstream openFileWrite(const std::string& path);

// Closes the chosen file.
void closeFile(std::ifstream& file);

// Reads the first two bytes of the chosen file to know if the type is BMP.
bool isBMP(std::ifstream& file);

// Reads the header of the chosen file to know the size of the file.
int getFileSize(std::ifstream& file);

// Reads the header of the chosen file to know if Reserved1 and 2 are equal to 0.
bool isbfReserved(std::ifstream& file);

// Creates an identical copy of the chosen file.
void cloneFile(std::ifstream& file);

// Returns the binary content of the file without the header.
// _ getImageInfo(std::ifstream& file);

// 
// 