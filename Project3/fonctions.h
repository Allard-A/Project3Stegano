#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>


std::ifstream openFileRead(const std::string& path);

std::ofstream openFileWrite(const std::string& path);

void closeFile(std::ifstream& file);

bool isBMP(std::ifstream& file);

int getFileSize(std::ifstream& file);

bool isbfReserved(std::ifstream& file);

