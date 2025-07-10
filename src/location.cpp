#include "../include/location.hpp"
#include "../include/config.hpp"
#include <iostream>
//  CONSTRUCTORS & DESTRUCTOR //
 Location::Location(void) {
 	_GET = false;
	_POST = false;
	_DELETE = false;
	_autoindex = false;
	_directory_listing = false;
	_error_parser = false;
	_error_parser = 0;
	//std::cout << "default is created (location)" << std::endl;
}
Location::Location(std::string directory) {
	_error_parser = 0;
	int start = directory.find(" ");
	_directory = directory.substr(start + 1);
	_error_parser += checkChars(_directory, " \\,'");
	_GET = false;
	_POST = false;
	_DELETE = false;
	_autoindex = false;
	_directory_listing = false;
	_error_parser = false;
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
		_error_parser = src._error_parser;
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

std::string Location::getUploadDir(void) const {return _uploadDir;}

int	Location::getErrorsParser(void) {return _error_parser;}

bool	Location::getIsUpload(void) const {return (!_uploadDir.empty()) ? true : false;}

//  SETTERS  //
void	Location::setDirectory(std::string directory) {_directory = directory;}

// This function search where begin the methods and search them looking for the space
void Location::setAllowMethods(std::string option) {
	int start = option.find(" ");
	int end = ++start + 1;

	_error_parser += checkChars(option, "\\,'/");
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
				_error_parser += 1;
			start = end + 1;
		}
		end++;
	}
}

void	Location::setRedirect(std::string redirect) {
	_redirect = redirect;
	_error_parser += checkChars(redirect, "\\,'");
}

void	Location::setAutoindex(std::string autoindex) {
	if (autoindex == "on")
		_autoindex = true;
	else if ("off") // value inicialized false
		;
	else
		_error_parser += 1;
}

void	Location::setDirectoryListing(std::string directory_listing) {
	if (directory_listing == "on") {
		_directory_listing = true;
		_GET = true;
	}
	else if ("off") // value inicialized false
		;
	else
		_error_parser += 1;
}

void	Location::setUploadDir(std::string uploadDir) {
	_uploadDir = uploadDir;
	_error_parser += checkChars(_uploadDir, " \\,'");
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
	else if (option.compare(0, 10, "upload_to ") == 0)
		setUploadDir(_takeParams(option, &error));
	else {
		std::cout << GREEN << "Error not found: " << option << RESET << std::endl;
		return 1;
	}
	return (error) ? 1 : 0;
}

std::string	Location::_takeParams(std::string option, int *error) {
	int start = option.find(" ");
	int end = option.find(";");
	if (end == -1 || start == -1) {
		std::cout << GREEN << "Error: found at \"" << option << "\"" << RESET << std::endl;
		*error = 1;
		return "Error";
	}
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

//  CHECKERS  //
bool	Location::checkAllowMethods(void) {
	if (!(!_GET && !_POST && !_DELETE))
		return false;
	std::cerr << GREEN << "\tError in Location " << _directory << ": Need a method" << RESET << std::endl;
	return true;
}

bool	Location::checkDirectory(std::string root) {
	std::string tmp = root + _directory;
	if (!access(tmp.data(), F_OK))
		return false;
	std::cout << GREEN << "\tError in Location: " << _directory << " is not accesible" << RESET << std::endl;
	return true;
}

bool	Location::checkUploadDir(std::string root) {
	std::string tmp = _uploadDir;
	_uploadDir = root + _uploadDir;
	if (!access(_uploadDir.data(), F_OK)) {
		//_uploadDir = tmp;
		return false;
	}
	std::cerr << GREEN << "\tError in Location: upload_to " << tmp << " is not accesible" << RESET << std::endl;
	return true;
}

bool	Location::checkRedirect(std::string root) {
	if (_redirect[0] != '/')
		return false;
	std::string tmp = _redirect;
	_redirect = root + _redirect;
	if (!access(_redirect.data(), F_OK))
		return false;
	std::cerr << GREEN << "\tError in Location: return " << tmp << " is not accesible" << RESET << std::endl;
	return true;
}
