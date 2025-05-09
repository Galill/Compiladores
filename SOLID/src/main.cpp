#include <iostream>
#include <vector>
#include <fstream>
#include <map>

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
using std::map;

int interpretar(shared_ptr<Node> raiz, const map<string, int>& ambiente);

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
        {")", "ParenDir"},
        {"=", "Atribuicao"}, 
        {";", "PontoVirgula"}
    };

    vector<string> linhas;
    ler_arquivo(argv[1], linhas); 

    
    for (const string& linha : linhas) {
        try {
            tolkenizer(tokens, linha, dicionario);
            validar(tokens);
        } catch (const std::runtime_error& e) {
            std::cerr << "Erro na linha \"" << linha << "\": " << e.what() << std::endl;
            return 1;
        }
    }

  
    try {
        ast = analisa(tokens);
        if (ast) {
            map<string, int> ambiente;

            int resultadoFinal = interpretar(ast, ambiente);
            std::cout << "Resultado da expressão final: " << resultadoFinal << std::endl;

            string codigoGerado = gerar_codigo(ast);
            std::cout << "Código Assembly Gerado:\n" << codigoGerado << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Erro na interpretação: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
