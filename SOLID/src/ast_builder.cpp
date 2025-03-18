#include "../include/ast_builder.h"
#include <stdexcept>

using std::shared_ptr;
using std::vector;
using std::runtime_error;
using std::make_shared;

shared_ptr<Node> ASTBuilder::build(const vector<shared_ptr<Token>> &tokens) {
    int pos = 0;
    return parseExpressao(tokens, pos);
};

shared_ptr<Node> ASTBuilder::parseExpressao(const vector<shared_ptr<Token>> &tokens, int  &pos) {
    if (pos >= tokens.size()) {
        throw runtime_error("Erro sintatico: expressao inesperada");
    }

    if (tokens[pos]->tipo == "Numero") {
        shared_ptr<Node> node = make_shared<Node>(tokens[pos]->lexema);
        pos++;
        return node;
    } else if (tokens[pos]->tipo == "ParenEsq") {
        pos++; // Consome "("

        shared_ptr<Node> esquerda = parseExpressao(tokens, pos);
        if (pos >= tokens.size() || (
            tokens[pos]->tipo != "Soma" && 
            tokens[pos]->tipo != "Subtracao" &&
            tokens[pos]->tipo != "Multiplicacao" &&
            tokens[pos]->tipo != "Divisao"
        )){
            throw runtime_error("Erro sintatico: Operador esperado.");
        }

        string operador = tokens[pos]->lexema;
        pos++;

        shared_ptr<Node> direita = parseExpressao(tokens, pos);

        if (pos >= tokens.size() || tokens[pos]->tipo != "ParenDir") {
            throw runtime_error("Erro sintatico: fechamento do parenteses esperado.");
        }
        pos++; // Consome ")"

        shared_ptr<Node> node = make_shared<Node>(operador);
        node->esquerda = esquerda;
        node->direita = direita;
        return node;
    } else {
        throw runtime_error("Erro sintatico: expressao invalida.");
    }

};