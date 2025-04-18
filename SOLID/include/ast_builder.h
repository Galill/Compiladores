#ifndef AST_BUILDER
#define AST_BUILDER

#include "token.h"
#include <string>
#include <memory>
#include <vector>
#include <set>

using std::shared_ptr;
using std::string;
using std::vector;
using std::set;

// 🧶 Estrutura para um nó
struct Node {
    string tipo; 
    string valor; 
    shared_ptr<Node> esquerda;
    shared_ptr<Node> direita;
    vector<shared_ptr<Node>> filhos;

    // Construtor com tipo e valor
    Node(string t, string v) : tipo(t), valor(v), esquerda(nullptr), direita(nullptr) {}
};

// 🌳 Cria a Árvore de Sintaxe Abstrata (AST)
class ASTBuilder {
    public:
        shared_ptr<Node> build(const vector<shared_ptr<Token>> &tokens);

    private:
        set<string> symbolTable;
        shared_ptr<Node> parsePrograma(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseDeclaracao(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseExpA(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseExpM(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parsePrim(const vector<shared_ptr<Token>> &tokens, int &pos);
};

#endif // ASTBuilder