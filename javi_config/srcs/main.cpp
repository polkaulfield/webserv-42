
#include <iostream>
#include <fstream>
#include <string>
#include "../includes/Config.hpp"
void deleteConfig(Config **config) {
	for (int i = 0; i < 5; i++) {
		if (config[i])
			delete config[i];
	}
}

int	takeConfig(char *configFile) {
	std::ifstream	configFd;
	std::string 	line;
	std::string		tmp;
	int				brackets = 0;
	Config			*config[5] = {NULL, NULL, NULL, NULL, NULL};

	configFd.open(configFile, std::ifstream::in);
	int j = -1;
	while(std::getline(configFd, line)) {
		tmp.clear();
		if (line.empty())
			continue ;
		for (size_t i = 0; i < line.length(); i++) {
			if ((tmp.empty()) && (line[i] == ' ' || line[i] == '\t'))
				continue ;
			tmp.push_back(line[i]);
		}
		if (tmp[0] == '#')
			;
		else if (tmp == "server")
			config[++j] = new Config;
		else if (j == -1) { // Error not found server {} ; //&& !config[j]
			std::cout << GREEN << "Error: not found server {}" << RESET << std::endl;
			quick_exit(1);
		}
		else if ((tmp[0] == '{' || tmp[0] == '}') && tmp.length() == 1) {
			if (tmp[0] == '{')
				brackets++;
			else
				brackets--;
		}
		else {
			if (brackets == 0 || brackets == 3) {
				std::cout << GREEN << "Error: brackets" << RESET << std::endl;
				quick_exit(1);
			}
			else if (brackets == 1 && config[j]->searchConfig(tmp)) {
				deleteConfig(config);
				configFd.close();
				quick_exit(1);
			}
			else if (brackets == 2 && config[j]->searchLocationConfig(tmp))
				;
		}
	}
	configFd.close();
	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;
	config[0]->printConfig();
	std::cout << std::endl;
	config[1]->printConfig();
	if (config[j]->checkRoot())
		;//std::cout << GREEN << "Error: Root \"" << config[j]->getRoot() << "\" not found" << RESET << std::endl;
	deleteConfig(config);
	line.~basic_string();
	tmp.~basic_string();
	std::exit(10);
	return 0;
}



int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << GREEN << "Error: 2 arguments needed" << RESET << std::endl;
		return 1;
	}
	if (takeConfig(argv[1]))
		return 1;
	return 0;
}
