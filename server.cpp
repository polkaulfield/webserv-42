#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <regex>
#include <map>
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
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int optval = 1;
    // This is EXTREMELY important. This tells the kernel it doesnt have to lock PORT
    // so if the serverSocket is closed we can reuse it right away. If this line is missing
    // we have to wait 2~ min until the system frees the port.
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    // Attach the socket to the port and listen
    int ret = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
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
    else if (hasExtension(path, {".jpg", ".jpeg"}))
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
    else if (endsWith(path, ".svg"))
        return "image/svg+xml";
    else if (hasExtension(path, {".tif", ".tiff"}))
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
Content-Length: " + std::to_string(buffer.length()) +
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
    std::cout << "Closing socket!" << std::endl;
    close(serverSocket);
    exit(0);
}

// This is to get the path requested by the browser in the header. We check if its dir or the file exists
// If the dir exists we append index.html before checking is the file exists
std::string pathFromGet(std::string petition)
{
    size_t pos1 = petition.find("/");
    size_t pos2 = petition.find("HTTP");
    std::string path = petition.substr(pos1, pos2 - 5);
    // Here we remove the first character / from the extracted path
    path = path.substr(1);
    // Append index.html if its a subdir or root dir
    if (isDir(path))
        path += "/index.html";
    if (path == "")
        path = "index.html";
    // URLS have spaced encoded with %20. We replace them with normal spaces
    std::regex pattern("%20");
    path = std::regex_replace(path, pattern, " ");
    // If the file exists, return the path
    if (access(path.c_str(), F_OK) != -1)
        return path;
    // We cannot return nullptr with the std::string datatype, so empty it is
    return "";
}
// WIP Post implementation:
// info here: https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods/POST
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

int main(int argc, char **argv)
{
    // We have to initialize the server socket first
    serverSocket = createServerSocket(PORT);
    signal(SIGINT, sigintHandle);
    if (!serverSocket)
    {
        std::cout << "Socket creation failed!" << std::endl;
        exit(0);
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
        clientSocket = accept(serverSocket, nullptr, nullptr);
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
                // If the file exists we create the string buffer and create an OK response
                // with the file content
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
