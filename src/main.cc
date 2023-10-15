#include "gateway.h"
#include "configuration.h"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

int main(void)
{
    Configuration::getInstance()->init("../conf/server.conf");
    CloudiskServer server;
    server.loadMudules();
    server.start(8080);
    return 0;
}

