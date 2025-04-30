
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


struct Node {
    string tipo;
    string valor;
    shared_ptr<Node> esquerda;
    shared_ptr<Node> direita;
    vector<shared_ptr<Node>> filhos;
    vector<shared_ptr<Node>> comandos;
    set<string> symbolTable;

    Node(string t, string v) : tipo(t), valor(v), esquerda(nullptr), direita(nullptr) {}
};


class ASTBuilder {
    public:
        shared_ptr<Node> build(const vector<shared_ptr<Token>> &tokens);
        
    private:
        set<string> symbolTable;

        // Parsing do programa completo Cmd
        shared_ptr<Node> parseProgramaCmd(const vector<shared_ptr<Token>> &tokens, int &pos);

        // Parsing de elementos da linguagem
        shared_ptr<Node> parseDeclaracao(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseCmd(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseIf(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseWhile(const vector<shared_ptr<Token>> &tokens, int &pos);
        shared_ptr<Node> parseAtrib(const vector<shared_ptr<Token>> &tokens, int &pos);

        // Parsing de expressões com diferentes níveis de precedência
        shared_ptr<Node> parseExpCmp(const vector<shared_ptr<Token>> &tokens, int &pos); // <, >, ==
        shared_ptr<Node> parseExpA(const vector<shared_ptr<Token>> &tokens, int &pos);   // +, -
        shared_ptr<Node> parseExpM(const vector<shared_ptr<Token>> &tokens, int &pos);   // *, /
        shared_ptr<Node> parsePrim(const vector<shared_ptr<Token>> &tokens, int &pos);   // variáveis, números, parênteses
};

#endif // AST_BUILDER
