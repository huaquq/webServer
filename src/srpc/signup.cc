#include "myconsul.h"
#include "../configuration.h"
#include "../mylogger.h"
#include "user.srpc.h"
#include <iostream>
#include <crypt.h>
#include <vector>
#include <map>
#include "workflow/WFFacilities.h"
#include <workflow/MySQLResult.h>
#include <ppconsul/ppconsul.h>

using std::map;
using std::vector;
using std::cout;
using std::endl;
using namespace srpc;

bool mysqlCheck(WFMySQLTask* mysqlTask)
{
    int state = mysqlTask->get_state();
    int error = mysqlTask->get_error();

    if(state != WFT_STATE_SUCCESS)
    {
        LogError("occurs error: %s\n", WFGlobal::get_error_string(state, error));
        return false;
    }
    auto resp = mysqlTask->get_resp();
    if(resp->get_packet_type() == MYSQL_PACKET_HEADER_ERROR)
    {
        LogError("error %d, %s\n", resp->get_error_code(), resp->get_error_msg().c_str());
        return false;
    }
    return true;
}
class SignupServiceServiceImpl : public SignupService::Service
{
public:

	void Signup(ReqSignup *request, RespSignup *response, srpc::RPCContext *ctx) override
    {
        string username = request->username();
        string password = request->password();
        //对用户名加密
        string encoded(crypt(password.c_str(), getConf("salt").c_str()));
        //将用户信息写到数据库中
        string query= "insert into cloudisk.tbl_user (user_name, user_pwd) Values('"
                    + username + "', '" + encoded + "');";
        LogInfo(query.c_str());
        string url = getConf("mysqlurl");
        auto mysqlTask = WFTaskFactory::create_mysql_task(url, 1,
        [response](WFMySQLTask* mysqlTask)
        {
            if(!mysqlCheck(mysqlTask))
            {
                LogInfo("mysqlCheck error");
                response->set_code(-1);
                response->set_msg("failed");
                return;
            }
            protocol::MySQLResultCursor cursor(mysqlTask->get_resp());
            if(cursor.get_cursor_status() == MYSQL_STATUS_OK &&
               cursor.get_affected_rows() == 1)
            {
                response->set_code(0);
                response->set_msg("SUCCESS");
            }
            else
            {
                LogInfo("insert tbl_user error");
                response->set_code(-1);
                response->set_msg("failed");
            }
        }
                                                          );
        mysqlTask->get_req()->set_query(query);
        ctx->get_series()->push_back(mysqlTask);
    }

};
int main(void)
{
    Configuration::getInstance()->init("../../conf/server.conf");
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    unsigned short port = 2222;
    SRPCServer server;
    SignupServiceServiceImpl impl;
    //用户服务注册
    server.add_service(&impl);

    server.start(port);
    MyConsul consul(getConf("consulhost"), "dc1", "SignupService1", "127.0.0.1", 
                    "SignupService1", port);
    consul.registerService();
    server.stop();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}

