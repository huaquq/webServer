#include "amqp.h"
#include <string>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

using std::string;
using namespace AmqpClient;

Publisher::Publisher()
    : _channel(Channel::Create())
{

}

void Publisher::doPublish(const string& msg)
{
    BasicMessage::ptr_t message = BasicMessage::Create(msg);
    _channel->BasicPublish(_info.exchangor, _info.routingKey, message);
}

Consumer::Consumer()
    : _channel(Channel::Create())
{
    _channel->BasicConsume(_info.amqpQueue);
}

bool Consumer::doConsumer(string& msg)
{
    Envelope::ptr_t envelope;
    bool ret = _channel->BasicConsumeMessage(envelope, 3000);
    if(ret)
    {
        msg = envelope->Message()->Body();
        return true;
    }
    else
        return false;
}

