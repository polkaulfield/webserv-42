#include "../include/serverResponse.hpp"
#include "../include/utils.hpp"
#include "../include/cgi.hpp"
#include <unistd.h>
#include <sstream>
#include <fstream>

// Private methods

std::string ServerResponse::_getContentType(std::string path)
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

std::string ServerResponse::_buildOkResponse(std::string &buffer, std::string path)
{
	std::string response = "HTTP/1.1 200 OK\n\
Content-Type: " + _getContentType(path) + "\n\
Content-Length: " + intToString(buffer.length()) +
"\r\n\r\n\
"+ buffer;
	// Debug
	//std::cout << "Returning response\n" << response << std::endl;
	return response.data();
}

// Here we return the classic 404 not found
std::string ServerResponse::buildNotFoundResponse(void)
{
	std::string response = "HTTP/1.1 404 Not Found\r\n\
Content-Type: text/html\r\n\
Content-Length: 0\r\n\
Connection: Closed";
	return response.data();
}

// Some cpp magic to load a file to an array of chars
std::string ServerResponse::_makeFileBuffer(std::string path)
{
	std::ostringstream buffer;
	std::ifstream input(path.c_str());
	buffer << input.rdbuf();
	return buffer.str();
}

std::string ServerResponse::_getExtension(std::string htmlPath) {
	size_t	pos = htmlPath.find(".");
	size_t  arg = htmlPath.find("?");
	if (pos == std::string::npos) // si no encuentra punto devuelve string vaciobuildOk
		return "";
	if (arg != std::string::npos)
		return htmlPath.substr(pos, arg - pos);
	return htmlPath.substr(pos);
}

// Constructor

ServerResponse::ServerResponse(void)
{
	_response = "";
}

ServerResponse::ServerResponse(ClientRequest& clientRequest, const Config& config, bool isUpload)
{
    std::cout << "Creating server response" << std::endl;
	std::string buffer;
	if (clientRequest.getMethod() == "GET")
	{
		if (isCGI(clientRequest.getPath(), config)) {
			Cgi				cgiHandler;
			buffer = cgiHandler.execScript(clientRequest, config);
			_response = _buildOkResponse(buffer, clientRequest.getPath());
		}
		else if (!clientRequest.getPath().empty())
		{
			buffer = _makeFileBuffer(clientRequest.getPath());
			_response = _buildOkResponse(buffer, clientRequest.getPath());
		}
		else {
			// If file doesnt exist make a 404 not found
			_response = buildNotFoundResponse();
		}
	}
	else if (clientRequest.getMethod() == "POST")
	{
        if(isUpload)
            std::cout << "HERE WE CALL UPLOAD HANDLER" << std::endl;
        _response = buildNotFoundResponse();
	}
}

ServerResponse::~ServerResponse(void)
{

}

// Method Getter
std::string ServerResponse::getResponse() { return _response; }
