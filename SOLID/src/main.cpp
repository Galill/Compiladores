#include <iostream>
#include <vector>
#include <fstream>
#include "../include/analisa.h"
#include "../include/token.h"
#include "../include/ast_builder.h"
#include "../include/utils.h"
#include "../include/interpreter.h"
#include "../include/dicionario.h"
#include "../include/validator.h"
#include "../include/gerar_codigo.h"

using std::string;
using std::shared_ptr;
using std::vector;
using std::ofstream;
int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nome_do_arquivo>" << std::endl;
        return 1;
    }

    shared_ptr<Node> ast;
    vector<shared_ptr<Token>> tokens;
    map<string, string> dicionario = {
        {"+", "Soma"},
        {"-", "Subtracao"},
        {"*", "Multiplicacao"},
        {"/", "Divisao"},
        {"(", "ParenEsq"},
        {")", "ParenDir"}
    };

    vector<string> linhas;
    ler_arquivo(argv[1], linhas); //carrega o arquivo de entradas

    for (const string& linha : linhas) {
        try {
            tokens.clear();
            tolkenizer(tokens, linha, dicionario);
            validar(tokens);
            ast = analisa(tokens);
            if (ast) {
                string codigoGerado = gerar_codigo(ast);
                //std::cout << "Codigo Assembly Gerado:\n" << codigoGerado << std::endl;
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Erro na linha \"" << linha << "\": " << e.what() << std::endl;
        }
    }

    return 0;
}

