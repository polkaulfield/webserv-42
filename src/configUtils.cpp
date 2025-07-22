
#include "../include/Config.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

void exitConfig(std::list<Config> &configList, std::ifstream *configFd,
                std::string error) {
  if (!error.empty())
    std::cout << GREEN << error << RESET << std::endl;
  if (configFd)
    (*configFd).close();
  configList.clear();
  std::exit(1);
}

// this function check all the variable, in config list and checks if have duped
// ports
int checkConfigList(std::list<Config> &configList) {
  int error = 0;
  int count = 0;

  for (std::list<Config>::iterator iter = configList.begin();
       iter != configList.end(); ++iter) {
    std::cerr << GREEN << "Checking Config: " << ++count << RESET << std::endl;
    std::cerr << GREEN << "\tErrors Found while parsing: ";
    if (iter->getErrorsParser())
      std::cerr << iter->getErrorsParser() << RESET << std::endl;
    else
      std::cerr << "OK" << RESET << std::endl;
    error += iter->checkConfig();
    for (std::list<Config>::iterator subiter = iter;
         subiter != configList.end(); ++subiter) {
      if (iter == subiter) {
        if (++subiter == configList.end())
          break;
      }
      if (/*iter->getHost() == subiter->getHost() || */ iter->getPort() ==
          subiter->getPort()) {
        std::cout
            << GREEN
            << "\tError: This Config has the same Host/Port than other server"
            << RESET << std::endl;
        error++;
        break;
      }
    }
  }
  int dp = configList.front().getDoublePort();
  if (dp != -1) {
    configList.push_back(Config(configList.front()));
    configList.back().setPort(dp);
  }
  if (error > 0)
    std::cout << GREEN << "Found " << error
              << " errors in the configuration file" << RESET << std::endl;
  else
    std::cout << GREEN << "OK!" << RESET << std::endl;
  return error;
}

bool checkDigits(std::string src) {
  if (src.empty())
    return false;
  for (size_t i = 0; i < src.length(); i++) {
    if (!isdigit(src[i]) && src[i] != '+' && src[i] != '-')
      return true;
  }
  return false;
}

bool checkChars(std::string src, std::string not_valid_chars) {
  int pos = 0;
  if (src.empty())
    return false;
  for (size_t i = 0; i < not_valid_chars.length(); i++) {
    pos = src.find(not_valid_chars[i]);
    if (pos != -1)
      return true;
  }
  return false;
}

std::list<Config> takeConfig(const char *configFile) {
  std::ifstream configFd;
  std::string line;
  std::string tmp;
  int brackets = 0;
  std::list<Config> configList;

  configFd.open(configFile, std::ifstream::in);
  if (!configFd.is_open())
    exitConfig(configList, NULL, "Error: Can't open config file");
  while (std::getline(configFd, line)) {
    tmp.clear();
    for (size_t i = 0; i < line.length();
         i++) { // this for remove tabs and space and put inside of tmp from
                // config file
      if ((tmp.empty()) && (line[i] == ' ' || line[i] == '\t'))
        continue;
      if (line[i] == '#')
        break;
      tmp.push_back(line[i]);
    }
    if (tmp.empty())
      continue;
    else if (tmp == "server" &&
             tmp.length() == 6) // append a new Config in the configList
      configList.push_back(Config());
    else if (configList.empty()) // if not server found
      exitConfig(configList, &configFd, "Error: not found server {}");
    else if (brackets == 2 &&
             configList.back()
                 .getLocationList()
                 .empty()) // if found a new identation of brackets and not
                           // found a Location{}
      exitConfig(configList, &configFd,
                 "Error: brackets found without a location");
    else if ((tmp[0] == '{' || tmp[0] == '}') && tmp.length() == 1)
      (tmp == "{") ? brackets++ : brackets--; // handle brackets indentation
    else {
      if (brackets == 0 || brackets >= 3) // wrong indentation lvl
        exitConfig(configList, &configFd, "Error: brackets");
      else if (brackets == 1 &&
               configList.back().searchConfig(tmp)) // indentation lvl 1
        exitConfig(configList, &configFd, "");
      else if (brackets == 2 &&
               configList.back().getLocationList().back().searchLocationConfig(
                   tmp)) // indentation lvl 2
        exitConfig(configList, &configFd, "");
    }
  }
  if (configList.empty())
    exitConfig(configList, &configFd, "Error: File is empty");
  // if (checkConfigList(configList))
  //;//exitConfig(configList, configFd, "");
  checkConfigList(configList);
  configFd.close();
  // configList.front().printConfig();
  return configList;
}
