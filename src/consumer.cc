#include "oss.h"
#include "amqp.h"
#include <iostream>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;
using std::string;
using Json = nlohmann::json;

void consume()
{
    Consumer consumer;
    OssUploader ossUploader;
    string msg;
    while(1)
    {
        bool hasMsg = consumer.doConsumer(msg);
        if(hasMsg)
        {
            Json uploaderInfo = Json::parse(msg);
            string filePath = uploaderInfo["filePath"];
            string objectName = uploaderInfo["objectName"];
            cout << "fileName: " << filePath << endl;
            cout << "objectName: " << objectName << endl;
            ossUploader.doUpload(filePath, objectName);
        }
        else
            cout << "get message timeout" << endl;
    }
}
int main(void)
{
    consume();
    return 0;
}

