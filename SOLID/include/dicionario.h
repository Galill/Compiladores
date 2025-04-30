#ifndef DICIONARIO_H
#define DICIONARIO_H

#include <map>
#include <string>

using std::string;
using std::map;



const map<string, string> dicionario = {
    {"+", "Soma"},
    {"-", "Subtracao"},
    {"*", "Multiplicacao"},
    {"/", "Divisao"},
    {"(", "ParenEsq"},
    {")", "ParenDir"},
    {"=", "Atribuicao"}, 
    {";", "PontoVirgula"}
};

#endif