#ifndef LOCATION_HPP
# define LOCATION_HPP
#include <string>
# include <unistd.h>

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
	void	setDirectory(std::string directory);
	void	setAllowMethods(std::string option);
	void	setRedirect(std::string redirect);
//  GETTER  //
	std::string	&getDirectory(void);
	std::string getRedirect(void);
	bool		getGet(void);
	bool		getPost(void);
	bool		getDelete(void);

//  METHODS  //
	int	searchLocationConfig(std::string option);
	bool hasMethod(std::string method);
};

struct location_t {
	Location *location;
	struct location_t	*next;
};
#endif
