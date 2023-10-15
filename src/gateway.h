#ifndef __HUAQUQ_APIGATEWAY_H__
#define __HUAQUQ_APIGATEWAY_H__
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <crypt.h>
#include <iostream>
#include <map>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/HttpServer.h>
#include <nlohmann/json.hpp>


using nJson =nlohmann::json;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using namespace wfrest;

class CloudiskServer
{
public:
    CloudiskServer();

    void start(unsigned int port);

    void loadMudules();
private:
    void loadStaticResources();
    void loadSignupModule();
    void loadSigninModule();
    void loadUserInfoModule();
    void loadUserFileListModule();
    void loadFileUploadMOdule();
    void loadFileDownloadModule();
    void getMethodHtt(WFHttpTask* httpTask, const string& id, string& ip, unsigned short& port);
private:
    WFFacilities::WaitGroup _waitGroup;
    HttpServer _server;    
    map<string, string> _confs;
};

#endif

