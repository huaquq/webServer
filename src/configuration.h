#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <map>
#include <string>
#include <stdlib.h>

using std::string;
using std::map;

//配置文件读取,单例模式,自动释放
class Configuration
{
public:
    static Configuration* getInstance();

    static void init_r();

    //配置文件读取
    void init(const string& configFilePath);

    //获取配置文件内容
    map<string, string>& getConfigMap();
private:
    Configuration();

    ~Configuration();

    static void destroy();
private:
    string _configFilePath;//配置文件路径
    map<string, string> _configs;//配置文件内容
    static Configuration* _pInstance;
    //确保线程安全
    static pthread_once_t _once;
};

#define getConf(str)(Configuration::getInstance()->getConfigMap()[(str)])
#endif

