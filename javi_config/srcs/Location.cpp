#include "../includes/Location.hpp"
#include "../includes/Config.hpp"
//  CONSTRUCTORS & DESTRUCTOR //
 Location::Location(void) {
	GET = false;
	POST = false;
	DELETE = false;
	//std::cout << "default is created (location)" << std::endl;
}
Location::Location(std::string _directory) {
	directory = _directory;
	GET = false;
	POST = false;
	DELETE = false;
	//std::cout << _directory << " is created (location)" << std::endl;
}

Location &Location::operator = (const Location &src) {
	if (this != &src) {
		directory = src.directory;
		GET = src.GET;
		POST = src.POST;
		DELETE = src.DELETE;
		redirect = src.redirect;
	}
	return *this;
}

Location::~Location(void) {
	;//std::cout << directory << ": is destroyed" << std::endl;
}

//  GETTER  //
std::string	Location::getDirectory(void) {return directory;}
std::string Location::getRedirect(void) {return redirect;}
bool	Location::getGet(void) {return GET;}
bool	Location::getPost(void) {return POST;}
bool	Location::getDelete(void) {return DELETE;}

//  SETTERS  //
void Location::setAllowMethods(std::string option) {
	int start = 14;
	int end = start + 1;
	//std::string tmp;
	if (option.length() <= 15)
		std::cout << "error in allow methods" <<std::endl;

	while(end > 0 && (size_t)end < option.length()) {
		if (option[end] == ' ' || option[end] == ';') {
			if (option.compare(start, end - start, "GET") == 0)
				GET = true;
			else if (option.compare(start, end - start, "POST") == 0)
				POST = true;
			else if (option.compare(start, end - start, "DELETE") == 0)
				DELETE = true;
			else
				std::cout << GREEN << option.substr(start, end - start) << RESET << std::endl;
			start = end + 1;
		}
		else {
			;//tmp.push_back(option[end]);
		}
		end++;
	}
}

void	Location::setRedirect(std::string _redirect) {
	redirect = _redirect;
}

//  METHODS

int Location::searchLocationConfig(std::string option) {
	int error = 0;
	if (option.compare(0, 14, "allow_methods ") == 0)
		setAllowMethods(option);
	else if (option.compare(0, 7, "return ") == 0)
		setRedirect(takeParams(option, &error));
	else if (option.compare(0, 1, "") == 0)
		;
	else {
		std::cout << GREEN << "Error not found: " << option << RESET << std::endl;
		return 1;
	}
	if (error)
		return 1;
	return 0;
}


std::string	Location::takeParams(std::string option, int *error) {
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
