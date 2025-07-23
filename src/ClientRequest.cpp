#include "../include/ClientRequest.hpp"
#include "../include/utils.hpp"
#include <bits/stdc++.h>
#include <cstddef>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

std::string ClientRequest::_parseChunkedBody(std::string const &_data) {

  std::string parsedBody;
  size_t pos = 0;
  size_t posLimit = _data.find("\r\n");

  while (1) {
    unsigned int chunkLength =
        std::strtol(_data.substr(pos, posLimit - pos).data(), NULL, 16);
    if (chunkLength == 0)
      break;
    pos = posLimit + 2;
    posLimit = _data.find("\r\n", pos);
    parsedBody += _data.substr(pos, chunkLength);
    pos = posLimit + 2;
    posLimit = _data.find("\r\n", pos);
  }
  return parsedBody;
}

std::string ClientRequest::_getBody(std::string request) {
  size_t pos1 = request.find("\r\n\r\n");

  if (pos1 == std::string::npos)
    return "";

  std::string body = request.substr(pos1 + 4);

  return body;
}

ClientRequest::ClientRequest(void) {
  _method = "GET";
  _path = "/";
  _httpVer = "HTTP/1.1";
}

std::map<std::string, std::string>
ClientRequest::_createHeaderMap(std::string request) {
  std::stringstream ss(request);
  std::map<std::string, std::string> map;
  std::string line, key, value;
  size_t pos;
  while (getline(ss, line)) {
    // If we reach the body, quit
    pos = line.find("\r\n\r\n");
    if (pos != std::string::npos)
      break;
    pos = line.find(':', 0);
    if (pos == std::string::npos)
      continue;
    key = line.substr(0, pos);
    if (pos + 2 <= line.length())
      value = line.substr(pos + 2);
    else
      value = "";
    map[key] = value;
  }
  return map;
}

ClientRequest::ClientRequest(std::string request, const Config &config) {
  std::stringstream ss(request);
  std::string field;
  ss >> field;
  _method = field;
  ss >> field;
  _queryPath = field;
  ss >> field;
  _httpVer = field;
  _headerMap = _createHeaderMap(request);

  std::string headerHost = getHeaderValue("Host");
  size_t pos = headerHost.find(":");
  std::string host = headerHost.substr(0, pos);
  if (config.getHost() != host)
    return;

  if (request.find("Transfer-Encoding: chunked") != std::string::npos)
    _data = _parseChunkedBody(_getBody(request));
  else
    _data = _getBody(request).substr(0, config.getClientMaxBodySize());
  _isMultipart = false;
  _boundary = "";
  _uploadedFiles.clear();
  _parseContentType(request);

  _path = config.getRoot() + _queryPath;
  // Append index.html if its a subdir or root dir
  if (isDir(_path) && _method == "GET") {
    if (_path[_path.length() - 1] != '/')
      _path += '/';
    std::string indexPath = _path + config.getIndex();
    if (access(indexPath.c_str(), R_OK) == 0)
      _path += "/" + config.getIndex();
  }
  if (_path == "")
    _path = config.getIndex();
  // If there is a query string split into _path and _queryString
  _queryString = "";
  size_t qPos = _path.find("?");
  if (qPos != std::string::npos) {
    _queryString = _path.substr(qPos + 1);
    _path = _path.substr(0, qPos);
  }
  // URLS have spaced encoded with %20. We replace them with normal spaces
  _path = searchAndReplace(_path, "%20", " ");
}

ClientRequest::~ClientRequest(void) {}

void ClientRequest::setPath(const std::string &path) { _path = path; }

// Method Getter
std::string ClientRequest::getMethod() const { return _method; }

// Path Getter
std::string ClientRequest::getPath() const { return _path; }

// Query path getter
std::string ClientRequest::getQueryPath() const { return _queryPath; }

// HTTP Version Getter
std::string ClientRequest::getHttpVer() const { return _httpVer; }

// Data getter
std::string ClientRequest::getData() const { return _data; }

// Query getter
std::string ClientRequest::getQuery() const { return _queryString; }

// Header dict getters
std::map<std::string, std::string> ClientRequest::getHeaderMap() const {
  return _headerMap;
}
std::string ClientRequest::getHeaderValue(const std::string &key) const {
  try {
    return _headerMap.at(key);
  } catch (...) {
    return "";
  }
}

bool ClientRequest::isMultipart() const { return _isMultipart; }

// getter for array of files to upload
const std::vector<UploadedFile> &ClientRequest::getUploadedFiles() const {
  return _uploadedFiles;
}

std::string const &ClientRequest::getBoundary() const { return _boundary; }
