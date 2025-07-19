#include <string>
#include <unistd.h>
#include <cstdio>
#include "../include/clientRequest.hpp"

struct	UploadedFile;

class ServerResponse
{
	private:
		std::string _response;

    std::string _getContentType(std::string path);
    std::string _buildOkResponse(std::string &buffer, std::string path);
    std::string _buildDirResponse(std::string &buffer);
    std::string _buildCgiResponse(std::string &buffer);
    std::string _buildRedirResponse(const std::string &url);
    std::string _makeFileBuffer(std::string path);
    std::string _getExtension(std::string htmlPath);

  public:
    ServerResponse();
    ServerResponse(ClientRequest& clientRequest, const Config& config, bool isUpload);
    ~ServerResponse();

		std::string getResponse(void);
		static std::string buildNotFoundResponse(void);

//------------------------------------------------
//UPLOAD FILES

	void	_handleFileUpload(ClientRequest const& clientRequest, Config config);
	bool	_validateUploadedFile(UploadedFile const& file, Config config);
	bool	_isAllowedType(std::string const& filename, std::string const& contentType);
	bool	_saveUploadedFile(UploadedFile const& file, std::string const& uploadDir);
	std::string _buildUploadSuccessResponse();

//DELETE METHODS

	void	_handleDeleteRequest(ClientRequest const& request, Config const& config);
	bool	_isDeleteAllowed(std::string const& method, std::string const& path, Config config);
	bool	_deleteFiles(std::string const& path);
	std::string _buildSuccessDeleteResponse();

	std::string buildErrorResponse(int code, std::string const& message);
};
