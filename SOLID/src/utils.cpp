#include "../include/utils.h"
#include <fstream>
#include <cctype>

using std::string;
using std::shared_ptr;
using std::vector;
using std::map;
using std::ifstream;
using std::getline;
using std::make_shared;

void ler_arquivo(string filename, string &line){
     ifstream file(filename);
     if (!file) {
        std::cerr << "Erro! Nao foi possivel abrir o arquivo!" << std::endl;
        return;
    }
    getline(file, line);
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
                std::cerr << "Erro! Caractere inválido: " << s << std::endl;
                exit(1);
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