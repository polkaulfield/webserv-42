#ifndef UTILS_HPP
# define UTILS_HPP
# include <string>
# include <sys/stat.h>
# include <map>

bool endsWith(const std::string& fullString,
              const std::string& ending);
bool isDir(std::string path);
std::string intToString(int n);
std::string searchAndReplace(std::string str, std::string searchWord, std::string replaceWord);
void    printDict(std::map<std::string, std::string> dict);
#endif
