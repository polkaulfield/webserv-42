#ifndef UTILS_HPP
# define UTILS_HPP
# include <sstream>
# include <vector>
# include <sys/stat.h>

bool endsWith(const std::string& fullString,
              const std::string& ending);
bool isDir(std::string path);
std::string intToString(int n);
#endif
