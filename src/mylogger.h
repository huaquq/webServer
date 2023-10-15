#ifndef __MYLOGGER_H__
#define __MYLOGGER_H__
#define MYDEBUG 1

#include <iostream>
#include <string>
#include <pthread.h>
#include <log4cpp/Category.hh>

using namespace log4cpp;
using std::string;

class MyLogger
{
public:
    static MyLogger* getInstance();

    static void init_r();

    template <typename ...Args>
        void info(const string &str,Args ...args)
        {
            _mycat.info(str.c_str(), args...);
        }

    template <typename ...Args>
        void error(const string &str,Args ...args)
        {
            _mycat.error(str.c_str(), args...);
        }

    template <typename ...Args>
        void debug(const string &str,Args ...args)
        {
            #if MYDEBUG
            _mycat.debug(str.c_str(), args...);
            #endif
        }

private:
    MyLogger();

    ~MyLogger();

    static void destory();
private:
    static MyLogger* _pInstance;

    static pthread_once_t _once;

    Category& _mycat;    
};

#define prefix2(msg) (string(" [") +\
                      string(__FILE__) + string(" ")\
                      + string(__func__) + string(" ")\
                      + string(std::to_string(__LINE__))\
                      + string("] ") + msg).c_str()

#define LogError(msg, ...) MyLogger::getInstance()->error(prefix2(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) MyLogger::getInstance()->debug(prefix2(msg), ##__VA_ARGS__)
#define LogInfo(msg, ...) MyLogger::getInstance()->info(prefix2(msg), ##__VA_ARGS__)
#endif

