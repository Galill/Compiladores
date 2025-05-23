#ifndef UTILS_H
#define UTILS_H

#include "token.h"
#include "ast_builder.h"
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

using std::shared_ptr;
using std::vector;
using std::string;
using std::map;


void ler_arquivo(string file, vector<string> &linhas);
void imprimirInfixo(shared_ptr<Node> raiz);
void tolkenizer(vector<shared_ptr<Token>> &tokens, string line, map<string, string> dicionario);

#endif