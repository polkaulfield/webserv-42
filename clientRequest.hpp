#include <string>
#include "utils.hpp"
#include <unistd.h>

class ClientRequest
{
  private:
    std::string _method;
    std::string _path;
    std::string _httpVer;
    std::string _returnCode;
    std::string _contentType;
  public:
    ClientRequest();
    ClientRequest(char *request);
    ClientRequest(const ClientRequest& clientRequest);
    ~ClientRequest();
    ClientRequest &operator=(const ClientRequest& clientRequest);
    void setMethod(std::string method);
    void setPath(std::string path);
    void setHttpVer(std::string httpVer);
    void setReturnCode(std::string returnCode);
    void setContentType(std::string contentType);

    std::string getMethod();
    std::string getPath();
    std::string getHttpVer();
    std::string getReturnCode();
    std::string getContentType();

};
