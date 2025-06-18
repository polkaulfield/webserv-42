#ifndef UTILS_HPP
# define UTILS_HPP
# include <sstream>
# include <vector>
# include <sys/stat.h>

bool endsWith(const std::string& fullString,
              const std::string& ending);
bool isDir(std::string path);
bool hasExtension(std::string file, std::vector<std::string> exts);
#endif
