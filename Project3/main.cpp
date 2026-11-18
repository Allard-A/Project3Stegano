#include "fonctions.h"

// WIP :
// - Add a --help option to display usage information.
// - Add a command to encode a message into a BMP file. - Done (needs testing)
// - Add a command to decode a message from a BMP file.
// - Add a command to compare two BMP files to see if they are identical. (check if any has the secret message)
// - Add a command to clone a BMP file.
// - Add a command to handle the Xor cypher (with a key obviously) for the hidden message. (both encode and decode)


// Commands should look like this : project3.exe <command> <args>
// The part that deals with commands should be moved to a separate function to make the code cleaner and easier to read.
// Also to avoid repetitive code.
// Good luck Mr Allard I cant do more than that tonight
int main(int argc, char** argv)
{
	// Checks if a command is specified
    if (argc < 2)
    {
        std::cout << "No command specified." << std::endl;
        return 1;
    }

    std::string command = argv[1];

    try
    {
        if (command == "--hide")
        {
			// Check 1 : Are there enough arguments ?
            if (argc != 4)
            {
                throw std::runtime_error("Usage: program.exe --hide \"image_path\" \"text\"");
            }

            std::ifstream input = openRead(argv[2]);

			// Check 2 : Is the file a BMP image ?
            if (!isBMP(input))
            {
                throw std::runtime_error("The file is not a BMP image.");
            }

            BMPInfo info = getBMPInfo(input);
            close(input);

            std::fstream image(argv[2], std::ios::binary | std::ios::in | std::ios::out);

            if (!image.is_open())
            {
                throw std::runtime_error("Unable to open the image.");
            }

            hideString(image, info, argv[3]);

            image.close();
        }

        // Add more commands here later
        else
        {
            throw std::runtime_error("Unknown command.");
        }
    }
    // Not sure about this part but I saw .what() in documentation so i should use it.
	// Suposevly it will return the error message of the exception.
    catch (const std::exception& error)
    {
        std::cout << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}