#include "../include/config.hpp"
#include <sstream>
#include <iostream>

//  CONSTRUCTOR & DESTRUCTOR //
Config::Config(void) {
	//std::cout << "Contructor Config" << std::endl;
	_port = 8080;
	_host = "localhost";
	_root = "/tmp/www";
	_index = "index.html";
	_error_page = "404.html";
	_client_max_body_size = 1000000;
	_cgi = false;
	//
	_firstLocation = NULL;
	_iterLocation = NULL;
	_lastLocation = NULL;
}

Config::~Config(void) {
	location_t	*iter = _firstLocation;
	location_t	*tmp;
	while (iter) {
		tmp = iter;
		iter = iter->next;
		delete tmp;
	}
	//std::cout << "Destroying Config" << std::endl;
}

//  GETTERS  //
std::string Config::getServerName(void) {return _server_name;}
int			Config::getPort(void) {return _port;}
std::string Config::getHost(void) {return _host;}
std::string Config::getRoot(void) {return _root;};
std::string Config::getIndex(void) {return _index;}
std::string Config::getErrorPage(void) {return _error_page;}
int			Config::getClientMaxBodySize(void) {return _client_max_body_size;}
location_t	*Config::getFirstLocation(void) {return _firstLocation;}
location_t	*Config::getIterLocation(void) {return _iterLocation;}
location_t	*Config::getLastLocation(void) {return _lastLocation;}

//  SETTERS  //
void	Config::setServerName(std::string server_name) {_server_name = server_name;}

void	Config::setPort(std::string port) {
	std::istringstream value(port);
	value >> _port;
}

void	Config::setHost(std::string host) {_host = host;}
void	Config::setRoot(std::string root) {_root = root;}
void	Config::setIndex(std::string index) {_index = index;}
void	Config::setErrorPage(std::string error_page) {_error_page = error_page;}

void	Config::setClientMaxBodySize(std::string client_max_body_size) {
	std::istringstream value(client_max_body_size);
	value >> _client_max_body_size;
}

void	Config::setCgiPath(std::string cgi_path) {_cgi = true; _cgi_path = cgi_path;}
void	Config::setCgiExt(std::string cgi_ext) {_cgi = true; _cgi_ext = cgi_ext;}
void	Config::setLocation(location_t *location) {_iterLocation = location;}

//  METHODS  //
std::string	Config::_takeParams(std::string option, int *error) {
	int start = option.find(" ");
	int end = option.find(";");
	if (end == -1 || start == -1) {
		std::cout << GREEN << "Error: found at \"" << option << "\"" << RESET << std::endl;
		*error = 1;
		return "Error";
	}
	return option.substr(start + 1, end - start - 1);
}

int	Config::searchConfig(std::string option) {
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
	else if (option.compare(0, 9, "location ") == 0)
		addLocation(option);
	else {
		std::cout << GREEN << "Error not found: " << option << RESET << std::endl;
		return 1;
	}
	if (error)
		return 1;
	return 0;
}

void	Config::printConfig(void) {
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
	location_t *iter = _firstLocation;
	if (!iter) {
		std::cout << GREEN << "Not location found" << RESET << std::endl;
		return ;
	}
	while (iter) {
		std::cout << "Location:" << iter->location.getDirectory() << std::endl;
		if (iter->location.getGet())
			std::cout << "GET (true)" << std::endl;
		if (iter->location.getPost())
			std::cout << "POST (true)" << std::endl;
		if (iter->location.getDelete())
			std::cout << "DELETE (true)" << std::endl;
		if (!iter->location.getRedirect().empty()) {
			std::cout << "redirect: " << iter->location.getRedirect() << std::endl;
		}
		std::cout << std::endl;
		iter = iter->next;
	}
}
//  CHECKERS  //
int	Config::checkRoot(void) {
	if (!access(_root.data(), F_OK))
		return 0;
	std::cout << GREEN << "\tError in root: " << _root << " is not accesible"<< RESET << std::endl;
	return 1;
}

int	Config::checkPort(void) {
	if (_port > 0 && _port < 10000)
		return 0;
	std::cout << GREEN << "\tError in port value: " << _port << " is not valid" << RESET << std::endl;
	return 1;
}

int Config::checkIndex(void) {
	std::string index_root(_root + _index);
	if (!access(index_root.data(), F_OK))
		return 0;
	std::cout << GREEN << "\tError in index: " << _index << " is not accesible" << RESET << std::endl;
	return 1;
}

int	Config::checkClientMaxBodySize(void) {
	if (_client_max_body_size > 0 && _client_max_body_size < 1000000)
		return 0;
	std::cout << GREEN << "\tError in cliente max body size: " << _client_max_body_size << " is not valid" << RESET << std::endl;
	return 1;
}

int Config::checkCgiPath(void) {
	if (!_cgi)
		return 0;
	if (_cgi_path.empty()) {
		std::cout << GREEN << "\tError: Not found Cgi Path" << RESET << std::endl;
		return 1;
	}
	std::string cgi_root(_root + _cgi_path);
	if (!access(cgi_root.data(), F_OK))
		return 0;
	std::cout << GREEN << "\tError in Cgi Path: " << _cgi_path << " is not accesible" << RESET << std::endl;
	return 1;
}

int	Config::checkCgiExt(void) {
	if (!_cgi)
		return 0;
	if (_cgi_ext.empty()) {
		std::cout << GREEN << "\tError: Not found Cgi Ext" << RESET << std::endl;
		return 1;
	}
	if (_cgi_ext[0] == '.')
		return 0;
	std::cout << GREEN << "\tError in Cgi Ext: " << _cgi_ext << " is wrong" << RESET << std::endl;
	return 1;
}

int Config::checkLocations(void) {
	location_t *iter = _firstLocation;
	std::string roots;
	while (iter) {
		if (iter->location.getDirectory() != "/" && iter->location.getDirectory() != "/redirect") {
			roots.append(_root + iter->location.getDirectory());
			if (access(roots.data(), F_OK)) {
				std::cout << GREEN << "\tError in Location: " << iter->location.getDirectory() << " is not accesible" << RESET << std::endl;
				return 1;
			}
			roots.clear();
		}
		iter = iter->next;
	}
	return 0;
}

int	Config::checkConfig(void) {
	int error = 0;
	error += checkRoot();
	error += checkPort();
	error += checkIndex();
	error += checkClientMaxBodySize();
	error += checkCgiPath();
	error += checkCgiExt();
	error += checkLocations();
	return error;
}


//  LINKED LIST LOCATIONS  //
location_t	*Config::addLocation(std::string _directory) {
	location_t *tmpLocation = new location_t;
	if (!_firstLocation) {
		_firstLocation = tmpLocation;
		_iterLocation = tmpLocation;
		_lastLocation = tmpLocation;
	}
	_lastLocation->next = tmpLocation;
	tmpLocation->next = NULL;
	int start = _directory.find(' ');

	tmpLocation->location = Location(_directory.substr(start + 1, _directory.length() - start));
	// set default values
	_lastLocation = tmpLocation;
	return tmpLocation;
}

Location	*Config::searchLocation(std::string option) {
	location_t	*iter = _firstLocation;
	Location	*location = NULL;
	while (iter) {
		if (iter->location.getDirectory() == option) {
			location = &iter->location;
			break ;
		}
		iter = iter->next;
	}
	return location;
}
