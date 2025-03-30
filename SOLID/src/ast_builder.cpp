#include "../include/ast_builder.h"
#include <stdexcept>

using std::shared_ptr;
using std::vector;
using std::runtime_error;
using std::make_shared;

shared_ptr<Node> ASTBuilder::build(const vector<shared_ptr<Token>> &tokens) {
    int pos = 0;
    return parseExpA(tokens, pos);
}

shared_ptr<Node> ASTBuilder::parseExpA(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parseExpM(tokens, pos);

    while (pos < tokens.size() && (tokens[pos]->tipo == "Soma" || tokens[pos]->tipo == "Subtracao")) {
        string operador = tokens[pos]->lexema;
        pos++;
        shared_ptr<Node> direita = parseExpM(tokens, pos);
        shared_ptr<Node> node = make_shared<Node>(operador);
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
        shared_ptr<Node> node = make_shared<Node>(operador);
        node->esquerda = esquerda;
        node->direita = direita;
        esquerda = node;
    }

    return esquerda;
}

shared_ptr<Node> ASTBuilder::parsePrim(const vector<shared_ptr<Token>> &tokens, int &pos) {
    if (pos >= tokens.size()) {
        throw runtime_error("Erro sintatico: expressao inesperada");
    }

    if (tokens[pos]->tipo == "Numero") {
        shared_ptr<Node> node = make_shared<Node>(tokens[pos]->lexema);
        pos++;
        return node;
    } else if (tokens[pos]->tipo == "ParenEsq") {
        pos++; // Consome "("
        shared_ptr<Node> node = parseExpA(tokens, pos);
        if (pos >= tokens.size() || tokens[pos]->tipo != "ParenDir") {
            throw runtime_error("Erro sintatico: fechamento do parenteses esperado.");
        }
        pos++; // Consome ")"
        return node;
    } else {
        throw runtime_error("Erro sintatico: expressao invalida.");
    }
}