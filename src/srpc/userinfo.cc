#include "myconsul.h"
#include "../configuration.h"
#include "../mylogger.h"
#include "../token.h"
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
class UserinfoServiceServiceImpl : public UserinfoService::Service
{
public:

	void Userinfo(ReqUserinfo *request, RespUserinfo *response, srpc::RPCContext *ctx) override
    {
        string username = request->username();
        string reqToken = request->token();
        Token token(getConf("token"));
        if(token.getToken() != reqToken)
        {
            response->set_code(-1);
            response->set_msg("token error");
            LogDebug("token error");
            return;
        }
        //登录验证
        //访问MySQL获取加密密码
        string url = getConf("mysqlurl");
        auto mysqlTask = WFTaskFactory::create_mysql_task(url, 1,
        [reqToken, username, url, response](WFMySQLTask* mysqltask)
        {
                
            if(!mysqlCheck(mysqltask))
            {
                LogInfo("mysqltask error");
                response->set_code(-1);
                response->set_msg("failed");
                return;
            }
           //获取密码
            vector<vector<protocol::MySQLCell>> rows;
            protocol::MySQLResultCursor cursor(mysqltask->get_resp());           
            cursor.fetch_all(rows);
            if(rows[0][0].is_datetime())
            {

                response->set_code(0);
                response->set_msg("success");
                response->set_username(username);
                response->set_signupat(rows[0][0].as_datetime());
            }
            else
            {
                 LogInfo("query error");
                 response->set_code(-1);
                 response->set_msg("error");
            }
            
        });
        string query = "select signup_at from cloudisk.tbl_user where user_name = '"
                     + username + "' limit 1;";
        mysqlTask->get_req()->set_query(query);
        ctx->get_series()->push_back(mysqlTask);
    }
};
            
int main(void)
{
    Configuration::getInstance()->init("../../conf/server.conf");
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    unsigned short port = 1414;
    SRPCServer server;
    UserinfoServiceServiceImpl impl;
    //用户服务注册
    server.add_service(&impl);
    server.start(port);
    MyConsul consul(getConf("consulhost"), "dc1", "UserinfoService1", "127.0.0.1", 
                    "UserinfoService1", port);
    consul.registerService();
    server.stop();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
