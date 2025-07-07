#include "../include/location.hpp"
#include "../include/config.hpp"
#include <iostream>
//  CONSTRUCTORS & DESTRUCTOR //
 Location::Location(void) {
	_GET = false;
	_POST = false;
	_DELETE = false;
	_autoindex = false;
	//std::cout << "default is created (location)" << std::endl;
}
Location::Location(std::string directory) {
	int start = directory.find(" ");
	_directory = directory.substr(start + 1);
	_GET = false;
	_POST = false;
	_DELETE = false;
	_autoindex = false;
	_directory_listing = false;
	//std::cout << _directory << ": is created" << std::endl;
}

Location &Location::operator = (const Location &src) {
	if (this != &src) {
		_directory = src._directory;
		_GET = src._GET;
		_POST = src._POST;
		_DELETE = src._DELETE;
		_redirect = src._redirect;
		_autoindex = src._autoindex;
		_directory_listing = src._directory_listing;
	}
	return *this;
}

Location::~Location(void) {
	//std::cout << _directory << ": is destroyed" << std::endl;
}

//  GETTER  //
std::string	Location::getDirectory(void) const {return _directory;}
std::string Location::getRedirect(void) const {return _redirect;}
bool	Location::getGet(void) const {return _GET;}
bool	Location::getPost(void) const {return _POST;}
bool	Location::getDelete(void) const {return _DELETE;}
bool	Location::getAutoindex(void) const {return _autoindex;}
bool	Location::getDirectoryListing(void) const {return _directory_listing;}

//  SETTERS  //
void	Location::setDirectory(std::string directory) {_directory = directory;}

void Location::setAllowMethods(std::string option) {
	int start = 14;
	int end = start + 1;
	//std::string tmp;
	if (option.length() <= 15)
		std::cout << "error in allow methods" <<std::endl;

	while(end > 0 && (size_t)end < option.length()) {
		if (option[end] == ' ' || option[end] == ';') {
			if (option.compare(start, end - start, "GET") == 0)
				_GET = true;
			else if (option.compare(start, end - start, "POST") == 0)
				_POST = true;
			else if (option.compare(start, end - start, "DELETE") == 0)
				_DELETE = true;
			else
				std::cout << GREEN << option.substr(start, end - start) << RESET << std::endl;
			start = end + 1;
		}
		else {
			;//tmp.push_back(option[end]);
		}
		end++;
	}
}

void	Location::setRedirect(std::string redirect) {_redirect = redirect;}

void	Location::setAutoindex(std::string autoindex) {
	if (autoindex == "on")
		_autoindex = true;
}

void	Location::setDirectoryListing(std::string directory_listing) {
	if (directory_listing == "on")
		_directory_listing = true;
}

//  METHODS

int Location::searchLocationConfig(std::string option) {
	int error = 0;
	if (option.compare(0, 14, "allow_methods ") == 0)
		setAllowMethods(option);
	else if (option.compare(0, 7, "return ") == 0)
		setRedirect(_takeParams(option, &error));
	else if (option.compare(0, 10, "autoindex ") == 0)
		setAutoindex(_takeParams(option, &error));
	else if (option.compare(0, 18, "directory_listing ") == 0)
		setDirectoryListing(_takeParams(option, &error));
	else {
		std::cout << GREEN << "Error not found: " << option << RESET << std::endl;
		return 1;
	}
	if (error)
		return 1;
	return 0;
}


std::string	Location::_takeParams(std::string option, int *error) {
	int start = option.find(" ");
	int end = option.find(";");
	if (end == -1 || start == -1) {
		std::cout << GREEN << "Error: found at \"" << option << "\"" << RESET << std::endl;
		*error = 1;
		return "Error";
	}
	//std::cout << start << std::endl;
	//std::cout << end << std::endl;
	return option.substr(start + 1, end - start - 1);
}

bool Location::hasMethod(std::string method)
{
    if (_GET && method == "GET")
        return true;
    if (_POST && method == "POST")
        return true;
    if (_DELETE && method == "DELETE")
        return true;
    return false;
}
