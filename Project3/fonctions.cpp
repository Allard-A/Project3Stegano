#include "fonctions.h"
std::ifstream openFileRead(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier en lecture : " + path);
    }

    return file;
}

std::ofstream openFileWrite(const std::string& path)
{
    std::ofstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier en ecriture : " + path);
    }

    return file;
}

void closeFile(std::ifstream& file)
{
    file.close();
}