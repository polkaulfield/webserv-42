#include <string>
#include <unistd.h>

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

    std::string _getBody(std::string request);
  public:
    ClientRequest();
    ClientRequest(char *request);
    //ClientRequest(const ClientRequest& clientRequest);
    ~ClientRequest();
    //ClientRequest &operator=(const ClientRequest& clientRequest);
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
    std::string getData();

};
