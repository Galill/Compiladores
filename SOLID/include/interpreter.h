#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast_builder.h"
#include <map>
using std::shared_ptr;
using std::map;

int interpretar(shared_ptr<Node> raiz, const map<string, int>& ambiente);

#endif