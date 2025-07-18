#ifndef UTILS_HPP
# define UTILS_HPP
#include <list>
# include <string>
# include <sys/stat.h>
# include <map>
# include <vector>

bool endsWith(const std::string& fullString,
			  const std::string& ending);
bool isDir(std::string path);
bool isFile(std::string path);
std::string intToString(int n);
std::string searchAndReplace(std::string str, std::string searchWord, std::string replaceWord);
void    printDict(std::map<std::string, std::string> dict);
std::list<std::string> listDirs(const std::string& dir);
std::list<std::string> listFiles(const std::string &dir);
bool    startsWith(std::string str, std::string prefix);
void	freeArray(char** array);
std::string extractDirectory(const std::string& filePath);
std::string extractFilename(const std::string& fileName);
char**	vectorToArray(std::vector<std::string> envVars);
std::string getExtension(std::string htmlPath);

std::string	determineInterpreter(std::string htmlPath);

#endif
