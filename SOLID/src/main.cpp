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
    shared_ptr<Node> ast;
    vector<shared_ptr<Token>> tokens;
    ofstream arquivoSaida("saida.s");
    string line;

    
    ler_arquivo(argv[1], line);
    tolkenizer(tokens, line, dicionario);
    validar(tokens);
    ast = analisa(tokens);
    string codigoGerado = gerar_codigo(ast);
    std::cout << codigoGerado << std::endl;

    if(!arquivoSaida){
        std::cout << "Não foi possivel gerar arquivo de saida!!\n" << std::endl;
    }
    arquivoSaida << codigoGerado;
    arquivoSaida.close();
    std::cout << "Arquivo de saida gerado corretamente!\n" << std::endl;
    return 0;
}

