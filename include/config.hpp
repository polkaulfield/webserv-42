#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <unistd.h>
# include <list>
# include "location.hpp"

# define GREEN "\x1B[32m"
# define RESET "\x1B[0m"

class Location;
class Config;

int					checkConfigList(std::list<Config> &configList);
void				exitConfig(std::list<Config> &configList, std::ifstream &configFd, std::string error);
bool				checkChars(std::string src, std::string not_valid_chars);
bool				checkDigits(std::string src);
std::list<Config>	takeConfig(const char *configFile);

class Config {
private:
	std::string			_server_name;
	int					_port;
	std::string			_host;
	std::string			_root;
	std::string			_index;
	std::string			_error_page;
	size_t				_client_max_body_size;
	bool				_cgi;
	std::string			_cgi_path;
	std::string			_cgi_ext;
	std::list<Location>	_locationList;
	int					_double_port;
	int					_error_parser; // this flag is used for detect errors after parser
//  METHODS  //
	std::string	_takeParams(std::string option, int *error);
//  CHECKERS  //
	int		_checkRoot(void);
	int		_checkPort(void);
	int		_checkIndex(void);
	int		_checkClientMaxBodySize(void);
	int		_checkCgiPath(void);
	int		_checkCgiExt(void);
	int		_checkErrorPage(void);
public:
//  CONSTRUCTOR  //
	Config(void);
	Config(const Config &src);
	Config &operator = (const Config &src);
	~Config(void);
//  GETTERS  //
	std::string			getServerName(void) const;
	int					getPort(void) const;
	std::string			getHost(void) const;
	std::string			getRoot(void) const;
	std::string			getIndex(void) const;
	std::string 		getErrorPage(void) const;
	int					getClientMaxBodySize(void) const;
	bool				getIsCgi(void) const;
	bool				getDirListing(void) const;
	std::string			getCgiExt(void) const;
	std::list<Location>	&getLocationList(void);
	int					getDoublePort(void);
	int					getErrorsParser(void);
	const std::string getRedirectFromPath(const std::string& queryPath) const;

	//  SETTERS  //
	void	setServerName(std::string _server_Name);
	void	setPort(std::string port);
	void	setPort(int port);
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
    bool    isPathAutoIndex(const std::string& path) const;
	int		checkConfig(void);
	int		checkLocations(void);

//  LINEKED LIST LOCATIONS //
	const Location	*searchLocation(std::string option) const;
};

#endif
