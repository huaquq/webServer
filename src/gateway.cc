#include "gateway.h"
#include "token.h"
#include "hash.h"
#include "amqp.h"
#include "mylogger.h"
#include "configuration.h"
#include "srpc/user.pb.h"
#include "srpc/user.srpc.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <crypt.h>
#include <iostream>
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/HttpServer.h>
#include <nlohmann/json.hpp>


using nJson =nlohmann::json;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace wfrest;

CloudiskServer::CloudiskServer()
    : _waitGroup(1),
    _server(),
    _confs(Configuration::getInstance()->getConfigMap())
{

}

void CloudiskServer::start(unsigned int port)
{
    if(_server.track().start(port) == 0)
    {
        _server.list_routes();
        _waitGroup.wait();
        _server.stop();
    }
    else
        cout << "CloudiskServer start failed" << endl;
}

void CloudiskServer::loadMudules()
{
    loadStaticResources();
    loadSignupModule();
    loadSigninModule();
    loadUserInfoModule();
    loadUserFileListModule();
    loadFileUploadMOdule();
    loadFileDownloadModule();
}

void CloudiskServer::loadStaticResources()
{
    _server.GET("/static/img/avatar.jpeg", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/img/avatar.jpeg"); 
                });

    _server.GET("/static/js/auth.js", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/js/auth.js"); 
                });

    _server.GET("/static/view/home.html", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/view/home.html"); 
                });

    _server.GET("/file/upload", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/view/upload.html"); 
                });

    _server.GET("/static/view/signin.html", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/view/signin.html"); 
                });

    _server.GET("/static/view/signup.html", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/view/signup.html"); 
                });

    _server.GET("/file/upload/success", [](const HttpReq* req, HttpResp* resp)
                {
                   resp->File("../static/view/uploadSuccess.html"); 
                });
}


void CloudiskServer::loadSignupModule()
{
    _server.POST("/user/signup", 
    [this](const HttpReq* req, HttpResp* resp, SeriesWork* series)
    {
        if(req->content_type() == APPLICATION_URLENCODED)
            {
                //解析请求获取用户名和密码
                auto formKV = req->form_kv();
                string username = formKV["username"];
                string password = formKV["password"];
                //从注册中心获取ip和port
                string consulUrl = this->_confs["consulurl"];
                auto consulTask = WFTaskFactory::create_http_task(consulUrl, 0, 0,
                [username, password, resp, series, this](WFHttpTask* httpTask)
                {
                    string ip;
                    unsigned short port;
                    this->getMethodHtt(httpTask, "SignupService1", ip, port);
                    SignupService::SRPCClient client(ip.c_str(), port);
                    ReqSignup signup_req;
                    signup_req.set_username(username);
                    signup_req.set_password(password);
                    //创建rpctask
                    auto rpcTask =client.create_Signup_task([resp](RespSignup* response, srpc::RPCContext* content)
                    {
                        if(content->success() && response->code() == 0)
                        {
                            resp->String("SUCCESS");
                            LogInfo("Signup success!");
                        }
                        else
                        {
                            resp->String("Signup Failed");
                            LogInfo("Signup Failed code : %d, msg: %s\n", response->code(), response->msg().c_str());
                        }
                    });
                    rpcTask->serialize_input(&signup_req);
                    series->push_back(rpcTask);
                }
                                                                  );
                series->push_back(consulTask);
            }
        else
        {
            resp->String("Signup Failded");
            LogInfo("signup content_type error");
        }
    });

}


void CloudiskServer::loadSigninModule()
{
    _server.POST("/user/signin",
    [this](const HttpReq* req, HttpResp* resp, SeriesWork* series)
    {
        if(req->content_type() == APPLICATION_URLENCODED)
        {
            //解析请求获取用户名和密码
            auto formKV = req->form_kv();
            string username = formKV["username"];
            string password = formKV["password"];
            //从注册中心获取ip和port
            string consulUrl = this->_confs["consulurl"];
            auto consulTask = WFTaskFactory::create_http_task(consulUrl, 0, 0,
            [username, password, resp, series, this](WFHttpTask* httpTask)
            {
                string ip;
                unsigned short port;
                this->getMethodHtt(httpTask, "SigninService1", ip, port);
                SigninService::SRPCClient client(ip.c_str(), port);
                ReqSignin signin_req;
                signin_req.set_username(username);
                signin_req.set_password(password);
                //创建rpctask
                auto rpcTask = client.create_Signin_task([resp](RespSignin* response, srpc::RPCContext* content)
                {
                    if(content->success() && response->code() == 0)
                    {
                       nJson respMsg;
                       nJson data;
                       data["Token"] = response->token();
                       data["Username"] = response->username();
                       data["Location"] = response->location();
                       respMsg["data"] = data;
                       LogDebug(respMsg.dump().c_str());
                       LogInfo("%s signin success\n", response->username());
                       resp->String(respMsg.dump());
                    }
                    else
                        resp->String("signin Failed");
                });
                rpcTask->serialize_input(&signin_req);
                series->push_back(rpcTask);
            }
                                                              );
            series->push_back(consulTask);
            }
        else
            resp->String("error");
    });
}

void CloudiskServer::loadUserInfoModule()
{
    _server.GET("/user/info", [this](const HttpReq* req, HttpResp* resp, SeriesWork* series)
    {
        //解析请求获取用户名和Token
        string username = req->query("username");
        string reqToken = req->query("token");
        LogDebug("username: %s, token: %s\n", username.c_str(), reqToken.c_str());
        
        //从注册中心获取ip和port
        string consulUrl = this->_confs["consulurl"];
        auto consulTask = WFTaskFactory::create_http_task(consulUrl, 0, 0,
        [username, reqToken, resp, series, this](WFHttpTask* httpTask)
        {
            string ip;
            unsigned short port;
            this->getMethodHtt(httpTask, "UserinfoService1", ip, port);
            UserinfoService::SRPCClient client(ip.c_str(), port);
            ReqUserinfo userinfo_req;
            userinfo_req.set_username(username);
            userinfo_req.set_token(reqToken);
            //创建rpctask
            auto rpcTask = client.create_Userinfo_task([resp](RespUserinfo* response, srpc::RPCContext* content)
            {
                if(content->success() && response->code() == 0)
                {
                   nJson respMsg;
                   nJson data;
                   data["Username"] = response->username();
                   data["SignupAt"] = response->signupat();
                   respMsg["data"] = data;
                   LogDebug(respMsg.dump().c_str());
                   LogInfo("%s get userinfo success\n", response->username());
                   resp->String(respMsg.dump());
                }
                else
                    resp->String("signin Failed");
            });
            rpcTask->serialize_input(&userinfo_req);
            series->push_back(rpcTask);
        }
                                                          );
        series->push_back(consulTask);
    });
}


void CloudiskServer::loadUserFileListModule()
{
    _server.POST("/file/query", [](const HttpReq * req, HttpResp * resp){
        //1. 解析请求
        string username = req->query("username");
        string token = req->query("token");
        auto & formKV = req->form_kv();
        string limitcnt = formKV["limit"];
        /* cout << "username:" << username << endl; */
        /* cout << "token:" << token << endl; */
        /* cout << "limitcnt:" << limitcnt << endl; */
        //2. 校验token
        //校验token
        Token mytoken("1234");
        if(mytoken.getToken() != token)
            resp->String("Token error");
        //3. 查询数据库
        string sql("select file_sha1, file_name, file_size, upload_at, last_update ");
        sql += " from cloudisk.tbl_user_file where user_name = '" + username;
        sql += "' limit " + limitcnt + ";";
        /* cout << "sql:\n" << sql << endl; */
        string url("mysql://root:1234@localhost");
        using namespace protocol;
        resp->MySQL(url, sql, [resp, username](MySQLResultCursor * pcursor){
            using std::vector;
            vector<vector<MySQLCell>> rows;
            pcursor->fetch_all(rows);
            if(rows.size() == 0) return;
            nJson arrMsg;
            for(size_t i = 0; i < rows.size(); ++i)  {
                nJson row;
                row["FileHash"] = rows[i][0].as_string();
                row["FileName"] = rows[i][1].as_string();
                row["FileSize"] = rows[i][2].as_ulonglong();
                row["UploadAt"] = rows[i][3].as_datetime();
                row["LastUpdated"] = rows[i][4].as_datetime();
                arrMsg.push_back(row);//将arrMsg当成数组来使用
            }
            resp->String(arrMsg.dump());
        });
    });
}


void CloudiskServer::loadFileUploadMOdule()
{
    _server.POST("/file/upload", [](const HttpReq* req, HttpResp* resp, SeriesWork* series)
    {
        if(req->content_type() == MULTIPART_FORM_DATA)
        {
            //解析请求
            auto& form = req->form();
            string username = req->query("username");
            string& filename = form["file"].first;
            string& content = form["file"].second;

            /* cout << "filename: " << filename << endl; */
            /* cout << "content: " << content << endl; */

            //将文件写到本地
            mkdir("tmp", 0755);
            string filepath = "tmp/" + filename;
            int fd = open(filepath.c_str(), O_CREAT | O_RDWR, 0644);
            int ret = write(fd, content.c_str(), content.size());
            if(ret < 1)
            {
                resp->String("error");
                perror("open");
                close(fd);
                return;
            }
            close(fd);
            
            //将备份的文件信息放到消息队列
            nJson uploaderInfo;
            uploaderInfo["filePath"] = filepath;
            string objectName = "oss/" + filename;
            uploaderInfo["objectName"] = objectName;
            /* cout << "uploaderInfo: " << uploaderInfo.dump(); */
            Publisher publisher;
            publisher.doPublish(uploaderInfo.dump());

            //重定向到一个上传成功的页面
            resp->headers["Location"] = "/file/upload/success";
            resp->headers["Content-Type"] = "text/html";
            resp->set_status_code("301");
            resp->set_reason_phrase("Moved Permanently");

            //计算sha1
            Hash hash(filepath);
            string url = "mysql://root:1234@localhost";
            string query = "insert into cloudisk.tbl_user_file(user_name, file_sha1, file_size, file_name, status) values('";
            query += username + "','" + hash.sha1() + "'," + std::to_string(content.size()) + ",'" + filename + "', 0);";
            /* cout << "query: " << query << endl; */
            auto mysqlTask = WFTaskFactory::create_mysql_task(url, 1, nullptr);
            mysqlTask->get_req()->set_query(query);
            series->push_back(mysqlTask);
        }
        else
            resp->String("error");
    });
}

void CloudiskServer::loadFileDownloadModule()
{
    _server.GET("/file/downloadurl",[](const HttpReq* req, HttpResp* resp)
    {
        string filename = req->query("filename");
        /* cout << "filename: " << filename << endl; */
        string filepath = "tmp/" + filename;


        //生成下载链接但还要实现方法
        /* string downloadurl = "http://192.168.1.13:8080/" + filepath; */
        /* resp->String(downloadurl); */

        //直接发送
        int fd = open(filepath.c_str(), O_RDWR);
        if(fd < 0)
        {
            perror("open");
            resp->String("error");
            close(fd);
            return;
        }
        char buff[1024] = {0};
        int ret = read(fd, buff, sizeof(buff));
        close(fd);

        //设置下载模式
        resp->headers["Content-Type"] = "applocation/ocfet-stream";
        resp->headers["Content-Disposition"] = "attachment;filename=" + filename;
        resp->append_output_body(buff, ret);
    });
}
void CloudiskServer::getMethodHtt(WFHttpTask* httpTask, const string& id, string& ip, unsigned short& port)
{
    auto consulResp = httpTask->get_resp();
    const void* body;
    size_t sz = 0;
    consulResp->get_parsed_body(&body, &sz);
    nJson servicesInfo = nJson::parse((const char*)body);
    //发起rpc请求
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    ip = servicesInfo[id]["Address"];
    port = servicesInfo[id]["Port"];
}
