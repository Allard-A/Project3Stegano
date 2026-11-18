#include "fonctions.h"

// WIP : 
// - Add more commentaries to make the code easier to understand.
// - Add a function to decode the hidden message in the BMP file.
// - Add a function to handle the Xor cypher (with a key obviously) for the hidden message.

static unsigned short readUInt16At(std::ifstream& file, unsigned int offset)
{
    std::uint8_t bytes[2] = {};

    file.clear();
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(bytes), 2);

    if (!file)
    {
        throw std::runtime_error("Cannot read BMP header.");
    }

    return static_cast<unsigned short>(
        bytes[0] | (static_cast<unsigned short>(bytes[1]) << 8));
}

static unsigned int readUInt32At(std::ifstream& file, unsigned int offset)
{
    std::uint8_t bytes[4] = {};

    file.clear();
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(bytes), 4);

    if (!file)
    {
        throw std::runtime_error("Cannot read BMP header.");
    }

    return static_cast<unsigned int>(bytes[0])
        | (static_cast<unsigned int>(bytes[1]) << 8)
        | (static_cast<unsigned int>(bytes[2]) << 16)
        | (static_cast<unsigned int>(bytes[3]) << 24);
}

std::ifstream openRead(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for reading.");
    }

    return file;
}

std::ofstream openWrite(const std::string& path)
{
    std::ofstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for writing.");
    }

    return file;
}

void close(std::ifstream& file)
{
    file.close();
}

void close(std::ofstream& file)
{
    file.close();

    if (!file)
    {
        throw std::runtime_error("Error while closing output file.");
    }
}

void displayFileBytes(std::ifstream& file)
{
    std::uint8_t byte;

    file.clear();
    file.seekg(0);

    while (file.read(reinterpret_cast<char*>(&byte), 1))
    {
        std::cout << static_cast<unsigned int>(byte) << ' ';
    }

    if (!file.eof())
    {
        throw std::runtime_error("Error while reading file.");
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

    while (file.get(byte))
    {
        output.put(byte);
    }

    if (!file.eof() || !output)
    {
        throw std::runtime_error("Error while copying file.");
    }

    close(output);
    file.clear();
    file.seekg(0);
}

bool compareFiles(const std::string& path1, const std::string& path2)
{
    std::ifstream first = openRead(path1);
    std::ifstream second = openRead(path2);
    char byte1, byte2;

    while (true)
    {
        bool read1 = static_cast<bool>(first.get(byte1));
        bool read2 = static_cast<bool>(second.get(byte2));

        if (read1 != read2)
        {
            return false;
        }

        if (!read1)
        {
            return first.eof() && second.eof();
        }

        if (byte1 != byte2)
        {
            return false;
        }
    }
}

bool isBMP(std::ifstream& file)
{
    std::uint8_t bytes[2] = {};

    file.clear();
    file.seekg(0);
    file.read(reinterpret_cast<char*>(bytes), 2);

    bool valid = file.gcount() == 2 && bytes[0] == 'B' && bytes[1] == 'M';

    file.clear();
    file.seekg(0);

    return valid;
}

unsigned int getFileSize(std::ifstream& file)
{
    return readUInt32At(file, 2);
}

bool isbfReserved(std::ifstream& file)
{
    return readUInt16At(file, 6) == 0 && readUInt16At(file, 8) == 0;
}

static void checkBMPFormat(std::ifstream& file)
{
    if (readUInt32At(file, 14) != 40
        || readUInt16At(file, 26) != 1
        || readUInt16At(file, 28) != 24
        || readUInt32At(file, 30) != 0)
    {
        throw std::runtime_error("Unsupported BMP format.");
    }
}

BMPInfo getBMPInfo(std::ifstream& file)
{
    BMPInfo info;

    checkBMPFormat(file);

    info.pixelOffset = readUInt32At(file, 10);
    info.width = readUInt32At(file, 18);
    info.height = readUInt32At(file, 22);

    if (info.width == 0 || info.height == 0
        || info.width > 0x7FFFFFFF || info.height > 0x7FFFFFFF)
    {
        throw std::runtime_error("Invalid BMP dimensions.");
    }

    unsigned int rowSize = info.width * 3;
    info.stride = ((rowSize + 3) / 4) * 4;
    info.componentCount = rowSize * info.height;

    file.clear();
    file.seekg(0, std::ios::end);
    unsigned int actualSize = static_cast<unsigned int>(file.tellg());

    unsigned int end = info.pixelOffset + info.stride * info.height;

    if (end > actualSize || end > getFileSize(file))
    {
        throw std::runtime_error("BMP pixel data is incomplete.");
    }

    file.clear();
    file.seekg(0);

    return info;
}

unsigned int getPixelComponentOffset(const BMPInfo& info, unsigned int index)
{
    if (index >= info.componentCount)
    {
        throw std::runtime_error("Pixel index out of range.");
    }

    unsigned int row = index / (info.width * 3);
    unsigned int column = index % (info.width * 3);

    return info.pixelOffset + row * info.stride + column;
}

std::uint8_t readByteAt(std::ifstream& file, unsigned int offset)
{
    std::uint8_t byte = 0;

    file.clear();
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(&byte), 1);

    if (!file)
    {
        throw std::runtime_error("Cannot read byte.");
    }

    return byte;
}

void writeLSB(std::fstream& file, unsigned int offset, std::uint8_t bit)
{
    std::uint8_t byte = 0;

    file.clear();
    file.seekg(offset);
    file.read(reinterpret_cast<char*>(&byte), 1);

    if (!file)
    {
        throw std::runtime_error("Cannot read byte.");
    }

    byte = (byte & 0xFE) | (bit & 1);

    file.clear();
    file.seekp(offset);
    file.write(reinterpret_cast<char*>(&byte), 1);

    if (!file)
    {
        throw std::runtime_error("Cannot write byte.");
    }
}

std::uint8_t readLSB(std::ifstream& file, unsigned int offset)
{
    return readByteAt(file, offset) & 1;
}

void hideByte(std::fstream& file, const BMPInfo& info, unsigned int startIndex, std::uint8_t value)
{
    if (startIndex > info.componentCount || info.componentCount - startIndex < 8)
    {
        throw std::runtime_error("Not enough pixel components.");
    }

    for (int i = 0; i < 8; i++)
    {
        std::uint8_t bit = (value >> (7 - i)) & 1;
        unsigned int offset = getPixelComponentOffset(info, startIndex + i);

        writeLSB(file, offset, bit);
    }
}

std::uint8_t extractByte(std::ifstream& file, const BMPInfo& info, unsigned int startIndex)
{
    std::uint8_t value = 0;

    if (startIndex > info.componentCount || info.componentCount - startIndex < 8)
    {
        throw std::runtime_error("Not enough pixel components.");
    }

    for (int i = 0; i < 8; i++)
    {
        unsigned int offset = getPixelComponentOffset(info, startIndex + i);

        value = static_cast<std::uint8_t>((value << 1) | readLSB(file, offset));
    }

    return value;
}

std::uint8_t charToASCII(char letter)
{
    return static_cast<std::uint8_t>(letter);
}

std::vector<std::uint8_t> strToASCII(const std::string& text)
{
    std::vector<std::uint8_t> result;

    for (char letter : text)
    {
        result.push_back(charToASCII(letter));
    }

    return result;
}

void hideString(std::fstream& file, const BMPInfo& info, const std::string& message)
{
    // check 1 : can we store the lenght in the image ?
    if (info.componentCount < 16)
    {
        throw std::runtime_error("Image is too small.");
    }

    // check 2 : is the message longer than the lenght we can store ?
    if (message.size() > 65535)
    {
        throw std::runtime_error("Message is too long.");
    }

	// check 3 : can we store the message in the image ?
    if (message.size() > (info.componentCount - 16) / 8)
    {
        throw std::runtime_error("Message is too large for this image.");
    }

    std::uint16_t length = static_cast<std::uint16_t>(message.size());

    for (int i = 0; i < 16; i++)
    {
        std::uint8_t bit = (length >> (15 - i)) & 1;
        unsigned int offset = getPixelComponentOffset(info, i);

        writeLSB(file, offset, bit);
    }

    std::vector<std::uint8_t> bytes = strToASCII(message);

    for (unsigned int i = 0; i < bytes.size(); i++)
    {
        hideByte(file, info, 16 + i * 8, bytes[i]);
    }
}