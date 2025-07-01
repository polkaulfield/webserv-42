#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <unistd.h>
# include "location.hpp"

# define GREEN "\x1B[32m"
# define RESET "\x1B[0m"

struct location_t;
class Location;
class Config;

void exitConfig(Config *config, std::ifstream &configFd, std::string error);
int checkArrayConfig(Config *config);
int number_configs(std::string configFile);
Config	*takeConfig(char *configFile);

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
	location_t	*_firstLocation;
	location_t	*_iterLocation;
	location_t	*_lastLocation;
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
	location_t	*getFirstLocation(void) const;
	location_t	*getIterLocation(void);
	location_t	*getLastLocation(void);
	//  SETTERS  //
	void	setServerName(std::string _server_Name);
	void	setPort(std::string port);
	void	setHost(std::string host);
	void	setRoot(std::string root);
	void	setIndex(std::string index);
	void	setErrorPage(std::string error_page);
	void	setClientMaxBodySize(std::string client_max_body_size);
	void	setLocation(location_t *location);
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
	location_t	*addLocation(std::string _directory);
	Location	*searchLocation(std::string option);
};

#endif
