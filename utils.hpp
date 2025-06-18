#ifndef UTILS_HPP
# define UTILS_HPP
# include <sstream>
# include <sys/stat.h>

bool endsWith(const std::string& fullString,
              const std::string& ending);
bool isDir(std::string path);
#endif
