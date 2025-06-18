#include "utils.hpp"
#include <vector>
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

bool hasExtension(std::string file, std::vector<std::string> exts)
{
    for(std::string ext : exts)
    {
        if (endsWith(file, ext))
            return true;
    }
    return false;
}
