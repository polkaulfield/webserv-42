#include "../include/utils.hpp"
#include <dirent.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

bool isDir(const std::string &path) {
  struct stat sb;
  return (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool isFile(const std::string &path) {
  struct stat sb;
  return (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode));
}

std::list<std::string> listDirs(const std::string &dir) {
  std::list<std::string> dirList;
  DIR *dirp = opendir(dir.c_str());
  if (!dirp)
    return dirList;

  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
    std::string name(dp->d_name);
    if (name == "." || name == "..")
      continue;

    std::string fullPath = dir + "/" + name;
    if (isDir(fullPath))
      dirList.push_back(name); // or push_back(fullPath) if full path is desired
  }

  closedir(dirp);
  return dirList;
}

std::list<std::string> listFiles(const std::string &dir) {
  std::list<std::string> fileList;
  DIR *dirp = opendir(dir.c_str());
  if (!dirp)
    return fileList;

  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
    std::string name(dp->d_name);
    if (name == "." || name == "..")
      continue;

    std::string fullPath = dir + "/" + name;
    if (isFile(fullPath))
      fileList.push_back(name); // or fullPath
  }

  closedir(dirp);
  return fileList;
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

std::string searchAndReplace(std::string const &str,
                             std::string const &searchWord,
                             std::string const &replaceWord) {
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

bool startsWith(std::string const &str, std::string const &prefix) {
  int last = str.find_last_of("/");
  if (prefix.length() == 1 && prefix == "/" && last == 0) {
    return true;
  }
  if (str.compare(0, prefix.length(), prefix) == 0) {
    return true;
  }
  return false;
}

void freeArray(char **array) {
  if (!array)
    return;

  for (int i = 0; array[i]; i++)
    delete[] array[i];
  delete[] array;
}

std::string extractDirectory(const std::string &filePath) {
  size_t lastSlash = filePath.find_last_of('/');
  if (lastSlash != std::string::npos) {
    return filePath.substr(0, lastSlash);
  }
  return ".";
}

std::string extractFilename(const std::string &filePath) {
  size_t lastSlash = filePath.find_last_of('/');
  if (lastSlash != std::string::npos) {
    return filePath.substr(lastSlash + 1);
  }
  return filePath;
}

bool isIpAddress(const std::string ipAddr) {
  std::stringstream ss(ipAddr);
  std::string chunk;
  std::list<std::string> chunks;
  int n;
  while (getline(ss, chunk, '.'))
    chunks.push_back(chunk);
  if (chunks.size() != 4)
    return false;
  for (std::list<std::string>::iterator it = chunks.begin(); it != chunks.end();
       ++it) {
    std::istringstream(it->data()) >> n;
    if (n < 0 || n > 255)
      return false;
  }
  return true;
}
