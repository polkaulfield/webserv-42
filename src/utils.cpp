#include "../include/utils.hpp"
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <map>
#include <iostream>

bool isDir(std::string path)
{
    struct stat sb;

    if (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IFDIR)
        return true;
    return false;
}

bool endsWith(const std::string& str, const std::string& end)
{
    if (end.size() > str.size())
        return false;
    return str.compare(str.size() - end.size(), end.size(), end) == 0;
}

std::string intToString(int n){
    std::ostringstream ostream;

    ostream << n;
    return ostream.str();
}

std::string searchAndReplace(std::string str, std::string searchWord, std::string replaceWord)
{
    size_t pos1 = 0;
    size_t pos2 = str.find(searchWord);
    std::string newStr = "";
    while (pos2 != std::string::npos)
    {
        newStr += str.substr(pos1, pos2 - pos1);
        newStr += replaceWord;
        pos1 = pos2 + searchWord.length();
        pos2 = str.find(searchWord, pos1);
    }
    newStr += str.substr(pos1);
    return newStr;
}

void    printDict(std::map<std::string, std::string> dict)
{
    std::map<std::string, std::string>::iterator it;

    for (it = dict.begin(); it != dict.end(); it++)
    {
        std::cout << it->first    // string (key)
              << ':'
              << it->second   // string's value
              << std::endl;
    }
}
