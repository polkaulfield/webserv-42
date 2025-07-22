#ifndef UTILS_HPP
# define UTILS_HPP
#include <list>
# include <string>
# include <sys/stat.h>
# include <map>
# include <vector>
# include "config.hpp"

bool endsWith(const std::string& fullString,
			  const std::string& ending);
bool isDir(std::string const& path);
bool isFile(std::string const& path);
std::string intToString(int n);
std::string searchAndReplace(std::string const& str, std::string const& searchWord, std::string const& replaceWord);
void	printDict(std::map<std::string, std::string> dict);
std::list<std::string> listDirs(const std::string& dir);
std::list<std::string> listFiles(const std::string &dir);
bool	startsWith(std::string const& str, std::string const& prefix);
void	freeArray(char** array);
std::string extractDirectory(const std::string& filePath);
std::string extractFilename(const std::string& fileName);
char**	vectorToArray(std::vector<std::string> envVars);
std::string getExtension(std::string const& htmlPath);
std::string	determineInterpreter(std::string const& htmlPath);
bool	isMethodAllowed(std::string const& method, std::string const& path, Config config);
bool isIpAddress(const std::string ipAddr);
#endif
