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
    while (i < line.size()) {
                if (isspace(line[i])) { i++; continue; }
        
                if (line[i] == '/' && i+1 < line.size() && line[i+1] == '/') {
                    break;
                }
                if (i+1 < line.size()) {
                    string doisChars = string(1, line[i]) + line[i+1];
                    if (doisChars == "==" || doisChars == "<=" || doisChars == ">=") {
                        auto token = make_shared<Token>();
                        token->tipo = (doisChars == "==") ? "Igualdade" : "Comparacao";
                        token->lexema = doisChars;
                        token->posicao = i;
                        tokens.push_back(token);
                        i += 2;
                        continue;
                    }
                }
        if (isspace(line[i])) { i++; continue; }

        if (line[i] == '=' && i+1 < line.size() && line[i+1] == '=') {
            auto token = make_shared<Token>();
            token->tipo = "Igualdade";
            token->lexema = "==";
            token->posicao = i;
            tokens.push_back(token);
            i += 2;
            continue;
        }

        if (isalpha(line[i])) {
            string ident;
            while (i < line.size() && isalnum(line[i])) ident += line[i++];
            auto token = make_shared<Token>();
            token->lexema = ident;
            token->posicao = i - ident.size();
            if (ident == "if" || ident == "else" || ident == "while" || ident == "return")
                token->tipo = "PalavraChave";
            else
                token->tipo = "Identificador";
            tokens.push_back(token);
            continue;
        }

        if (isdigit(line[i])) {
            string num;
            while (i < line.size() && isdigit(line[i])) num += line[i++];
            auto token = make_shared<Token>();
            token->tipo = "Numero";
            token->lexema = num;
            token->posicao = i - num.size();
            tokens.push_back(token);
            continue;
        }

        string s(1, line[i]);
        auto token = make_shared<Token>();
        token->lexema = s;
        token->posicao = i;

        if (s == "{") token->tipo = "ChaveEsq";
        else if (s == "}") token->tipo = "ChaveDir";
        else if (s == "=") token->tipo = "Atribuicao";
        else if (s == ";") token->tipo = "PontoVirgula";
        else if (s == "<") token->tipo = "Menor";
        else if (s == ">") token->tipo = "Maior";
        else if (dicionario.count(s)) token->tipo = dicionario[s];
        else throw runtime_error("Caractere inválido: " + s);

        tokens.push_back(token);
        i++;
    }
}
