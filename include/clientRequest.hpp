#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include <string>
# include <unistd.h>
# include "config.hpp"
# include <map>

#include <unistd.h>
#include "uploadedFile.hpp"

class ClientRequest
{
	private:
		std::map<std::string, std::string> _headerMap;
		std::string _method;
		std::string _path;
		std::string _queryPath;
		std::string _httpVer;
		std::string _queryString;
		std::string _data;

		//------------------------------------
		//upload Shit
		bool _isMultipart;
		std::string _boundary;
		std::string _contentType;
		std::vector<UploadedFile> _uploadedFiles;
		//-----------------------------------

		std::map<std::string, std::string>  _createHeaderMap(std::string request);
		std::string _getBody(std::string request);
	public:
		ClientRequest();
		ClientRequest(std::string request, const Config& config);
		//ClientRequest(const ClientRequest& clientRequest);
		~ClientRequest();
		//ClientRequest &operator=(const ClientRequest& clientRequest);
		void setMethod(const std::string& method);
		void setPath(const std::string& path);
		void setHttpVer(const std::string& httpVer);
		void setReturnCode(const std::string& returnCode);
		void setContentType(const std::string& contentType);
		void setIsFileUpload(const bool val);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getQueryPath() const;
		std::string getHttpVer() const;
		std::map<std::string, std::string> getHeaderMap() const;
		std::string getHeaderValue(const std::string& key) const;
		std::string getReturnCode() const;
		std::string getContentType() const;
		std::string getData() const;
		std::string getQuery() const;
		std::string getUserAgent() const;
		std::string getAccept() const;
		std::string getAcceptLanguage() const;
		std::string getConnection() const;
		bool        getIsFileUpload(const bool val);

		bool isMultipart() const;
		const std::vector<UploadedFile>& getUploadedFiles() const;
		const std::string& getBoundary() const;

		std::string	_parseChunkedBody(std::string _data);
		void _parseContentType(std::string const& request);
		std::string _trimLeft(std::string contentType);
		std::string _extractBoundary(std::string const& contentType);
		bool _parseMultipartBody(std::string const& body);
		void _parseMultipartPart(std::string const& part);
};

#endif
