#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <unistd.h>
# include <list>
# include "location.hpp"

# define GREEN "\x1B[32m"
# define RESET "\x1B[0m"

class Location;
class Config;

void exitConfig(std::list<Config> &configList, std::ifstream &configFd, std::string error);
int checkConfigList(std::list<Config> &configList);
int number_configs(std::string configFile);
std::list<Config> takeConfig(char *configFile);

class Config {
private:
	std::string	_server_name;
	//std::string ip_listen;
	int			_port;
	std::string	_host;
	std::string	_root;
	std::string	_index;
	std::string	_error_page;
	int			_client_max_body_size;
	bool		_cgi;
	std::string	_cgi_path;
	std::string	_cgi_ext;
	std::list<Location*> _locationList;
//  METHODS  //
	std::string	_takeParams(std::string option, int *error);
//  CHECKERS  //
	int		_checkRoot(void);
	int		_checkPort(void);
	int		_checkIndex(void);
	int		_checkClientMaxBodySize(void);
	int		_checkCgiPath(void);
	int		_checkCgiExt(void);
public:
//  CONSTRUCTOR  //
	Config(void);
	~Config(void);
//  GETTERS  //
	std::string getServerName(void) const;
	int			getPort(void) const;
	std::string getHost(void) const;
	std::string getRoot(void) const;
	std::string getIndex(void);
	std::string getErrorPage(void);
	int			getClientMaxBodySize(void);
	std::list<Location*>	getLocationList(void);
	//  SETTERS  //
	void	setServerName(std::string _server_Name);
	void	setPort(std::string port);
	void	setHost(std::string host);
	void	setRoot(std::string root);
	void	setIndex(std::string index);
	void	setErrorPage(std::string error_page);
	void	setClientMaxBodySize(std::string client_max_body_size);
	void	setCgiPath(std::string cgi_path);
	void	setCgiExt(std::string cgi_ext);
//  METHODS  //
	int		searchConfig(std::string option);
	int		searchLocationConfig(std::string option);
	void	printConfig(void) const;
//  CHECKERS  //
	int		checkConfig(void);
	int		checkLocations(void);

//  LINEKED LIST LOCATIONS //
	Location	*searchLocation(std::string option);
};

#endif
