
#include "user.srpc.h"
#include "workflow/WFFacilities.h"

using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

class SignupServiceServiceImpl : public SignupService::Service
{
public:

	void Signup(ReqSignup *request, RespSignup *response, srpc::RPCContext *ctx) override
	{}
};
class SigninServiceServiceImpl : public SigninService::Service
{
public:

	void Signin(ReqSignin *request, RespSignin *response, srpc::RPCContext *ctx) override
	{}
};
class UserinfoServiceServiceImpl : public UserinfoService::Service
{
public:

	void Userinfo(ReqUserinfo *request, RespUserinfo *response, srpc::RPCContext *ctx) override
	{}
};

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	unsigned short port = 1412;
	SRPCServer server;

	SignupServiceServiceImpl signupservice_impl;
	server.add_service(&signupservice_impl);

	SigninServiceServiceImpl signinservice_impl;
	server.add_service(&signinservice_impl);

	UserinfoServiceServiceImpl userinfoservice_impl;
	server.add_service(&userinfoservice_impl);

	server.start(port);
	wait_group.wait();
	server.stop();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
