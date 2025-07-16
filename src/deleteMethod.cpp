#include "../include/serverResponse.hpp"
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>


std::string ServerResponse::_buildSuccessDeleteResponse() {
	// Crear el cuerpo de la respuesta
	std::string body = "File deleted successfully";

	// Construir la respuesta HTTP completa
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: " + intToString(body.length()) + "\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";  // Línea vacía que separa headers del body
	response += body;

	return response;
}

bool	ServerResponse::_deleteFiles(std::string const& path) {
	return (std::remove(path.data()) == 0) ? true : false;
}

bool	ServerResponse::_isDeleteAllowed(std::string const& method, std::string const& path, Config config) {
	std::list<Location>& location = config.getLocationList();

	Location*	bestMatch = NULL;
	size_t		bestMatchLenght = 0;

	for (std::list<Location>::iterator it = location.begin(); it != location.end(); it++) {
		std::string locationPath = config.getRoot() + it->getDirectory();
		std::cout << "Location " << locationPath << std::endl;
		if (path.find(locationPath) == 0) {
			if (locationPath.length() > bestMatchLenght){
				bestMatch = &(*it);
				bestMatchLenght = locationPath.length();
				std::cout << "Delete Location " << it->getDirectory() << std::endl;
				if (it->hasMethod(method))
					break ;
			}
		}
	}
	return (bestMatch && bestMatch->hasMethod(method)) ? true : false;
}

void	ServerResponse::_handleDeleteRequest(ClientRequest const& request, Config const& config) {
	std::string	requestPath = request.getPath();
	std::cout << GREEN << "erwsetsdlkgsd" << RESET << std::endl;
	if (!_isDeleteAllowed("DELETE", request.getPath(), config)) {
		_response = _buildErrorResponse(405, "Method Not Allowed");
		return;
	}
	std::cout << GREEN << "erwsetsdlkgsd" << RESET << std::endl;
	//nos saltamos el canonizar el path de momento si falla puede ser por eso
	std::string fullPath = requestPath;
	std::cout << fullPath << std::endl;
	if (access(fullPath.c_str(), F_OK)) {
		_response = _buildErrorResponse(404,"File not found");
		return ;
	}

	if (access(fullPath.c_str(), W_OK)) {
		_response = _buildErrorResponse(403, "Permission denied");
		return ;
	}

	//stat es una funcion que sirve para consultar metadatos de un archivo
	//en este caso nos interesa saber si es un archivo refular o un directorio
	//le pasamos la struct fileStat para que almacene ahi la informacion del archivo que se encuentra en la path correspondiente
	//luego consultamos si es un archivo regular, si no lo es construimos un mensaje de error
	struct stat fileStat;
	if (stat(fullPath.c_str(), &fileStat) == 0 && !S_ISREG(fileStat.st_mode)) {
		_response = _buildErrorResponse(403, "Cannot delete directories");
		return ;
	}

	if (_deleteFiles(fullPath))
		_response = _buildSuccessDeleteResponse();
	else
		_response = _buildErrorResponse(500, "Failed to delete file");
}
