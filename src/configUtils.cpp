
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../include/config.hpp"

void exitConfig(Config *config, std::ifstream &configFd, std::string error) {
	if (!error.empty())
		std::cout << GREEN << error << RESET << std::endl;
	configFd.close();
	delete []config;
	std::exit(1);
}

int checkArrayConfig(Config *config) {
	int error = 0;
	int size = sizeof(config) / 4;

//	std::cout << "size :" << sizeof(config) / 4 << std::endl;

	for (int i = 0; i < size; i++) {
		std::cout << GREEN << "Checking Config: " << i + 1 << RESET << std::endl;
		error += config[i].checkConfig();
		std::cout << std::endl;
	}
	if (error > 0)
		std::cout << GREEN << "Found " << error << " errors in the configuration file" <<  RESET << std::endl;
	else
		std::cout << GREEN << "OK!" << RESET << std::endl;;
	return error;
}

int number_configs(std::string configFile) {
	std::ifstream	configFd;
	std::string		line;

	int server_count = 0;
	configFd.open(configFile.data(), std::ifstream::in);
	while (std::getline(configFd, line)) {
		if (line.compare(0, 6, "server") == 0)
			server_count++;
	}

	configFd.close();
	//std::cout << server_count << std::endl;
	return server_count;
}

Config	*takeConfig(char *configFile) {
	std::ifstream	configFd;
	std::string 	line;
	std::string		tmp;
	int				brackets = 0;
	Config			*config;

	int server_count = number_configs(configFile);
	if (server_count < 1) {
		std::cout << GREEN << "Not Found \"server\" in config" << RESET << std::endl;
		std::exit(1);
	}
	config = new Config[server_count];
	configFd.open(configFile, std::ifstream::in);
	int j = -1;
	while(std::getline(configFd, line)) {
		tmp.clear();
		for (size_t i = 0; i < line.length(); i++) { // remove tabs and space and put inside of tmp
			if ((tmp.empty()) && (line[i] == ' ' || line[i] == '\t'))
				continue ;
			if (line[i] == '#')
				break ;
			tmp.push_back(line[i]);
		}
		if (tmp.empty())
			continue ;
		else if (tmp == "server" && tmp.length() == 6) // handle in what config[j] we are
			++j;
		else if (j == -1) //if not server found
			exitConfig(config, configFd, "Error: not found server {}");
		else if (brackets == 2 && !config[j].getLastLocation()) {
			exitConfig(config, configFd, "Error: brackets found without a location");
		}
		else if ((tmp[0] == '{' || tmp[0] == '}') && tmp.length() == 1) { // handle brackets indentation
			if (tmp[0] == '{')
				brackets++;
			else
				brackets--;
		}
		else {
			if (brackets == 0 || brackets >= 3)
				exitConfig(config, configFd, "Error: brackets");
			else if (brackets == 1 && config[j].searchConfig(tmp)) // indentation lvl 1
				exitConfig(config, configFd, "");
			else if (brackets == 2 && config[j].getLastLocation()->location->searchLocationConfig(tmp))// indentation lvl 2
				exitConfig(config, configFd, "");
		}
	}

	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;
	//config[0].printConfig();
//	std::cout << std::endl;
	//config[1].printConfig();

//	std::cout << std::endl;
	if (checkArrayConfig(config))
		;//exitConfig(config, configFd, "");
	//deleteConfig(config);
	configFd.close();
	//delete []config;
	return config;
}



/*int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << GREEN << "Error: 2 arguments needed" << RESET << std::endl;
		return 1;
	}
	Config *config = takeConfig(argv[1]);
	config[0].printConfig();
	delete []config;
	return 0;
}*/
