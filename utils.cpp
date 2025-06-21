#include "utils.hpp"
#include <sstream>
#include <string>
#include <sys/stat.h>

bool isDir(std::string path)
{
    struct stat sb;

    if (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IFDIR)
        return true;
    return false;
}

bool endsWith(const std::string& fullString, const std::string& ending)
{
    // Check if the ending string is longer than the full
    // string
    if (ending.size() > fullString.size())
        return false;

    // Compare the ending of the full string with the target
    // ending
    return fullString.compare(fullString.size()
                                  - ending.size(),
                              ending.size(), ending)
           == 0;
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
