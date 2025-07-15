#include "../include/clientRequest.hpp"
#include "../include/utils.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <bits/stdc++.h>

// std::string ClientRequest::_getBody(std::string request)
// {
// 	std::cout << "Full request:\n" << request << std::endl;

// 	size_t pos1 = request.find("\r\n\r\n");

// 	if (pos1 == std::string::npos) {
// 		std::cout << "Delimiter not found!" << std::endl;
// 		return "";
// 	}

// 	std::cout << "Header end position: " << pos1 << std::endl;

// 	std::string body = request.substr(pos1 + 4);

// 	std::cout << "Body:\n" << body << std::endl;

// 	return body;
// }

std::string ClientRequest::_getBody(std::string request)
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
}

std::map<std::string, std::string> ClientRequest::_createHeaderMap(std::string request)
{
	std::stringstream ss(request);
	std::map<std::string, std::string> map;
	std::string line, key, value;
	size_t pos;
	while (getline(ss, line))
	{
		// If we reach the body, quit
		pos = line.find("\r\n\r\n");
		if (pos != std::string::npos)
			break ;
		pos = line.find(':', 0);
		if (pos == std::string::npos)
			continue;
		key = line.substr(0, pos);
		value = line.substr(pos + 2);
		map[key] = value;
	}
	return map;
}

ClientRequest::ClientRequest(std::string request, const Config& config)
{
	std::stringstream ss(request);
	std::string field;
	ss >> field;
	_method = field;
	std::cout << _method << std::endl;
	ss >> field;
	_path = field;
	ss >> field;
	_httpVer = field;
	_headerMap = _createHeaderMap(request);

	_data = _getBody(request);

//-----------------------------------------------------
	//PARTE DE UPLOAD FILES
	_isMultipart = false;
	_boundary = "";
	_uploadedFiles.clear();
	_parseContentType(request);

//-----------------------------------------------------

	_path = config.getRoot() + _path;

	// Append index.html if its a subdir or root dir
	if (isDir(_path) && _method == "GET") // esta linea jodia las POST requests
		_path += "/" + config.getIndex();
	if (_path == "")
		_path = config.getIndex();
	// If there is a query string split into _path and _queryString
	_queryString = "";
	size_t qPos = _path.find("?");
	if (qPos != std::string::npos)
	{
		_queryString = _path.substr(qPos + 1);
		_path = _path.substr(0, qPos);
	}
	// URLS have spaced encoded with %20. We replace them with normal spaces
	_path = searchAndReplace(_path, "%20"," ");
	if (access(_path.data(), F_OK)) // handle error page
		_path = config.getRoot() + "/" + config.getErrorPage();
}

ClientRequest::~ClientRequest(void)
{

}

// Method Getter
std::string ClientRequest::getMethod() const { return _method; }

// Path Getter
std::string ClientRequest::getPath() const { return _path; }

// HTTP Version Getter
std::string ClientRequest::getHttpVer() const { return _httpVer; }

// Data getter
std::string ClientRequest::getData() const { return _data; }

// Query getter
std::string ClientRequest::getQuery() const { return _queryString; }

// Header dict getters
std::map<std::string, std::string> ClientRequest::getHeaderMap() const { return _headerMap; }
std::string ClientRequest::getHeaderValue(const std::string& key) const {
	try {
		return _headerMap.at(key);
	}
	catch (...) {
		return "";
	}
}


//-------------------------------------------------------------
// getters de upload files

bool	ClientRequest::isMultipart() const { return _isMultipart; }

const std::vector<UploadedFile>& ClientRequest::getUploadedFiles() const { return _uploadedFiles; }

std::string const& ClientRequest::getBoundary() const { return _boundary; }

//---------------------------------------------------------------
