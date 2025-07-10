#include "../include/clientRequest.hpp"
#include "../include/utils.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <bits/stdc++.h>

//-----------------------------------------------------
//esto pasarlo a un utils mejor no?
std::string	extractUserAgent(std::string petition) {
	size_t	pos = petition.find("User-Agent:");
	if (pos == std::string::npos)
		return "";
	size_t	start = pos + 12;
	size_t	end = petition.find("\r\n", start);
	if (end == std::string::npos)
		return "";
	return petition.substr(start, end - start);
}

std::string	extractAccept(std::string petition) {
	size_t	pos = petition.find("Accept: ");
	if (pos == std::string::npos)
		return "";
	size_t	start = pos + 8;
	size_t	end = petition.find("\r\n", start);
	if (end == std::string::npos)
		return "";
	return petition.substr(start, end - start);
}

std::string	extractAcceptLanguage(std::string petition) {
	size_t	pos = petition.find("Accept-Language: ");
	if (pos == std::string::npos)
		return "";
	size_t	start = pos + 17;
	size_t	end = petition.find("\r\n", start);
	if (end == std::string::npos)
		return "";
	return petition.substr(start, end - start);
}

std::string	extractConnection(std::string petition) {
	size_t	pos = petition.find("Connection: ");
	if (pos == std::string::npos)
		return "";
	size_t	start = pos + 12;
	size_t	end = petition.find("\r\n", start);
	if (end == std::string::npos)
		return "";
	return petition.substr(start, end - start);
}

//-----------------------------------------------------------

std::string ClientRequest::_getBody(std::string const& request)
{
	size_t pos1 = request.find("\r\n\r\n");
	if (pos1 == std::string::npos)
		return "";

	std::string body = request.substr(pos1 + 4);
	return body;
}

ClientRequest::ClientRequest(void)
{
	_method = "GET";
	_path = "/";
	_httpVer = "HTTP/1.1";
	_returnCode = "200 OK";
	_contentType = "text/html";
}

ClientRequest::ClientRequest(char *req, const Config& config)
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

//-----------------------------------------------------
	//PARTE DE UPLOAD FILES
	_isMultipart = false;
	_boundary = "";
	_uploadedFiles.clear();
	_parseContentType(req);
//-----------------------------------------------------

	// Process path req
	_path = config.getRoot() + _path;
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
void ClientRequest::setMethod(const std::string& method) { _method = method; }
std::string ClientRequest::getMethod() const { return _method; }

// Path Getter
void ClientRequest::setPath(const std::string& path) { _path = path; }
std::string ClientRequest::getPath() const { return _path; }

// HTTP Version Getter
void ClientRequest::setHttpVer(const std::string& httpVer) { _httpVer = httpVer; }
std::string ClientRequest::getHttpVer() const { return _httpVer; }

// Return Code Getter
void ClientRequest::setReturnCode(const std::string& returnCode) { _returnCode = returnCode; }
std::string ClientRequest::getReturnCode() const { return _returnCode; }

// Content Type Getter
void ClientRequest::setContentType(const std::string& contentType) { _contentType = contentType; }
std::string ClientRequest::getContentType() const { return _contentType; }

std::string ClientRequest::getData() const { return _data; }

// Query getter
std::string ClientRequest::getQuery() const { return _queryString; }

// some other getters
std::string ClientRequest::getUserAgent() const { return _userAgent; }
std::string ClientRequest::getAccept() const { return _accept; }
std::string ClientRequest::getAcceptLanguage() const { return _acceptLanguage; }
std::string ClientRequest::getConnection() const { return _connection; }


//-------------------------------------------------------------
// getters de upload files

bool	ClientRequest::isMultipart() const { return _isMultipart; }

const std::vector<UploadedFile>& ClientRequest::getUploadedFiles() const { return _uploadedFiles; }

std::string const& ClientRequest::getBoundary() const { return _boundary; }

//---------------------------------------------------------------
