#include "../include/serverResponse.hpp"
#include "../include/utils.hpp"
#include "../include/cgi.hpp"
#include "../include/directory.hpp"
#include <string>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include "../include/utils.hpp"

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
	return response.data();
}

std::string ServerResponse::_buildDirResponse(std::string &buffer)
{
	std::string response = "HTTP/1.1 200 OK\n\
Content-Type: text/html \n\
Content-Length: " + intToString(buffer.length()) +
"\r\n\r\n\
"+ buffer;
	return response.data();
}

std::string ServerResponse::_buildCgiResponse(std::string &buffer)
{
    // Parse cgi header
    // Check if cgi added Content-Type and
    size_t cgiHeaderSize = 0;
    if (buffer.find("Content-Type:") != std::string::npos) {
        // Check for header separator type 1
        cgiHeaderSize = buffer.find("\r\n\r\n");
        if (cgiHeaderSize != std::string::npos)
            cgiHeaderSize += 5;
        else
        {
            // Check for header separator type 2
            cgiHeaderSize = buffer.find("\n\n");
            if (cgiHeaderSize != std::string::npos)
                cgiHeaderSize += 3;
            else
                cgiHeaderSize = 0;
        }
    }
	std::string response = "HTTP/1.1 200 OK\r\n\
Content-Length: " + intToString(buffer.length() - cgiHeaderSize) + "\n" + buffer;
	return response.data();
}

std::string ServerResponse::_buildRedirResponse(const std::string &url) {
    std::string response = "HTTP/1.1 302 OK\r\n\
    Location: " + url + "\
    Content-Type: text/html; charset=UTF-8 \
    Content-Length: 0";
    return response.data();
}

// Here we return the classic 404 not found
std::string ServerResponse::buildNotFoundResponse(void)
{
	std::string response = "HTTP/1.1 404 Not Found\r\n\
Content-Type: text/html\r\n\
Content-Length: 0\r\n\
Connection: Closed\r\n\r\n";
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

std::string	ServerResponse::buildErrorResponse(int code, std::string const& message) {
	std::string data = "";
	std::string line;
    std::ifstream templateFile("./templates/error.html");
    while (std::getline(templateFile, line))
        data += line + "\n";
    data = searchAndReplace(data, "%%ERRORMSG%%", message);
    data = searchAndReplace(data, "%%ERRORCODE%%", intToString(code));
	std::string response = "HTTP/1.1 " + intToString(code) + " Error\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + intToString(data.length()) + "\r\n";
	response += "\r\n";
	response += data;
	return response;
}

ServerResponse::ServerResponse(void)
{
	_response = "";
}

ServerResponse::ServerResponse(ClientRequest& clientRequest, const Config& config, bool isUpload)
{

	std::string buffer;

	std::cout << "path inside serverresponse: " << clientRequest.getPath() << std::endl;
	if (clientRequest.getMethod() == "GET")
	{
		if (isCGI(clientRequest.getPath(), config)) {

			Cgi				cgiHandler;
			buffer = cgiHandler.execScript(clientRequest, config);
			_response = _buildCgiResponse(buffer);
		}
		else if (config.getRedirectFromPath(clientRequest.getPath()) != "")	{

            _response = _buildRedirResponse(config.getRedirectFromPath(clientRequest.getPath()));
		}
		else if (isDir(clientRequest.getPath()) && config.isPathAutoIndex(clientRequest.getQueryPath())) {

			Directory directory  = Directory(clientRequest.getPath());
			_response = _buildDirResponse(directory.getHtml());
		}
		else if (isFile(clientRequest.getPath()))
		{
			buffer = _makeFileBuffer(clientRequest.getPath());
			_response = _buildOkResponse(buffer, clientRequest.getPath());
		}
		else {
		std::cout << "redir path: " << config.getRedirectFromPath(clientRequest.getPath()) << std::endl;
		std::cout << "gepath: "<<clientRequest.getPath() <<std::endl;
			// If file doesnt exist make a 404 not found
			_response = buildErrorResponse(404, "Not found!");
		}
	}
	else if (clientRequest.getMethod() == "POST")
	{
		if(isUpload) {
			if (clientRequest.isMultipart())
				_handleFileUpload(clientRequest, config);
			else
				_response = buildErrorResponse(400, "Not a multipart request");
			if (_response.empty())
				_response = buildErrorResponse(500, "Upload processing failed");
		}
		else
			_response = buildErrorResponse(400, "Bad Request");
	}
	else if (clientRequest.getMethod() == "DELETE")
		_handleDeleteRequest(clientRequest, config);
}

ServerResponse::~ServerResponse(void)
{

}

// Method Getter
std::string ServerResponse::getResponse() { return _response; }
