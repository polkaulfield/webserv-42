#include "../include/cgi.hpp"
//INFO for cgi: https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db

// This function is to be called before executing the cgi behind fork. We set all the parameters inside
// the params map as a new environment for the child cgi process

char**	vectorToArray(std::vector<std::string> envVars) {
	std::vector<std::string>::size_type	size = envVars.size();
	char	**env = new char*[size + 1];

	for (std::vector<std::string>::size_type i = 0; i < size; i++) {
		std::vector<std::string>::size_type	len = envVars[i].length();
		env[i] = new char[len + 1];
		std::strcpy(env[i], envVars[i].c_str());
	}
	env[size] = NULL;
	return env;
}

char** Cgi::_populateEnv(ClientRequest& clientRequest, Config& config, std::string petition, std::string htmlPath)
{
    //TODO: We need to pass all the needed env variables from info to cgi after forking
	std::vector<std::string>	envVars;

	// Variables basicas
	envVars.push_back("REQUEST_METHOD=" + clientRequest.getMethod());
	envVars.push_back("CONTENT_TYPE=" + clientRequest.getContentType());
	envVars.push_back("SCRIPT_NAME=" + htmlPath);
	envVars.push_back("QUERY_STRING=" + extractQuery(petition));

	//utilizamos ostringstream para convertir int en string
	std::string	postData = clientRequest.getData();
	std::ostringstream	dataLength;
	dataLength << postData.length();
	envVars.push_back("CONTENT_LENGTH=" + dataLength.str());

	//variables del servidor --> cuando este juntado lo de javi
	std::ostringstream	portNumber;
	portNumber << config.getPort();
	envVars.push_back("SERVER_NAME=" + config.getServerName());
	envVars.push_back("SERVER_PORT=" + portNumber.str());
	envVars.push_back("DOCUMENT_ROOT=" + config.getRoot());

	//Variables HTTP
	envVars.push_back("HTTP_HOST=" + extractHost(petition));

	return vectorToArray(envVars);
}

std::string getExtension(std::string htmlPath) {
	size_t	pos = htmlPath.find(".");
	size_t  arg = htmlPath.find("?");
	if (pos == std::string::npos) // si no encuentra punto devuelve string vaciobuildOk
		return "";
	if (arg != std::string::npos)
	    return htmlPath.substr(pos, arg - pos);
	return htmlPath.substr(pos);
}

bool	isCGI(std::string htmlPath) {
	std::string	extension = getExtension(htmlPath);
	std::cout << "GOT CGI FILE!" << std::endl;
	return	(extension == ".py" || extension == ".php" ||
			 extension == ".pl" || extension == ".cgi");
}

std::string	determineInterpreter(std::string htmlPath) {
	std::string	extension = getExtension(htmlPath);
	if (extension == ".py") return "/usr/bin/python3";
	else if (extension == ".php") return "/usr/bin/php";
	else if (extension == ".pl") return "/usr/bin/perl";
	else return ""; // para los .cgi
}

std::string extractMethod(std::string petition) {
	size_t	pos = petition.find(" ");
	return petition.substr(0, pos);
}

std::string	extractQuery(std::string petition) {
	size_t	posQuestion = petition.find("?");
	if (posQuestion == std::string::npos)
		return ""; // si no hay query no se devuelve parametro
	size_t	posHTTP = petition.find(" HTTP");
	if (posHTTP == std::string::npos)
		return petition.substr(posQuestion + 1); // por si no acaba en " HTTP"
	else
		return petition.substr(posQuestion + 1, posHTTP - posQuestion - 1);
}

// void	extractHostandPort(std::string petition, std::string& serverName, std::string& serverPort) {
// 	std::string	host = extractHost(petition);

// 	size_t	colonPos = host.find(":");
// 	if (colonPos != std::string::npos) {
// 		serverName = host.substr(0, colonPos);
// 		serverPort = host.substr(colonPos + 1);
// 	}
// 	else {
// 		serverName = host;
// 		serverPort = "80";
// 	}
// }

std::string	extractHost(std::string petition) {
	size_t	posHost = petition.find("Host: ");
	if (posHost == std::string::npos)
		return "";
	size_t	start = posHost + 6;
	size_t	posNextLine = petition.find("\r\n", start);
	if (posNextLine == std::string::npos)
		return "";
	return petition.substr(start, posNextLine - start);

}

std::string	getCGIOutput(char **args) {
	int	pipeFd[2];
	pipe(pipeFd);
	pid_t	pid;

	pid = fork();
	if (pid == 0) {
		close(pipeFd[0]);
		dup2(pipeFd[1], STDOUT_FILENO);
		close(pipeFd[1]);
		execve(args[0], args, environ);
		perror("execve failed");
		exit(1);
	}
	else if (pid > 0) {
		close(pipeFd[1]);
		std::string	output;
		char	buffer[1024];
		ssize_t	bytesRead;

		while((bytesRead = read(pipeFd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			output += buffer;
		}

		close(pipeFd[0]);
		waitpid(pid, NULL, 0);

		return output;
	}
	else {
		close(pipeFd[0]);
		close(pipeFd[1]);
		return "";
	}
}

std::string	execScript(std::string htmlPath, std::string petition) {

    std::cout << "Executing script!!" << std::endl;
	std::string	interpreter = determineInterpreter(htmlPath);

	std::string	method = extractMethod(petition);
	std::string	query = extractQuery(petition);
	std::cout << "QUERY: " <<  query << std::endl;
	std::string	host = extractHost(petition);

	setenv("REQUEST_METHOD", method.c_str(), 1); // pasar std::string a const char* de c
	setenv("QUERY_STRING", query.c_str(), 1);
	setenv("HTTP_HOST", host.c_str(), 1);
	setenv("SCRIPT_NAME", htmlPath.c_str(), 1);

	if (!interpreter.empty()) {
		char	*args[] = {
			(char *)interpreter.c_str(), // el (char *) entiendo que es para castearlo a char * sin const
			(char *)htmlPath.c_str(),
			(char *)query.c_str(),
			NULL
		};
		return getCGIOutput(args);
	}
	else {
		char	*args[] = {
			(char *)htmlPath.c_str(),
			NULL
		};
		return getCGIOutput(args);
	}
}
