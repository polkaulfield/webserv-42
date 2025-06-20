#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include "utils.hpp"
#define PORT 9000

int serverSocket = 0;

// Some cpp magic to load a file to an array of chars
std::string makeFileBuffer(std::string &path)
{
    std::ostringstream buffer;
    std::ifstream input(path.c_str());
    buffer << input.rdbuf();
    return buffer.str();
}

// This creates a socket listening on PORT. From it we create the clientSocket to handle the connections
int createServerSocket(int port)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int optval = 1;
    // This is EXTREMELY important. This tells the kernel it doesnt have to lock PORT
    // so if the serverSocket is closed we can reuse it right away. If this line is missing
    // we have to wait 2~ min until the system frees the port.
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    // Attach the socket to the port and listen
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        std::exit(1);
    listen(serverSocket, 5);
    return serverSocket;
}

// This is to generate the correct data header based on the extension of the file requested by the browser
// MIME types from: https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/MIME_types/Common_types
std::string getContentType(std::string path)
{
    if (endsWith(path, ".css"))
        return "text/css";
    else if (endsWith(path, ".html"))
        return "text/html";
    else if (endsWith(path, ".js"))
        return "text/javascript";
    else if (endsWith(path, ".json"))
        return "application/json";
    else if (endsWith(path, ".jpg") || endsWith(path, ".jpeg"))
        return "image/jpeg";
    else if (endsWith(path, ".md"))
        return "text/markdown";
    else if (endsWith(path, ".mp3"))
        return "audio/mpeg";
    else if (endsWith(path, ".png"))
        return "image/png";
    else if (endsWith(path, ".pdf"))
        return "application/pdf";
    else if (endsWith(path, ".php"))
        return "application/php";
    else if (endsWith(path, ".py"))
        return "text/html";
    else if (endsWith(path, ".svg"))
        return "image/svg+xml";
    else if (endsWith(path, ".tif") || endsWith(path, ".tiff"))
        return "image/tiff";
    else if (endsWith(path, ".ttf"))
        return "font/ttf";
    else if (endsWith(path, ".txt"))
        return "text/plain";
    else
        return "application/octet-stream";
}

// This response is to be called when the content requested exists.
// It needs the size of the char array that contains the requested content
// After the header is important to have an empty line and then the
// char array content (basically a string its easier to work with in cpp)
std::string buildOkResponse(std::string &buffer, std::string &path)
{
    std::string response = "HTTP/1.1 200 OK\n\
Content-Type: " + getContentType(path) + "\n\
Content-Length: " + intToString(buffer.length()) +
"\r\n\r\n\
"+ buffer;
    // Debug
    //std::cout << "Returning response\n" << response << std::endl;
    return response;
}

// Here we return the classic 404 not found
std::string buildNotFoundResponse(void)
{
    std::string response = "HTTP/1.1 404 Not Found\r\n\
Content-Type: text/html\r\n\
Content-Length: 0\r\n\
Connection: Closed";
    return response;
}

// This is to close the server socket on ctrl+c
void    sigintHandle(int signum)
{
    (void)signum;
    std::cout << "Closing socket!" << std::endl;
    close(serverSocket);
    std::exit(0);
}

// This is to get the path requested by the browser in the header. We check if its dir or the file exists
// If the dir exists we append index.html before checking is the file exists
std::string pathFromGet(std::string petition)
{
    size_t pos1 = petition.find("/");
    size_t pos2 = petition.find("HTTP");
    std::string path = petition.substr(pos1, pos2 - pos1 - 1); //millor (pos2 - pos1 - 1)
    // Here we remove the first character / from the extracted path
    path = path.substr(1);
    // Append index.html if its a subdir or root dir
    if (isDir(path))
        path += "/index.html";
    if (path == "")
        path = "index.html";
    // URLS have spaced encoded with %20. We replace them with normal spaces
    //std::regex pattern("%20");
    //path = std::regex_replace(path, pattern, " ");
    // If the file exists, return the path
    //
    // Remove after ? TODO Better fix
    int qPos = path.find("?");
    if (qPos != std::string::npos)
        path = path.substr(0, qPos);
    std::cout << "Got path " + path << std::endl;
    if (access(path.c_str(), F_OK) != -1)
        return path;
    // We cannot return nullptr with the std::string datatype, so empty it is
    return "";
}
// WIP Post implementation:
// info here: https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods/POST
/*
std::map<std::string, std::string> getMapFromPost(std::string petition)
{
    size_t pos1 = petition.find("/");
    size_t pos2 = petition.find("HTTP");
    std::string endpoint = petition.substr(pos1, pos2 - 5);

    // Create a map with key,value structure. First value is the endpoint
    std::map<std::string, std::string> values;
    values.insert(std::pair("endpoint",endpoint));
    return values;
    //size_t pos_values = petition.find("\r\n\r\n");
}
*/


std::string getExtension(std::string htmlPath) {
	size_t	pos = htmlPath.find(".");
	size_t  arg = htmlPath.find("?");
	if (pos == std::string::npos) // si no encuentra punto devuelve string vacio
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

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    // We have to initialize the server socket first
    serverSocket = createServerSocket(PORT);
    signal(SIGINT, sigintHandle);
    if (!serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        std::exit(0);
    }

    // Declare all the stuff we need in the main loop
    std::string path;
    std::string buffer;
    // Petition buff is where we place the request made with the browser with recv
    char petition_buf[1024] = {0};
    // Here is where the petition is converted to a cpp string for easier manipulation
    std::string petition;
    std::string response;
    int clientSocket;
    do
    {
        // We need to create a client socket for each connection
        // using the bound socket to PORT we created before (serverSocket)
        clientSocket = accept(serverSocket, NULL, NULL);
        // This gets the data clientSocket listened to petition_buf
        recv(clientSocket, petition_buf, sizeof(petition_buf), 0);
        // We convert the petition_buf char array to a cpp string
        petition = petition_buf;
        // For now we only handle GET request
        if (petition.find("GET", 0) != std::string::npos)
        {
            // Get the requested path from the request headers
            path = pathFromGet(petition);
            if (!path.empty())
            {
                if (isCGI(path))
                    buffer = execScript(path, petition);
                // If the file exists we create the string buffer and create an OK response
                // with the file content
                else
                    buffer = makeFileBuffer(path);
                response = buildOkResponse(buffer, path);
            }
            else {
                // If file doesnt exist make a 404 not found
                response = buildNotFoundResponse();
                path = "404";
            }
            // Here the response string gets converted to a char arr with .data() and sent to the clientSocket
            send(clientSocket, response.data(), response.length(), 0);
            std::cout << "Got a GET petition! Sending the contents of " << path << " with length " << response.length() << std::endl;
            // We need to close after sending
            close(clientSocket);
        }
    } while (clientSocket);
    return 0;
}
