#include <string>
#include <cstring>
#include <iostream>
#include <wait.h>
#include <vector>
#include <sstream>
#include "clientRequest.hpp"
#include "Config.hpp"

class Cgi
{
    private:
        std::string _executable;
        char** _populateEnv(ClientRequest const& clientRequest, Config const& config, std::string petition, std::string htmlPath);

    public:
        Cgi(ClientRequest clientRequest);
        Cgi();
        ~Cgi();
};
