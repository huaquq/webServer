
#include "user.srpc.h"
#include "workflow/WFFacilities.h"

using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

static void signup_done(RespSignup *response, srpc::RPCContext *context)
{
}

static void signin_done(RespSignin *response, srpc::RPCContext *context)
{
}

static void userinfo_done(RespUserinfo *response, srpc::RPCContext *context)
{
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	const char *ip = "127.0.0.1";
	unsigned short port = 1412;

	SignupService::SRPCClient client(ip, port);

	// example for RPC method call
	ReqSignup signup_req;
	//signup_req.set_message("Hello, srpc!");
	client.Signup(&signup_req, signup_done);

	SigninService::SRPCClient client1(ip, port);

	// example for RPC method call
	ReqSignin signin_req;
	//signin_req.set_message("Hello, srpc!");
	client1.Signin(&signin_req, signin_done);

	UserinfoService::SRPCClient client2(ip, port);

	// example for RPC method call
	ReqUserinfo userinfo_req;
	//userinfo_req.set_message("Hello, srpc!");
	client2.Userinfo(&userinfo_req, userinfo_done);

	wait_group.wait();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
