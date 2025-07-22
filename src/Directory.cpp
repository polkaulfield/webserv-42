#include "../include/Directory.hpp"
#include "../include/utils.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unistd.h>

Directory::Directory() {};

Directory::~Directory() {};

Directory::Directory(const std::string &path) {
  _data = "";
  std::string suffix = "/";

  if (endsWith(path, suffix))
    suffix = "";

  if (access(path.c_str(), R_OK) != 0) {
    std::cerr << "Directory template not found!" << std::endl;
    return;
  }
  std::string line;
  std::ifstream templateFile("./templates/directory.html");
  while (std::getline(templateFile, line))
    _data += line + "\n";
  std::list<std::string> dirList = listDirs(path);
  std::list<std::string> fileList = listFiles(path);
  std::string dataToReplace = "";
  for (std::list<std::string>::iterator iter = dirList.begin();
       iter != dirList.end(); ++iter) {
    dataToReplace += "<li><a href=\"" + (const std::string)iter->data() +
                     "/\">" + iter->data() + "/</a></li>\n";
  }
  for (std::list<std::string>::iterator iter = fileList.begin();
       iter != fileList.end(); ++iter) {
    dataToReplace += "<li><a href=\"" + (const std::string)iter->data() +
                     "\">" + iter->data() + "</a></li>\n";
  }
  _data = searchAndReplace(_data, "%%DIRLIST%%", dataToReplace);
}

std::string &Directory::getHtml() { return _data; };
