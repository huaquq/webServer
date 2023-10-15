#include "myconsul.h"
#include <workflow/WFFacilities.h>
#include <functional>
void timerTaskCallback(const string& id,WFTimerTask* timerTask)
{
    using namespace ppconsul::agent;
    Agent* pagent = (Agent*)timerTask->user_data;
    pagent->servicePass(id);

    auto nextTask = WFTaskFactory::create_timer_task(5 * 1000 * 1000, std::bind(timerTaskCallback, id, std::placeholders::_1));
    nextTask->user_data = pagent;
    series_of(timerTask)->push_back(nextTask);
}

MyConsul::MyConsul(const string& host, const string& dc, const string& name,
         const string& address, const string& id, unsigned short port)
    : _consul(host, ppconsul::kw::dc = dc),
    _agent(_consul),
    _waitGroup(1),
    _name(name),
    _address(address),
    _id(id),
    _port(port)
{
}
void MyConsul::registerService()
{
    _agent.registerService(
        agent::kw::name = _name,
        agent::kw::address = _address,
        agent::kw::id = _id,
        agent::kw::port = _port,
        agent::kw::check = agent::TtlCheck(std::chrono::seconds(10))
                          );
    auto timerTask = WFTaskFactory::create_timer_task(5 * 1000 * 1000, 
                                                      std::bind(timerTaskCallback, _id, std::placeholders::_1));
    timerTask->user_data = &_agent;
    timerTask->start();
    _waitGroup.wait();
}
void MyConsul::deregisterService()
{
    _agent.deregisterCheck(_id);
}
