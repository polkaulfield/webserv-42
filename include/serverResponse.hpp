#include <string>
#include <unistd.h>

class ServerResponse
{
  private:
    std::string _response;

    std::string _getContentType(std::string path);
    std::string _buildOkResponse(std::string &buffer, std::string path);
    std::string _buildNotFoundResponse(void);
    std::string _makeFileBuffer(std::string path);
    std::string _getExtension(std::string htmlPath);

  public:
    ServerResponse();
    ServerResponse(std::string method, std::string path);
    ServerResponse(const ServerResponse& serverResponse);
    ~ServerResponse();
    ServerResponse &operator=(const ServerResponse& serverResponse);

    std::string getResponse(void);

};
