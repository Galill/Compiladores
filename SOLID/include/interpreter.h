#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast_builder.h"

using std::shared_ptr;

int interpretar(shared_ptr<Node> raiz);

#endif