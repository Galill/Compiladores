#include <iostream>
#include <vector>
#include "../include/analisa.h"
#include "../include/token.h"
#include "../include/ast_builder.h"
#include "../include/utils.h"
#include "../include/interpreter.h"

using std::shared_ptr;
using std::vector;

shared_ptr<Node> analisa(vector<shared_ptr<Token>> tokens) {
    try {
        ASTBuilder builder;
        shared_ptr<Node> ast = builder.build(tokens);

        return ast;
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return nullptr;
    }
}