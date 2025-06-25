#include "utils.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    std::string toreplace = "hey this is an apple and a banana and an apple wow apple";
    std::string search = "apple";
    std::string replace = "banana";

    std::cout << "Original: " << toreplace << std::endl << "Replaced: " << searchAndReplace(toreplace, search, replace) << std::endl;
    return 0;
}
