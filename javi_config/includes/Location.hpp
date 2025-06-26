#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <iostream>
# include <cstdlib>
# include <unistd.h>
# include <sstream>
# include "Config.hpp"

class Location {
private:
	std::string directory;
	bool GET;
	bool POST;
	bool DELETE;
	std::string	redirect;
//  METHODS  //
	std::string	takeParams(std::string option, int *error);
public:
//  CONSTRUCTORS & DESTRUCTORS  //
	Location(void);
	Location(std::string _directory);
	Location &operator = (const Location &src);
	~Location(void);

//  SETTERS  //
	void	setAllowMethods(std::string option);
	void	setRedirect(std::string _redirect);
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
