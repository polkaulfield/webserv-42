#include "../include/uploadedFile.hpp"
#include "../include/serverResponse.hpp"
#include <iostream>

std::string	ServerResponse::_buildSuccessResponse() {
	std::string body = "<h1>Files uploaded succesfully!</h1>";
	std::string response = "HTTP/1.1 201 Created\r\n";

	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + intToString(body.length()) + "\r\n";
	response += "\r\n";
	response += body;
	return response;
}

std::string	ServerResponse::_buildErrorResponse(int code, std::string const& message) {
	std::string response = "HTTP/1.1 " + intToString(code) + " Error\r\n";

	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + intToString(message.length()) + "\r\n";
	response += "\r\n";
	response += message;
	return response;
}

bool	ServerResponse::_isAllowedType(std::string const& filename, std::string const& contentType) {
	const char *badTypes[] = {".exe", ".bat", ".cmd", ".com", ".pif", ".scr", ".php", ".jsp", ".asp", NULL};

	for (int i = 0; badTypes[i] ; i++) {
		if (filename.find(badTypes[i]) != std::string::npos)
			return false;
	}

	if (contentType.find("application/x-executable") != std::string::npos ||
		contentType.find("application/x-msdownload") != std::string::npos)
		return false;
	return true;
}

bool	ServerResponse::_validateUploadedFile(UploadedFile const& file, Config config) {
	//Verificar tamaño frente client_max_body_size
	size_t	maxSize = config.getClientMaxBodySize();
	if (file._size > maxSize)
		return false;

	//Aqui se podria intentar sanitizar el nombre del archivo pero parece pateo y dice el bueno de Javier
	//que no lo pone el subject, pero bueno que poder se puede hacer

	if (!_isAllowedType(file._filename, file._contentType))
		return false;

	return true;
}

bool	ServerResponse::_saveUploadedFile(UploadedFile const& file, std::string const& uploadDir) {
	std::string fullPath = uploadDir;

	if (!fullPath.empty() && fullPath[fullPath.length() - 1] != '/')
		fullPath += "/";
	fullPath += file._filename;

	//crear el archivo en modo binario
	std::ofstream outFile(fullPath.c_str(), std::ios::binary);
	if(!outFile.is_open())
		return false;

	//escribir en el outfile
	if (!file._content.empty())
		outFile.write(&file._content[0], file._size);

	outFile.close();

	return outFile.good();
}

void	ServerResponse::_handleFileUpload(ClientRequest const& clientRequest, Config config) {
	std::vector<UploadedFile> const& files = clientRequest.getUploadedFiles();
	//std::string	const& uploadDir = config.getUploadDir(); // aun no esta implementado
	std::string const& uploadDir = "./uploads";

	//SI ESTAS VERIFIACIONES SE COMPRUEBAN EN CONFIG HABRA QUE QUITARLAS DE AQUI ENTIENDO SUPONGO
	//verificar si el directorio existe
	if (uploadDir.empty()) {
		_response = _buildErrorResponse(500, "Upload directory not configured");
		return;
	}
	//verificar si tiene permisos de escritura
	if (access(uploadDir.c_str(), W_OK) != 0) {
		_response = _buildErrorResponse(500, "Upload directory not writable");
		return;
	}
	std::cout << "DEBUG: Procesando " << files.size() << " archivos" << std::endl;
	for (size_t i = 0; i < files.size(); i++) {
		UploadedFile const& file = files[i];

		//si filename esta vacio pasamos al siguiente
		if (file._filename.empty())
			continue;

		//validaciones de seguridad
		if (!_validateUploadedFile(file, config)) {
			_response = _buildErrorResponse(400, "Invalid file: " + file._filename);
			return;
		}

		if(!_saveUploadedFile(file, uploadDir)) {
			_response = _buildErrorResponse(500, "Failed to save file: " + file._filename);
			return;
		}
	}
	_response = _buildSuccessResponse();
}
