#include <iostream>
#include <vector>
#include "../include/analisa.h"
#include "../include/token.h"
#include "../include/ast_builder.h"
#include "../include/utils.h"
#include "../include/interpreter.h"
#include "../include/dicionario.h"

using std::string;
using std::shared_ptr;
using std::vector;


int main(int argc, char **argv) {
    shared_ptr<Node> ast;
    vector<shared_ptr<Token>> tokens;

    string line;
    ler_arquivo(argv[1], line);
    tolkenizer(tokens, line, dicionario);
    ast = analisa(tokens);

    return 0;
}

