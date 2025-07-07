#include <string>
#include <unistd.h>
#include "../include/config.hpp"
#include "../include/clientRequest.hpp"

struct	UploadedFile;

class ServerResponse
{
	private:
		std::string _response;

		std::string _getContentType(std::string path);
		std::string _buildOkResponse(std::string &buffer, std::string path);
		std::string _makeFileBuffer(std::string path);
		std::string _getExtension(std::string htmlPath);

	public:
		ServerResponse();
		ServerResponse(ClientRequest& clientRequest, const Config& config);
		//ServerResponse(const ServerResponse& serverResponse);
		~ServerResponse();
		//ServerResponse &operator=(const ServerResponse& serverResponse);

		std::string getResponse(void);
		static std::string buildNotFoundResponse(void);

//------------------------------------------------
//UPLOAD FILES

	void	_handleFileUpload(ClientRequest const& clientRequest, Config config);
	bool	_validateUploadedFile(UploadedFile const& file, Config config);
	bool	_isAllowedType(std::string const& filename, std::string const& contentType);
	bool	_saveUploadedFile(UploadedFile const& file, std::string const& uploadDir);
	std::string _buildSuccessResponse();
	std::string _buildErrorResponse(int code, std::string const& message);

};
