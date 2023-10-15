#ifndef __HUAQUQ_HASH_H__
#define __HUAQUQ_HASH_H__
#include <string>

using std::string;

class Hash
{
public:
    Hash(string filename);
    string sha1();
private:
    string _filename;
};

#endif
