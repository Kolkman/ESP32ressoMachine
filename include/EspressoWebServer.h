
#ifndef DEF_MYwebServer_H
#define DEF_MYwebServer_H



#include <ESPAsyncWebServer.h>


#define CREDENTIAL_LENGTH 64

class EspressoWebServer : public AsyncWebServer
{
public:
    EspressoWebServer(uint16_t,const char *, const char *);
    bool authRequired; 

    void setPassword(const char *);
    void setUsername(const char *);
    
    void  authenticate(AsyncWebServerRequest *);
    char *getUsername();
    char *getPassword();
private:
    char username[CREDENTIAL_LENGTH + 1];
    char password[CREDENTIAL_LENGTH + 1];
};

#endif