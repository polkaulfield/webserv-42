#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <iostream>
# include <cstdlib>
# include <unistd.h>
# include <sstream>
# include "Config.hpp"

class Location {
private:
	std::string _directory;
	bool _GET;
	bool _POST;
	bool _DELETE;
	std::string	_redirect;
//  METHODS  //
	std::string	_takeParams(std::string option, int *error);
public:
//  CONSTRUCTORS & DESTRUCTORS  //
	Location(void);
	Location(std::string directory);
	Location &operator = (const Location &src);
	~Location(void);

//  SETTERS  //
	void	setAllowMethods(std::string option);
	void	setRedirect(std::string redirect);
//  GETTER  //
	std::string	getDirectory(void);
	std::string getRedirect(void);
	bool		getGet(void);
	bool		getPost(void);
	bool		getDelete(void);

//  METHODS  //
	int	searchLocationConfig(std::string option);
};

struct location_t {
	Location location;
	struct location_t	*next;
};
#endif
