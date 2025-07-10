#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include <string>
# include <unistd.h>
# include "config.hpp"

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

    std::string _getBody(std::string request);
  public:
    ClientRequest();
    ClientRequest(char *request, const Config& config);
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
    std::string getHttpVer() const;
    std::string getReturnCode() const;
    std::string getContentType() const;
    std::string getData() const;
    std::string getQuery() const;
    std::string getUserAgent() const;
    std::string getAccept() const;
    std::string getAcceptLanguage() const;
    std::string getConnection() const;
    bool        getIsFileUpload(const bool val);

};
#endif
