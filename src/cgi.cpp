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
	envVars.push_back("CONTENT_TYPE=" + clientRequest.getHeaderValue("Content-Type"));
	envVars.push_back("SCRIPT_NAME=" + clientRequest.getPath());
	envVars.push_back("QUERY_STRING=" + clientRequest.getQuery());
	config.printConfig();
	envVars.push_back("CONTENT_LENGTH=" + intToString(clientRequest.getData().length()));
	envVars.push_back("SERVER_NAME=" + config.getServerName());
	//envVars.push_back("SERVER_PORT=" + portNumber.str());
	envVars.push_back("SERVER_PORT=" + intToString(config.getPort()));
	envVars.push_back("DOCUMENT_ROOT=" + config.getRoot());
	envVars.push_back("HTTP_HOST=" + config.getHost());
	envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVars.push_back("SERVER_SOFTWARE=WebServ/1.0");
	envVars.push_back("HTTP_USER_AGENT=" + clientRequest.getHeaderValue("User-Agent"));
	envVars.push_back("HTTP_ACCEPT=" + clientRequest.getHeaderValue("Accept"));
	envVars.push_back("HTTP_ACCEPT_LANGUAGE=" + clientRequest.getHeaderValue("Accept-Language"));
	envVars.push_back("HTTP_CONNECTION=" + clientRequest.getHeaderValue("Connection"));
	envVars.push_back("PATH_INFO=" + clientRequest.getPath());
	return vectorToArray(envVars);
}

std::string	 Cgi::getCGIOutput(char **args, char **env, std::string const& postData, std::string const& path) {

	int	outputFd[2], inputFd[2];

	pipe(outputFd);
	pipe(inputFd);
	pid_t	pid = fork();
	if (pid == 0) {
		std::string scriptDirectory = extractDirectory(path);
		if (!scriptDirectory.empty()) {
			if (chdir(scriptDirectory.data()) != 0) {
				perror("chdir failed");
				std::exit(1);
			}
		}

		std::string scriptName = extractFilename(path);
		
		if (args[1])
			args[1] = (char*)scriptName.data();

		close(outputFd[0]);
		dup2(outputFd[1], STDOUT_FILENO);
		close(outputFd[1]);

		close(inputFd[1]);
		dup2(inputFd[0], STDIN_FILENO);
		close(inputFd[0]);

		execve(args[0], args, env);
		perror("execve failed");
		freeArray(env);
		std::exit(1);
	}
	else if (pid > 0) {
		close(outputFd[1]);

		close(inputFd[0]);
		if (!postData.empty())
			write(inputFd[1], postData.data(), postData.length());
		close(inputFd[1]);

		std::string	output;
		char	buffer[1024];
		ssize_t	bytesRead;

		while((bytesRead = read(outputFd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			output += buffer;
		}
		close(outputFd[0]);
		waitpid(pid, NULL, 0);

		freeArray(env);
		return output;
	}
	else {
		close(outputFd[0]);
		close(outputFd[1]);
		close(inputFd[0]);
		close(inputFd[1]);
		freeArray(env);

		return "";
	}
}

std::string	Cgi::execScript(const ClientRequest& clientRequest, const Config& config) {
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
		return getCGIOutput(args, env, postData, path);
	}
	else {
		char	*args[] = {
			(char *)path.c_str(),
			NULL
		};
		return getCGIOutput(args, env, postData, path);
	}
}
