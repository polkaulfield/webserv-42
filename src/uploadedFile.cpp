/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFile.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:45 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/02 22:12:04 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uploadedFile.hpp"

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
			file.fieldName = headers.substr(nameStart, nameEnd - nameStart);
	}

	size_t filenameStart = headers.find("filename=\"");
	if (filenameStart != std::string::npos) {
		filenameStart += 10;
		size_t filenameEnd = headers.find("\"", filenameStart);
		if (filenameEnd != std::string::npos)
			file.filename = headers.substr(filenameStart, filenameEnd - filenameStart);
	}

	size_t cTypeStart = headers.find("Content-Type:");
	if (cTypeStart != std::string::npos) {
		cTypeStart += 13;
		size_t cTypeEnd = headers.find("\r\n", cTypeStart);
		if (cTypeEnd == std::string::npos)
			cTypeEnd = headers.length();
		file.contentType = headers.substr(cTypeStart, cTypeEnd - cTypeStart);
		file.contentType = _trimLeft(file.contentType);
	}

	file.content.clear();
	//content se extrae asi por si hay datos binarios y no se corte con algun \0
	//los archivos binarios puedes contener muchos bytes 0x00 lo que cortaria la extraccion de su contenido
	for (size_t i = 0; i < content.length(); i++)
		file.content.push_back(content[i]);
	file.size = content.length();

	_uploadedFiles.push_back(file);
}

bool ClientRequest::_parseMultipartBody(std::string const& _data) {
	if (_data.empty())
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

std::string	ClientRequest::_extractBoundary(std::string const& contentType) {
	size_t	pos = contentType.find("boundary=");
	if (pos == std::string::npos)
		return "";
	size_t end = contentType.find(";", pos);
	if (end == std::string::npos)
		end = contentType.length();
	size_t start = pos + 9;
	return contentType.substr(start, end - start);
}

std::string ClientRequest::_trimLeft(std::string contentType) {
	size_t i = 0;

	while (i < contentType.length() && (contentType[i] == ' ' || contentType[i] == '\t'))
		i++;
	if (i > contentType.length())
		return "";
	return contentType.substr(i);
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
		//si hay boundary y datos, parsear el bofy multipart
		if (!_boundary.empty() && !_data.empty())
			_parseMultipartBody(_data);
	}
}
