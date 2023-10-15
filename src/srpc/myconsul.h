#ifndef __HUAQUQ_MYCONSUL_H__
#define __HUAQUQ_MYCONSUL_H__
#include <string>
#include <ppconsul/agent.h>
#include <workflow/WFFacilities.h>

using std::string;
using namespace ppconsul;
class MyConsul
{
public:
    MyConsul(const string& host, const string& dc, const string& name,
             const string& address, const string& id, unsigned short port);
    void registerService();
    void deregisterService();
private:
    Consul _consul;
    agent::Agent _agent;
    WFFacilities::WaitGroup _waitGroup;
    string _name;
    string _address;
    string _id;
    unsigned short _port;
};
#endif

