#include "../include/ServerResponse.hpp"
#include "../include/Cgi.hpp"
#include "../include/Directory.hpp"
#include "../include/utils.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>

// Private methods

std::string ServerResponse::_getContentType(std::string const &path) {
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

std::string ServerResponse::_buildOkResponse(std::string &buffer,
                                             std::string const &path) {
  std::string response = "HTTP/1.1 200 OK\n\
Content-Type: " + _getContentType(path) +
                         "\n\
Content-Length: " + intToString(buffer.length()) +
                         "\r\n\r\n\
" + buffer;
  return response;
}

std::string ServerResponse::_buildDirResponse(std::string &buffer) {
  std::string response = "HTTP/1.1 200 OK\n\
Content-Type: text/html \n\
Content-Length: " + intToString(buffer.length()) +
                         "\r\n\r\n\
" + buffer;
  return response;
}

std::string ServerResponse::_buildCgiResponse(std::string &buffer) {
  // Parse cgi header
  // Check if cgi added Content-Type and
  size_t cgiHeaderSize = 0;
  if (buffer.find("Content-Type:") != std::string::npos) {
    // Check for header separator type 1
    cgiHeaderSize = buffer.find("\r\n\r\n");
    if (cgiHeaderSize != std::string::npos)
      cgiHeaderSize += 5;
    else {
      // Check for header separator type 2
      cgiHeaderSize = buffer.find("\n\n");
      if (cgiHeaderSize != std::string::npos)
        cgiHeaderSize += 3;
      else
        cgiHeaderSize = 0;
    }
  }
  std::string response = "HTTP/1.1 200 OK\r\n\
Content-Length: " + intToString(buffer.length() - cgiHeaderSize) +
                         "\n" + buffer;
  return response;
}

std::string ServerResponse::buildErrorResponse(int code,
                                               std::string const &message,
                                               const Config &config) {
  std::string data = "";
  std::string line;
  std::ifstream templateFile(std::string(config.getRoot() + config.getErrorPage()).data());
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

std::string ServerResponse::_buildRedirResponse(const std::string &url) {
  std::string response = "HTTP/1.1 308 Permanent Redirect\r\n\
Location: " + url + "\r\n\
Content-Type: text/html; charset=UTF-8 \r\n\
Content-Length: 0\r\n";
  return response.data();
}

// Some cpp magic to load a file to an array of chars
std::string ServerResponse::_makeFileBuffer(std::string const &path) {
  std::ostringstream buffer;
  std::ifstream input(path.c_str());
  buffer << input.rdbuf();

  return buffer.str();
}

std::string ServerResponse::_getExtension(std::string const &htmlPath) {
  size_t pos = htmlPath.find(".");
  size_t arg = htmlPath.find("?");
  if (pos ==
      std::string::npos) // si no encuentra punto devuelve string vaciobuildOk
    return "";
  if (arg != std::string::npos)
    return htmlPath.substr(pos, arg - pos);
  return htmlPath.substr(pos);
}

ServerResponse::ServerResponse(void) { _response = ""; }

ServerResponse::ServerResponse(ClientRequest &clientRequest,
                               const Config &config, bool isUpload) {

  std::string buffer;

  //std::cout << clientRequest.getData() << std::endl;

  if (clientRequest.getMethod() == "GET") {
    if (isCGI(clientRequest.getPath(), config)) {

      Cgi cgiHandler;
      buffer = cgiHandler.execScript(clientRequest, config);
      _response = _buildCgiResponse(buffer);
    } else if (config.getRedirectFromPath(clientRequest.getQueryPath()) != "") {

      _response = _buildRedirResponse(
          config.getRedirectFromPath(clientRequest.getQueryPath()));
    } else if (isDir(clientRequest.getPath()) &&
               config.isPathAutoIndex(clientRequest.getQueryPath())) {

      Directory directory = Directory(clientRequest.getPath());
      _response = _buildDirResponse(directory.getHtml());
    } else if (!clientRequest.getPath().empty()) {
      buffer = _makeFileBuffer(clientRequest.getPath());
      _response = _buildOkResponse(buffer, clientRequest.getPath());
    } else {
      // If file doesnt exist make a 404 not found
      _response = buildErrorResponse(404, "Not found", config);
    }
  } else if (clientRequest.getMethod() == "POST") {
  	std::cout << "ServerResponse constructor is POST" << std::endl;
    if (isCGI(clientRequest.getPath(), config)) {

      Cgi cgiHandler;
      buffer = cgiHandler.execScript(clientRequest, config);
      _response = _buildCgiResponse(buffer);

    } else if (isUpload) {
      std::cout << "is upload file " << clientRequest.isMultipart() << std::endl;
      if (clientRequest.isMultipart()) {
      	std::cout << "before _handleFileUpload and multipart" << std::endl;
        _handleFileUpload(clientRequest, config);
      }
      else{
        _response = buildErrorResponse(404, "Not a multipart request", config);
      }
      std::cout << "llega a aqui" << std::endl;
      if (_response.empty())
        _response = buildErrorResponse(500, "Upload processing failed", config);
    } else
      _response = buildErrorResponse(400, "Bad Request", config);
  } else if (clientRequest.getMethod() == "DELETE")
    _handleDeleteRequest(clientRequest, config);
}

ServerResponse::~ServerResponse(void) {}

std::string ServerResponse::getResponse() { return _response; }
