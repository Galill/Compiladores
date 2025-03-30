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
    vector<string> linhas;

    
    ler_arquivo(argv[1], linhas); //carrega o arquivo de entradas corretas
    for(int i = 0 ;i < linhas.size(); i++){
        //testa todas as entradas
        tolkenizer(tokens, linhas[i], dicionario);
        validar(tokens);
        ast = analisa(tokens);
        tokens.clear(); //limpa o vetor de tokens 
        string codigoGerado = gerar_codigo(ast);
        std::cout << codigoGerado << std::endl;
    }

    linhas.clear();
    ler_arquivo(argv[1], linhas); //carrega o arquivo de entradas erradas
    for(int i = 0 ;i < linhas.size(); i++){
        //testa todas as entradas
        tolkenizer(tokens, linhas[i], dicionario);
        validar(tokens);
        ast = analisa(tokens);
        tokens.clear();
    }

    return 0;
}

