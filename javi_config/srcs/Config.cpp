#include "../includes/Config.hpp"

//  CONSTRUCTOR & DESTRUCTOR //
Config::Config(void) {
	//std::cout << "Contructor Config" << std::endl;
	port = 443;

	client_max_body_size = -1;

	cgi = false;

	firstLocation = NULL;
	iterLocation = NULL;
	lastLocation = NULL;
}

Config::~Config(void) {
	location_t	*iter = firstLocation;
	location_t	*tmp;
	while (iter) {
		tmp = iter;
		iter = iter->next;
		delete tmp;
	}
	//std::cout << "Destroying Config" << std::endl;
}

//  GETTERS  //
std::string Config::getServerName(void) {return server_name;}
int			Config::getPort(void) {return port;}
std::string Config::getHost(void) {return host;}
std::string Config::getRoot(void) {return root;};
std::string Config::getIndex(void) {return index;}
std::string Config::getErrorPage(void) {return error_page;}
int			Config::getClientMaxBodySize(void) {return client_max_body_size;}
location_t	*Config::getFirstLocation(void) {return firstLocation;}
location_t	*Config::getIterLocation(void) {return iterLocation;}
location_t	*Config::getLastLocation(void) {return lastLocation;}

//  SETTERS  //
void	Config::setServerName(std::string _server_name) {server_name = _server_name;}

void	Config::setPort(std::string _port) {
	std::istringstream value(_port);
	value >> port;
}

void	Config::setHost(std::string _host) {host = _host;}
void	Config::setRoot(std::string _root) {root = _root;}
void	Config::setIndex(std::string _index) {index = _index;}
void	Config::setErrorPage(std::string _error_page) {error_page = _error_page;}

void	Config::setClientMaxBodySize(std::string _client_max_body_size) {
	std::istringstream value(_client_max_body_size);
	value >> client_max_body_size;
}

void	Config::setCgiPath(std::string _cgi_path) {cgi_path = _cgi_path;}
void	Config::setCgiExt(std::string _cgi_ext) {cgi_ext = _cgi_ext;}
void	Config::setLocation(location_t *_location) {iterLocation = _location;}

//  METHODS  //
std::string	Config::takeParams(std::string option, int *error) {
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

int	Config::searchConfig(std::string option) {
	int error = 0;
	//std::cout << option << std::endl;
	if (option.compare(0, 12, "server_name ") == 0)
		setServerName(takeParams(option, &error));
	else if (option.compare(0, 7, "listen ") == 0)
		setPort(takeParams(option, &error));
	else if (option.compare(0, 5, "host ") == 0)
		setHost(takeParams(option, &error));
	else if (option.compare(0, 5, "root ") == 0)
		setRoot(takeParams(option, &error));
	else if (option.compare(0, 6, "index ") == 0)
		setIndex(takeParams(option, &error));
	else if (option.compare(0, 11, "error_page ") == 0)
		setErrorPage(takeParams(option, &error));
	else if (option.compare(0, 21, "client_max_body_size ") == 0)
		setClientMaxBodySize(takeParams(option, &error));
	else if (option.compare(0, 9, "cgi_path ") == 0)
		setCgiPath(takeParams(option, &error));
	else if (option.compare(0, 8, "cgi_ext ") == 0)
		setCgiExt(takeParams(option, &error));
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
	std::cout << "server name: " << server_name << std::endl;
	std::cout << "port listen: " << port << std::endl;
	std::cout << "host: " << host << std::endl;
	std::cout << "root: " << root << std::endl;
	std::cout << "index: " << index << std::endl;
	std::cout << "error page: " << error_page << std::endl;
	std::cout << "client max body size: " << client_max_body_size << std::endl;
	std::cout << "cgi path:" << cgi_path << std::endl;
	std::cout << "cgi ext:" << cgi_ext << std::endl;
	std::cout << std::endl;
	location_t *iter = firstLocation;
	if (!iter) {
		std::cout << GREEN << "Not location found" << RESET << std::endl;
		return ;
	}
	while (iter) {
		std::cout << "Location:" << iter->location.getDirectory() << std::endl;
		if (iter->location.getGet())
			std::cout << "GET" << std::endl;
		if (iter->location.getPost())
			std::cout << "POST" << std::endl;
		if (iter->location.getDelete())
			std::cout << "DELETE" << std::endl;
		if (!iter->location.getRedirect().empty()) {
			std::cout << "redirect: " << iter->location.getRedirect() << std::endl;
		}
		std::cout << std::endl;
		iter = iter->next;
	}
}
//  CHECKERS  //
int	Config::checkRoot(void) {
	if (!access(root.data(), F_OK))
		return 0;
	std::cout << GREEN << "Error in root: " << root << " is not accesible"<< RESET << std::endl;
	return 1;
}

int	Config::checkPort(void) {
	if (port > 0 || port < 10000)
		return 0;
	std::cout << GREEN << "Error in port value: " << port << " is not valid" << RESET << std::endl;
	return 1;
}

int Config::checkIndex(void) {
	std::string index_root(root + index);
	if (!access(index_root.data(), F_OK))
		return 0;
	std::cout << GREEN << "Error in index: " << index << " is not accesible" << RESET << std::endl;
	return 1;
}

int Config::checkCgiPath(void) {
	if (!cgi)
		return 0;
	if (cgi_path.empty()) {
		std::cout << GREEN << "Error: Not found Cgi Path" << RESET << std::endl;
		return 1;
	}
	std::string cgi_root(root + cgi_path);
	if (!access(cgi_root.data(), F_OK))
		return 0;
	std::cout << GREEN << "Error in Cgi Path: " << cgi_path << " is not accesible" << RESET << std::endl;
	return 1;
}

int	Config::checkCgiExt(void) {
	if (!cgi)
		return 0;
	if (cgi_ext.empty()) {
		std::cout << GREEN << "Error: Not found Cgi Ext" << RESET << std::endl;
		return 1;
	}
	if (cgi_ext[0] == '.')
		return 0;
	std::cout << GREEN << "Error in Cgi Ext: " << cgi_ext << " is wrong" << RESET << std::endl;
	return 1;
}

int Config::checkLocations(void) {
	location_t *iter = firstLocation;
	std::string roots;
	while (iter) {
		if (iter->location.getDirectory() != "/" && iter->location.getDirectory() != "/redirect") {
			roots.append(root + iter->location.getDirectory());
			if (access(roots.data(), F_OK)) {
				std::cout << GREEN << "Error in Location:" << iter->location.getDirectory() << " is not accesible" << RESET << std::endl;
				return 1;
			}
			roots.clear();
		}
		iter = iter->next;
	}
	return 0;
}

int Config::checkConfig(void) {
	//if (checkRoot() || checkRoot() || checkPort() || checkIndex() || checkCgiPath() || checkCgiPath() || checkCgiExt() || checkLocations())
	//	return 1;
	checkRoot();
	checkPort();
	checkIndex();
	checkCgiPath();
	checkCgiExt();
	checkLocations();
	return 0;
}


//  LINKED LIST LOCATIONS  //
location_t *Config::addLocation(std::string _directory) {
	location_t *tmpLocation = new location_t;
	if (!firstLocation) {
		firstLocation = tmpLocation;
		iterLocation = tmpLocation;
		lastLocation = tmpLocation;
	}
	lastLocation->next = tmpLocation;
	tmpLocation->next = NULL;
	int start = _directory.find(' ');

	tmpLocation->location = Location(_directory.substr(start + 1, _directory.length() - start));
	// set default values
	lastLocation = tmpLocation;
	return tmpLocation;
}

Location *Config::searchLocation(std::string option) {
	location_t	*iter = firstLocation;
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
