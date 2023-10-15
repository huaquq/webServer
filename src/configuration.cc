#include "configuration.h"
#include <fstream>
#include <pthread.h>

using std::ifstream;

Configuration* Configuration::_pInstance = nullptr;

pthread_once_t Configuration::_once = PTHREAD_ONCE_INIT;

Configuration* Configuration::getInstance()
{
    pthread_once(&_once, &init_r);
    return _pInstance;
}


void Configuration::init_r()
{
    _pInstance = new Configuration();
    atexit(destroy);
}

void Configuration::init(const string& configFilePath)
{
    _configFilePath = configFilePath;
    ifstream ifs(_configFilePath);
    string key, value;

    while(ifs >> key && getline(ifs, value))
        _configs[key] = value.substr(1);
}

map<string, string>& Configuration::getConfigMap()
{
    return _configs;
}

Configuration::Configuration()
{

}

Configuration::~Configuration()
{

}

void Configuration::destroy()
{
    if(_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}
