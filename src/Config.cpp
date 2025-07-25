#include "../include/Config.hpp"
#include <iostream>
#include <sstream>

//  CONSTRUCTOR & DESTRUCTOR //
Config::Config(void) {
  _server_name = "localhost";
  _port = 8080;
  _host = "localhost";
  _root = ".";
  _index = "index.html";
  _error_page = "404.html";
  _client_max_body_size = 1000000;
  _cgi = false;
  _error_parser = false;
  _error_parser = 0;
  _double_port = -1;
}

Config::Config(const Config &src) {
  *this = src;
}

Config &Config::operator=(const Config &src) {
  if (this != &src) {
    _server_name = src._server_name;
    _port = src._port;
    _host = src._host;
    _root = src._root;
    _index = src._index;
    _error_page = src._error_page;
    _client_max_body_size = src._client_max_body_size;
    _cgi = src._cgi;
    _cgi_path = src._cgi_path;
    _cgi_ext = src._cgi_ext;
    _locationList = src._locationList;
    _double_port = src._double_port;
    _error_parser = src._error_parser;
  }
  return *this;
}

Config::~Config(void) {
  ;
}

//  GETTERS  //
std::string Config::getServerName(void) const { return _server_name; }

int Config::getPort(void) const { return _port; }

std::string Config::getHost(void) const { return _host; }

std::string Config::getRoot(void) const { return _root; }

std::string Config::getIndex(void) const { return _index; }

std::string Config::getErrorPage(void) const { return _error_page; }

int Config::getClientMaxBodySize(void) const { return _client_max_body_size; }

std::list<Location> &Config::getLocationList(void) { return _locationList; }

bool Config::getIsCgi(void) const { return _cgi; }

std::string Config::getCgiExt(void) const { return _cgi_ext; }

int Config::getErrorsParser(void) { return _error_parser; }

int Config::getDoublePort(void) { return _double_port; }

//  SETTERS  //
void Config::setServerName(std::string server_name) {
  _server_name = server_name;
}

// istringstream converts the std::string to numbers
void Config::setPort(std::string port) {
  bool flag = false;
  if (port.find("-") != (size_t)-1 ||
      (port.length() != 4 && port.length() != 9))
    _double_port = -2;
  if (port.find(" ") == port.find_last_of(" ") &&
      port.find(" ") != port.length() - 1) {
    std::istringstream value(port);
    value >> _port;
    if (port.length() <= 4)
      return;
    port = port.substr(port.find(" ") + 1);
    flag = true;
  }
  if (port.find(" ") == (size_t)-1 ||
      (port.find(" ") != (size_t)-1 && port.find(" ") == (size_t)-1)) {
    std::istringstream value(port);
    if (flag)
      value >> _double_port;
    else
      value >> _port;
  } else
    _error_parser += 1;
  _error_parser += checkChars(port, ".,/\\");
}

void Config::setPort(int port) { _port = port; }

void Config::setHost(std::string host) {
  _host = host;
  _error_parser += checkChars(host, " ,/\\");
}

void Config::setRoot(std::string root) {
  _root = root;
  _error_parser += checkChars(root, " \\,'");
}

void Config::setIndex(std::string index) {
  _index = index;
  _error_parser += checkChars(index, " \\,'");
}

void Config::setErrorPage(std::string error_page) { _error_page = error_page; }

// istringstream converts the std::string to numbers
void Config::setClientMaxBodySize(std::string client_max_body_size) {
  _error_parser += checkDigits(client_max_body_size);
  std::istringstream(client_max_body_size) >> _client_max_body_size;
}

void Config::setCgiPath(std::string cgi_path) {
  _cgi = true;
  _cgi_path = cgi_path;
}

void Config::setCgiExt(std::string cgi_ext) {
  _cgi = true;
  _cgi_ext = cgi_ext;
}

//  METHODS  //
// this function take the string from config and search the begind of value till
// ';'
std::string Config::_takeParams(std::string option, int *error) {
  int start = option.find(" ");
  int end = option.find(";");
  if (end == -1 || start == -1) {
    std::cerr << GREEN << "Error: found at \"" << option << "\"" << RESET
              << std::endl;
    *error = 1;
    return "Error";
  }
  return option.substr(start + 1, end - start - 1);
}

int Config::searchConfig(std::string option) {
  int error = 0;
  if (option.compare(0, 12, "server_name ") == 0)
    setServerName(_takeParams(option, &error));
  else if (option.compare(0, 7, "listen ") == 0)
    setPort(_takeParams(option, &error));
  else if (option.compare(0, 5, "host ") == 0)
    setHost(_takeParams(option, &error));
  else if (option.compare(0, 5, "root ") == 0)
    setRoot(_takeParams(option, &error));
  else if (option.compare(0, 6, "index ") == 0)
    setIndex(_takeParams(option, &error));
  else if (option.compare(0, 11, "error_page ") == 0)
    setErrorPage(_takeParams(option, &error));
  else if (option.compare(0, 21, "client_max_body_size ") == 0)
    setClientMaxBodySize(_takeParams(option, &error));
  else if (option.compare(0, 9, "cgi_path ") == 0)
    setCgiPath(_takeParams(option, &error));
  else if (option.compare(0, 8, "cgi_ext ") == 0)
    setCgiExt(_takeParams(option, &error));
  else if (option.compare(0, 9, "location ") == 0) {
    if (!_locationList.empty())
      _error_parser +=
          _locationList.back().getErrorsParser(); // merge the errors from last
                                                  // location with config
    _locationList.push_back(Location(option));
  } else {
    std::cerr << GREEN << "Error not found: " << option << RESET << std::endl;
    return 1;
  }
  if (error)
    return 1;
  return 0;
}

void Config::printConfig(void) const { // debug fuction
  std::cout << "server name: " << _server_name << std::endl;
  std::cout << "port listen: " << _port << std::endl;
  std::cout << "host: " << _host << std::endl;
  std::cout << "root: " << _root << std::endl;
  std::cout << "index: " << _index << std::endl;
  std::cout << "error page: " << _error_page << std::endl;
  std::cout << "client max body size: " << _client_max_body_size << std::endl;
  if (_cgi) {
    std::cout << "CGI (true)" << std::endl;
    std::cout << "cgi path:" << _cgi_path << std::endl;
    std::cout << "cgi ext:" << _cgi_ext << std::endl;
  }
  std::cout << std::endl;
  if (_locationList.empty()) {
    std::cerr << GREEN << "Not location found" << RESET << std::endl;
    return;
  }
  for (std::list<Location>::const_iterator iter = _locationList.begin();
       iter != _locationList.end(); ++iter) {
    std::cout << "Location:" << iter->getDirectory() << std::endl;
    if (iter->getGet())
      std::cout << "GET (true)" << std::endl;
    if (iter->getPost())
      std::cout << "POST (true)" << std::endl;
    if (iter->getDelete())
      std::cout << "DELETE (true)" << std::endl;
    if (!iter->getRedirect().empty())
      std::cout << "redirect: " << iter->getRedirect() << std::endl;
    if (iter->getAutoindex())
      std::cout << "Autoindex: true" << std::endl;
    if (iter->getDirectoryListing())
      std::cout << "Directory Listing: true" << std::endl;
    if (iter->getIsUpload())
      std::cout << "Upload Dir: " << iter->getUploadDir() << std::endl;
  }
}
//  CHECKERS  //
int Config::_checkRoot(void) {
  if (!access(_root.data(), F_OK))
    return 0;
  std::cerr << GREEN << "\tError in root: " << _root << " is not accesible"
            << RESET << std::endl;
  return 1;
}

int Config::_checkPort(void) {
  if ((_port > 0 && _port < 10000) && _double_port != -2)
    return 0;
  std::cerr << GREEN << "\tError in port value: " << _port << " is not valid"
            << RESET << std::endl;
  return 1;
}

int Config::_checkIndex(void) {
  std::string index_root(_root + "/" + _index);
  if (!access(index_root.data(), F_OK))
    return 0;
  std::cerr << GREEN << "\tError in index: " << _index << " is not accesible"
            << RESET << std::endl;
  return 1;
}

int Config::_checkClientMaxBodySize(void) {
  // if (_client_max_body_size > 0 && _client_max_body_size <
  // (size_t)18446744073709551615) 	return 0; std::cout << GREEN << "\tError
  // in cliente max body size: " << _client_max_body_size << " is not valid" <<
  // RESET << std::endl; return 1;
  return 0; // need check this function
}

int Config::_checkCgiPath(void) {
  if (!_cgi)
    return 0;
  if (_cgi_path.empty()) {
    std::cerr << GREEN << "\tError: Not found Cgi Path" << RESET << std::endl;
    return 1;
  }
  std::string cgi_root(_root + _cgi_path + _cgi_ext);
  if (!access(cgi_root.data(), F_OK))
    return 0;
  std::cerr << GREEN << "\tError in Cgi Path: " << _cgi_path + _cgi_ext
            << " is not accesible" << RESET << std::endl;
  return 1;
}

int Config::_checkCgiExt(void) {
  if (!_cgi)
    return 0;
  if (_cgi_ext.empty()) {
    std::cerr << GREEN << "\tError: Not found Cgi Ext" << RESET << std::endl;
    return 1;
  }
  if (_cgi_ext[0] == '.')
    return 0;
  std::cerr << GREEN << "\tError in Cgi Ext: " << _cgi_ext << " is wrong"
            << RESET << std::endl;
  return 1;
}

int Config::_checkErrorPage(void) {
  if (!access(std::string(_root + _error_page).data(), F_OK))
    return 0;
  std::cerr << GREEN << "\tError in error_page: " << _error_page
            << " is not accesible" << RESET << std::endl;
  return 1;
}

// roots is a tmp string for take the _root string and append the directory of
// current location
//  if not found location create "/" default with get
int Config::checkLocations(void) {
  int errors = 0;
  if (_locationList.empty()) {
    _locationList.push_back(Location());
    return errors;
  }
  for (std::list<Location>::iterator iter = _locationList.begin();
       iter != _locationList.end(); ++iter) {
    if (iter->getDirectory() == "/redirect") {
      errors += iter->checkRedirectLocation(_root);
      continue;
    }
    if (!iter->getDirectory().empty())
      errors += iter->checkDirectory(_root);
    if (iter->getIsUpload())
      errors += iter->checkUploadDir(_root);
    if (iter->getPost())
      errors += iter->checkPost();
    if (!iter->getRedirect().empty()) {
      std::cerr << GREEN << "\tError in Location: Can't has return" << RESET
                << std::endl;
      errors += 1;
    }
    errors += iter->checkAllowMethods();
  }
  return errors;
}

int Config::checkConfig(void) {
  int error = 0;
  if (_error_parser)
    error++;
  error += _checkRoot();
  error += _checkPort();
  error += _checkIndex();
  error += _checkClientMaxBodySize();
  error += _checkCgiPath();
  error += _checkCgiExt();
  error += _checkErrorPage();
  error += checkLocations();
  return error;
}

bool Config::isPathAutoIndex(const std::string &queryPath) const {
  std::string tmp;
  for (std::list<Location>::const_iterator iter = _locationList.begin();
       iter != _locationList.end(); iter++) {
    if (iter->getAutoindex()) {
      if (queryPath.find(iter->getDirectory()) == 0)
        return true;
    }
  }

  return false;
}

const std::string
Config::getRedirectFromPath(const std::string &queryPath) const {
  std::string redirUrl = "";
  for (std::list<Location>::const_iterator iter = _locationList.begin();
       iter != _locationList.end(); iter++) {
    redirUrl = iter->getRedirect();

    if (iter->getDirectory() == queryPath && redirUrl != "") {
      return redirUrl;
    }
  }
  return "";
}

//  LINKED LIST LOCATIONS  //

const Location *Config::searchLocation(std::string option) const {
  for (std::list<Location>::const_iterator iter = _locationList.begin();
       iter != _locationList.end(); iter++) {
    if (iter->getDirectory() == option || iter->getDirectory() + "/" == option)
      return &*iter;
  }
  return NULL;
}
