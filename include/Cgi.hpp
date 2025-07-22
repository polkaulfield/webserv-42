#ifndef CGI_HPP
#define CGI_HPP
#include "../include/Config.hpp"
#include "ClientRequest.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <wait.h>

class Cgi {
private:
  std::string _executable;
  char **_populateEnv(const ClientRequest &clientRequest, const Config &config);

public:
  Cgi(const ClientRequest &clientRequest);
  Cgi();
  ~Cgi();
  std::string execScript(const ClientRequest &clientRequest,
                         const Config &config);
  std::string getCGIOutput(char **args, char **env, std::string const &postData,
                           std::string const &path);
};
bool isCGI(std::string const &htmlPath, const Config &config);

#endif
