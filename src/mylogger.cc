#include "mylogger.h"
#include "configuration.h"
#include <map>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>

using namespace log4cpp;
using std::map;
MyLogger* MyLogger::_pInstance = nullptr;

pthread_once_t MyLogger::_once = PTHREAD_ONCE_INIT;

MyLogger* MyLogger::getInstance()
{
    pthread_once(&_once, &init_r);
    return _pInstance;
}

void MyLogger::init_r()
{
   _pInstance = new MyLogger(); 
   atexit(destory);
}


MyLogger::MyLogger()
    : _mycat(Category::getRoot().getInstance("mycat"))
{
    map<string, string> confs = Configuration::getInstance()->getConfigMap();
    //日志布局
    auto ptn1 = new PatternLayout();
    ptn1->setConversionPattern(confs["PATTERN_LAYOUT"]);

    auto ptn2 = new PatternLayout();
    ptn2->setConversionPattern(confs["PATTERN_LAYOUT"]);

    //日志目的地
    auto pos = new OstreamAppender("console", &std::cout);
    pos->setLayout(ptn1);

    auto pfile = new FileAppender("console", confs["LOG_PATH"]);
    pfile->setLayout(ptn2);

    //设置优先级
    _mycat.setPriority(Priority::DEBUG);
    //设置目的地
    _mycat.addAppender(pos);
    _mycat.addAppender(pfile);
}

MyLogger::~MyLogger()
{
    Category::shutdown();
}

void MyLogger::destory()
{
    if(_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}
