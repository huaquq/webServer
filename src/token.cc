#include "token.h"
#include <string.h>
#include <l8w8jwt/encode.h>


Token::Token(string secretKey)
    : _secretKey(secretKey)
{

}

string Token::getToken()
{
    char* jwt;
    size_t jwt_length;

    struct l8w8jwt_encoding_params params;
    l8w8jwt_encoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;

    params.sub = "Gordon Freeman";
    params.iss = "Black Mesa";
    params.aud = "HUAQUQ";

    params.iat = 0;
    params.exp = 60 * 60 * 24 * 365; //1 year

    params.secret_key = (unsigned char*)_secretKey.c_str();
    params.secret_key_length = _secretKey.size();

    params.out = &jwt;
    params.out_length = &jwt_length;

    int r = l8w8jwt_encode(&params);

    /* printf("\n l8w8jwt example HS512 token: %s \n", r == L8W8JWT_SUCCESS ? jwt : " (encoding failure) "); */
    string res = jwt;
    /* Always free the output jwt string! */
    l8w8jwt_free(jwt);
    return res;
}

