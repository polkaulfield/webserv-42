#include <string>

class ServerResponse
{
  private:
    std::string _method;
    std::string _path;
    std::string _httpVer;
    std::string _returnCode;
    std::string _contentType;
  public:
    ServerResponse();
    ServerResponse(const ServerResponse& ServerResponse);
    ~ServerResponse();
    ServerResponse &operator=(const ServerResponse& ServerResponse);
    std::string getServerResponse(std::string data);
    void setMethod(std::string method);
    void setPath(std::string path);
    void setHttpVer(std::string httpVer);
    void setReturnCode(std::string returnCode);
    void setContentType(std::string contentType);

};
