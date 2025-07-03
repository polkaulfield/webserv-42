#ifndef CLIENTREQUEST_HPP
#define CLIENTREQUEST_HPP

#include <unistd.h>
#include "uploadedFile.hpp"

class ClientRequest
{
	private:
		std::string _method;
		std::string _path;
		std::string _httpVer;
		std::string _queryString;
		std::string _returnCode;
		std::string _contentType;
		std::string _data;
		std::string _userAgent;
		std::string _accept;
		std::string _acceptLanguage;
		std::string _connection;

//----------------------------------------------------------
		//uploadFile
		std::string _boundary;
		std::vector<UploadedFile> _uploadedFiles;
		bool _isMultipart;

		void _parseContentType(std::string const& request);
		std::string _trimLeft(std::string contentType);
		std::string _extractBoundary(std::string const& contentType);
		bool _parseMultipartBody(std::string const& body);
		void _parseMultipartPart(std::string const& part);
//------------------------------------------------------------

		std::string _getBody(std::string request);


	public:
		ClientRequest();
		ClientRequest(char *request);
		//ClientRequest(const ClientRequest& clientRequest);
		~ClientRequest();
		//ClientRequest &operator=(const ClientRequest& clientRequest);
		void setMethod(const std::string& method);
		void setPath(const std::string& path);
		void setHttpVer(const std::string& httpVer);
		void setReturnCode(const std::string& returnCode);
		void setContentType(const std::string& contentType);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getHttpVer() const;
		std::string getReturnCode() const;
		std::string getContentType() const;
		std::string getData() const;
		std::string getQuery() const;
		std::string getUserAgent() const;
		std::string getAccept() const;
		std::string getAcceptLanguage() const;
		std::string getConnection() const;

//--------------------------------------------------------------------
		//uploadFile
		bool isMultipart() const;
		const std::vector<UploadedFile>& getUploadedFiles() const;
		const std::string& getBoundary() const;
//--------------------------------------------------------------------
};

#endif
