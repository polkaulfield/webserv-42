#include <string>
#include <unistd.h>
#include "../include/config.hpp"
#include "../include/clientRequest.hpp"

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

};
