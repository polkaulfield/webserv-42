#include "../include/cgi.hpp"
//INFO for cgi: https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db

// This function is to be called before executing the cgi behind fork. We set all the parameters inside
// the params map as a new environment for the child cgi process

Cgi::Cgi() {}

//Cgi::Cgi(ClientRequest clientRequest) {}

Cgi::~Cgi() {}

char** Cgi::_populateEnv(const ClientRequest& clientRequest, const Config& config)
{
    //TODO: We need to pass all the needed env variables from info to cgi after forking
	std::vector<std::string>	envVars;

	// Variables basicas
	envVars.push_back("REQUEST_METHOD=" + clientRequest.getMethod());
	envVars.push_back("CONTENT_TYPE=" + clientRequest.getContentType());
	envVars.push_back("SCRIPT_NAME=" + clientRequest.getPath());
	envVars.push_back("QUERY_STRING=" + clientRequest.getQuery());
	std::cout << "QUERY ENV" << clientRequest.getQuery() << std::endl;
	std::cout << "=========================================" << std::endl;
	config.printConfig();
	std::cout << "=========================================" << std::endl;
	//utilizamos ostringstream para convertir int en string
	// std::string	postData = clientRequest.getData();
	// std::ostringstream	dataLength;
	// dataLength << postData.length();
	// envVars.push_back("CONTENT_LENGTH=" + dataLength.str());

	//lo mismo que la parte comentada pero utilizando la funcion de utils, no borro lo de arriba por si acaso
	envVars.push_back("CONTENT_LENGHT=" + intToString(clientRequest.getData().length()));

	// std::ostringstream	portNumber;
	// portNumber << config.getPort();
	envVars.push_back("SERVER_NAME=" + config.getServerName());
	//envVars.push_back("SERVER_PORT=" + portNumber.str());
	envVars.push_back("SERVER_PORT=" + intToString(config.getPort()));
	envVars.push_back("DOCUMENT_ROOT=" + config.getRoot());
	envVars.push_back("HTTP_HOST=" + config.getHost());
	envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVars.push_back("SERVER_SOFTWARE=WebServ/1.0");
	envVars.push_back("HTTP_USER_AGENT=" + clientRequest.getUserAgent());
	envVars.push_back("HTTP_ACCEPT=" + clientRequest.getAccept());
	envVars.push_back("HTTP_ACCEPT_LANGUAGE=" + clientRequest.getAcceptLanguage());
	envVars.push_back("HTTP_CONNECTION=" + clientRequest.getConnection());
	envVars.push_back("PATH_INFO=" + clientRequest.getPath());

	//variables opcionales que se podrian implementar. Hacen falta??
	//envVars.push_back("PATH_INFO=");
	//envVars.push_back("REMOTE_ADDR=127.0.0.1");

	return vectorToArray(envVars);
}

std::string	getCGIOutput(char **args, char **env, std::string postData) {
	//int	pipeFd[2];
	(void) postData;
	int	outputFd[2];

//	pipe(pipeFd);
	pipe(outputFd);

	pid_t	pid = fork();

	if (pid == 0) {
		//close(pipeFd[1]);
		//dup2(pipeFd[0], STDIN_FILENO);
		//close(pipeFd[0]);

		close(outputFd[0]);
		dup2(outputFd[1], STDOUT_FILENO);
		close(outputFd[1]);

		execve(args[0], args, env);
		perror("execve failed");
		std::exit(1);
	}
	else if (pid > 0) {
		//close(pipeFd[0]);
		//if (!postData.empty())
		//	write(pipeFd[1], postData.c_str(), postData.length());
		//close(pipeFd[1]);
		close(outputFd[1]);
		std::string	output;
		char	buffer[1024];
		ssize_t	bytesRead;

		while((bytesRead = read(outputFd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			output += buffer;
		}

		close(outputFd[0]);
		waitpid(pid, NULL, 0);

		std::cout << output << std::endl;
		return output;
	}
	else {
		//close(pipeFd[0]);
	//	close(pipeFd[1]);
		close(outputFd[0]);
		close(outputFd[1]);
		return "";
	}
}

std::string	Cgi::execScript(const ClientRequest& clientRequest, const Config& config) {

    std::cout << "Executing script!!" << std::endl;
    std::string path = clientRequest.getPath();
	std::string	interpreter = determineInterpreter(path);

	char** env = _populateEnv(clientRequest, config);
	std::string postData = clientRequest.getData();

	if (!interpreter.empty()) {
		char	*args[] = {
			(char *)interpreter.c_str(), // el (char *) entiendo que es para castearlo a char * sin const
			(char *)path.c_str(),
			NULL
		};
		return getCGIOutput(args, env, postData);
	}
	else {
		char	*args[] = {
			(char *)path.c_str(),
			NULL
		};
		return getCGIOutput(args, env, postData);
	}
}
