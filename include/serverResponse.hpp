#include <string>
#include <unistd.h>
#include "../include/clientRequest.hpp"

class ServerResponse
{
  private:
    std::string _response;

    std::string _getContentType(std::string path);
    std::string _buildOkResponse(std::string &buffer, std::string path);
    std::string _buildCgiResponse(std::string &buffer);
    std::string _makeFileBuffer(std::string path);
    std::string _getExtension(std::string htmlPath);

  public:
    ServerResponse();
    ServerResponse(ClientRequest& clientRequest, const Config& config, bool isUpload);
    ~ServerResponse();

    std::string getResponse(void);
    static std::string buildNotFoundResponse(void);

};
