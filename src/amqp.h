#ifndef __HUAQUQ_AMQP_H__
#define __HUAQUQ_AMQP_H__

#include <string>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

using std::string;
using namespace AmqpClient;

struct AmqpInfo
{
    string amqpUrl = "qmqp://guest:guest@localhost:5672";
    string exchangor = "uploadserver.trans";
    string amqpQueue = "ossqueue";
    string routingKey = "oss";
};

class Publisher
{
public:
    Publisher();

    void doPublish(const string& msg);
private:
    AmqpInfo _info;
    Channel::ptr_t _channel;
};

class Consumer
{
public:
    Consumer();

    bool doConsumer(string& msg);
private:
    AmqpInfo _info;
    Channel::ptr_t _channel;
};
#endif

