#ifndef AST_BUILDER
#define AST_BUILDER

#include "token.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>

using std::shared_ptr;
using std::string;
using std::vector;
using std::map;
using std::set;


struct Node {
    string tipo;   // Ex: "Programa", "Funcao", "Parametro", "Local", "Variavel", "Numero",
                   //     "Operador", "Comparacao", "Atribuicao", "If", "While", "Chamada"
    string valor;  // Nome, número ou símbolo
    shared_ptr<Node> esquerda;   // Usado para expressão de retorno ou operandos
    shared_ptr<Node> direita;    // (opcional) para variáveis locais no nó Funcao
    vector<shared_ptr<Node>> filhos;    // Para:
                                        //   - Programa: lista de funções
                                        //   - Funcao: lista de parâmetros
                                        //   - Comparacao/Operador: [esq, dir]
                                        //   - Chamada: lista de argumentos
    vector<shared_ptr<Node>> comandos;  // Para blocos (If, While, Funcao, Programa)
    vector<string> locais;              // Variáveis locais (no nó Funcao)

    Node(string t, string v)
      : tipo(t), valor(v), esquerda(nullptr), direita(nullptr) {}
};

// 🌳 Classe para construir a AST da linguagem Fun
class ASTBuilder {
public:
    // Recebe o vetor de tokens e retorna o nó raiz ("Programa")
    shared_ptr<Node> build(const vector<shared_ptr<Token>>& tokens);

private:
    set<string> symbolTable;                        // Tabela de símbolos atual (globais ou no escopo de função)
    map<string, shared_ptr<Node>> funcoesDeclaradas;// Armazena funções definidas (nome → nó Funcao)

    // Parsing de todo o programa Fun (substitui parseProgramaCmd)
    shared_ptr<Node> parseProgramaFun(const vector<shared_ptr<Token>>& tokens, int& pos);

    // Parsing de declaração de função
    shared_ptr<Node> parseFuncDecl(const vector<shared_ptr<Token>>& tokens, int& pos);

    // Parsing de declarações de variável global (var x = exp;)
    shared_ptr<Node> parseVarDecl(const vector<shared_ptr<Token>>& tokens, int& pos);

    // Parsing de comandos (if, while, atribuição)
    shared_ptr<Node> parseCmd(const vector<shared_ptr<Token>>& tokens, int& pos);
    shared_ptr<Node> parseIf(const vector<shared_ptr<Token>>& tokens, int& pos);
    shared_ptr<Node> parseWhile(const vector<shared_ptr<Token>>& tokens, int& pos);
    shared_ptr<Node> parseAtrib(const vector<shared_ptr<Token>>& tokens, int& pos);

    // Parsing de expressões, em ordem de precedência
    shared_ptr<Node> parseExpCmp(const vector<shared_ptr<Token>>& tokens, int& pos); // ==, <, >
    shared_ptr<Node> parseExpA(const vector<shared_ptr<Token>>& tokens, int& pos);   // +, -
    shared_ptr<Node> parseExpM(const vector<shared_ptr<Token>>& tokens, int& pos);   // *, /
    shared_ptr<Node> parsePrim(const vector<shared_ptr<Token>>& tokens, int& pos);   // número, variável, chamada, (exp)
};

#endif // AST_BUILDER