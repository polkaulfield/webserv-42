#include "../include/utils.hpp"
#include <dirent.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

bool isDir(std::string path) {
  struct stat sb;

  if (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IFDIR)
    return true;
  return false;
}

bool endsWith(const std::string &str, const std::string &end) {
  if (end.size() > str.size())
    return false;
  return str.compare(str.size() - end.size(), end.size(), end) == 0;
}

std::string intToString(int n) {
  std::ostringstream ostream;

  ostream << n;
  return ostream.str();
}

std::string searchAndReplace(std::string str, std::string searchWord,
                             std::string replaceWord) {
  size_t pos1 = 0;
  size_t pos2 = str.find(searchWord);
  std::string newStr = "";
  while (pos2 != std::string::npos) {
    newStr += str.substr(pos1, pos2 - pos1);
    newStr += replaceWord;
    pos1 = pos2 + searchWord.length();
    pos2 = str.find(searchWord, pos1);
  }
  newStr += str.substr(pos1);
  return newStr;
}

void printDict(std::map<std::string, std::string> dict) {
  std::map<std::string, std::string>::iterator it;

  for (it = dict.begin(); it != dict.end(); it++) {
    std::cout << it->first         // string (key)
              << ':' << it->second // string's value
              << std::endl;
  }
}

bool startsWith(std::string str, std::string prefix) {
  int last = str.find_last_of("/");
  if (prefix.length() == 1 && prefix == "/" && last == 0) {
    return true;
  }
  if (str.compare(0, prefix.length(), prefix) == 0) {
    return true;
  }
  return false;
}

std::list<std::string> listDirs(const std::string &dir) {
  DIR *dirp = opendir(dir.c_str());
  std::list<std::string> dirList;
  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
      if (isDir(dp->d_name))
        dirList.push_back(dp->d_name);
  }
  closedir(dirp);
  return dirList;
}

std::list<std::string> listFiles(const std::string &dir) {
  DIR *dirp = opendir(dir.c_str());
  std::list<std::string> dirList;
  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
      if (!isDir(dp->d_name))
        dirList.push_back(dp->d_name);
  }
  closedir(dirp);
  return dirList;
}

void	freeArray(char** array) {
	if (!array)
		return;

	for (int i = 0; array[i]; i++)
		delete[] array[i];
	delete[] array;
}
