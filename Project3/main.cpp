#include "fonctions.h"

// WIP :
// - Add a --help option to display usage information.
// - Add a command to encode a message into a BMP file.
// - Add a command to decode a message from a BMP file.
// - Add a command to compare two BMP files to see if they are identical. (check if any has the secret message)
// - Add a command to clone a BMP file.
// - Add a command to handle the Xor cypher for the hidden message. (both encode and decode)

int main(int argc, char** argv)
{
    std::cout << "Nombre d'arguments : " << argc << std::endl;

    for (int i = 0; i < argc; i++)
    {
        std::cout << "Argument " << i << " : " << argv[i] << std::endl;
    }
    return 0;
}