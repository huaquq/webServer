#ifndef __HUAQUQ_OSS_H__
#define __HUAQUQ_OSS_H__
#include <string>
#include <alibabacloud/oss/OssClient.h>

using std::string;

struct OssInfo
{
    string AccessKeyID = "LTAI5t6pz3aArEZVKbGzYqmt";
    string AccessKeySecret = "j2iL1CxydIl55enxAkTc1COoqWeI3B";
    string Endpoint = "oss-cn-shanghai.aliyuncs.com";
    string BucketName = "aliyun-oss-bucket-huaquq";
};

class OssUploader
{
public:
    OssUploader(const OssInfo& info = OssInfo());

    bool doUpload(const string& filename, const string& objectName);

    ~OssUploader();
private:
    OssInfo _info;
    AlibabaCloud::OSS::ClientConfiguration _conf;
    AlibabaCloud::OSS::OssClient _ossClient;
};
#endif

