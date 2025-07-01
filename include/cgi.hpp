#include <string>
#include <cstring>
#include <iostream>
#include <wait.h>
#include <vector>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "clientRequest.hpp"
#include "../include/config.hpp"


class Cgi
{
	private:
		std::string _executable;
		char** _populateEnv(const ClientRequest& clientRequest, const Config& config);
	public:
		Cgi(const ClientRequest& clientRequest);
		Cgi();
		~Cgi();
		std::string	execScript(const ClientRequest& clientRequest, const Config& config);
};

std::string getExtension(std::string htmlPath);
bool	isCGI(std::string htmlPath);
std::string	determineInterpreter(std::string htmlPath);
std::string extractMethod(std::string petition);
std::string	extractQuery(std::string petition);
std::string	extractHost(std::string petition);
std::string extractUserAgent(std::string petition);
std::string extractAccept(std::string petition);
std::string extractAcceptLanguage(std::string petition);
std::string extractConnection(std::string petition);
char**	vectorToArray(std::vector<std::string> envVars);
