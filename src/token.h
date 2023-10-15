#ifndef __HUAQUQ_TOKEN_H__
#define __HUAQUQ_TOKEN_H__

#include <iostream>
using std::cout;
using std::endl;
using std::string;

class Token
{
public:
    Token(string secretKey);
    string getToken();
private:
    string _secretKey;
};

#endif
