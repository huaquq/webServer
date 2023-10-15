#include "hash.h"
#include <strings.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <openssl/sha.h>

using std::string;

Hash::Hash(string filename)
    : _filename(filename)
{

}
string Hash::sha1()
{
    int fd = open(_filename.c_str(), O_RDWR);
    if(fd < 0)
    {
        perror("open");
        return string();
    }

    char buff[1024] = {0};
    SHA_CTX ctx;
    SHA1_Init(&ctx);

    while(1)
    {
        int ret = read(fd, buff, sizeof(buff));
        if(ret == 0)
            break;
        SHA1_Update(&ctx, buff, ret);
        bzero(buff, sizeof(buff));        
    }

    unsigned char md[20] = {0};
    SHA1_Final(md, &ctx);
    char fragment[3] = {0};
    string result;
    for(int i = 0; i < 20; ++i)
    {
        sprintf(fragment, "%02x", md[i]);
        result += fragment;
    }
    std::cout << "sha1: " <<result <<  std::endl;
    return result;
}

