#include "fonctions.h"

static unsigned int readUInt16At(std::ifstream& file, unsigned int offset)
{
    unsigned char bytes[2];

    file.seekg(offset);
    file.read(reinterpret_cast<char*>(bytes), 2);

    return bytes[0] | (bytes[1] << 8);
}

static unsigned int readUInt32At(std::ifstream& file, unsigned int offset)
{
    unsigned char bytes[4];

    file.seekg(offset);
    file.read(reinterpret_cast<char*>(bytes), 4);

    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

static void displayHexByte(unsigned char byte)
{
    const char digits[] = "0123456789ABCDEF";

    std::cout << digits[byte >> 4];
    std::cout << digits[byte & 15];
}

static void displayBinaryByte(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
    {
        std::cout << ((byte >> i) & 1);
    }
}

std::ifstream openRead(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Cannot open file for reading.");
    }

    return file;
}

std::ofstream openWrite(const std::string& path)
{
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Cannot open file for writing.");
    }

    return file;
}

void closeRead(std::ifstream& file)
{
    file.close();
}

void closeWrite(std::ofstream& file)
{
    file.close();
}

void displayFileBytes(std::ifstream& file, unsigned int count)
{
    file.clear();
    file.seekg(0);

    for (unsigned int i = 0; i < count; i++)
    {
        unsigned char byte;
        file.read(reinterpret_cast<char*>(&byte), 1);

        if (!file) {
            break;
        }

        std::cout << i << ": ";
        displayHexByte(byte);
        std::cout << "  ";
        displayBinaryByte(byte);
        std::cout << '\n';
    }

    file.clear();
    file.seekg(0);
}

void cloneFile(std::ifstream& file, const std::string& outputPath)
{
    std::ofstream output = openWrite(outputPath);
    char byte;

    file.clear();
    file.seekg(0);

    while (file.get(byte)) {
        output.put(byte);
    }

    if (!output) {
        closeWrite(output);
		throw std::runtime_error("Error while writing to the output file.");
    }

    closeWrite(output);
    file.clear();
    file.seekg(0);
}

bool compareFiles(const std::string& path1, const std::string& path2)
{
    std::ifstream first = openRead(path1);
    std::ifstream second = openRead(path2);
    char byte1, byte2;

    while (first.get(byte1) && second.get(byte2))
    {
        if (byte1 != byte2)
            return false;
    }

    return first.eof() && second.eof();
}

bool isBMP(std::ifstream& file)
{
    unsigned char bytes[2];

    file.clear();
    file.seekg(0);
    file.read(reinterpret_cast<char*>(bytes), 2);
    file.seekg(0);

    return bytes[0] == 'B' && bytes[1] == 'M';
}

unsigned int getFileSize(std::ifstream& file)
{
    return readUInt32At(file, 2);
}

bool isbfReserved(std::ifstream& file)
{
    return readUInt16At(file, 6) == 0
        && readUInt16At(file, 8) == 0;
}

BMPInfo getBMPInfo(std::ifstream& file)
{
    BMPInfo info;

    unsigned int headerSize = readUInt32At(file, 14);
    unsigned int planes = readUInt16At(file, 26);
    unsigned int bits = readUInt16At(file, 28);
    unsigned int compression = readUInt32At(file, 30);

    info.pixelOffset = readUInt32At(file, 10);
    info.width = readUInt32At(file, 18);
    info.height = readUInt32At(file, 22);

    if (headerSize != 40 || planes != 1 || bits != 24) {
        throw std::runtime_error("Unsupported BMP format.");
    }

    if (compression != 0 || info.width == 0 || info.height == 0) {
        throw std::runtime_error("Invalid BMP dimensions or compression.");
    }

    if (info.width > 0x7FFFFFFF || info.height > 0x7FFFFFFF) {
        throw std::runtime_error("Unsupported BMP dimensions.");
    }

    info.stride = ((info.width * 3 + 3) / 4) * 4;
    info.componentCount = info.width * info.height * 3;

    unsigned int fileSize = getFileSize(file);

    if (info.pixelOffset + info.stride * info.height > fileSize) {
        throw std::runtime_error("BMP pixel data is incomplete.");
    }

    return info;
}

unsigned int getPixelComponentOffset(
    const BMPInfo& info, unsigned int index)
{
    if (index >= info.componentCount) {
        throw std::runtime_error("Pixel index out of range.");
    }

    unsigned int row = index / (info.width * 3);
    unsigned int column = index % (info.width * 3);

    return info.pixelOffset + row * info.stride + column;
}

unsigned char readByteAt(std::ifstream& file, unsigned int offset)
{
    unsigned char byte;

    file.clear();
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(&byte), 1);

    if (!file) {
        throw std::runtime_error("Cannot read byte.");
    }

    return byte;
}

void writeLSB(std::fstream& file, unsigned int offset, unsigned char bit)
{
    unsigned char byte;

    file.clear();
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(&byte), 1);

    if (!file){
        throw std::runtime_error("Cannot read byte.");
    }

    byte = (byte & 254) | bit;

    file.clear();
    file.seekp(offset);
    file.write(reinterpret_cast<char*>(&byte), 1);

    if (!file) {
        throw std::runtime_error("Cannot write byte.");
    }
}

unsigned char readLSB(std::ifstream& file, unsigned int offset)
{
    return readByteAt(file, offset) & 1;
}

void hideByte(
    std::fstream& file, const BMPInfo& info,
    unsigned int startIndex, unsigned char value)
{
    if (startIndex + 8 > info.componentCount){
        throw std::runtime_error("Not enough pixel components.");
    }

    for (int i = 0; i < 8; i++)
    {
        unsigned char bit = (value >> (7 - i)) & 1;
        unsigned int offset = getPixelComponentOffset(info, startIndex + i);

        writeLSB(file, offset, bit);
    }
}

unsigned char extractByte(
    std::ifstream& file, const BMPInfo& info, unsigned int startIndex)
{
    unsigned char value = 0;

    if (startIndex + 8 > info.componentCount){
        throw std::runtime_error("Not enough pixel components.");
    }

    for (int i = 0; i < 8; i++)
    {
        unsigned int offset = getPixelComponentOffset(info, startIndex + i);
        value = (value << 1) | readLSB(file, offset);
    }

    return value;
}