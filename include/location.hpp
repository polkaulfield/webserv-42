#ifndef LOCATION_HPP
# define LOCATION_HPP
#include <string>
# include <unistd.h>

class Location {
private:
	std::string	_directory;
	bool		_GET;
	bool		_POST;
	bool		_DELETE;
	std::string	_redirect;
	bool		_autoindex;
	bool		_directory_listing;
	std::string	_uploadDir;
	bool		_error_parser; // this flag is used for detect errors after parser
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
	void	setAutoindex(std::string autoindex);
	void	setDirectoryListing(std::string directory_listing);
	void	setUploadDir(std::string uploadDir);
//  GETTER  //
	std::string	getDirectory(void) const;
	std::string getRedirect(void) const;
	bool		getGet(void) const;
	bool		getPost(void) const;
	bool		getDelete(void) const;
	bool		getAutoindex(void) const;
	bool		getDirectoryListing(void) const;
	std::string getUploadDir(void) const;
	int			getErrorsParser(void);
	bool		getIsUpload(void) const;
//  METHODS  //
	int	searchLocationConfig(std::string option);
	bool hasMethod(std::string method);
//  CHECKERS  //
bool	checkAllowMethods(void);
};
#endif
