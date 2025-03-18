#include <iostream>
#include "token.h"
#include <vector>
#include "ast_builder.h"

using std::shared_ptr;

shared_ptr<Node> analisa(vector<shared_ptr<Token>> tokens);