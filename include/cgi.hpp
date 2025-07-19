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
#include "../include/utils.hpp"


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
		std::string	getCGIOutput(char **args, char **env, std::string const& postData, std::string const& path);
	};
bool	isCGI(std::string htmlPath, const Config &config);

