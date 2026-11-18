#include "fonctions.h"

// WIP :
// - Add a --help option to display usage information.
// - Add a command to encode a message into a BMP file.
// - Add a command to decode a message from a BMP file.
// - Add a command to compare two BMP files to see if they are identical. (check if any has the secret message)
// - Add a command to clone a BMP file.
// - Add a command to handle the Xor cypher for the hidden message. (both encode and decode)


// Commands should look like this : project3.exe <command> <args>
int main(int argc, char** argv)
{
	// Checks if a command is specified
    if (argc < 2)
    {
        std::cout << "No command specified." << std::endl;
        return 1;
    }
}