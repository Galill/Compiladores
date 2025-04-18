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

void ler_arquivo(string filename, vector<string> &linhas) {
    fstream file(filename);
    string line;
    if (!file) {
        std::cerr << "Erro! Nao foi possivel abrir o arquivo!" << std::endl;
        return;
    }
    while (getline(file, line)) {
        linhas.push_back(line);
    }
    file.close();
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
    int i = 0;
    string buffer;
    while (i < line.size()) {
        if (isspace(line[i])) {
            i++;
            continue;
        }

        // Processar números
        if (isdigit(line[i])) {
            string numeroBuffer;
            while (i < line.size() && isdigit(line[i])) {
                numeroBuffer += line[i];
                i++;
            }
            // Verificar se o próximo caractere é uma letra (erro léxico)
            if (i < line.size() && isalpha(line[i])) {
                throw runtime_error("Erro léxico: número seguido de letra sem operador.");
            }
            auto token = make_shared<Token>();
            token->tipo = "Numero";
            token->lexema = numeroBuffer;
            token->posicao = i - numeroBuffer.size();
            tokens.push_back(token);
        }
        // Processar identificadores
        else if (isalpha(line[i])) {
            string identBuffer;
            while (i < line.size() && isalnum(line[i])) {
                identBuffer += line[i];
                i++;
            }
            auto token = make_shared<Token>();
            token->tipo = "Identificador";
            token->lexema = identBuffer;
            token->posicao = i - identBuffer.size();
            tokens.push_back(token);
        }
        // Processar símbolos
        else {
            string s(1, line[i]);
            if (dicionario.count(s)) {
                auto token = make_shared<Token>();
                token->tipo = dicionario[s];
                token->lexema = s;
                token->posicao = i;
                tokens.push_back(token);
            }else if (s == "=") {
                auto token = make_shared<Token>();
                token->tipo = "Atribuicao"; // ✅ Tipo correto para o '=' final
                token->lexema = s;
                token->posicao = i;
                tokens.push_back(token);
            }else if (s == ";") {
                auto token = make_shared<Token>();
                token->tipo = "PontoVirgula";
                token->lexema = s;
                token->posicao = i;
                tokens.push_back(token);
            } else {
                throw runtime_error("Erro léxico: caractere inválido '" + s + "'.");
            }
            i++;
        }
    }
}