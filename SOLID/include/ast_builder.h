#ifndef AST_BUILDER
#define AST_BUILDER

#include "token.h"
#include <string>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::string;
using std::vector;

// 🧶 Estrutura para um nó
struct Node {
    string valor;
    shared_ptr<Node> esquerda;
    shared_ptr<Node> direita;

    Node(string c) : valor(c), esquerda(nullptr), direita(nullptr){}

};

// 🌳 Cria a Árvore de Sintaxe Abstrata (AST)
class ASTBuilder {
    public:
        shared_ptr<Node> build(const vector<shared_ptr<Token>> &tokens);

    private:
        shared_ptr<Node> parseExpressao(const vector<shared_ptr<Token>> &tokens, int &pos);        


};

#endif // ASTBuilder