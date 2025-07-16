#include "../include/directory.hpp"
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

  if (access(path.c_str(), R_OK) != 0) {
    std::cerr << "Directory template not found!" << std::endl;
    return;
  }
  std::string line;
  std::ifstream templateFile("./templates/directory.html");
  std::getline(templateFile, line);
  std::cout << "first line: " << line << std::endl;
  if (line != "<!-- HEAD") {
    std::cerr << "Head not found";
    return;
  }
  while (std::getline(templateFile, line) && line != "END HEAD-->") {
    _data += line + "\n";
  }
  if (std::getline(templateFile, line) && line != "<!-- START BODY TOP") {
    std::cerr << "Start body not found: " << line << std::endl;
    return;
  }
  while (std::getline(templateFile, line) && line != "END BODY TOP -->") {
    _data += line + "\n";
  }
  std::list<std::string> dirList = listDirs(path);
  std::list<std::string> fileList = listFiles(path);
  for (std::list<std::string>::iterator iter = dirList.begin();
       iter != dirList.end(); ++iter) {
    _data += "<li><a href=\"" + (const std::string)iter->data() + "/\">" +
             iter->data() + "/</a></li>\n";
  }
  for (std::list<std::string>::iterator iter = fileList.begin();
       iter != fileList.end(); ++iter) {
    _data += "<li><a href=\"" + (const std::string)iter->data() + "\">" +
             iter->data() + "/</a></li>\n";
  }
  if (std::getline(templateFile, line) && line != "<!-- START BODY BOTTOM") {
    std::cerr << "Head not found";
    return;
  }
  while (std::getline(templateFile, line) && line != "END BODY BOTTOM -->") {
    _data += line + "\n";
  }
}

std::string &Directory::getHtml() { return _data; };
