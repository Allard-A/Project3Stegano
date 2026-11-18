#include "fonctions.h"


int main(int argc, char** argv)
{
    std::cout << "Nombre d'arguments : " << argc << std::endl;

    for (int i = 0; i < argc; i++)
    {
        std::cout << "Argument " << i << " : " << argv[i] << std::endl;
    }
    return 0;
}