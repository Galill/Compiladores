#include "../include/ast_builder.h"
#include <stdexcept>
#include <iostream>

using std::shared_ptr;
using std::vector;
using std::runtime_error;
using std::make_shared;
using std::string;

shared_ptr<Node> ASTBuilder::build(const vector<shared_ptr<Token>> &tokens) {
    int pos = 0;
    symbolTable.clear();
    return parseProgramaCmd(tokens, pos);
}

shared_ptr<Node> ASTBuilder::parseProgramaCmd(const vector<shared_ptr<Token>> &tokens, int &pos) {
    vector<shared_ptr<Node>> declaracoes;
    bool inBlock = false;

    while (pos < tokens.size()) {
        if (tokens[pos]->tipo == "ChaveEsq") {
            inBlock = true;
            pos++;
            break;
        }
        if (tokens[pos]->tipo != "Identificador") {
            throw runtime_error("Erro: esperado declaração ou '{' para iniciar bloco principal.");
        }
        auto decl = parseDeclaracao(tokens, pos);
        declaracoes.push_back(decl);
        
        if (decl->tipo == "Declaracao") {
            symbolTable.insert(decl->valor);
        }
    }

    if (!inBlock) {
        throw runtime_error("Erro: bloco principal faltando.");
    }

    vector<shared_ptr<Node>> comandos;
    while (tokens[pos]->lexema != "return") {
        auto cmd = parseCmd(tokens, pos);
        comandos.push_back(cmd);
    }

    if (tokens[pos]->lexema != "return") {
        throw runtime_error("Erro: esperado 'return' antes da expressão final.");
    }
    pos++;
    auto expr = parseExpCmp(tokens, pos);

    if (tokens[pos]->tipo != "PontoVirgula") {
        throw runtime_error("Erro: esperado ';' após expressão de retorno.");
    }
    pos++;

    if (tokens[pos]->tipo != "ChaveDir") {
        throw runtime_error("Erro: esperado '}' ao final do bloco.");
    }
    pos++;

    auto bloco = make_shared<Node>("Bloco", "");
    bloco->filhos = declaracoes;
    bloco->comandos = comandos;
    bloco->esquerda = expr;
    bloco->symbolTable = symbolTable; 
    
    return bloco;
}

shared_ptr<Node> ASTBuilder::parseDeclaracao(const vector<shared_ptr<Token>> &tokens, int &pos) {
    string varName = tokens[pos]->lexema;
    pos++;

    if (tokens[pos]->lexema != "=") {
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

shared_ptr<Node> ASTBuilder::parseCmd(const vector<shared_ptr<Token>> &tokens, int &pos) {
    if (tokens[pos]->lexema == "if") return parseIf(tokens, pos);
    if (tokens[pos]->lexema == "while") return parseWhile(tokens, pos);
    if (tokens[pos]->tipo == "Identificador") return parseAtrib(tokens, pos);
    throw runtime_error("Erro: comando inválido");
}

shared_ptr<Node> ASTBuilder::parseIf(const vector<shared_ptr<Token>> &tokens, int &pos) {
    pos++;
    auto cond = parseExpCmp(tokens, pos);

    if (tokens[pos]->tipo != "ChaveEsq") throw runtime_error("Erro: esperado '{' após if");
    pos++;
    vector<shared_ptr<Node>> cmdVerdadeiro;
    while (tokens[pos]->tipo != "ChaveDir") {
        cmdVerdadeiro.push_back(parseCmd(tokens, pos));
    }
    pos++;

    if (tokens[pos]->lexema != "else") throw runtime_error("Erro: esperado 'else'");
    pos++;
    if (tokens[pos]->tipo != "ChaveEsq") throw runtime_error("Erro: esperado '{' após else");
    pos++;
    vector<shared_ptr<Node>> cmdFalso;
    while (tokens[pos]->tipo != "ChaveDir") {
        cmdFalso.push_back(parseCmd(tokens, pos));
    }
    pos++;

    auto noIf = make_shared<Node>("If", "if");
    noIf->esquerda = cond;
    auto bloco1 = make_shared<Node>("Bloco", "");
    bloco1->comandos = cmdVerdadeiro;
    auto bloco2 = make_shared<Node>("Bloco", "");
    bloco2->comandos = cmdFalso;
    noIf->filhos.push_back(bloco1);
    noIf->filhos.push_back(bloco2);
    return noIf;
}

shared_ptr<Node> ASTBuilder::parseWhile(const vector<shared_ptr<Token>> &tokens, int &pos) {
    pos++;
    auto cond = parseExpCmp(tokens, pos);
    if (tokens[pos]->tipo != "ChaveEsq") throw runtime_error("Erro: esperado '{' no while");
    pos++;
    vector<shared_ptr<Node>> comandos;
    while (tokens[pos]->tipo != "ChaveDir") {
        comandos.push_back(parseCmd(tokens, pos));
    }
    pos++;
    auto no = make_shared<Node>("While", "while");
    no->esquerda = cond;
    auto bloco = make_shared<Node>("Bloco", "");
    bloco->comandos = comandos;
    no->filhos.push_back(bloco);
    return no;
}

shared_ptr<Node> ASTBuilder::parseAtrib(const vector<shared_ptr<Token>> &tokens, int &pos) {
    string var = tokens[pos]->lexema;
    if (!symbolTable.count(var)) {
        throw runtime_error("Erro: variável '" + var + "' não declarada");
    }
    pos++;
    if (tokens[pos]->tipo != "Atribuicao") throw runtime_error("Erro: esperado '=' na atribuição");
    pos++;
    auto expr = parseExpCmp(tokens, pos);
    if (tokens[pos]->tipo != "PontoVirgula") throw runtime_error("Erro: esperado ';' após atribuição");
    pos++;
    auto no = make_shared<Node>("Atribuicao", var);
    no->esquerda = expr;
    return no;
}

shared_ptr<Node> ASTBuilder::parseExpCmp(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parseExpA(tokens, pos);
    while (pos < tokens.size() &&
        (tokens[pos]->tipo == "Igualdade" || tokens[pos]->tipo == "Menor" || tokens[pos]->tipo == "Maior")) {
        string op = tokens[pos]->lexema;
        pos++;
        auto direita = parseExpA(tokens, pos);
        auto cmp = make_shared<Node>("Comparacao", op);
        cmp->esquerda = esquerda;
        cmp->direita = direita;
        esquerda = cmp;
    }
    return esquerda;
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
            throw runtime_error("Erro semântico: variável '" + varName + "' não declarada (posição " + 
                              std::to_string(tokens[pos]->posicao) + ").");
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
        pos++;
        shared_ptr<Node> node = parseExpA(tokens, pos);
        if (pos >= tokens.size() || tokens[pos]->tipo != "ParenDir") {
            throw runtime_error("Erro sintático: fechamento do parênteses esperado.");
        }
        pos++;
        return node;
    } else {
        throw runtime_error("Erro sintático: expressão inválida.");
    }
}
