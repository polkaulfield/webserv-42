#include <string>
#include <cstring>
#include <wait.h>
#include <vector>
#include "clientRequest.hpp"
#include "config.hpp"

class Cgi
{
	private:
		std::string _executable;
		char** _populateEnv(ClientRequest& clientRequest, Config& config, std::string petition, std::string htmlPath);

	public:
		Cgi(ClientRequest clientRequest);
		Cgi();
		~Cgi();
		std::string	execScript(std::string htmlPath, std::string petition, ClientRequest& clientRequest, Config& config);
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
