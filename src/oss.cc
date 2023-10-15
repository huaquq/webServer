#include "oss.h"
#include <string>
#include <alibabacloud/oss/OssClient.h>

using std::string;


OssUploader::OssUploader(const OssInfo& info)
    : _info(info),
    _conf(),
    _ossClient(_info.Endpoint, _info.AccessKeyID, _info.AccessKeySecret, _conf)
{
    AlibabaCloud::OSS::InitializeSdk();
}

bool OssUploader::doUpload(const string& filename, const string& objectName)
{
    auto outcome = _ossClient.PutObject(_info.BucketName, objectName, filename);
    bool ret = outcome.isSuccess();
    if(!ret)
    {
        std::cout << "PutObject fail, code:" << outcome.error().Code() <<
            ",message:" << outcome.error().Message() <<
            ",requestID" << outcome.error().RequestId() << std::endl;
    }
    return ret;
}

OssUploader::~OssUploader()
{
    AlibabaCloud::OSS::ShutdownSdk();
}

