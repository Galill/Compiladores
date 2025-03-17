#ifndef TOKEN_H
#define TOKEN_H

#include <string>
 
using std::string;

struct Token {
    string tipo;
    string lexema;
    int posicao;
};

#endif