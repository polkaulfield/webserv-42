#include "../include/clientRequest.hpp"
#include "../include/utils.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <bits/stdc++.h>

std::string ClientRequest::_getBody(std::string request)
{
    std::string body;
    size_t pos1;

    pos1 = request.find("\r\n\r\n");
    if (pos1 == std::string::npos)
        return "";
    return request.substr(pos1 + 4);
}

ClientRequest::ClientRequest(void)
{
    _method = "GET";
    _path = "/";
    _httpVer = "HTTP/1.1";
    _returnCode = "200 OK";
    _contentType = "text/html";
}

ClientRequest::ClientRequest(char *req)
{
    std::string request = req;
    std::stringstream ss(request);
    std::string field;
    ss >> field;
    _method = field;
    ss >> field;
    _path = field;
    ss >> field;
    _httpVer = field;
    _data = _getBody(request);

    // Debug view params
    std::cout << "Got this data: " << std::endl << _data << std::endl << "End Data" << std::endl;
    std::cout << "Got this req: " << std::endl << request << std::endl << "End req" << std::endl;

    // Process path req
    _path = _path.substr(1);

    // Append index.html if its a subdir or root dir
    if (isDir(_path))
        _path += "/index.html";
    if (_path == "")
        _path = "index.html";
    // If there is a query string split into _path and _queryString
    _queryString = "";
    size_t qPos = _path.find("?");
    if (qPos != std::string::npos)
    {
        _queryString = _path.substr(qPos + 1);
        _path = _path.substr(0, qPos);
    }
    std::cout << "_queryString: " << _queryString << std::endl;
    // URLS have spaced encoded with %20. We replace them with normal spaces
    _path = searchAndReplace(_path, "%20"," ");
    if (access(_path.c_str(), F_OK) == -1)
        _path = "";
}

ClientRequest::~ClientRequest(void)
{

}

// Method Getter
void ClientRequest::setMethod(std::string method) { _method = method; }
std::string ClientRequest::getMethod() { return _method; }

// Path Getter
void ClientRequest::setPath(std::string path) { _path = path; }
std::string ClientRequest::getPath() { return _path; }

// HTTP Version Getter
void ClientRequest::setHttpVer(std::string httpVer) { _httpVer = httpVer; }
std::string ClientRequest::getHttpVer() { return _httpVer; }

// Return Code Getter
void ClientRequest::setReturnCode(std::string returnCode) { _returnCode = returnCode; }
std::string ClientRequest::getReturnCode() { return _returnCode; }

// Content Type Getter
void ClientRequest::setContentType(std::string contentType) { _contentType = contentType; }
std::string ClientRequest::getContentType() { return _contentType; }

std::string ClientRequest::getData() { return _data; }
