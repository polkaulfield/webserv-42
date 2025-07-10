/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFilesCR.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:45 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/10 16:19:18 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/uploadedFile.hpp"
#include "../include/clientRequest.hpp"
#include <iostream>

void ClientRequest::_parseMultipartPart(std::string const& part) {
	size_t headerEnd = part.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return;
	std::string headers = part.substr(0, headerEnd);
	std::string content = part.substr(headerEnd + 4);

	UploadedFile file;

	size_t nameStart = headers.find("name=\"");
	if (nameStart != std::string::npos) {
		nameStart += 6;
		size_t nameEnd = headers.find("\"", nameStart);
		if (nameEnd != std::string::npos)
			file._fieldName = headers.substr(nameStart, nameEnd - nameStart);
	}

	size_t filenameStart = headers.find("filename=\"");
	if (filenameStart != std::string::npos) {
		filenameStart += 10;
		size_t filenameEnd = headers.find("\"", filenameStart);
		if (filenameEnd != std::string::npos)
			file._filename = headers.substr(filenameStart, filenameEnd - filenameStart);
	}

	size_t cTypeStart = headers.find("Content-Type:");
	if (cTypeStart != std::string::npos) {
		cTypeStart += 13;
		size_t cTypeEnd = headers.find("\r\n", cTypeStart);
		if (cTypeEnd == std::string::npos)
			cTypeEnd = headers.length();
		file._contentType = headers.substr(cTypeStart, cTypeEnd - cTypeStart);
		file._contentType = _trimLeft(file._contentType);
	}

	file._content.clear();
	//content se extrae asi por si hay datos binarios y no se corte con algun \0
	//los archivos binarios puedes contener muchos bytes 0x00 lo que cortaria la extraccion de su contenido
	for (size_t i = 0; i < content.length(); i++)
		file._content.push_back(content[i]);
	file._size = content.length();

	_uploadedFiles.push_back(file);
}

bool ClientRequest::_parseMultipartBody(std::string const& _data) {
	if (_data.empty() || _boundary.empty())
		return false;

	std::string delimiter = "--" + _boundary;
	size_t pos = 0;
	while (_data.find(delimiter) != std::string::npos) {
		pos += delimiter.length();
		//saltar el "\r\n" despues de boundary"
		if (pos + 2 < _data.length() && _data.substr(pos, 2) == "\r\n")
			pos += 2;
		size_t nextDelimiter = _data.find(delimiter, pos);
		if (nextDelimiter == std::string::npos)
			break;

		std::string part = _data.substr(pos, nextDelimiter - pos);
		_parseMultipartPart(part);
		pos = nextDelimiter;
	}
	return true;
}

std::string	ClientRequest::_extractBoundary(std::string const& _contentType) {
	size_t	pos = _contentType.find("boundary=");
	if (pos == std::string::npos)
		return "";
	size_t end = _contentType.find(";", pos);
	if (end == std::string::npos)
		end = _contentType.length();
	size_t start = pos + 9;
	return _contentType.substr(start, end - start);
}

std::string ClientRequest::_trimLeft(std::string _contentType) {
	size_t i = 0;

	while (i < _contentType.length() && (_contentType[i] == ' ' || _contentType[i] == '\t'))
		i++;
	if (i > _contentType.length())
		return "";
	return _contentType.substr(i);
}


void ClientRequest::_parseContentType(std::string const& request) {
	size_t	ctPos = request.find("Content-Type:");
	if (ctPos == std::string::npos)
		return;
	size_t	end = request.find("\r\n", ctPos);
	if (end == std::string::npos)
		return;
	size_t	start = ctPos + 13;
	_contentType = _trimLeft(request.substr(start, end - start));

	//detectar si hay multipart
	if (_contentType.find("multipart/form-data") != std::string::npos) {
		_isMultipart = true;
		_boundary = _extractBoundary(_contentType);

		if(_boundary.empty()) {
			_isMultipart = false;
			return;
		}

		if (!_data.empty() && !_parseMultipartBody(_data))
			_isMultipart = false;
	}
}

