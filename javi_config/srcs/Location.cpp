#include "../includes/Location.hpp"
#include "../includes/Config.hpp"

Location::Location(std::string _directory) {
	directory = _directory;
	GET = false;
	POST = false;
	DELETE = false;
}

void Location::setAllowMethods(std::string option) {
	int start = 14;
	int end = start + 1;
	//std::string tmp;
	if (option.length() <= 15)
		std::cout << "error in allow methods" <<std::endl;

	while(end < option.length()) {
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
