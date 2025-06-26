#include <string>
#include "clientRequest.hpp"

class Cgi
{
    private:
        std::string _executable;
        void _populateEnv(char** env, ClientRequest clientRequest);

    public:
        Cgi(ClientRequest clientRequest);
        Cgi();
        ~Cgi();
};
