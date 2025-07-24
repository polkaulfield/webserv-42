#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP
#include "../include/ClientRequest.hpp"
#include <cstdio>
#include <string>
#include <unistd.h>

struct UploadedFile;

class ServerResponse {
private:
  std::string _response;
  std::string _sessionID;

  std::string _getContentType(std::string const &path);
  std::string _buildOkResponse(std::string &buffer, std::string const &path);
  std::string _buildDirResponse(std::string &buffer);
  std::string _buildCgiResponse(std::string &buffer);
  std::string _buildRedirResponse(const std::string &url);
  std::string _makeFileBuffer(std::string const &path);
  std::string _getExtension(std::string const &htmlPath);

public:
  ServerResponse();
  ServerResponse(ClientRequest &clientRequest, const Config &config,
                 bool isUpload);
  ~ServerResponse();

  std::string getResponse(void);
  void  setSessionID(std::string const& sessionID);
  static std::string buildErrorResponse(int err, const std::string &msg,
                                        const Config &config);

  void _handleFileUpload(ClientRequest const &clientRequest,
                         const Config &config);
  bool _validateUploadedFile(UploadedFile const &file, const Config &config);
  bool _isAllowedType(std::string const &filename,
                      std::string const &contentType);
  bool _saveUploadedFile(UploadedFile const &file,
                         std::string const &uploadDir);
  std::string _buildUploadSuccessResponse();

  void _handleDeleteRequest(ClientRequest const &request, Config const &config);
  bool _containsTraversalPath(std::string const &rawPath);
  bool _deleteFiles(std::string const &path);
  std::string _buildSuccessDeleteResponse();

  bool _isPathOutsideServerScope(const std::string &queryPath);
};

#endif
