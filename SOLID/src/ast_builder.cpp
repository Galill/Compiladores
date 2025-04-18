#include "../include/ast_builder.h"
#include <stdexcept>
#include <iostream>

using std::shared_ptr;
using std::vector;
using std::runtime_error;
using std::make_shared;
using std::string;

shared_ptr<Node> ASTBuilder::build(const vector<shared_ptr<Token>> &tokens) {
    // std::cout << "==== TOKENS DE ENTRADA ====\n";
    // for (const auto& token : tokens) {
    //     std::cout << token->lexema << " [" << token->tipo << "]\n";
    // }
    // std::cout << "===========================\n";
    int pos = 0;
    symbolTable.clear();
    return parsePrograma(tokens, pos);
}

shared_ptr<Node> ASTBuilder::parsePrograma(const vector<shared_ptr<Token>> &tokens, int &pos) {
    vector<shared_ptr<Node>> declaracoes;

    while (pos < tokens.size() && tokens[pos]->lexema != "=") {
        //std::cout << "parsePrograma: analisando token " << tokens[pos]->lexema << " [" << tokens[pos]->tipo << "]\n";

        if (tokens[pos]->tipo != "Identificador") {
            throw runtime_error("Erro sintático: identificador esperado no início da declaração.");
        }

        auto declaracao = parseDeclaracao(tokens, pos);
        declaracoes.push_back(declaracao);
    }

    if (pos >= tokens.size()) {
        throw runtime_error("Erro sintático: fim inesperado, '=' esperado para expressão final.");
    }

    //std::cout << "Token antes da expressão final: " << tokens[pos]->lexema << " [" << tokens[pos]->tipo << "]\n";

    if (tokens[pos]->lexema != "=") {
        throw runtime_error("Erro sintático: '=' esperado para expressão final.");
    }
    pos++;

    auto expFinal = parseExpA(tokens, pos);

    auto programa = make_shared<Node>("Programa", "");
    programa->filhos = declaracoes;
    programa->esquerda = expFinal;
    return programa;
}

shared_ptr<Node> ASTBuilder::parseDeclaracao(const vector<shared_ptr<Token>> &tokens, int &pos) {
    if (pos >= tokens.size()) throw runtime_error("Erro sintático: fim inesperado ao tentar ler identificador.");

    string varName = tokens[pos]->lexema;
    pos++;

    if (pos >= tokens.size() || tokens[pos]->lexema != "=") {
        throw runtime_error("Erro sintático: '=' esperado após identificador.");
    }
    pos++;

    auto expr = parseExpA(tokens, pos);

    if (pos >= tokens.size() || tokens[pos]->lexema != ";") {
        throw runtime_error("Erro sintático: ';' esperado após expressão.");
    }
    pos++;

    if (symbolTable.count(varName)) {
        throw runtime_error("Erro semântico: variável '" + varName + "' já declarada.");
    }
    symbolTable.insert(varName);

    auto declNode = make_shared<Node>("Declaracao", varName);
    declNode->esquerda = expr;
    return declNode;
}

shared_ptr<Node> ASTBuilder::parseExpA(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parseExpM(tokens, pos);

    while (pos < tokens.size() && (tokens[pos]->tipo == "Soma" || tokens[pos]->tipo == "Subtracao")) {
        string operador = tokens[pos]->lexema;
        pos++;
        shared_ptr<Node> direita = parseExpM(tokens, pos);
        shared_ptr<Node> node = make_shared<Node>("Operador", operador);
        node->esquerda = esquerda;
        node->direita = direita;
        esquerda = node;
    }

    return esquerda;
}

shared_ptr<Node> ASTBuilder::parseExpM(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parsePrim(tokens, pos);

    while (pos < tokens.size() && (tokens[pos]->tipo == "Multiplicacao" || tokens[pos]->tipo == "Divisao")) {
        string operador = tokens[pos]->lexema;
        pos++;
        shared_ptr<Node> direita = parsePrim(tokens, pos);
        shared_ptr<Node> node = make_shared<Node>("Operador", operador);
        node->esquerda = esquerda;
        node->direita = direita;
        esquerda = node;
    }

    return esquerda;
}

shared_ptr<Node> ASTBuilder::parsePrim(const vector<shared_ptr<Token>> &tokens, int &pos) {
    if (pos >= tokens.size()) {
        throw runtime_error("Erro sintático: expressão inesperada (fim da entrada).");
    }

    if (tokens[pos]->tipo == "Identificador") {
        string varName = tokens[pos]->lexema;
        if (!symbolTable.count(varName)) {
            throw runtime_error("Erro semântico: variável '" + varName + "' não declarada.");
        }
        auto node = make_shared<Node>("Variavel", varName);
        pos++;
        return node;
    }

    if (tokens[pos]->tipo == "Numero") {
        shared_ptr<Node> node = make_shared<Node>("Numero", tokens[pos]->lexema);
        pos++;
        return node;
    } else if (tokens[pos]->tipo == "ParenEsq") {
        pos++; // Consome "("
        shared_ptr<Node> node = parseExpA(tokens, pos);
        if (pos >= tokens.size() || tokens[pos]->tipo != "ParenDir") {
            throw runtime_error("Erro sintático: fechamento do parêntese ')' esperado.");
        }
        pos++; // Consome ")"
        return node;
    } else {
        throw runtime_error("Erro sintático: expressão inválida.");
    }
}
