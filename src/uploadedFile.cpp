/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFile.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:45 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/02 21:26:25 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uploadedFile.hpp"

void ClientRequest::_parseMultipartPart(std::string const& part) {

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

	//a partir de aqui hay que extraer

}
