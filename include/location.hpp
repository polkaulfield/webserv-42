#ifndef LOCATION_HPP
# define LOCATION_HPP
#include <string>
# include <unistd.h>

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
	bool hasMethod(std::string method);
};

struct location_t {
	Location location;
	struct location_t	*next;
};
#endif
