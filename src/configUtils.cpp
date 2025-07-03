
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../include/config.hpp"

void exitConfig(std::list<Config> &configList, std::ifstream &configFd, std::string error) {
	if (!error.empty())
		std::cout << GREEN << error << RESET << std::endl;
	configFd.close();
	//delete []configList.back();
	(void)configList;
	std::exit(1);
}

int checkConfigList(std::list<Config> &configList) { // ARREGLAR ESTA FUNCION
	int error = 0;
	int count = 0;

	for (std::list<Config>::iterator iter = configList.begin(); iter != configList.end(); ++iter) {
		std::cout << GREEN << "Checking Config: " << ++count << RESET << std::endl;
		error += iter->checkConfig();
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

std::list<Config>	takeConfig(char *configFile) {
	std::ifstream	configFd;
	std::string 	line;
	std::string		tmp;
	int				brackets = 0;
	std::list<Config>	configList;

	configFd.open(configFile, std::ifstream::in);
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
		else if (tmp == "server" && tmp.length() == 6) // handle in what config[count] we are
			configList.push_back(Config());
		else if (configList.empty()) //if not server found
			exitConfig(configList, configFd, "Error: not found server {}");
		else if (brackets == 2 && configList.back().getLocationList().empty()) {
			exitConfig(configList, configFd, "Error: brackets found without a location");
		}
		else if ((tmp[0] == '{' || tmp[0] == '}') && tmp.length() == 1) { // handle brackets indentation
			if (tmp[0] == '{')
				brackets++;
			else
				brackets--;
		}
		else {
			if (brackets == 0 || brackets >= 3)
				exitConfig(configList, configFd, "Error: brackets");
			else if (brackets == 1 && configList.back().searchConfig(tmp)) // indentation lvl 1
				exitConfig(configList, configFd, "");
			else if (brackets == 2 && configList.back().getLocationList().back()->searchLocationConfig(tmp))// indentation lvl 2
				exitConfig(configList, configFd, "");
			//configList.back().printConfig();
		}
	}
	//configList.front().printConfig();
	//if (checkConfigList(configList))
		//exitConfig(config, configFd, "");
	configFd.close();
	return configList;
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
