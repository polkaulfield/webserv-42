#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <iostream>
# include <cstdlib>
# include <unistd.h>
# include <sstream>
# include "Config.hpp"

class Location {
private:
public:
	Location(std::string _directory);
	std::string directory;
	bool GET;
	bool POST;
	bool DELETE;
	//  METHODS  //
	void	setAllowMethods(std::string option);
};

struct location_t {
	Location *Location;
	struct location_t	*next;
};
#endif
