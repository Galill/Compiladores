#include "../include/utils.h"
#include <fstream>
#include <cctype>
#include <stdexcept>

using std::string;
using std::shared_ptr;
using std::vector;
using std::map;
using std::fstream;
using std::getline;
using std::make_shared;
using std::runtime_error;

void ler_arquivo(string filename, vector<string> &linhas){
     fstream file(filename);
     string line;
     if (!file) {
        std::cerr << "Erro! Nao foi possivel abrir o arquivo!" << std::endl;
        return;
    }
    int i = 0;
    while(!file.eof()){
        getline(file,line);
        linhas.push_back(line);
        i++;
    }
    file.close();
    return;
}


void imprimirInfixo(shared_ptr<Node> raiz) {
    if (!raiz) {
        return;
    }

    if (raiz->esquerda || raiz->direita) {
        std::cout << "(";
    }

    imprimirInfixo(raiz->esquerda);

    std::cout << raiz->valor;

    imprimirInfixo(raiz->direita);

    if (raiz->esquerda || raiz->direita) {
        std::cout << ")";
    }
}

void tolkenizer(vector<shared_ptr<Token>> &tokens, string line, map<string, string> dicionario) {
    string numeroBuffer;

    for (int i = 0; i < line.size(); i++) {
        if (isspace(line[i])) continue;

        string s(1, line[i]);
        if (isdigit(line[i])) {
            numeroBuffer += line[i];
        } else {
            if (!numeroBuffer.empty()) {
                auto token = make_shared<Token>();
                token->tipo = "Numero";
                token->lexema = numeroBuffer;
                token->posicao = static_cast<int>(i - numeroBuffer.size());
                tokens.push_back(token);
                numeroBuffer = "";
            }
            if (dicionario.count(s)) {
                auto token = make_shared<Token>();
                token->tipo = dicionario[s];
                token->lexema = s;
                token->posicao = static_cast<int>(i);
                tokens.push_back(token);
            } else {
                throw runtime_error("Erro: Caractere inválido");
            }
        }
    }
    if (!numeroBuffer.empty()) {
        auto token = make_shared<Token>();
        token->tipo = "Numero";
        token->lexema = numeroBuffer;
        token->posicao = static_cast<int>(line.size() - numeroBuffer.size());
        tokens.push_back(token);
    }
}